#include "plane.hpp"

#include "line3d.hpp"
#include "utils.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {

Plane Plane::From3Points(Point3D p1, Point3D p2, Point3D p3) { return FromOriginAndAxes(p1, p2 - p1, p3 - p1); }

Plane Plane::FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v) {
  if (compare(u.Length(), 0) == 0) {
    throw new std::runtime_error("zero length axis u");
  }
  if (compare(v.Length(), 0) == 0) {
    throw new std::runtime_error("zero length axis v");
  }
  return {origin, u, v};
}

Plane Plane::FromOriginAndNormal(Point3D origin, Vector3D normal) {
  if (compare(normal.Length(), 0) == 0) {
    throw new std::runtime_error("zero length normal");
  }
  return {origin, normal};
}

Plane::Plane(Point3D origin, Vector3D normal)
    : Origin(origin), Normal(normal), AxisU(normal.Perp().Normalize()), AxisV(normal.Cross(AxisU).Normalize()) {}

Plane::Plane(Point3D origin, Vector3D u, Vector3D v)
    : Origin(origin), Normal(u.Cross(v).Normalize()), AxisU(u), AxisV(v) {}

bool Plane::AlmostEquals(Plane const& other, double epsilon) const {
  return (
      // same normal (or parallel)
      (Normal.AlmostEquals(other.Normal, epsilon) || Normal.AlmostEquals(-other.Normal, epsilon)) &&
      // same offset from the origin
      (compare(Normal.Dot(Origin.ToVector()), other.Normal.Dot(other.Origin.ToVector()), epsilon) == 0));
}

Plane& Plane::operator=(Plane const& other) {
  if (this != &other) {
    Origin = other.Origin;
    Normal = other.Normal;
    AxisU = other.AxisU;
    AxisV = other.AxisV;
  }
  return *this;
}

#pragma region Geometrical Operations

double Plane::SignedDistanceTo(Point3D const& p) const { return round((p - Origin).Dot(Normal)); }

double Plane::DistanceTo(Point3D const& p) const { return std::abs(SignedDistanceTo(p)); }

Point3D Plane::ProjectOnto(Point3D const& p) const { return p - SignedDistanceTo(p) * Normal; }

Point2D Plane::ProjectInto(Point3D const& p) const {
  auto pproj = ProjectOnto(p);
  double u = (pproj - Origin).Dot(AxisU);
  double v = (pproj - Origin).Dot(AxisV);
  return {u, v};
}

Point3D Plane::Evaluate(Point2D const& p) const {
  // we "project onto" the evaluated one, to avoid complaints on decimal precision ?
  return ProjectOnto(Origin + AxisU * p.x() + AxisV * p.y());
}

bool Plane::Contains(Point3D const& point) const { return compare((point - Origin).Dot(Normal), 0) == 0; }
bool Plane::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }
Plane::ReturnSet Plane::Intersection(Line3D const& line) const {
  auto V = line.Last() - line.First();
  auto W = line.First() - Origin;
  auto denominator = V.Dot(Normal);
  if (compare(denominator, 0) == 0) {
    // parallel or part of the plane
    return std::nullopt;
  }
  double t = -W.Dot(Normal) / denominator;
  // we "project onto" the evaluated one, to avoid complaints on decimal precision ?
  return ProjectOnto(line.First() + t * V);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Plane const& lhs, Plane const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Collection Operations

bool are_coplanar(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() < 4) {
    return true;
  }

  // avoid building a plane and making a constructor
  auto normal = (unique_points[1] - unique_points[0]).Cross(unique_points[2] - unique_points[0]);

  // if normal and Pi-P0 are not orthogonal, then the point is not in the plane defined by P0, P1 and P2
  for (int i = 3; i < unique_points.size(); ++i) {
    if (compare(normal.Dot(unique_points[i] - unique_points[0]), 0) != 0) {
      return false;
    }
  }

  return true;
}

double orientation(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() < 3) {
    return true;
  }

  auto normal = (unique_points[1] - unique_points[0]).Cross(unique_points[2] - unique_points[0]);

  Vector3D signed_area = {0, 0, 0};
  for (int i = 0; i < unique_points.size(); ++i) {
    auto const& p1 = unique_points[i];
    auto const& p2 = unique_points[(i + 1) % unique_points.size()];
    signed_area += p1.ToVector().Cross(p2.ToVector());
  }

  return normal.Dot(signed_area);
}

bool are_ccw(std::vector<Point3D> const& points) { return compare(orientation(points), 0) > 0; }

bool are_cw(std::vector<Point3D> const& points) { return compare(orientation(points), 0) < 0; }

#pragma endregion

}  // namespace geompp
