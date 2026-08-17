#include "calc_utils/polygonization3d.hpp"

#include "calc_utils/polygon_queries2d.hpp"
#include "calc_utils/triangulation2d.hpp"
#include "grid_cell3d.hpp"
#include "line_segment2d.hpp"
#include "polygon3d.hpp"
#include "vector3d.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

namespace detail {

Triangle3D MeshFaceView3D::Geometry() const {
  return Triangle3D::Make(m_vertices[VertexIndex(0)], m_vertices[VertexIndex(1)], m_vertices[VertexIndex(2)]);
}

std::optional<MeshFaceView3D> MeshFaceView3D::Neighbor(TriangleCompactNeighborRef::TriangleEdge edge) const {
  auto const& ref = m_neighbor_base[m_face_id][static_cast<std::uint32_t>(edge)];
  if (ref.is_boundary()) {
    return std::nullopt;
  }
  return MeshFaceView3D(m_vertices, m_face_index_base, m_neighbor_base, ref.triangle_id());
}

TriangleCompactNeighborRef::TriangleEdge MeshFaceView3D::NeighborEntryEdge(
    TriangleCompactNeighborRef::TriangleEdge edge) const {
  auto const& ref = m_neighbor_base[m_face_id][static_cast<std::uint32_t>(edge)];
  if (ref.is_boundary()) {
    return TriangleCompactNeighborRef::TriangleEdge::INVALID;
  }
  return ref.edge_id();
}

}  // namespace detail

std::vector<Polygon3D> polygonize(std::vector<Triangle3D> const& triangles, PolygonizationParams const& params) {
  // Every edge must have at most 1 neighbor -- same adjacency invariant Mesh3D/ConnectedMesh3D::FromTriangles
  // both Assert on untrusted raw input.
  detail::assert_adjacency(validate_adjacency(triangles));

  // GridCellMapForMesh3D::Make() throws std::invalid_argument if triangles is empty. Weld only (no
  // ConnectedMesh3D scaffolding) -- same welding Mesh3D::FromTriangles itself uses.
  auto mesh_maker = detail::GridCellMapForMesh3D::Make(triangles);
  auto vertices = mesh_maker.GetUniques();
  auto face_indices = mesh_maker.GetFaceIndices();

  std::size_t n = face_indices->size();
  auto neighbor_refs = detail::build_neighbor_refs(face_indices->data()->data(), n);

  std::vector<detail::MeshFaceView3D> faces;
  faces.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    faces.emplace_back(vertices->data(), face_indices->data()->data(), neighbor_refs.data(), i);
  }

  auto pieces = detail::polygonize_impl(faces, params);

  std::vector<Polygon3D> polygons;
  polygons.reserve(pieces.size());
  for (auto& [outer, holes] : pieces) {
    polygons.push_back(Polygon3D::Make(std::move(outer), std::move(holes)));
  }
  return polygons;
}

namespace {

// Does any point of @p a lie on @p b's perimeter (view-projected), or vice versa? Mirrors
// polygonization2d.cpp's rings_touch(), but native Point3D rings need a View2D to project the query point
// through (is_on_perimeter projects @p b's own ring internally; only the query px/py need to already be
// in view space).
bool rings_touch(std::vector<Point3D> const& a, std::vector<Point3D> const& b, View2D const& view) {
  for (auto const& p : a) {
    if (detail::view::is_on_perimeter(b, std::vector<std::vector<Point3D>>{}, view, view.x(p), view.y(p))) {
      return true;
    }
  }
  for (auto const& p : b) {
    if (detail::view::is_on_perimeter(a, std::vector<std::vector<Point3D>>{}, view, view.x(p), view.y(p))) {
      return true;
    }
  }
  return false;
}

}  // namespace

std::vector<Polygon3D> merge(std::vector<Polygon3D> const& polygons) {
  if (polygons.empty()) {
    return {};
  }

  // Group input polygons by plane: coarse hash bucket keyed by the plane's Normal (floor-bucketed the
  // same way GridCell2D/3DHash bucket points for vertex welding), then verified within a normal-bucket
  // against each existing sub-group's representative plane via Plane::AlmostEquals() -- correctly keeps
  // two same-normal-but-different-offset planes (e.g. two parallel floors) in separate groups.
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  auto normal_key = [scale](Vector3D const& n) -> std::array<long long, 3> {
    return {llround(n.x() * scale), llround(n.y() * scale), llround(n.z() * scale)};
  };

  std::vector<Plane> group_planes;
  std::vector<std::size_t> group_of(polygons.size());
  std::map<std::array<long long, 3>, std::vector<std::size_t>> normal_buckets;

  for (std::size_t i = 0; i < polygons.size(); ++i) {
    Plane plane = polygons[i].GetPlane();
    auto& candidates = normal_buckets[normal_key(plane.normal())];

    std::size_t found = candidates.size();  // sentinel: not found yet
    for (std::size_t k = 0; k < candidates.size(); ++k) {
      if (group_planes[candidates[k]].AlmostEquals(plane)) {
        found = k;
        break;
      }
    }

    if (found == candidates.size()) {
      std::size_t new_group = group_planes.size();
      group_planes.push_back(plane);
      candidates.push_back(new_group);
      group_of[i] = new_group;
    } else {
      group_of[i] = candidates[found];
    }
  }

  std::map<std::size_t, std::vector<std::size_t>> groups;  // group id -> polygon indices
  for (std::size_t i = 0; i < polygons.size(); ++i) {
    groups[group_of[i]].push_back(i);
  }

  std::vector<Polygon3D> result;

  for (auto const& [group_id, members] : groups) {
    View2D view = View2D::OnPlane(group_planes[group_id]);

    // Cancel every OUTER-ring edge shared between two polygons in this group, trace what's left.
    std::vector<LineSegment2D> outer_edges;
    for (std::size_t idx : members) {
      auto const& outer = polygons[idx].Perimeter();
      std::size_t n = outer.size();
      for (std::size_t i = 0; i < n; ++i) {
        Point2D a(view.x(outer[i]), view.y(outer[i]));
        Point2D b(view.x(outer[(i + 1) % n]), view.y(outer[(i + 1) % n]));
        outer_edges.push_back(LineSegment2D::Make(a, b));
      }
    }
    auto outer_survivors = detail::cancel_reverse_pairs(outer_edges);
    auto outer_rings_2d = detail::trace_directed_boundary(outer_survivors);

    // Detect touching holes (union-find over an O(h^2) pairwise touch test) within this plane group, then
    // union each touching group -- see the Polygon2D overload's own header doc comment for why.
    std::vector<std::vector<Point3D>> all_holes;
    for (std::size_t idx : members) {
      for (auto const& hole : polygons[idx].Holes()) {
        all_holes.push_back(hole);
      }
    }

    std::size_t nh = all_holes.size();
    std::vector<std::size_t> hole_parent(nh);
    std::iota(hole_parent.begin(), hole_parent.end(), std::size_t{0});

    for (std::size_t i = 0; i < nh; ++i) {
      for (std::size_t j = i + 1; j < nh; ++j) {
        if (!rings_touch(all_holes[i], all_holes[j], view)) {
          continue;
        }
        std::size_t ri = detail::uf_find(hole_parent, i);
        std::size_t rj = detail::uf_find(hole_parent, j);
        if (ri != rj) {
          hole_parent[ri] = rj;
        }
      }
    }

    std::map<std::size_t, std::vector<std::size_t>> hole_groups;
    for (std::size_t i = 0; i < nh; ++i) {
      hole_groups[detail::uf_find(hole_parent, i)].push_back(i);
    }

    std::vector<std::vector<Point2D>> holes_2d;
    holes_2d.reserve(hole_groups.size());
    for (auto const& [root, hmembers] : hole_groups) {
      std::vector<Point3D> merged;
      if (hmembers.size() == 1) {
        merged = all_holes[hmembers[0]];
      } else {
        // Fold the touching group together: reverse each hole's winding, Union() them pairwise (reused,
        // already-tested set-union logic), then reverse the result's winding back.
        std::vector<Point3D> first = all_holes[hmembers[0]];
        std::reverse(first.begin(), first.end());
        Polygon3D acc = Polygon3D::Make(std::move(first));

        for (std::size_t k = 1; k < hmembers.size(); ++k) {
          std::vector<Point3D> next = all_holes[hmembers[k]];
          std::reverse(next.begin(), next.end());
          auto unioned = acc.Union(Polygon3D::Make(std::move(next)));
          if (unioned.size() != 1) {
            throw std::invalid_argument("merge: a group of touching holes did not union into a single region");
          }
          acc = std::move(unioned[0]);
        }

        merged = acc.Perimeter();
        std::reverse(merged.begin(), merged.end());
      }

      std::vector<Point2D> merged_2d;
      merged_2d.reserve(merged.size());
      for (auto const& p : merged) {
        merged_2d.emplace_back(view.x(p), view.y(p));
      }
      holes_2d.push_back(std::move(merged_2d));
    }

    // Package this group's traced outer boundaries + (merged or untouched) holes into {outer, holes}
    // pieces by containment, then unproject every point back to Point3D.
    std::vector<std::vector<Point2D>> all_rings_2d = std::move(outer_rings_2d);
    all_rings_2d.insert(all_rings_2d.end(), std::make_move_iterator(holes_2d.begin()),
                       std::make_move_iterator(holes_2d.end()));
    auto pieces_2d = detail::package_result_rings(all_rings_2d);

    for (auto& [outer2d, holes2d] : pieces_2d) {
      std::vector<Point3D> outer3d;
      outer3d.reserve(outer2d.size());
      for (auto const& p : outer2d) {
        outer3d.push_back(view.xyz(p));
      }

      std::vector<std::vector<Point3D>> holes3d;
      holes3d.reserve(holes2d.size());
      for (auto const& hole2d : holes2d) {
        std::vector<Point3D> hole3d;
        hole3d.reserve(hole2d.size());
        for (auto const& p : hole2d) {
          hole3d.push_back(view.xyz(p));
        }
        holes3d.push_back(std::move(hole3d));
      }

      result.push_back(Polygon3D::Make(std::move(outer3d), std::move(holes3d)));
    }
  }

  return result;
}

}  // namespace geometry

}  // namespace geompp
