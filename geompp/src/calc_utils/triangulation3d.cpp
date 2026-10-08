#include "calc_utils/triangulation3d.hpp"

#include "calc_utils/convex_hull3d.hpp"
#include "calc_utils/triangulation2d.hpp"
#include "triangle3d.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include <stdexcept>

namespace geompp {

inline namespace geometry {

std::vector<Triangle3D> triangulate(std::vector<Point3D> const& input, Vector3D normal,
                                    TriangulationParams const& settings) {
  if (input.size() < 3) {
    throw std::invalid_argument("less than 3 points");
  }

  auto dax = normal.DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  auto tris = detail::view::triangulate_impl(input, view, settings);
  std::vector<Triangle3D> result;
  result.reserve(tris.size());
  for (auto const& t : tris) {
    result.push_back(Triangle3D::Make(t[0], t[1], t[2]));
  }
  return result;
}

std::vector<Triangle3D> triangulate(std::vector<Point3D> const& input, TriangulationParams const& settings) {
  if (input.size() < 3) {
    throw std::invalid_argument("less than 3 points");
  }

  auto frame = principal_axes(input);
  Vector3D normal = frame.Z;

  return triangulate(input, normal, settings);
}

std::vector<Triangle3D> delaunay(std::vector<Point3D> const& points, Vector3D normal) {
  if (points.size() < 3) {
    throw std::invalid_argument("delaunay: less than 3 points");
  }

  auto dax = normal.DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  auto tri_indices = detail::view::delaunay_triangulation(points, view);
  std::vector<Triangle3D> result;
  result.reserve(tri_indices.size());
  for (auto const& t : tri_indices) {
    result.push_back(Triangle3D::Make(points[t[0]], points[t[1]], points[t[2]]));
  }
  return result;
}

std::vector<Triangle3D> delaunay(std::vector<Point3D> const& points) {
  if (points.size() < 3) {
    throw std::invalid_argument("delaunay: less than 3 points");
  }
  return delaunay(points, principal_axes(points).Z);
}

}  // namespace geometry

}  // namespace geompp
