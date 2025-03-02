#include "plane.hpp"

#include "line3d.hpp"
#include "utils.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {

Plane Plane::From3Points(Point3D p1, Point3D p2, Point3D p3, int decimal_precision) {
  return FromOriginAndAxes(p1, p2 - p1, p3 - p1, decimal_precision);
}

Plane Plane::FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v, int decimal_precision) {
  if (round_to(u.Length(), decimal_precision) == 0) {
    throw new std::runtime_error("zero length axis u");
  }
  if (round_to(v.Length(), decimal_precision) == 0) {
    throw new std::runtime_error("zero length axis v");
  }
  return {origin, u, v};
}

Plane Plane::FromOriginAndNormal(Point3D origin, Vector3D normal, int decimal_precision) {
  if (round_to(normal.Length(), decimal_precision) == 0) {
    throw new std::runtime_error("zero length normal");
  }
  return {origin, normal};
}

Plane::Plane(Point3D origin, Vector3D normal) {
  Origin = origin;
  Normal = normal;
  AxisU = normal.Perp().Normalize();
  AxisV = normal.Cross(AxisU).Normalize();
}

Plane::Plane(Point3D origin, Vector3D u, Vector3D v) {
  Origin = origin;
  AxisU = u;
  AxisV = v;
  Normal = AxisU.Cross(AxisV).Normalize();
}

bool Plane::AlmostEquals(Plane const& other, int decimal_precision) const {
  throw new std::runtime_error("not implemented");
  // return round_to(X - other.X, decimal_precision) == 0.0 && round_to(Y - other.Y, decimal_precision) == 0.0 &&
  // round_to(Z - other.Z, decimal_precision) == 0.0;
}

Plane& Plane::operator=(Plane const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

#pragma region Geometrical Operations

double Plane::SignedDistanceTo(Point3D const& p, int decimal_precision) const {
  return round_to((p - Origin).Dot(Normal), decimal_precision);
}

double Plane::DistanceTo(Point3D const& p, int decimal_precision) const {
  return std::abs(SignedDistanceTo(p, decimal_precision));
}

Point3D Plane::ProjectOnto(Point3D const& p, int decimal_precision) const {
  return p - SignedDistanceTo(p, decimal_precision) * Normal;
}

Point2D Plane::ProjectInto(Point3D const& p, int decimal_precision) const {
  auto pproj = ProjectOnto(p, decimal_precision);
  double u = (pproj - Origin).Dot(AxisU);
  double v = (pproj - Origin).Dot(AxisV);
  return {u, v};
}

Point3D Plane::Evaluate(Point2D const& p, int decimal_precision) const {
  // we "project onto" the evaluated one, to avoid complaints on decimal precision ?
  return ProjectOnto(Origin + AxisU * p.x() + AxisV * p.y(), decimal_precision);
}

bool Plane::Contains(Point3D const& point, int decimal_precision) const {
  return round_to((point - Origin).Dot(Normal), decimal_precision) == 0;
}
bool Plane::Intersects(Line3D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}
Plane::ReturnSet Plane::Intersection(Line3D const& line, int decimal_precision) const {
  auto V = line.Last() - line.First();
  auto W = line.First() - Origin;
  auto denominator = V.Dot(Normal);
  if (round_to(denominator, decimal_precision) == 0) {
    // parallel or part of the plane
    return std::nullopt;
  }
  double t = -W.Dot(Normal) / denominator;
  // we "project onto" the evaluated one, to avoid complaints on decimal precision ?
  return ProjectOnto(line.First() + t * V, decimal_precision);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Plane const& lhs, Plane const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
