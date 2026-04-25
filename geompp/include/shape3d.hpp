#pragma once

#include "constants.hpp"

#include <variant>

namespace geompp {

class Point3D;
class Line3D;
class Ray3D;
class LineSegment3D;
class Polyline3D;
class Triangle3D;
class Polygon3D;
class GeometryCollection3D;

using Shape3D =
    std::variant<Point3D, Line3D, Ray3D, LineSegment3D, Polyline3D, Triangle3D, Polygon3D, GeometryCollection3D>;

}  // namespace geompp
