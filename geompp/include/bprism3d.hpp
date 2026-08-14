#pragma once

#include "calc_utils2d.hpp"
#include "calc_utils3d.hpp"
#include "generic_concepts.hpp"
#include "constants.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include <array>
#include <cmath>
#include <limits>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class BPrism3D {
 public:
  /// @brief Builds the minimum oriented bounding prism using PCA for the best-fit plane,
  /// then rotating calipers for the minimum bounding rectangle in that plane.
  /// The W axis is the plane normal; HALF_LEN_W comes from projecting all points onto it.
  /// Accepts any random-access sized range of Point3D-compatible elements.
  template <PointContainer Points>
  BPrism3D(Points const& points);

  BPrism3D(BPrism3D const&) = default;
  BPrism3D(BPrism3D&&) = default;
  ~BPrism3D() = default;

  Point3D center() const;
  Vector3D axis_u() const;
  Vector3D axis_v() const;
  Vector3D axis_w() const;
  double half_len_u() const;
  double half_len_v() const;
  double half_len_w() const;
  double width() const;   // 2 * half_len_u
  double height() const;  // 2 * half_len_v
  double depth() const;   // 2 * half_len_w
  double volume() const;  // 8 * half_len_u * half_len_v * half_len_w
  std::array<Point3D, 8> Corners() const;

  bool AlmostEquals(BPrism3D const& other, double epsilon = DOUBLE_EPSILON) const;
  BPrism3D& operator=(BPrism3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this oriented bounding prism (inclusive of the boundary).
  /// Projects the point onto the prism's local axes; returns true when all three projections are
  /// within [-half_len, half_len] along each axis.
  bool Contains(Point3D const& p) const;

#pragma endregion

 private:
  Point3D CENTER{0.0, 0.0, 0.0};
  Vector3D AXIS_U{1.0, 0.0, 0.0};
  Vector3D AXIS_V{0.0, 1.0, 0.0};
  Vector3D AXIS_W{0.0, 0.0, 1.0};
  double HALF_LEN_U{0.0};
  double HALF_LEN_V{0.0};
  double HALF_LEN_W{0.0};
};

#pragma region Operators Overloading

bool operator==(BPrism3D const& lhs, BPrism3D const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point3D BPrism3D::center() const { return CENTER; }
inline Vector3D BPrism3D::axis_u() const { return AXIS_U; }
inline Vector3D BPrism3D::axis_v() const { return AXIS_V; }
inline Vector3D BPrism3D::axis_w() const { return AXIS_W; }
inline double BPrism3D::half_len_u() const { return HALF_LEN_U; }
inline double BPrism3D::half_len_v() const { return HALF_LEN_V; }
inline double BPrism3D::half_len_w() const { return HALF_LEN_W; }
inline double BPrism3D::width() const { return 2.0 * HALF_LEN_U; }
inline double BPrism3D::height() const { return 2.0 * HALF_LEN_V; }
inline double BPrism3D::depth() const { return 2.0 * HALF_LEN_W; }
inline double BPrism3D::volume() const { return 8.0 * HALF_LEN_U * HALF_LEN_V * HALF_LEN_W; }

#pragma endregion

extern template BPrism3D::BPrism3D(std::vector<Point3D> const&);

}  // namespace geometry

}  // namespace geompp
