#include "transformations/transformations3d.hpp"

#include <variant>

namespace geompp::transformations {

geometry::Point3D transform(geometry::Point3D const& p, maths::Matrix4 const& m) {
  maths::Vector4 homogeneous = m * maths::Vector4(p.x(), p.y(), p.z(), 1.0);
  return geometry::Point3D(homogeneous.x(), homogeneous.y(), homogeneous.z());
}

geometry::Vector3D transform(geometry::Vector3D const& v, maths::Matrix4 const& m) {
  maths::Vector4 homogeneous = m * maths::Vector4(v.x(), v.y(), v.z(), 0.0);
  return geometry::Vector3D(homogeneous.x(), homogeneous.y(), homogeneous.z());
}

geometry::Ray3D transform(geometry::Ray3D const& ray, maths::Matrix4 const& m) {
  return geometry::Ray3D::Make(transform(ray.Origin(), m), transform(ray.Direction(), m));
}

geometry::Line3D transform(geometry::Line3D const& line, maths::Matrix4 const& m) {
  return geometry::Line3D::Make(transform(line.Origin(), m), transform(line.Direction(), m));
}

geometry::LineSegment3D transform(geometry::LineSegment3D const& seg, maths::Matrix4 const& m) {
  return geometry::LineSegment3D::Make(transform(seg.First(), m), transform(seg.Last(), m));
}

geometry::Polyline3D transform(geometry::Polyline3D const& polyline, maths::Matrix4 const& m) {
  std::vector<geometry::Point3D> points;
  points.reserve(polyline.Size());
  for (int i = 0; i < polyline.Size(); ++i) {
    points.push_back(transform(polyline[i], m));
  }
  return geometry::Polyline3D::Make(points);
}

geometry::Triangle3D transform(geometry::Triangle3D const& tri, maths::Matrix4 const& m) {
  auto [p0, p1, p2] = tri.Vertices();
  return geometry::Triangle3D::Make(transform(p0, m), transform(p1, m), transform(p2, m));
}

namespace {
std::vector<geometry::Point3D> transform_ring(std::vector<geometry::Point3D> const& ring, maths::Matrix4 const& m) {
  std::vector<geometry::Point3D> result;
  result.reserve(ring.size());
  for (auto const& p : ring) {
    result.push_back(transform(p, m));
  }
  return result;
}
}  // namespace

geometry::Polygon3D transform(geometry::Polygon3D const& poly, maths::Matrix4 const& m) {
  if (!poly.HasHoles()) {
    return geometry::Polygon3D::Make(transform_ring(poly.Perimeter(), m));
  }
  std::vector<std::vector<geometry::Point3D>> holes;
  holes.reserve(poly.Holes().size());
  for (auto const& hole : poly.Holes()) {
    holes.push_back(transform_ring(hole, m));
  }
  return geometry::Polygon3D::Make(transform_ring(poly.Perimeter(), m), holes);
}

geometry::Mesh3D transform(geometry::Mesh3D const& mesh, maths::Matrix4 const& m) {
  std::vector<geometry::Triangle3D> triangles;
  triangles.reserve(mesh.Size());
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    triangles.push_back(transform(mesh[i], m));
  }
  return geometry::Mesh3D::FromTriangles(triangles);
}

geometry::PolyMesh3D transform(geometry::PolyMesh3D const& mesh, maths::Matrix4 const& m) {
  std::vector<geometry::Polygon3D> polygons;
  polygons.reserve(mesh.Size());
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    polygons.push_back(transform(mesh[i], m));
  }
  return geometry::PolyMesh3D::FromPolygons(polygons);
}

geometry::ConnectedMesh3D transform(geometry::ConnectedMesh3D const& mesh, maths::Matrix4 const& m) {
  std::vector<geometry::Triangle3D> triangles;
  triangles.reserve(mesh.Size());
  for (auto const& face : mesh.Faces()) {
    triangles.push_back(transform(face.Geometry(), m));
  }
  return geometry::ConnectedMesh3D::FromTriangles(triangles);
}

geometry::GeometryCollection3D transform(geometry::GeometryCollection3D const& collection, maths::Matrix4 const& m) {
  geometry::GeometryCollection3D result;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    std::visit([&](auto const& shape) { result.Add(transform(shape, m)); }, collection.Get(i));
  }
  return result;
}

// -- translate()/rotate()/scale()/shear()/reflect() convenience wrappers --

geometry::Point3D translate(geometry::Point3D const& p, maths::Vector3 const& offset) {
  return geometry::Point3D(p.x() + offset.x(), p.y() + offset.y(), p.z() + offset.z());
}

geometry::Point3D rotate(geometry::Point3D const& p, double angle_rad, maths::Vector3 const& axis) {
  return transform(p, maths::Matrix4::Rotation(angle_rad, axis));
}

geometry::Point3D scale(geometry::Point3D const& p, double factor) { return scale(p, factor, factor, factor); }

geometry::Point3D scale(geometry::Point3D const& p, double sx, double sy, double sz) {
  return geometry::Point3D(p.x() * sx, p.y() * sy, p.z() * sz);
}

geometry::Point3D shear(geometry::Point3D const& p, double xy, double xz, double yx, double yz, double zx, double zy) {
  return geometry::Point3D(p.x() + xy * p.y() + xz * p.z(), p.y() + yx * p.x() + yz * p.z(),
                           p.z() + zx * p.x() + zy * p.y());
}

geometry::Point3D reflect(geometry::Point3D const& p, maths::Vector3 const& normal) {
  maths::Vector3 n = normal.Normalized();  // throws on zero-length normal
  double d = p.x() * n.x() + p.y() * n.y() + p.z() * n.z();
  return geometry::Point3D(p.x() - 2.0 * d * n.x(), p.y() - 2.0 * d * n.y(), p.z() - 2.0 * d * n.z());
}

geometry::Vector3D rotate(geometry::Vector3D const& v, double angle_rad, maths::Vector3 const& axis) {
  return transform(v, maths::Matrix4::Rotation(angle_rad, axis));  // throws on zero-length axis
}

geometry::Vector3D scale(geometry::Vector3D const& v, double factor) { return scale(v, factor, factor, factor); }

geometry::Vector3D scale(geometry::Vector3D const& v, double sx, double sy, double sz) {
  return geometry::Vector3D(v.x() * sx, v.y() * sy, v.z() * sz);
}

geometry::Vector3D shear(geometry::Vector3D const& v, double xy, double xz, double yx, double yz, double zx,
                         double zy) {
  return geometry::Vector3D(v.x() + xy * v.y() + xz * v.z(), v.y() + yx * v.x() + yz * v.z(),
                            v.z() + zx * v.x() + zy * v.y());
}

geometry::Vector3D reflect(geometry::Vector3D const& v, maths::Vector3 const& normal) {
  maths::Vector3 n = normal.Normalized();  // throws on zero-length normal
  double d = v.x() * n.x() + v.y() * n.y() + v.z() * n.z();
  return geometry::Vector3D(v.x() - 2.0 * d * n.x(), v.y() - 2.0 * d * n.y(), v.z() - 2.0 * d * n.z());
}

geometry::Ray3D translate(geometry::Ray3D const& ray, maths::Vector3 const& offset) {
  return transform(ray, maths::Matrix4::Translation(offset));
}
geometry::Ray3D rotate(geometry::Ray3D const& ray, double angle_rad, maths::Vector3 const& axis) {
  return transform(ray, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Ray3D scale(geometry::Ray3D const& ray, double factor) {
  return transform(ray, maths::Matrix4::Scale(factor));
}
geometry::Ray3D scale(geometry::Ray3D const& ray, double sx, double sy, double sz) {
  return transform(ray, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Ray3D shear(geometry::Ray3D const& ray, double xy, double xz, double yx, double yz, double zx, double zy) {
  return transform(ray, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Ray3D reflect(geometry::Ray3D const& ray, maths::Vector3 const& normal) {
  return transform(ray, maths::Matrix4::Reflection(normal));
}

geometry::Line3D translate(geometry::Line3D const& line, maths::Vector3 const& offset) {
  return transform(line, maths::Matrix4::Translation(offset));
}
geometry::Line3D rotate(geometry::Line3D const& line, double angle_rad, maths::Vector3 const& axis) {
  return transform(line, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Line3D scale(geometry::Line3D const& line, double factor) {
  return transform(line, maths::Matrix4::Scale(factor));
}
geometry::Line3D scale(geometry::Line3D const& line, double sx, double sy, double sz) {
  return transform(line, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Line3D shear(geometry::Line3D const& line, double xy, double xz, double yx, double yz, double zx, double zy) {
  return transform(line, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Line3D reflect(geometry::Line3D const& line, maths::Vector3 const& normal) {
  return transform(line, maths::Matrix4::Reflection(normal));
}

geometry::LineSegment3D translate(geometry::LineSegment3D const& seg, maths::Vector3 const& offset) {
  return transform(seg, maths::Matrix4::Translation(offset));
}
geometry::LineSegment3D rotate(geometry::LineSegment3D const& seg, double angle_rad, maths::Vector3 const& axis) {
  return transform(seg, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::LineSegment3D scale(geometry::LineSegment3D const& seg, double factor) {
  return transform(seg, maths::Matrix4::Scale(factor));
}
geometry::LineSegment3D scale(geometry::LineSegment3D const& seg, double sx, double sy, double sz) {
  return transform(seg, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::LineSegment3D shear(geometry::LineSegment3D const& seg, double xy, double xz, double yx, double yz, double zx,
                              double zy) {
  return transform(seg, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::LineSegment3D reflect(geometry::LineSegment3D const& seg, maths::Vector3 const& normal) {
  return transform(seg, maths::Matrix4::Reflection(normal));
}

geometry::GeometryCollection3D translate(geometry::GeometryCollection3D const& collection,
                                         maths::Vector3 const& offset) {
  return transform(collection, maths::Matrix4::Translation(offset));
}
geometry::GeometryCollection3D rotate(geometry::GeometryCollection3D const& collection, double angle_rad,
                                      maths::Vector3 const& axis) {
  return transform(collection, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::GeometryCollection3D scale(geometry::GeometryCollection3D const& collection, double factor) {
  return transform(collection, maths::Matrix4::Scale(factor));
}
geometry::GeometryCollection3D scale(geometry::GeometryCollection3D const& collection, double sx, double sy,
                                     double sz) {
  return transform(collection, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::GeometryCollection3D shear(geometry::GeometryCollection3D const& collection, double xy, double xz, double yx,
                                     double yz, double zx, double zy) {
  return transform(collection, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::GeometryCollection3D reflect(geometry::GeometryCollection3D const& collection, maths::Vector3 const& normal) {
  return transform(collection, maths::Matrix4::Reflection(normal));
}

geometry::Polygon3D translate(geometry::Polygon3D const& poly, maths::Vector3 const& offset) {
  return transform(poly, maths::Matrix4::Translation(offset));
}
geometry::Polygon3D rotate(geometry::Polygon3D const& poly, double angle_rad, maths::Vector3 const& axis) {
  return transform(poly, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Polygon3D scale(geometry::Polygon3D const& poly, double factor) {
  return transform(poly, maths::Matrix4::Scale(factor));
}
geometry::Polygon3D scale(geometry::Polygon3D const& poly, double sx, double sy, double sz) {
  return transform(poly, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Polygon3D shear(geometry::Polygon3D const& poly, double xy, double xz, double yx, double yz, double zx,
                          double zy) {
  return transform(poly, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Polygon3D reflect(geometry::Polygon3D const& poly, maths::Vector3 const& normal) {
  return transform(poly, maths::Matrix4::Reflection(normal));
}

geometry::Polyline3D translate(geometry::Polyline3D const& polyline, maths::Vector3 const& offset) {
  return transform(polyline, maths::Matrix4::Translation(offset));
}
geometry::Polyline3D rotate(geometry::Polyline3D const& polyline, double angle_rad, maths::Vector3 const& axis) {
  return transform(polyline, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Polyline3D scale(geometry::Polyline3D const& polyline, double factor) {
  return transform(polyline, maths::Matrix4::Scale(factor));
}
geometry::Polyline3D scale(geometry::Polyline3D const& polyline, double sx, double sy, double sz) {
  return transform(polyline, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Polyline3D shear(geometry::Polyline3D const& polyline, double xy, double xz, double yx, double yz, double zx,
                           double zy) {
  return transform(polyline, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Polyline3D reflect(geometry::Polyline3D const& polyline, maths::Vector3 const& normal) {
  return transform(polyline, maths::Matrix4::Reflection(normal));
}

geometry::Triangle3D translate(geometry::Triangle3D const& tri, maths::Vector3 const& offset) {
  return transform(tri, maths::Matrix4::Translation(offset));
}
geometry::Triangle3D rotate(geometry::Triangle3D const& tri, double angle_rad, maths::Vector3 const& axis) {
  return transform(tri, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Triangle3D scale(geometry::Triangle3D const& tri, double factor) {
  return transform(tri, maths::Matrix4::Scale(factor));
}
geometry::Triangle3D scale(geometry::Triangle3D const& tri, double sx, double sy, double sz) {
  return transform(tri, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Triangle3D shear(geometry::Triangle3D const& tri, double xy, double xz, double yx, double yz, double zx,
                           double zy) {
  return transform(tri, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Triangle3D reflect(geometry::Triangle3D const& tri, maths::Vector3 const& normal) {
  return transform(tri, maths::Matrix4::Reflection(normal));
}

geometry::Mesh3D translate(geometry::Mesh3D const& mesh, maths::Vector3 const& offset) {
  return transform(mesh, maths::Matrix4::Translation(offset));
}
geometry::Mesh3D rotate(geometry::Mesh3D const& mesh, double angle_rad, maths::Vector3 const& axis) {
  return transform(mesh, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::Mesh3D scale(geometry::Mesh3D const& mesh, double factor) {
  return transform(mesh, maths::Matrix4::Scale(factor));
}
geometry::Mesh3D scale(geometry::Mesh3D const& mesh, double sx, double sy, double sz) {
  return transform(mesh, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::Mesh3D shear(geometry::Mesh3D const& mesh, double xy, double xz, double yx, double yz, double zx, double zy) {
  return transform(mesh, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::Mesh3D reflect(geometry::Mesh3D const& mesh, maths::Vector3 const& normal) {
  return transform(mesh, maths::Matrix4::Reflection(normal));
}

geometry::PolyMesh3D translate(geometry::PolyMesh3D const& mesh, maths::Vector3 const& offset) {
  return transform(mesh, maths::Matrix4::Translation(offset));
}
geometry::PolyMesh3D rotate(geometry::PolyMesh3D const& mesh, double angle_rad, maths::Vector3 const& axis) {
  return transform(mesh, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::PolyMesh3D scale(geometry::PolyMesh3D const& mesh, double factor) {
  return transform(mesh, maths::Matrix4::Scale(factor));
}
geometry::PolyMesh3D scale(geometry::PolyMesh3D const& mesh, double sx, double sy, double sz) {
  return transform(mesh, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::PolyMesh3D shear(geometry::PolyMesh3D const& mesh, double xy, double xz, double yx, double yz, double zx,
                           double zy) {
  return transform(mesh, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::PolyMesh3D reflect(geometry::PolyMesh3D const& mesh, maths::Vector3 const& normal) {
  return transform(mesh, maths::Matrix4::Reflection(normal));
}

geometry::ConnectedMesh3D translate(geometry::ConnectedMesh3D const& mesh, maths::Vector3 const& offset) {
  return transform(mesh, maths::Matrix4::Translation(offset));
}
geometry::ConnectedMesh3D rotate(geometry::ConnectedMesh3D const& mesh, double angle_rad, maths::Vector3 const& axis) {
  return transform(mesh, maths::Matrix4::Rotation(angle_rad, axis));
}
geometry::ConnectedMesh3D scale(geometry::ConnectedMesh3D const& mesh, double factor) {
  return transform(mesh, maths::Matrix4::Scale(factor));
}
geometry::ConnectedMesh3D scale(geometry::ConnectedMesh3D const& mesh, double sx, double sy, double sz) {
  return transform(mesh, maths::Matrix4::Scale(sx, sy, sz));
}
geometry::ConnectedMesh3D shear(geometry::ConnectedMesh3D const& mesh, double xy, double xz, double yx, double yz,
                                double zx, double zy) {
  return transform(mesh, maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}
geometry::ConnectedMesh3D reflect(geometry::ConnectedMesh3D const& mesh, maths::Vector3 const& normal) {
  return transform(mesh, maths::Matrix4::Reflection(normal));
}

}  // namespace geompp::transformations
