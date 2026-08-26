#include "polymesh3d.hpp"

#include "calc_utils3d.hpp"
#include "geometry_collection3d.hpp"
#include "grid_cell3d.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "mesh3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

PolyMesh3D PolyMesh3D::FromPolygons(std::vector<Polygon3D> const& polygons) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(polygons));

  // GridCellMapForPolyMesh3D::Make() throws std::invalid_argument if polygons is empty or holed.
  auto mesh_maker = detail::GridCellMapForPolyMesh3D::Make(polygons);

  // compute and save area
  double area = 0;
  for (auto const& p : polygons) {
    area += p.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), mesh_maker.GetFaceIdxBegins(),
          mesh_maker.GetFaceIdxOffsets(), area};
}

Polygon3D PolyMesh3D::operator[](std::size_t i) const {
  if (i >= FACE_IDX_BEGINS->size()) {
    throw std::out_of_range("index out of FACE_IDX_BEGINS list");
  }
  std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
  std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

  std::vector<Point3D> vertices;
  vertices.reserve(f_idx_offset);  // number of vertices per polygon
  for (std::size_t i = 0; i < f_idx_offset; ++i) {
    std::size_t v_idx = (*FACE_INDICES)[f_idx_begin + i];
    vertices.emplace_back((*VERTICES)[v_idx]);
  }

  // NOT Make(): vertices came straight from FromPolygons()'s own validated, welded storage, and may
  // deliberately still contain a collinear vertex that's load-bearing for a neighboring facet (see
  // Polygon3D's PolyMesh3D friend-grant doc comment). Make()'s remove_collinear() would silently strip it
  // back out on every read, reintroducing the exact T-junction FromPolygons() already proved doesn't exist.
  return Polygon3D::FromUniquePoints(std::move(vertices));
}

Mesh3D PolyMesh3D::Triangulate(TriangulationParams::Strategy strategy) const {
  // VERTICES is already a simple, CCW-wound, duplicate-free point set (guaranteed by how PolyMesh3D is
  // built from valid Polygon3D instances), so every one of triangulate_impl's input-quality checks can be
  // skipped.
  std::size_t n_faces = FACE_IDX_BEGINS->size();

  // Every simple facet triangulates into exactly (vertex_count - 2) triangles, known upfront from
  // FACE_IDX_OFFSETS — reserve once so the outer vector never reallocates/copies triangles already
  // appended by earlier facets as later ones are added.
  std::size_t total_triangles = 0;
  for (std::size_t i = 0; i < n_faces; ++i) {
    total_triangles += (*FACE_IDX_OFFSETS)[i] - 2;
  }

  std::vector<Triangle3D> triangles;
  triangles.reserve(total_triangles);

  for (std::size_t i = 0; i < n_faces; ++i) {
    std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
    std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

    std::vector<Point3D> vertices;
    vertices.reserve(f_idx_offset);  // number of vertices per polygon
    for (std::size_t j = 0; j < f_idx_offset; ++j) {
      std::size_t v_idx = (*FACE_INDICES)[f_idx_begin + j];
      vertices.emplace_back((*VERTICES)[v_idx]);
    }

    auto tris = triangulate(vertices, TriangulationParams{strategy, TriangulationParams::Simplicity::Guaranteed,
                                                          TriangulationParams::Winding::Guaranteed,
                                                          TriangulationParams::Collinearity::Guaranteed});
    triangles.insert(triangles.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
  }

  return Mesh3D::FromTriangles(triangles);
}

GeometryCollection3D PolyMesh3D::ToGeometryCollection() const {
  GeometryCollection3D collection;
  for (auto const& poly : Faces()) {
    collection.Add(poly);
  }
  return collection;
}

std::string PolyMesh3D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYMESH (";
  std::size_t n = FACE_IDX_BEGINS->size();
  for (std::size_t i = 0; i < n; ++i) {
    std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];
    std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];

    buf << "((";
    for (std::size_t j = 0; j < f_idx_offset; ++j) {
      Point3D const& p = (*VERTICES)[(*FACE_INDICES)[f_idx_begin + j]];
      buf << std::format("{} {} {}, ", round(p.x()), round(p.y()), round(p.z()));
    }
    Point3D const& p0 = (*VERTICES)[(*FACE_INDICES)[f_idx_begin]];
    buf << std::format("{} {} {}", round(p0.x()), round(p0.y()), round(p0.z()));
    buf << "))";
    if (i + 1 < n) {
      buf << ", ";
    }
  }
  buf << ")";
  return buf.str();
}

PolyMesh3D PolyMesh3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POLYMESH") {
      throw std::runtime_error("geometry name");
    }

    std::size_t outer_close = wkt.rfind(')');
    if (outer_close == std::string::npos) {
      throw std::runtime_error("brakets (outer close)");
    }

    std::string content = wkt.substr(end_gtype + 1, outer_close - end_gtype - 1);
    auto facets = detail::extract_wkt_top_level_groups(content);
    if (facets.empty()) {
      throw std::runtime_error("no facets");
    }

    std::vector<Polygon3D> polygons;
    polygons.reserve(facets.size());
    for (auto const& facet : facets) {
      std::size_t ring_open = facet.find('(');
      if (ring_open == std::string::npos) {
        throw std::runtime_error("brakets (ring open)");
      }
      std::size_t ring_close = facet.find(')', ring_open);
      if (ring_close == std::string::npos) {
        throw std::runtime_error("brakets (ring close)");
      }
      std::string ring_str = facet.substr(ring_open + 1, ring_close - ring_open - 1);

      std::vector<Point3D> ring;
      for (std::string const& tok : geompp::tokenize_string(ring_str, ',')) {
        auto nums = geompp::tokenize_to_doubles(geompp::trim(tok));
        if (nums.size() != 3) {
          throw std::runtime_error("numbers");
        }
        ring.emplace_back(nums[0], nums[1], nums[2]);
      }
      // WKT rings close by repeating the first point -- drop it before passing to Polygon3D::Make.
      if (ring.size() > 1 && ring.back().AlmostEquals(ring.front())) {
        ring.pop_back();
      }
      if (ring.size() < 3) {
        throw std::runtime_error("polymesh facet must have at least 3 vertices");
      }
      polygons.push_back(Polygon3D::Make(ring));
    }

    return PolyMesh3D::FromPolygons(polygons);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

  throw std::runtime_error("failed to parse WKT");
}

void PolyMesh3D::ToFile(std::string const& path) const {
  try {
    std::string content = ToWkt();

    std::ofstream outfile(path);
    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open file");
    }

    outfile << content;
    outfile.close();

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }
}

PolyMesh3D PolyMesh3D::FromFile(std::string const& path) {
  try {
    std::string content;

    std::ifstream in_file(path);
    if (!in_file.is_open()) {
      throw std::runtime_error("could not open file");
    }

    in_file.seekg(0, std::ios::end);
    std::streamsize fileSize = in_file.tellg();
    in_file.seekg(0, std::ios::beg);

    content.resize(static_cast<std::size_t>(fileSize));
    in_file.read(&content[0], fileSize);

    return FromWkt(content);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }

  throw std::runtime_error("failed to parse WKT");
}

}  // namespace geometry

}  // namespace geompp
