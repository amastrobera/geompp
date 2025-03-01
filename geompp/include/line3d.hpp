#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

//class Ray3D;
//class LineSegment3D;
class Triangle3D;

class Line3D {
 public:
  static Line3D Make(Point3D const& p0, Point3D const& p1, int decimal_precision = DP_THREE);
  static Line3D Make(Point3D const& orig, Vector3D const& dir, int decimal_precision = DP_THREE);
  Line3D(Line3D const&) = default;
  Line3D(Line3D&&) = default;
  ~Line3D() = default;

  inline Point3D const& First() const { return P0; }
  inline Point3D const& Last() const { return P1; }
  inline Point3D const& Origin() const { return P0; }
  inline Vector3D const& Direction() const { return DIR; }
  bool AlmostEquals(Line3D const& other, int decimal_precision = DP_THREE) const;
  double DistanceTo(Point3D const& point, int decimal_precision = DP_THREE) const;
  Point3D ProjectOnto(Point3D const& point, int decimal_precision = DP_THREE) const;
  double Location(Point3D const& point, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Line3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Line3D FromFile(std::string const& path);

  Line3D& operator=(Line3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& other, int decimal_precision = DP_THREE) const;
  //bool Intersects(Ray3D const& ray, int decimal_precision = DP_THREE) const;
  //bool Intersects(LineSegment3D const& segment, int decimal_precision = DP_THREE) const;
  bool Intersects(Triangle3D const& t, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line3D const& other, int decimal_precision = DP_THREE) const;
  //ReturnSet Intersection(Ray3D const& ray, int decimal_precision = DP_THREE) const;
  //ReturnSet Intersection(LineSegment3D const& segment, int decimal_precision = DP_THREE) const;
  // TODO make ReturnSet public, and write Intersection(triangle)
#pragma endregion

 private:
  Point3D P0, P1;
  Vector3D DIR;  // unit

  Line3D(Point3D const& p0, Point3D const& p1);
  Line3D(Point3D const& orig, Vector3D const& dir);
};

#pragma region Operator Overloading

bool operator==(Line3D const& lhs, Line3D const& rhs);

std::ostream& operator<<(std::ostream& os, Line3D const& g);

#pragma endregion

}  // namespace geompp