#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line2D;
class Ray2D;
class LineSegment2D;

namespace {
int default_prec() {
  static int d = DP_THREE;
  return 0;
}
}  // namespace

class Polyline2D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline2D Make(std::vector<Point2D> const& points, int decimal_precision = DP_THREE);
  Polyline2D(Polyline2D const&) = default;
  Polyline2D(Polyline2D&&) = default;
  ~Polyline2D() = default;

  inline int Size() const { return KNOTS.size(); }
  // TODO: it would be nice to have a "generator" with coroutines that "yields" point by point

  bool AlmostEquals(Polyline2D const& other, int decimal_precision = DP_THREE) const;
  std::vector<LineSegment2D> ToSegments() const;
  double Length() const;
  double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;
  // double Location(Point2D const& point, int decimal_precision = DP_THREE) const;
  // Point2D Interpolate(double pct) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Polyline2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Polyline2D FromFile(std::string const& path);

  Polyline2D& operator=(Polyline2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  // using MultiPoint = std::vector<Point2D>;
  // using ReturnSet = std::optional<std::variant<Point2D, MultiPoint>>;
  // bool Intersects(Line2D const& line, int decimal_precision = DP_THREE) const;
  // bool Intersects(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  // bool Intersects(Polyline2D const& segment, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Line2D const& line, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(LineSegment2D const& other, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Polyline2D const& line, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  std::vector<Point2D> KNOTS;

  Polyline2D(std::vector<Point2D>&& points);
};

#pragma region Operator Overloading

bool operator==(Polyline2D const& lhs, Polyline2D const& rhs);

#pragma endregion

}  // namespace geompp