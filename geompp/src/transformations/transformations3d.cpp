#include "transformations/transformations3d.hpp"

namespace geompp::transformations {

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

geometry::Point3D shear(geometry::Point3D const& p, double xy, double xz, double yx, double yz, double zx,
                        double zy) {
  return geometry::Point3D(p.x() + xy * p.y() + xz * p.z(), p.y() + yx * p.x() + yz * p.z(),
                           p.z() + zx * p.x() + zy * p.y());
}

geometry::Point3D reflect(geometry::Point3D const& p, maths::Vector3 const& normal) {
  maths::Vector3 n = normal.Normalized();  // throws on zero-length normal
  double d = p.x() * n.x() + p.y() * n.y() + p.z() * n.z();
  return geometry::Point3D(p.x() - 2.0 * d * n.x(), p.y() - 2.0 * d * n.y(), p.z() - 2.0 * d * n.z());
}

geometry::Point3D transform(geometry::Point3D const& p, maths::Matrix4 const& m) {
  maths::Vector4 homogeneous = m * maths::Vector4(p.x(), p.y(), p.z(), 1.0);
  return geometry::Point3D(homogeneous.x(), homogeneous.y(), homogeneous.z());
}

geometry::Vector3D transform(geometry::Vector3D const& v, maths::Matrix4 const& m) {
  maths::Vector4 homogeneous = m * maths::Vector4(v.x(), v.y(), v.z(), 0.0);
  return geometry::Vector3D(homogeneous.x(), homogeneous.y(), homogeneous.z());
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
std::vector<geometry::Point3D> transform_ring(std::vector<geometry::Point3D> const& ring,
                                              maths::Matrix4 const& m) {
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

}  // namespace geompp::transformations
