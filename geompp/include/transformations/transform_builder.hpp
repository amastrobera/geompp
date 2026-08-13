#pragma once

#include "../maths.hpp"

/// @file transform_builder.hpp
/// @brief Fluent composer for a single 4x4 homogeneous affine transform (translation + rotation +
/// scale, in any order/repetition), applied to 3D primitives via transform(primitive, TransformBuilder
/// ::Get()) (transformations3d.hpp) -- or its 2D counterpart via a plain geompp::maths::Matrix3, since
/// a 2D affine transform needs far fewer knobs (no axis to pick for rotation) to make a dedicated
/// builder worth it.
namespace geompp::transformations {

/// @brief Builds a composite Matrix4 by chaining translate()/rotate()/scale()/combine() calls, each
/// PRE-multiplying the new operation onto the accumulated matrix (`new_op * accumulated`) so that
/// operations apply in the order they're called: `builder.translate(t).rotate(r)` moves a point first
/// by `t`, then rotates the result by `r` (not the other way around) -- matches how a reader expects a
/// chain of method calls to read left-to-right as "do this, then this".
class TransformBuilder {
 public:
  TransformBuilder() = default;

  /// @brief Appends a translation by `offset`, applied after every operation already chained.
  TransformBuilder& Translate(maths::Vector3 const& offset);

  /// @brief Appends a rotation by `angle_rad` radians about `axis` (through the origin, Rodrigues'
  /// formula), applied after every operation already chained.
  /// @throws std::invalid_argument if `axis` is zero-length.
  TransformBuilder& Rotate(double angle_rad, maths::Vector3 const& axis);

  /// @brief Appends a uniform scale by `factor` (about the origin), applied after every operation
  /// already chained.
  TransformBuilder& Scale(double factor);

  /// @brief Appends a non-uniform per-axis scale (about the origin), applied after every operation
  /// already chained.
  TransformBuilder& Scale(double sx, double sy, double sz);

  /// @brief Appends a general shear (each axis offset by a multiple of the other two), applied after
  /// every operation already chained.
  TransformBuilder& Shear(double xy, double xz, double yx, double yz, double zx, double zy);

  /// @brief Appends a reflection across the plane through the origin whose normal is `normal`, applied
  /// after every operation already chained.
  /// @throws std::invalid_argument if `normal` is zero-length.
  TransformBuilder& Reflect(maths::Vector3 const& normal);

  /// @brief Appends an arbitrary caller-supplied Matrix4, applied after every operation already
  /// chained -- an escape hatch for a transform this builder has no dedicated method for (perspective,
  /// a matrix loaded from a scene file, ...).
  TransformBuilder& Combine(maths::Matrix4 const& mat);

  /// @brief The composed matrix so far.
  maths::Matrix4 const& Get() const;

  /// @brief Copy of the composed matrix so far -- same value as Get(), but by value for a caller who
  /// wants to keep it independent of this builder's further chaining.
  maths::Matrix4 Build() const;

 private:
  maths::Matrix4 m_matrix = maths::Matrix4::Identity();
};

}  // namespace geompp::transformations
