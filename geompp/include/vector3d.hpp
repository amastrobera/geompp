#pragma once

#include "utils.hpp"

#include <ostream>
#include <string>

namespace geompp {

class Point3D;

class Vector3D {
 public:
  Vector3D(double x = 0.0, double y = 0.0, double z = 0.0);
  Vector3D(Vector3D const&) = default;
  Vector3D(Vector3D&&) = default;
  ~Vector3D() = default;

  double inline const x() const { return X; }
  double inline const y() const { return Y; }
  double inline const z() const { return Z; }

  Point3D ToPoint();

  double Length() const;
  bool AlmostEquals(Vector3D const& other, int decimal_precision = DP_THREE) const;
  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Vector3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Vector3D FromFile(std::string const& path);

  Vector3D& operator=(Vector3D const& other);

  double Dot(Vector3D const& v) const;
  Vector3D Cross(Vector3D const& v) const;
  Vector3D Perp() const;
  Vector3D Normalize() const;

  Vector3D operator-();

  static inline Vector3D BasisX() { return Vector3D(1, 0); }
  static inline Vector3D BasisY() { return Vector3D(0, 1); }

 private:
  double X, Y, Z;
};

#pragma region Operator Overloading

bool operator==(Vector3D const& lhs, Vector3D const& rhs);

Point3D operator+(Vector3D const& lhs, Point3D const& point);
Vector3D operator+(Vector3D const& lhs, Vector3D const& vec);

Vector3D operator-(Vector3D const& lhs, Vector3D const& vec);

Vector3D operator*(Vector3D const& lhs, double a);
Vector3D operator*(double a, Vector3D const& rhs);
double operator*(Vector3D const& lhs, Vector3D const& vec);

Vector3D operator/(Vector3D const& lhs, Vector3D const& vec) = delete;
Vector3D operator/(Vector3D const& lhs, double a);

std::ostream& operator<<(std::ostream& os, Vector3D const& g);

#pragma endregion

}  // namespace geompp