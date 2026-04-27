#include "wkt_parser.hpp"

#include "geompp_log.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace geompp {

namespace {

enum class Dimention { UNKNOWN = -1, TWO, THREE };

std::size_t GetNumberOfElementsInBetween(std::string wkt, char open_bracket = '(', char close_bracket = ')',
                                         char separator = ',') {
  std::size_t open_bracket_pos = wkt.find(open_bracket);
  if (open_bracket_pos == std::string::npos) {
    throw std::runtime_error("brakets");
  }

  std::size_t close_bracket_pos = wkt.find(close_bracket, open_bracket_pos);
  if (close_bracket_pos == std::string::npos) {
    throw std::runtime_error("brakets");
  }

  std::string in_between = wkt.substr(open_bracket_pos + 1, close_bracket_pos - open_bracket_pos - 1);

  return std::count(in_between.begin(), in_between.end(), separator) + 1;
}

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

Dimention GetDimentionMultiElementCommaSeparated(std::string wkt, bool has_inner_loops = false) {
  std::size_t end_gtype, end_p1, end_p2;

  end_gtype = has_inner_loops ? wkt.find("((") : wkt.find('(');
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

WktParser WktParser::Open(std::string const& file_path) {
  if (!fs::exists(file_path)) {
    throw std::runtime_error("file does not exist " + file_path);
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("could not open the file " + file_path);
  }

  return WktParser(file_path, std::move(file));
}

WktParser::WktParser(std::string const& file_name, std::ifstream&& file)
    : FILE_NAME(file_name), FILE(std::move(file)), HAS_NEXT(true) {}

WktParser::WktParser(WktParser&& other) noexcept
    : FILE_NAME(std::move(other.FILE_NAME)), FILE(std::move(other.FILE)), HAS_NEXT(other.HAS_NEXT) {}

WktParser::~WktParser() { FILE.close(); }

bool WktParser::HasNext() const { return !FILE.eof(); }

WktParser::ReturnSet WktParser::FromWkt(std::string const& wkt) {
  try {
    std::string clean_line = trim(wkt);

    if (clean_line.find("VECTOR", 0) == 0) {
      switch (GetDimentionSingleElement(clean_line)) {
        case Dimention::TWO:
          return Vector2D::FromWkt(clean_line);
        case Dimention::THREE:
          return Vector3D::FromWkt(clean_line);
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

    if (clean_line.find("LINESTRING", 0) == 0) {
      if (GetNumberOfElementsInBetween(clean_line) == 2) {
        switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
          case Dimention::TWO:
            return LineSegment2D::FromWkt(clean_line);
          case Dimention::THREE:
            return LineSegment3D::FromWkt(clean_line);
        }
      } else {
        switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
          case Dimention::TWO:
            return Polyline2D::FromWkt(clean_line);
          case Dimention::THREE:
            return Polyline3D::FromWkt(clean_line);
        }
      }
      throw std::runtime_error("bad number format, not 2D or 3D");
    }

    if (clean_line.find("TRIANGLE", 0) == 0) {
      switch (GetDimentionMultiElementCommaSeparated(clean_line)) {
        case Dimention::TWO:
          return Triangle2D::FromWkt(clean_line);
        case Dimention::THREE:
          return Triangle3D::FromWkt(clean_line);
      }
      throw std::runtime_error("bad number format, not 2D or 3D");
    }

    if (clean_line.find("POLYGON", 0) == 0) {
      switch (GetDimentionMultiElementCommaSeparated(clean_line, true)) {
        case Dimention::TWO:
          return Polygon2D::FromWkt(clean_line);
        case Dimention::THREE:
          return Polygon3D::FromWkt(clean_line);
      }
      throw std::runtime_error("bad number format, not 2D or 3D");
    }

    // TODO add support for geometry collection

    throw std::runtime_error("unknown type");

  } catch (...) {
    GEOMPP_LOG(ERROR) << "unsupported geometry " << wkt;
  }

  return std::nullopt;
}

std::string WktParser::ToWkt(ReturnSet const& shape) {
  if (!shape.has_value()) {
    throw std::runtime_error("shape is nullopt");
  }

  // thanks to the use of concepts for the variant type, this function is very simple to write
  return std::visit([](const auto& obj) { return obj.ToWkt(); }, shape.value());
}

WktParser::ReturnSet WktParser::Next() {
  std::string line, clean_line;
  while (std::getline(FILE, line)) {
    std::string clean_line = trim(line);

    if (clean_line.find("#", 0) == 0) {  // skip comments
      continue;
    }

    return WktParser::FromWkt(line);
  }
  return std::nullopt;
}

std::string WktParser::GetFilePath() const { return FILE_NAME; }

}  // namespace geompp
