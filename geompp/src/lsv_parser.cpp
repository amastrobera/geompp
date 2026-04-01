#include "lsv_parser.hpp"

#include "geompp_log.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace geompp {

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

  std::size_t end_nums;

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

  end_p1 = wkt.substr(end_gtype).find(',');
  if (end_p1 == std::string::npos) {
    throw std::runtime_error("brakets");
  }
  std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1 - 1);

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

bool LVSParser::HasNext() const { return FILE.eof(); }

LVSParser::ReturnSet LVSParser::Next() {
  std::string line, clean_line;
  while (std::getline(FILE, line)) {
    try {
      clean_line = trim(line);

      if (clean_line.find("#", 0) == 0) {  // skip comments
        continue;
      }

      if (clean_line.find("LINESTRING", 0) == 0) {
        if (GetDimentionLineString(clean_line) == Dimention::TWO) {
          return LineSegment2D::FromWkt(clean_line);
        }
        return LineSegment3D::FromWkt(clean_line);
      }

      if (clean_line.find("LINE", 0) == 0) {
        return Line2D::FromWkt(clean_line);
      }

      if (clean_line.find("RAY", 0) == 0) {
        return Ray2D::FromWkt(clean_line);
      }

      if (clean_line.find("POINT", 0) == 0) {
        if (GetDimentionPoint(clean_line) == Dimention::TWO) {
          return Point2D::FromWkt(clean_line);
        }
        return Point3D::FromWkt(clean_line);
      }

      throw std::runtime_error("unknown type");

    } catch (...) {
      GEOMPP_LOG(ERROR) << "unsupported geometry " << clean_line;
      return std::nullopt;
    }
  }
  return std::nullopt;
}

}  // namespace geompp
