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
  static Ray3D Make(Point3D const& orig, Vector3D const& dir);
  Ray3D(Ray3D const&) = default;
  Ray3D(Ray3D&&) = default;
  ~Ray3D() = default;

  inline Point3D const& Origin() const { return ORIGIN; }
  inline Vector3D const& Direction() const { return DIR; }
  bool AlmostEquals(Ray3D const& other) const;
  bool IsAhead(Point3D const& point) const;
  bool IsBehind(Point3D const& point) const;
  Line3D ToLine() const;
  double DistanceTo(Point3D const& point) const;

  std::string ToWkt() const;
  static Ray3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Ray3D FromFile(std::string const& path);

  Ray3D& operator=(Ray3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& other) const;
  bool Intersects(LineSegment3D const& segment) const;
  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& other) const;
  ReturnSet Intersection(LineSegment3D const& segment) const;
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
