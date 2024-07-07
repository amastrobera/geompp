#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Ray2D;
class LineSegment2D;
class Triangle2D;

class Line2D {
 public:
  static Line2D Make(Point2D const& p0, Point2D const& p1, int decimal_precision = DP_THREE);
  static Line2D Make(Point2D const& orig, Vector2D const& dir, int decimal_precision = DP_THREE);
  Line2D(Line2D const&) = default;
  Line2D(Line2D&&) = default;
  ~Line2D() = default;

  inline Point2D const& First() const { return P0; }
  inline Point2D const& Last() const { return P1; }
  inline Point2D const& Origin() const { return P0; }
  inline Vector2D const& Direction() const { return DIR; }
  bool AlmostEquals(Line2D const& other, int decimal_precision = DP_THREE) const;
  double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;
  Point2D ProjectOnto(Point2D const& point, int decimal_precision = DP_THREE) const;
  double Location(Point2D const& point, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Line2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Line2D FromFile(std::string const& path);

  Line2D& operator=(Line2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point2D>>;
  bool Intersects(Line2D const& other, int decimal_precision = DP_THREE) const;
  bool Intersects(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  bool Intersects(Triangle2D const& t, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line2D const& other, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  // TODO make ReturnSet public, and write Intersection(triangle)
#pragma endregion

 private:
  Point2D P0, P1;
  Vector2D DIR;  // unit

  Line2D(Point2D const& p0, Point2D const& p1);
  Line2D(Point2D const& orig, Vector2D const& dir);
};

#pragma region Operator Overloading

bool operator==(Line2D const& lhs, Line2D const& rhs);

std::ostream& operator<<(std::ostream& os, Line2D const& g);

#pragma endregion

}  // namespace geompp