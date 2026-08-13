#include "transformations/transformations2d.hpp"

#include <variant>

namespace geompp::transformations {

geometry::Point2D transform(geometry::Point2D const& p, maths::Matrix3 const& m) {
  maths::Vector3 homogeneous = m * maths::Vector3(p.x(), p.y(), 1.0);
  return geometry::Point2D(homogeneous.x(), homogeneous.y());
}

geometry::Vector2D transform(geometry::Vector2D const& v, maths::Matrix3 const& m) {
  maths::Vector3 homogeneous = m * maths::Vector3(v.x(), v.y(), 0.0);
  return geometry::Vector2D(homogeneous.x(), homogeneous.y());
}

geometry::Ray2D transform(geometry::Ray2D const& ray, maths::Matrix3 const& m) {
  return geometry::Ray2D::Make(transform(ray.Origin(), m), transform(ray.Direction(), m));
}

geometry::Line2D transform(geometry::Line2D const& line, maths::Matrix3 const& m) {
  return geometry::Line2D::Make(transform(line.Origin(), m), transform(line.Direction(), m));
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
std::vector<geometry::Point2D> transform_ring(std::vector<geometry::Point2D> const& ring, maths::Matrix3 const& m) {
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

geometry::ConnectedMesh2D transform(geometry::ConnectedMesh2D const& mesh, maths::Matrix3 const& m) {
  std::vector<geometry::Triangle2D> triangles;
  triangles.reserve(mesh.Size());
  for (auto const& face : mesh.Faces()) {
    triangles.push_back(transform(face.Geometry(), m));
  }
  return geometry::ConnectedMesh2D::FromTriangles(triangles);
}

geometry::GeometryCollection2D transform(geometry::GeometryCollection2D const& collection, maths::Matrix3 const& m) {
  geometry::GeometryCollection2D result;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    std::visit([&](auto const& shape) { result.Add(transform(shape, m)); }, collection.Get(i));
  }
  return result;
}

// -- translate()/rotate()/scale()/shear()/reflect() convenience wrappers --

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

geometry::Vector2D rotate(geometry::Vector2D const& v, double angle_rad) {
  double c = std::cos(angle_rad);
  double s = std::sin(angle_rad);
  return geometry::Vector2D(v.x() * c - v.y() * s, v.x() * s + v.y() * c);
}

geometry::Vector2D scale(geometry::Vector2D const& v, double factor) { return scale(v, factor, factor); }

geometry::Vector2D scale(geometry::Vector2D const& v, double sx, double sy) {
  return geometry::Vector2D(v.x() * sx, v.y() * sy);
}

geometry::Vector2D shear(geometry::Vector2D const& v, double shx, double shy) {
  return geometry::Vector2D(v.x() + shx * v.y(), v.y() + shy * v.x());
}

geometry::Vector2D reflect(geometry::Vector2D const& v, maths::Vector2 const& normal) {
  maths::Vector2 n = normal.Normalized();  // throws on zero-length normal
  double d = v.x() * n.x() + v.y() * n.y();
  return geometry::Vector2D(v.x() - 2.0 * d * n.x(), v.y() - 2.0 * d * n.y());
}

geometry::Ray2D translate(geometry::Ray2D const& ray, maths::Vector2 const& offset) {
  return transform(ray, maths::Matrix3::Translation(offset));
}
geometry::Ray2D rotate(geometry::Ray2D const& ray, double angle_rad) {
  return transform(ray, maths::Matrix3::Rotation(angle_rad));
}
geometry::Ray2D scale(geometry::Ray2D const& ray, double factor) {
  return transform(ray, maths::Matrix3::Scale(factor));
}
geometry::Ray2D scale(geometry::Ray2D const& ray, double sx, double sy) {
  return transform(ray, maths::Matrix3::Scale(sx, sy));
}
geometry::Ray2D shear(geometry::Ray2D const& ray, double shx, double shy) {
  return transform(ray, maths::Matrix3::Shear(shx, shy));
}
geometry::Ray2D reflect(geometry::Ray2D const& ray, maths::Vector2 const& normal) {
  return transform(ray, maths::Matrix3::Reflection(normal));
}

geometry::Line2D translate(geometry::Line2D const& line, maths::Vector2 const& offset) {
  return transform(line, maths::Matrix3::Translation(offset));
}
geometry::Line2D rotate(geometry::Line2D const& line, double angle_rad) {
  return transform(line, maths::Matrix3::Rotation(angle_rad));
}
geometry::Line2D scale(geometry::Line2D const& line, double factor) {
  return transform(line, maths::Matrix3::Scale(factor));
}
geometry::Line2D scale(geometry::Line2D const& line, double sx, double sy) {
  return transform(line, maths::Matrix3::Scale(sx, sy));
}
geometry::Line2D shear(geometry::Line2D const& line, double shx, double shy) {
  return transform(line, maths::Matrix3::Shear(shx, shy));
}
geometry::Line2D reflect(geometry::Line2D const& line, maths::Vector2 const& normal) {
  return transform(line, maths::Matrix3::Reflection(normal));
}

geometry::LineSegment2D translate(geometry::LineSegment2D const& seg, maths::Vector2 const& offset) {
  return transform(seg, maths::Matrix3::Translation(offset));
}
geometry::LineSegment2D rotate(geometry::LineSegment2D const& seg, double angle_rad) {
  return transform(seg, maths::Matrix3::Rotation(angle_rad));
}
geometry::LineSegment2D scale(geometry::LineSegment2D const& seg, double factor) {
  return transform(seg, maths::Matrix3::Scale(factor));
}
geometry::LineSegment2D scale(geometry::LineSegment2D const& seg, double sx, double sy) {
  return transform(seg, maths::Matrix3::Scale(sx, sy));
}
geometry::LineSegment2D shear(geometry::LineSegment2D const& seg, double shx, double shy) {
  return transform(seg, maths::Matrix3::Shear(shx, shy));
}
geometry::LineSegment2D reflect(geometry::LineSegment2D const& seg, maths::Vector2 const& normal) {
  return transform(seg, maths::Matrix3::Reflection(normal));
}

geometry::GeometryCollection2D translate(geometry::GeometryCollection2D const& collection,
                                         maths::Vector2 const& offset) {
  return transform(collection, maths::Matrix3::Translation(offset));
}
geometry::GeometryCollection2D rotate(geometry::GeometryCollection2D const& collection, double angle_rad) {
  return transform(collection, maths::Matrix3::Rotation(angle_rad));
}
geometry::GeometryCollection2D scale(geometry::GeometryCollection2D const& collection, double factor) {
  return transform(collection, maths::Matrix3::Scale(factor));
}
geometry::GeometryCollection2D scale(geometry::GeometryCollection2D const& collection, double sx, double sy) {
  return transform(collection, maths::Matrix3::Scale(sx, sy));
}
geometry::GeometryCollection2D shear(geometry::GeometryCollection2D const& collection, double shx, double shy) {
  return transform(collection, maths::Matrix3::Shear(shx, shy));
}
geometry::GeometryCollection2D reflect(geometry::GeometryCollection2D const& collection, maths::Vector2 const& normal) {
  return transform(collection, maths::Matrix3::Reflection(normal));
}

geometry::Polygon2D translate(geometry::Polygon2D const& poly, maths::Vector2 const& offset) {
  return transform(poly, maths::Matrix3::Translation(offset));
}
geometry::Polygon2D rotate(geometry::Polygon2D const& poly, double angle_rad) {
  return transform(poly, maths::Matrix3::Rotation(angle_rad));
}
geometry::Polygon2D scale(geometry::Polygon2D const& poly, double factor) {
  return transform(poly, maths::Matrix3::Scale(factor));
}
geometry::Polygon2D scale(geometry::Polygon2D const& poly, double sx, double sy) {
  return transform(poly, maths::Matrix3::Scale(sx, sy));
}
geometry::Polygon2D shear(geometry::Polygon2D const& poly, double shx, double shy) {
  return transform(poly, maths::Matrix3::Shear(shx, shy));
}
geometry::Polygon2D reflect(geometry::Polygon2D const& poly, maths::Vector2 const& normal) {
  return transform(poly, maths::Matrix3::Reflection(normal));
}

geometry::Polyline2D translate(geometry::Polyline2D const& polyline, maths::Vector2 const& offset) {
  return transform(polyline, maths::Matrix3::Translation(offset));
}
geometry::Polyline2D rotate(geometry::Polyline2D const& polyline, double angle_rad) {
  return transform(polyline, maths::Matrix3::Rotation(angle_rad));
}
geometry::Polyline2D scale(geometry::Polyline2D const& polyline, double factor) {
  return transform(polyline, maths::Matrix3::Scale(factor));
}
geometry::Polyline2D scale(geometry::Polyline2D const& polyline, double sx, double sy) {
  return transform(polyline, maths::Matrix3::Scale(sx, sy));
}
geometry::Polyline2D shear(geometry::Polyline2D const& polyline, double shx, double shy) {
  return transform(polyline, maths::Matrix3::Shear(shx, shy));
}
geometry::Polyline2D reflect(geometry::Polyline2D const& polyline, maths::Vector2 const& normal) {
  return transform(polyline, maths::Matrix3::Reflection(normal));
}

geometry::Triangle2D translate(geometry::Triangle2D const& tri, maths::Vector2 const& offset) {
  return transform(tri, maths::Matrix3::Translation(offset));
}
geometry::Triangle2D rotate(geometry::Triangle2D const& tri, double angle_rad) {
  return transform(tri, maths::Matrix3::Rotation(angle_rad));
}
geometry::Triangle2D scale(geometry::Triangle2D const& tri, double factor) {
  return transform(tri, maths::Matrix3::Scale(factor));
}
geometry::Triangle2D scale(geometry::Triangle2D const& tri, double sx, double sy) {
  return transform(tri, maths::Matrix3::Scale(sx, sy));
}
geometry::Triangle2D shear(geometry::Triangle2D const& tri, double shx, double shy) {
  return transform(tri, maths::Matrix3::Shear(shx, shy));
}
geometry::Triangle2D reflect(geometry::Triangle2D const& tri, maths::Vector2 const& normal) {
  return transform(tri, maths::Matrix3::Reflection(normal));
}

geometry::Mesh2D translate(geometry::Mesh2D const& mesh, maths::Vector2 const& offset) {
  return transform(mesh, maths::Matrix3::Translation(offset));
}
geometry::Mesh2D rotate(geometry::Mesh2D const& mesh, double angle_rad) {
  return transform(mesh, maths::Matrix3::Rotation(angle_rad));
}
geometry::Mesh2D scale(geometry::Mesh2D const& mesh, double factor) {
  return transform(mesh, maths::Matrix3::Scale(factor));
}
geometry::Mesh2D scale(geometry::Mesh2D const& mesh, double sx, double sy) {
  return transform(mesh, maths::Matrix3::Scale(sx, sy));
}
geometry::Mesh2D shear(geometry::Mesh2D const& mesh, double shx, double shy) {
  return transform(mesh, maths::Matrix3::Shear(shx, shy));
}
geometry::Mesh2D reflect(geometry::Mesh2D const& mesh, maths::Vector2 const& normal) {
  return transform(mesh, maths::Matrix3::Reflection(normal));
}

geometry::PolyMesh2D translate(geometry::PolyMesh2D const& mesh, maths::Vector2 const& offset) {
  return transform(mesh, maths::Matrix3::Translation(offset));
}
geometry::PolyMesh2D rotate(geometry::PolyMesh2D const& mesh, double angle_rad) {
  return transform(mesh, maths::Matrix3::Rotation(angle_rad));
}
geometry::PolyMesh2D scale(geometry::PolyMesh2D const& mesh, double factor) {
  return transform(mesh, maths::Matrix3::Scale(factor));
}
geometry::PolyMesh2D scale(geometry::PolyMesh2D const& mesh, double sx, double sy) {
  return transform(mesh, maths::Matrix3::Scale(sx, sy));
}
geometry::PolyMesh2D shear(geometry::PolyMesh2D const& mesh, double shx, double shy) {
  return transform(mesh, maths::Matrix3::Shear(shx, shy));
}
geometry::PolyMesh2D reflect(geometry::PolyMesh2D const& mesh, maths::Vector2 const& normal) {
  return transform(mesh, maths::Matrix3::Reflection(normal));
}

geometry::ConnectedMesh2D translate(geometry::ConnectedMesh2D const& mesh, maths::Vector2 const& offset) {
  return transform(mesh, maths::Matrix3::Translation(offset));
}
geometry::ConnectedMesh2D rotate(geometry::ConnectedMesh2D const& mesh, double angle_rad) {
  return transform(mesh, maths::Matrix3::Rotation(angle_rad));
}
geometry::ConnectedMesh2D scale(geometry::ConnectedMesh2D const& mesh, double factor) {
  return transform(mesh, maths::Matrix3::Scale(factor));
}
geometry::ConnectedMesh2D scale(geometry::ConnectedMesh2D const& mesh, double sx, double sy) {
  return transform(mesh, maths::Matrix3::Scale(sx, sy));
}
geometry::ConnectedMesh2D shear(geometry::ConnectedMesh2D const& mesh, double shx, double shy) {
  return transform(mesh, maths::Matrix3::Shear(shx, shy));
}
geometry::ConnectedMesh2D reflect(geometry::ConnectedMesh2D const& mesh, maths::Vector2 const& normal) {
  return transform(mesh, maths::Matrix3::Reflection(normal));
}

}  // namespace geompp::transformations
