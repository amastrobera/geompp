#include "transformations/transform_builder.hpp"

namespace geompp::transformations {

TransformBuilder& TransformBuilder::Translate(maths::Vector3 const& offset) {
  return Combine(maths::Matrix4::Translation(offset));
}

TransformBuilder& TransformBuilder::Rotate(double angle_rad, maths::Vector3 const& axis) {
  return Combine(maths::Matrix4::Rotation(angle_rad, axis));  // throws on zero-length axis
}

TransformBuilder& TransformBuilder::Scale(double factor) { return Combine(maths::Matrix4::Scale(factor)); }

TransformBuilder& TransformBuilder::Scale(double sx, double sy, double sz) {
  return Combine(maths::Matrix4::Scale(sx, sy, sz));
}

TransformBuilder& TransformBuilder::Shear(double xy, double xz, double yx, double yz, double zx, double zy) {
  return Combine(maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy));
}

TransformBuilder& TransformBuilder::Reflect(maths::Vector3 const& normal) {
  return Combine(maths::Matrix4::Reflection(normal));  // throws on zero-length normal
}

TransformBuilder& TransformBuilder::Combine(maths::Matrix4 const& mat) {
  m_matrix = mat * m_matrix;  // pre-multiply so chained ops apply in call order
  return *this;
}

maths::Matrix4 const& TransformBuilder::Get() const { return m_matrix; }

maths::Matrix4 TransformBuilder::Build() const { return m_matrix; }

}  // namespace geompp::transformations
