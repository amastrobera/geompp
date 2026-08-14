#pragma once

#include "utils.hpp"

#include <ostream>
#include <string>

namespace geompp {

inline namespace geometry {

class Point3D;

enum class Axis { X, Y, Z };

class Vector3D {
 public:
  Vector3D(double x, double y, double z);
  Vector3D(Vector3D const&) = default;
  Vector3D(Vector3D&&) = default;
  ~Vector3D() = default;

  double x() const;
  double y() const;
  double z() const;

  Point3D ToPoint() const;
  double Length() const;
  Axis DominantAxis() const;

  bool AlmostEquals(Vector3D const& other, double epsilon = DOUBLE_EPSILON) const;
  std::string ToWkt() const;
  static Vector3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Vector3D FromFile(std::string const& path);

  Vector3D& operator=(Vector3D const& other);

  double Dot(Vector3D const& other) const;
  Vector3D Cross(Vector3D const& other) const;
  Vector3D Perp() const;
  Vector3D Normalize() const;
  bool IsParallel(Vector3D const& other) const;

  Vector3D operator-() const;

  static Vector3D BasisX();
  static Vector3D BasisY();
  static Vector3D BasisZ();

 private:
  double X, Y, Z;
};

#pragma region Operator Overloading

bool operator==(Vector3D const& lhs, Vector3D const& rhs);

Point3D operator+(Vector3D const& lhs, Point3D const& point);
Vector3D operator+(Vector3D const& lhs, Vector3D const& vec);
Vector3D operator+=(Vector3D& lhs, Vector3D const& vec);

Vector3D operator-(Vector3D const& lhs, Vector3D const& vec);

Vector3D operator*(Vector3D const& lhs, double a);
Vector3D operator*(double a, Vector3D const& rhs);
double operator*(Vector3D const& lhs, Vector3D const& vec);

Vector3D operator/(Vector3D const& lhs, Vector3D const& vec) = delete;
Vector3D operator/(Vector3D const& lhs, double a);

std::ostream& operator<<(std::ostream& os, Vector3D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Vector3D::Vector3D(double x, double y, double z) : X(x), Y(y), Z(z) {}
inline double Vector3D::x() const { return X; }
inline double Vector3D::y() const { return Y; }
inline double Vector3D::z() const { return Z; }
inline Vector3D Vector3D::BasisX() { return Vector3D(1, 0, 0); }
inline Vector3D Vector3D::BasisY() { return Vector3D(0, 1, 0); }
inline Vector3D Vector3D::BasisZ() { return Vector3D(0, 0, 1); }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
