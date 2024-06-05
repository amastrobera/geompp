#include "lsv_parser.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"

#include <filesystem>
#include <iostream>  // TODO: replace with log library

namespace fs = std::filesystem;

namespace geom_viewer {

LVSParser LVSParser::Open(std::string const& file_path) {
  if (!fs::exists(file_path)) {
    throw std::runtime_error("files does not exist " + file_path);
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("could not open the file " + file_path);
  }

  return LVSParser(std::move(file));
}

LVSParser::LVSParser(std::ifstream&& file) : FILE(std::move(file)), HAS_NEXT(true) {}

LVSParser::~LVSParser() { FILE.close(); }

LVSParser::ReturnSet LVSParser::Next() {
  std::string line, clean_line;
  while (std::getline(FILE, line)) {
    try {
      clean_line = g::trim(line);

      if (clean_line.find("#", 0) == 0) {  // skip comments
        continue;
      }

      if (clean_line.find("LINESTRING", 0) == 0) {
        return g::LineSegment2D::FromWkt(clean_line);
      }

      if (clean_line.find("LINE", 0) == 0) {
        return g::Line2D::FromWkt(clean_line);
      }

      if (clean_line.find("RAY", 0) == 0) {
        return g::Ray2D::FromWkt(clean_line);
      }

      if (clean_line.find("POINT", 0) == 0) {
        return g::Point2D::FromWkt(clean_line);
      }

      throw std::runtime_error("unknown type");

    } catch (...) {
      std::cerr << "unsupported geometry " << clean_line << std::endl;
      return std::nullopt;
    }
  }

  HAS_NEXT = false;
  return std::nullopt;
}

}  // namespace geom_viewer