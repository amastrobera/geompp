#include "transformations/transform_builder3d.hpp"

namespace geompp::transformations {

TransformBuilder3D& TransformBuilder3D::Translate(maths::Vector3 const& offset) {
  return Combine(maths::Matrix4::Translation(offset));
}

TransformBuilder3D& TransformBuilder3D::Rotate(double angle_rad, maths::Vector3 const& axis) {
  return Combine(maths::Matrix4::Rotation(angle_rad, axis));  // throws on zero-length axis
}

TransformBuilder3D& TransformBuilder3D::Scale(double factor) { return Combine(maths::Matrix4::Scale(factor)); }

TransformBuilder3D& TransformBuilder3D::Scale(double sx, double sy, double sz) {
  return Combine(maths::Matrix4::Scale(sx, sy, sz));
}

TransformBuilder3D& TransformBuilder3D::Shear(double xy, double xz, double yx, double yz, double zx, double zy) {
  return Combine(maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}

TransformBuilder3D& TransformBuilder3D::Reflect(maths::Vector3 const& normal) {
  return Combine(maths::Matrix4::Reflection(normal));  // throws on zero-length normal
}

TransformBuilder3D& TransformBuilder3D::Combine(maths::Matrix4 const& mat) {
  m_matrix = mat * m_matrix;  // pre-multiply so chained ops apply in call order
  return *this;
}

maths::Matrix4 const& TransformBuilder3D::Get() const { return m_matrix; }

maths::Matrix4 TransformBuilder3D::Build() const { return m_matrix; }

}  // namespace geompp::transformations
