#include "plane.hpp"

#include "utils.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {


Plane Plane::From3Points(Point3D p1, Point3D p2, Point3D p3) {
    throw new std::runtime_error("not implemented");
}

Plane Plane::FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v){
    throw new std::runtime_error("not implemented");
}

Plane Plane::FromOriginAndNormal(Point3D origin, Vector3D normal) {
    throw new std::runtime_error("not implemented");
}

Plane::Plane(Point3D origin, Vector3D normal) {
    Origin = origin;
    Normal = normal;
    AxisU = Normal.Perp();
    AxisV = Normal.Cross(AxisU);
}

Plane::Plane(Point3D origin, Vector3D u, Vector3D v) {
    Origin = origin;
    AxisU = u;
    AxisV = v;
    Normal = AxisU.Cross(AxisV);
}


bool Plane::AlmostEquals(Plane const& other, int decimal_precision) const {
    throw new std::runtime_error("not implemented");
  //return round_to(X - other.X, decimal_precision) == 0.0 && round_to(Y - other.Y, decimal_precision) == 0.0 && round_to(Z - other.Z, decimal_precision) == 0.0;
}

Plane& Plane::operator=(Plane const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

#pragma region Geometrical Operations

double Plane::DistanceTo(Point3D const& other, int decimal_precision) const {
    throw new std::runtime_error("not implemented");
  //return round_to((other - *this).Length(), decimal_precision);
}

Point3D Plane::ProjectOnto(Point3D const& p) const {
    throw new std::runtime_error("not implemented");
}

Point2D Plane::ProjectInto(Point3D const& p) const {
    throw new std::runtime_error("not implemented");
}

Point3D Plane::Evaluate(Point2D const& p) const {
    throw new std::runtime_error("not implemented");
}

#pragma endregion


#pragma region Operator Overloading

bool operator==(Plane const& lhs, Plane const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion



}  // namespace geompp
