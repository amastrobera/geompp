#pragma once

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"

#include <fstream>
#include <optional>
#include <string>
#include <variant>

namespace geompp {

class LVSParser {
 public:
  static LVSParser Open(std::string const& fle_path);
  ~LVSParser();

  using ReturnSet =
      std::optional<std::variant<Point2D, Line2D, Ray2D, LineSegment2D, Point3D, Line3D, Ray3D, LineSegment3D>>;
  bool HasNext() const;
  ReturnSet Next();

 private:
  std::ifstream FILE;
  bool HAS_NEXT;

  LVSParser(std::ifstream&& file_path);
};

}  // namespace geompp
