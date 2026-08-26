#include "mesh2d.hpp"

#include "calc_utils2d.hpp"
#include "connected_mesh2d.hpp"
#include "geometry_collection2d.hpp"
#include "grid_cell2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "polymesh2d.hpp"
#include "ray2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

Mesh2D Mesh2D::FromTriangles(std::vector<Triangle2D> const& triangles) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(triangles));

  // GridCellMapForMesh2D::Make() throws std::invalid_argument if triangles is empty.
  auto mesh_maker = detail::GridCellMapForMesh2D::Make(triangles);

  // compute and save area
  double area = 0;
  for (auto const& t : triangles) {
    area += t.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), area};
}

Triangle2D Mesh2D::operator[](std::size_t i) const {
  if (i >= FACE_INDICES->size()) {
    throw std::out_of_range("index out of FACE_INDICES list");
  }
  auto f_idx = (*FACE_INDICES)[i];
  return Triangle2D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]);
}

ConnectedMesh2D Mesh2D::Connect() const {
  std::vector<Triangle2D> triangles;
  triangles.reserve(FACE_INDICES->size());
  for (std::size_t i = 0; i < FACE_INDICES->size(); ++i) {
    auto f_idx = (*FACE_INDICES)[i];
    triangles.emplace_back(Triangle2D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]));
  }
  return ConnectedMesh2D::FromTriangles(triangles);
}

PolyMesh2D Mesh2D::Polygonize(PolygonizationParams const& params) const {
  // FACE_INDICES's std::array<size_t,3> elements are laid out contiguously (std::array's own layout
  // guarantee), so data()->data() is a valid flat size_t[3*n] view with no copy -- see this method's own
  // header doc comment for why that (plus build_neighbor_refs' pure index-hashmap math) makes this cheaper
  // than going through Connect().
  std::size_t n = FACE_INDICES->size();
  auto neighbor_refs = detail::build_neighbor_refs(FACE_INDICES->data()->data(), n);

  std::vector<detail::MeshFaceView2D> faces;
  faces.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    faces.emplace_back(VERTICES->data(), FACE_INDICES->data()->data(), neighbor_refs.data(), i);
  }

  auto pieces = detail::polygonize_impl(faces, params);
  auto polygons = detail::polygons_from_pieces(std::move(pieces));
  return PolyMesh2D::FromPolygons(polygons);
}

GeometryCollection2D Mesh2D::ToGeometryCollection() const {
  GeometryCollection2D collection;
  for (auto const& tri : Faces()) {
    collection.Add(tri);
  }
  return collection;
}

std::string Mesh2D::ToWkt() const {
  std::ostringstream buf;
  buf << "MESH (";
  std::size_t n = FACE_INDICES->size();
  for (std::size_t i = 0; i < n; ++i) {
    auto const& f_idx = (*FACE_INDICES)[i];
    Point2D const& p0 = (*VERTICES)[f_idx[0]];
    Point2D const& p1 = (*VERTICES)[f_idx[1]];
    Point2D const& p2 = (*VERTICES)[f_idx[2]];
    buf << std::format("(({} {}, {} {}, {} {}, {} {}))", round(p0.x()), round(p0.y()), round(p1.x()), round(p1.y()),
                        round(p2.x()), round(p2.y()), round(p0.x()), round(p0.y()));
    if (i + 1 < n) {
      buf << ", ";
    }
  }
  buf << ")";
  return buf.str();
}

Mesh2D Mesh2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "MESH") {
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

    std::vector<Triangle2D> triangles;
    triangles.reserve(facets.size());
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

      std::vector<Point2D> ring;
      for (std::string const& tok : geompp::tokenize_string(ring_str, ',')) {
        auto nums = geompp::tokenize_to_doubles(geompp::trim(tok));
        if (nums.size() != 2) {
          throw std::runtime_error("numbers");
        }
        ring.emplace_back(nums[0], nums[1]);
      }
      // WKT rings close by repeating the first point -- drop it before passing to Triangle2D::Make.
      if (ring.size() > 1 && ring.back().AlmostEquals(ring.front())) {
        ring.pop_back();
      }
      if (ring.size() != 3) {
        throw std::runtime_error("mesh facet must have exactly 3 vertices");
      }
      triangles.push_back(Triangle2D::Make(ring[0], ring[1], ring[2]));
    }

    return Mesh2D::FromTriangles(triangles);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

  throw std::runtime_error("failed to parse WKT");
}

void Mesh2D::ToFile(std::string const& path) const {
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

Mesh2D Mesh2D::FromFile(std::string const& path) {
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
