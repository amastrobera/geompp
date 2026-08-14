#include "transformations/transform_builder2d.hpp"

namespace geompp::transformations {

TransformBuilder2D& TransformBuilder2D::Translate(maths::Vector2 const& offset) {
  return Combine(maths::Matrix3::Translation(offset));
}

TransformBuilder2D& TransformBuilder2D::Rotate(double angle_rad) {
  return Combine(maths::Matrix3::Rotation(angle_rad));
}

TransformBuilder2D& TransformBuilder2D::Scale(double factor) { return Combine(maths::Matrix3::Scale(factor)); }

TransformBuilder2D& TransformBuilder2D::Scale(double sx, double sy) {
  return Combine(maths::Matrix3::Scale(sx, sy));
}

TransformBuilder2D& TransformBuilder2D::Shear(double shx, double shy) {
  return Combine(maths::Matrix3::Shear(shx, shy));
}

TransformBuilder2D& TransformBuilder2D::Reflect(maths::Vector2 const& normal) {
  return Combine(maths::Matrix3::Reflection(normal));  // throws on zero-length normal
}

TransformBuilder2D& TransformBuilder2D::Combine(maths::Matrix3 const& mat) {
  m_matrix = mat * m_matrix;  // pre-multiply so chained ops apply in call order
  return *this;
}

maths::Matrix3 const& TransformBuilder2D::Get() const { return m_matrix; }

maths::Matrix3 TransformBuilder2D::Build() const { return m_matrix; }

}  // namespace geompp::transformations
