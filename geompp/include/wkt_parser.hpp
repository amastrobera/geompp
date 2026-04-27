#pragma once

#include "geometry_collection2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "ray2d.hpp"
#include "triangle2d.hpp"
#include "vector2d.hpp"

#include "geometry_collection3d.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
#include "vector3d.hpp"

#include <concepts>
#include <fstream>
#include <optional>
#include <string>
#include <variant>

namespace geompp {

template <typename T>
concept WktSerializable = requires(const T& obj, const std::string& wkt) {
  { obj.ToWkt() } -> std::convertible_to<std::string>;
  { T::FromWkt(wkt) } -> std::same_as<T>;
};

template <WktSerializable... Ts>
using WktVariant = std::variant<Ts...>;

class WktParser {
 public:
  static WktParser Open(std::string const& fle_path);
  WktParser(WktParser&&) noexcept;
  ~WktParser();

  using ReturnSet = std::optional<WktVariant<
      // 2D objects
      Vector2D, Point2D, Line2D, Ray2D, LineSegment2D, Polyline2D, Triangle2D, Polygon2D, GeometryCollection2D,
      // 3D objects
      Vector3D, Point3D, Line3D, Ray3D, LineSegment3D, Polyline3D, Triangle3D, Polygon3D, GeometryCollection3D>>;
  bool HasNext() const;

  std::string GetFilePath() const;

  static ReturnSet FromWkt(std::string const& wkt);
  static std::string ToWkt(ReturnSet const& shape);

  ReturnSet Next();

 private:
  std::string FILE_NAME;
  std::ifstream FILE;
  bool HAS_NEXT;

  WktParser(std::string const& file_name, std::ifstream&& file_path);
};

}  // namespace geompp
