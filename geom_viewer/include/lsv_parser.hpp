#pragma once

#include "line_segment2d.hpp"

#include <fstream>
#include <optional>
#include <string>
#include <variant>

namespace g = geompp;

namespace geom_viewer {

class LVSParser {
 public:
  static LVSParser Open(std::string const& fle_path);
  ~LVSParser();

  using ReturnSet = std::optional<std::variant<g::Point2D, g::Line2D, g::Ray2D, g::LineSegment2D>>;
  bool inline HasNext() const { return HAS_NEXT; }
  ReturnSet Next();

 private:
  std::ifstream FILE;
  bool HAS_NEXT;

  LVSParser(std::ifstream&& file_path);
};

}  // namespace geom_viewer