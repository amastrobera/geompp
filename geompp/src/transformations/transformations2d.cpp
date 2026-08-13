#include "transformations/transformations2d.hpp"

namespace geompp::transformations {

geometry::Point2D translate(geometry::Point2D const& p, maths::Vector2 const& offset) {
  return geometry::Point2D(p.x() + offset.x(), p.y() + offset.y());
}

geometry::Point2D rotate(geometry::Point2D const& p, double angle_rad) {
  double c = std::cos(angle_rad);
  double s = std::sin(angle_rad);
  return geometry::Point2D(p.x() * c - p.y() * s, p.x() * s + p.y() * c);
}

geometry::Point2D scale(geometry::Point2D const& p, double factor) { return scale(p, factor, factor); }

geometry::Point2D scale(geometry::Point2D const& p, double sx, double sy) {
  return geometry::Point2D(p.x() * sx, p.y() * sy);
}

geometry::Point2D shear(geometry::Point2D const& p, double shx, double shy) {
  return geometry::Point2D(p.x() + shx * p.y(), p.y() + shy * p.x());
}

geometry::Point2D reflect(geometry::Point2D const& p, maths::Vector2 const& normal) {
  maths::Vector2 n = normal.Normalized();  // throws on zero-length normal
  double d = p.x() * n.x() + p.y() * n.y();
  return geometry::Point2D(p.x() - 2.0 * d * n.x(), p.y() - 2.0 * d * n.y());
}

geometry::Point2D transform(geometry::Point2D const& p, maths::Matrix3 const& m) {
  maths::Vector3 homogeneous = m * maths::Vector3(p.x(), p.y(), 1.0);
  return geometry::Point2D(homogeneous.x(), homogeneous.y());
}

geometry::Vector2D transform(geometry::Vector2D const& v, maths::Matrix3 const& m) {
  maths::Vector3 homogeneous = m * maths::Vector3(v.x(), v.y(), 0.0);
  return geometry::Vector2D(homogeneous.x(), homogeneous.y());
}

geometry::LineSegment2D transform(geometry::LineSegment2D const& seg, maths::Matrix3 const& m) {
  return geometry::LineSegment2D::Make(transform(seg.First(), m), transform(seg.Last(), m));
}

geometry::Polyline2D transform(geometry::Polyline2D const& polyline, maths::Matrix3 const& m) {
  std::vector<geometry::Point2D> points;
  points.reserve(polyline.Size());
  for (int i = 0; i < polyline.Size(); ++i) {
    points.push_back(transform(polyline[i], m));
  }
  return geometry::Polyline2D::Make(points);
}

geometry::Triangle2D transform(geometry::Triangle2D const& tri, maths::Matrix3 const& m) {
  auto [p0, p1, p2] = tri.Vertices();
  return geometry::Triangle2D::Make(transform(p0, m), transform(p1, m), transform(p2, m));
}

namespace {
std::vector<geometry::Point2D> transform_ring(std::vector<geometry::Point2D> const& ring,
                                              maths::Matrix3 const& m) {
  std::vector<geometry::Point2D> result;
  result.reserve(ring.size());
  for (auto const& p : ring) {
    result.push_back(transform(p, m));
  }
  return result;
}
}  // namespace

geometry::Polygon2D transform(geometry::Polygon2D const& poly, maths::Matrix3 const& m) {
  if (!poly.HasHoles()) {
    return geometry::Polygon2D::Make(transform_ring(poly.Perimeter(), m));
  }
  std::vector<std::vector<geometry::Point2D>> holes;
  holes.reserve(poly.Holes().size());
  for (auto const& hole : poly.Holes()) {
    holes.push_back(transform_ring(hole, m));
  }
  return geometry::Polygon2D::Make(transform_ring(poly.Perimeter(), m), holes);
}

geometry::Mesh2D transform(geometry::Mesh2D const& mesh, maths::Matrix3 const& m) {
  std::vector<geometry::Triangle2D> triangles;
  triangles.reserve(mesh.Size());
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    triangles.push_back(transform(mesh[i], m));
  }
  return geometry::Mesh2D::FromTriangles(triangles);
}

geometry::PolyMesh2D transform(geometry::PolyMesh2D const& mesh, maths::Matrix3 const& m) {
  std::vector<geometry::Polygon2D> polygons;
  polygons.reserve(mesh.Size());
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    polygons.push_back(transform(mesh[i], m));
  }
  return geometry::PolyMesh2D::FromPolygons(polygons);
}

}  // namespace geompp::transformations
