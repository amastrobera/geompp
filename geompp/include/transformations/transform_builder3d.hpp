#pragma once

#include "../maths.hpp"
#include "transformations3d.hpp"

/// @file transform_builder3d.hpp
/// @brief Fluent composer for a single 4x4 homogeneous affine transform (translation + rotation + scale,
/// in any order/repetition), applied to 3D primitives via transform(primitive, TransformBuilder3D::Get())
/// (transformations3d.hpp), or in one step via TransformBuilder3D::Apply(). See transform_builder2d.hpp
/// for the Matrix3-backed 2D counterpart -- both
/// compose the same way: each chained call PRE-multiplies the new operation onto the accumulated matrix
/// (`new_op * accumulated`) so operations apply in the order they're called, matching how a reader
/// expects a chain of method calls to read left-to-right as "do this, then this" -- see Combine()'s doc
/// for why that's the opposite of raw matrix-multiplication order.
namespace geompp::transformations {

/// @brief Builds a composite Matrix4 by chaining translate()/rotate()/scale()/combine() calls, each
/// PRE-multiplying the new operation onto the accumulated matrix (`new_op * accumulated`) so that
/// operations apply in the order they're called: `builder.translate(t).rotate(r)` moves a point first
/// by `t`, then rotates the result by `r` (not the other way around) -- matches how a reader expects a
/// chain of method calls to read left-to-right as "do this, then this".
class TransformBuilder3D {
 public:
  TransformBuilder3D() = default;

  /// @brief Appends a translation by `offset`, applied after every operation already chained.
  TransformBuilder3D& Translate(maths::Vector3 const& offset);

  /// @brief Appends a rotation by `angle_rad` radians about `axis` (through the origin, Rodrigues'
  /// formula), applied after every operation already chained.
  /// @throws std::invalid_argument if `axis` is zero-length.
  TransformBuilder3D& Rotate(double angle_rad, maths::Vector3 const& axis);

  /// @brief Appends a uniform scale by `factor` (about the origin), applied after every operation
  /// already chained.
  TransformBuilder3D& Scale(double factor);

  /// @brief Appends a non-uniform per-axis scale (about the origin), applied after every operation
  /// already chained.
  TransformBuilder3D& Scale(double sx, double sy, double sz);

  /// @brief Appends a general shear (each axis offset by a multiple of the other two), applied after
  /// every operation already chained.
  TransformBuilder3D& Shear(double xy, double xz, double yx, double yz, double zx, double zy);

  /// @brief Appends a reflection across the plane through the origin whose normal is `normal`, applied
  /// after every operation already chained.
  /// @throws std::invalid_argument if `normal` is zero-length.
  TransformBuilder3D& Reflect(maths::Vector3 const& normal);

  /// @brief Appends an arbitrary caller-supplied Matrix4, applied after every operation already
  /// chained -- an escape hatch for a transform this builder has no dedicated method for (perspective,
  /// a matrix loaded from a scene file, ...).
  TransformBuilder3D& Combine(maths::Matrix4 const& mat);

  /// @brief The composed matrix so far.
  maths::Matrix4 const& Get() const;

  /// @brief Copy of the composed matrix so far -- same value as Get(), but by value for a caller who
  /// wants to keep it independent of this builder's further chaining.
  maths::Matrix4 Build() const;

  /// @brief Applies the composed matrix to `shape` and returns the transformed copy -- shorthand for
  /// transform(shape, builder.Get()), for any 3D primitive transform() has an overload for (Point3D,
  /// Polygon3D, Mesh3D, ...). Doesn't consume or store `shape` -- the builder keeps composing normally
  /// afterward, so the same chain can Apply() to several different shapes.
  template <typename T>
  T Apply(T const& shape) const {
    return transform(shape, m_matrix);
  }

 private:
  maths::Matrix4 m_matrix = maths::Matrix4::Identity();
};

}  // namespace geompp::transformations
