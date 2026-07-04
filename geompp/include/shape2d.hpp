#pragma once

#include <variant>

namespace geompp {

class Point2D;
class Line2D;
class Ray2D;
class LineSegment2D;
class Polyline2D;
class Triangle2D;
class Polygon2D;
class GeometryCollection2D;

using Shape2D =
    std::variant<Point2D, Line2D, Ray2D, LineSegment2D, Polyline2D, Triangle2D, Polygon2D, GeometryCollection2D>;

}  // namespace geompp
