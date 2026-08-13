#pragma once

#include "../maths.hpp"

/// @file transform_builder2d.hpp
/// @brief Fluent composer for a single 3x3 homogeneous affine transform (translation + rotation + scale,
/// in any order/repetition), applied to 2D primitives via transform(primitive, TransformBuilder2D::Get())
/// (transformations2d.hpp). 2D counterpart of TransformBuilder3D (transform_builder3d.hpp) -- see its
/// docs for the pre-multiply / call-order composition semantics, identical here. Rotate() takes a plain
/// angle (no axis -- 2D rotation is about the implicit Z), and Shear() takes the 2 terms of a 2D shear
/// rather than 6.
namespace geompp::transformations {

/// @brief 2D counterpart of TransformBuilder3D -- builds a composite Matrix3 by chaining translate()/
/// rotate()/scale()/combine() calls with the same call-order composition semantics (see
/// TransformBuilder3D::Combine()).
class TransformBuilder2D {
 public:
  TransformBuilder2D() = default;

  /// @brief Appends a translation by `offset`, applied after every operation already chained.
  TransformBuilder2D& Translate(maths::Vector2 const& offset);

  /// @brief Appends a rotation by `angle_rad` radians (CCW, right-hand rule) about the origin, applied
  /// after every operation already chained.
  TransformBuilder2D& Rotate(double angle_rad);

  /// @brief Appends a uniform scale by `factor` (about the origin), applied after every operation
  /// already chained.
  TransformBuilder2D& Scale(double factor);

  /// @brief Appends a non-uniform per-axis scale (about the origin), applied after every operation
  /// already chained.
  TransformBuilder2D& Scale(double sx, double sy);

  /// @brief Appends a shear (`shx` shears X by Y, `shy` shears Y by X), applied after every operation
  /// already chained.
  TransformBuilder2D& Shear(double shx, double shy);

  /// @brief Appends a reflection across the line through the origin whose normal is `normal`, applied
  /// after every operation already chained.
  /// @throws std::invalid_argument if `normal` is zero-length.
  TransformBuilder2D& Reflect(maths::Vector2 const& normal);

  /// @brief Appends an arbitrary caller-supplied Matrix3, applied after every operation already
  /// chained -- an escape hatch for a transform this builder has no dedicated method for.
  TransformBuilder2D& Combine(maths::Matrix3 const& mat);

  /// @brief The composed matrix so far.
  maths::Matrix3 const& Get() const;

  /// @brief Copy of the composed matrix so far -- same value as Get(), but by value for a caller who
  /// wants to keep it independent of this builder's further chaining.
  maths::Matrix3 Build() const;

 private:
  maths::Matrix3 m_matrix = maths::Matrix3::Identity();
};

}  // namespace geompp::transformations
