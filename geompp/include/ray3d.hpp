#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line3D;
class LineSegment3D;

class Ray3D {
 public:
  static Ray3D Make(Point3D const& orig, Vector3D const& dir, int decimal_precision = DP_THREE);
  Ray3D(Ray3D const&) = default;
  Ray3D(Ray3D&&) = default;
  ~Ray3D() = default;

  inline Point3D const& Origin() const { return ORIGIN; }
  inline Vector3D const& Direction() const { return DIR; }
  bool AlmostEquals(Ray3D const& other, int decimal_precision = DP_THREE) const;
  bool IsAhead(Point3D const& point, int decimal_precision = DP_THREE) const;
  bool IsBehind(Point3D const& point, int decimal_precision = DP_THREE) const;
  Line3D ToLine(int decimal_precision = DP_THREE) const;
  double DistanceTo(Point3D const& point, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Ray3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Ray3D FromFile(std::string const& path);

  Ray3D& operator=(Ray3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line, int decimal_precision = DP_THREE) const;
  bool Intersects(Ray3D const& other, int decimal_precision = DP_THREE) const;
  bool Intersects(LineSegment3D const& segment, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line3D const& line, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Ray3D const& other, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(LineSegment3D const& segment, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  Point3D ORIGIN;
  Vector3D DIR;  // unit

  Ray3D(Point3D const& orig, Vector3D const& dir);
};

#pragma region Operator Overloading

bool operator==(Ray3D const& lhs, Ray3D const& rhs);

std::ostream& operator<<(std::ostream& os, Ray3D const& g);

#pragma endregion

}  // namespace geompp