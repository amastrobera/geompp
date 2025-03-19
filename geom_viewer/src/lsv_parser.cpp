#include "lsv_parser.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"

#include "line_segment3d.hpp"
#include "point3d.hpp"

#include <filesystem>
#include <iostream>  // TODO: replace with log library

namespace fs = std::filesystem;

namespace geom_viewer {

LVSParser LVSParser::Open(std::string const& file_path) {
  if (!fs::exists(file_path)) {
    throw std::runtime_error("file does not exist " + file_path);
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("could not open the file " + file_path);
  }

  return LVSParser(std::move(file));
}

LVSParser::LVSParser(std::ifstream&& file) : FILE(std::move(file)), HAS_NEXT(true) {}

LVSParser::~LVSParser() { FILE.close(); }

namespace {

enum class Dimention { UNKNOWN = -1, TWO, THREE };

Dimention GetDimentionPoint(std::string wkt) {
  std::size_t end_gtype, end_p1, end_p2;

  std::size_t end_gtype, end_nums;

  end_gtype = wkt.find('(');
  if (end_gtype == std::string::npos) {
    throw std::runtime_error("brakets");
  }

  end_nums = wkt.substr(end_gtype + 1).find(')');
  if (end_nums == std::string::npos) {
    throw std::runtime_error("brakets");
  }
  std::string s_nums = wkt.substr(end_gtype + 1, end_nums);

  auto nums = geompp::tokenize_to_doubles(s_nums);

  switch (nums.size()) {
    case 2:
      return Dimention::TWO;
    case 3:
      return Dimention::THREE;
  }

  throw std::runtime_error("bad number format, not 2D or 3D");
}

Dimention GetDimentionLineString(std::string wkt) {
  std::size_t end_gtype, end_p1, end_p2;

  end_gtype = wkt.find('(');
  if (end_gtype == std::string::npos) {
    throw std::runtime_error("brakets");
  }

  end_p1 = wkt.substr(end_gtype + 1).find(',');
  if (end_p1 == std::string::npos) {
    throw std::runtime_error("brakets");
  }
  std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1);

  auto nums_p1 = geompp::tokenize_to_doubles(s_nums_p1);

  switch (nums_p1.size()) {
    case 2:
      return Dimention::TWO;
    case 3:
      return Dimention::THREE;
  }

  throw std::runtime_error("bad number format, not 2D or 3D");
}

}  // namespace

LVSParser::ReturnSet LVSParser::Next() {
  std::string line, clean_line;
  while (std::getline(FILE, line)) {
    try {
      clean_line = g::trim(line);

      if (clean_line.find("#", 0) == 0) {  // skip comments
        continue;
      }

      if (clean_line.find("LINESTRING", 0) == 0) {
        if (GetDimentionLineString(clean_line) == Dimention::TWO) {
          return g::LineSegment2D::FromWkt(clean_line);
        }
        return g::LineSegment3D::FromWkt(clean_line);
      }

      if (clean_line.find("LINE", 0) == 0) {
        return g::Line2D::FromWkt(clean_line);
      }

      if (clean_line.find("RAY", 0) == 0) {
        return g::Ray2D::FromWkt(clean_line);
      }

      if (clean_line.find("POINT", 0) == 0) {
        if (GetDimentionPoint(clean_line) == Dimention::TWO) {
          return g::Point2D::FromWkt(clean_line);
        }
        return g::Point3D::FromWkt(clean_line);
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
