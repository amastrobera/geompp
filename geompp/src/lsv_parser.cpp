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

  return LVSParser(file_path, std::move(file));
}

LVSParser::LVSParser(std::string const& file_name, std::ifstream&& file)
    : FILE_NAME(file_name), FILE(std::move(file)), HAS_NEXT(true) {}

LVSParser::LVSParser(LVSParser&& other) noexcept
    : FILE_NAME(std::move(other.FILE_NAME)),
      FILE(std::move(other.FILE)),
      HAS_NEXT(other.HAS_NEXT) {}

LVSParser::~LVSParser() { FILE.close(); }

namespace {

enum class Dimention { UNKNOWN = -1, TWO, THREE };

Dimention GetDimentionSingleElement(std::string wkt) {
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

Dimention GetDimentionMultiElementCommaSeparated(std::string wkt) {
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

bool LVSParser::HasNext() const { return !FILE.eof(); }

LVSParser::ReturnSet LVSParser::Next() {
  std::string line, clean_line;
  while (std::getline(FILE, line)) {
    try {
      clean_line = trim(line);

      if (clean_line.find("#", 0) == 0) {  // skip comments
        continue;
      }

      if (clean_line.find("LINESTRING", 0) == 0) {
        switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
          case Dimention::TWO:
            return LineSegment2D::FromWkt(clean_line);
          case Dimention::THREE:
            return LineSegment3D::FromWkt(clean_line);
        }
        throw std::runtime_error("bad number format, not 2D or 3D");
      }

      if (clean_line.find("LINE", 0) == 0) {
        switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
          case Dimention::TWO:
            return Line2D::FromWkt(clean_line);
          case Dimention::THREE:
            return Line3D::FromWkt(clean_line);
        }
        throw std::runtime_error("bad number format, not 2D or 3D");
      }

      if (clean_line.find("RAY", 0) == 0) {
        switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
          case Dimention::TWO:
            return Ray2D::FromWkt(clean_line);
          case Dimention::THREE:
            return Ray3D::FromWkt(clean_line);
        }
        throw std::runtime_error("bad number format, not 2D or 3D");
      }

      if (clean_line.find("POINT", 0) == 0) {
        switch (GetDimentionSingleElement(clean_line)) {
          case Dimention::TWO:
            return Point2D::FromWkt(clean_line);
          case Dimention::THREE:
            return Point3D::FromWkt(clean_line);
        }
        throw std::runtime_error("bad number format, not 2D or 3D");
      }

      throw std::runtime_error("unknown type");

    } catch (...) {
      GEOMPP_LOG(ERROR) << "unsupported geometry " << clean_line;
      return std::nullopt;
    }
  }
  return std::nullopt;
}

std::string LVSParser::GetFilePath() const { return FILE_NAME; }

std::string LVSParser::ToWkt(ReturnSet const& item) {
  if (!item.has_value()) {
    return "GEOMETRYCOLLECTION EMPTY";
  }

  // TODO: make a geometry collection class
  //      and transform the variant<point, line, ray...> into a (base) class where this logic exists
  auto& v = item.value();
  if (std::holds_alternative<Point2D>(v)) {
    return std::get<Point2D>(v).ToWkt();
  }

  if (std::holds_alternative<Line2D>(v)) {
    return std::get<Line2D>(v).ToWkt();
  }

  if (std::holds_alternative<Ray2D>(v)) {
    return std::get<Ray2D>(v).ToWkt();
  }

  if (std::holds_alternative<LineSegment2D>(v)) {
    return std::get<LineSegment2D>(v).ToWkt();
  }

  if (std::holds_alternative<Point3D>(v)) {
    return std::get<Point3D>(v).ToWkt();
  }

  if (std::holds_alternative<Line3D>(v)) {
    return std::get<Line3D>(v).ToWkt();
  }

  if (std::holds_alternative<Ray3D>(v)) {
    return std::get<Ray3D>(v).ToWkt();
  }

  if (std::holds_alternative<LineSegment3D>(v)) {
    return std::get<LineSegment3D>(v).ToWkt();
  }

  GEOMPP_LOG(WARNING) << "unknown geometry type in LSV::ToWkt()";
  return "GEOMETRYCOLLECTION EMPTY";
}

}  // namespace geompp
