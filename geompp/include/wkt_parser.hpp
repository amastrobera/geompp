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
  /// @brief Opens a WKT file for streaming reads.
  /// @param fle_path Filesystem path to the WKT file.
  /// @return A WktParser positioned at the start of the file.
  /// @throws std::runtime_error if the file cannot be opened.
  static WktParser Open(std::string const& fle_path);

  WktParser(WktParser&&) noexcept;
  ~WktParser();

  using ReturnSet = std::optional<WktVariant<
      // 2D objects
      Vector2D, Point2D, Line2D, Ray2D, LineSegment2D, Polyline2D, Triangle2D, Polygon2D, GeometryCollection2D,
      // 3D objects
      Vector3D, Point3D, Line3D, Ray3D, LineSegment3D, Polyline3D, Triangle3D, Polygon3D, GeometryCollection3D>>;

  /// @brief Tests whether the file has at least one more geometry to read.
  /// @return true if a subsequent call to @ref Next will return a value.
  bool HasNext() const;

  /// @brief Path of the file currently being parsed.
  /// @return The file path passed to @ref Open.
  std::string GetFilePath() const;

  /// @brief Parses a single WKT string into one of the supported geometry types.
  /// @param wkt Well-Known Text string for any 2D or 3D primitive (POINT, LINE, RAY, LINESTRING, POLYGON, ...).
  /// @return The parsed geometry wrapped in the variant, or std::nullopt if the WKT could not be parsed.
  static ReturnSet FromWkt(std::string const& wkt);

  /// @brief Serializes any of the supported geometry variants back to WKT.
  /// @param shape The geometry to serialize. std::nullopt produces an empty string.
  /// @return WKT representation at the current decimal precision.
  static std::string ToWkt(ReturnSet const& shape);

  /// @brief Reads the next geometry from the open file.
  /// @return The next geometry as a variant, or std::nullopt on end-of-file or parse failure.
  ReturnSet Next();

 private:
  std::string FILE_NAME;
  std::ifstream FILE;
  bool HAS_NEXT;

  WktParser(std::string const& file_name, std::ifstream&& file_path);
};

}  // namespace geompp
