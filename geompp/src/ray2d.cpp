#include "ray2d.hpp"

#include "constants.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Ray2D Ray2D::Make(Point2D const& p0, Vector2D const& dir, int decimal_precision) {
  if (round_to(dir.Length(), decimal_precision) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", decimal_precision));
  }
  return {p0, dir};
}

Ray2D::Ray2D(Point2D const& orig, Vector2D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

Ray2D& Ray2D::operator=(Ray2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

bool Ray2D::IsAhead(Point2D const& point, int decimal_precision) const {
  return round_to(DIR.Dot(point - ORIGIN), decimal_precision) >= 0.0;
}

bool Ray2D::IsBehind(Point2D const& point, int decimal_precision) const {
  return round_to(DIR.Dot(point - ORIGIN), decimal_precision) < 0.0;
}

Line2D Ray2D::ToLine(int decimal_precision) const {
    return Line2D::Make(ORIGIN, DIR, decimal_precision);
}

double Ray2D::DistanceTo(Point2D const& point, int decimal_precision) const {
    return IsAhead(point, decimal_precision) ? 
                ToLine(decimal_precision).DistanceTo(point, decimal_precision):
                ORIGIN.DistanceTo(point, decimal_precision);
}

bool Ray2D::AlmostEquals(Ray2D const& other, int decimal_precision) const {
  return ORIGIN.AlmostEquals(other.ORIGIN, decimal_precision) && DIR.AlmostEquals(other.DIR, decimal_precision);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Ray2D const& lhs, Ray2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Geometrical Operations

bool Ray2D::Contains(Point2D const& point, int decimal_precision) const {
  return round_to((point - ORIGIN).Cross(DIR), decimal_precision) == 0.0 && IsAhead(point, decimal_precision);
}

bool Ray2D::Intersects(Line2D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}

bool Ray2D::Intersects(Ray2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

bool Ray2D::Intersects(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersects(*this, decimal_precision);
}

Ray2D::ReturnSet Ray2D::Intersection(Line2D const& line, int decimal_precision) const {
  auto u = DIR;
  auto v = line.Direction();
  auto vp = v.Perp();
  auto w = (ORIGIN - line.First());

  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = ORIGIN + t * u;
  if (!IsAhead(inter_p, decimal_precision)) {
    return std::nullopt;
  }

  return inter_p;
}

Ray2D::ReturnSet Ray2D::Intersection(Ray2D const& other, int decimal_precision) const {
  auto u = DIR;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = other.DIR;
  auto vp = v.Perp();
  auto w = (ORIGIN - other.ORIGIN);

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = ORIGIN + t * u;
  if (!IsAhead(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.ORIGIN + s * v;
  if (!other.IsAhead(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

Ray2D::ReturnSet Ray2D::Intersection(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersection(*this, decimal_precision);
}

#pragma endregion

#pragma region Formatting

std::string Ray2D::ToWkt(int decimal_precision) const {
  return std::format("RAY ({} {}, {} {})", round_to(ORIGIN.x(), decimal_precision),
                     round_to(ORIGIN.y(), decimal_precision), round_to(DIR.x(), decimal_precision),
                     round_to(DIR.y(), decimal_precision));
}

Ray2D Ray2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "RAY") {
      throw std::runtime_error("geometry name");
    }

    end_p1 = wkt.substr(end_gtype + 1).find(',');
    if (end_p1 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1);

    auto nums_p1 = geompp::tokenize_space_separated_string_to_doubles(s_nums_p1);
    if (nums_p1.size() != 2) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2);

    auto nums_p2 = geompp::tokenize_space_separated_string_to_doubles(s_nums_p2);
    if (nums_p2.size() != 2) {
      throw std::runtime_error("numbers p2");
    }

    return Make(Point2D{nums_p1[0], nums_p1[1]}, Vector2D{nums_p2[0], nums_p2[1]});

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Ray2D::ToFile(std::string const& path, int decimal_precision) const {
  try {
    std::string content = ToWkt(decimal_precision);

    // Open the file in write mode (truncates existing content)
    std::ofstream outfile(path);

    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open file");
    }

    // Write the text to the file
    outfile << content;

    outfile.close();

  } catch (...) {
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger lib
  }
}

Ray2D Ray2D::FromFile(std::string const& path) {
  try {
    std::string content;

    // Open the file in read mode
    std::ifstream in_file(path);

    if (!in_file.is_open()) {
      throw std::runtime_error("could not open file");
    }

    // Get the file size (optional, for efficiency)
    in_file.seekg(0, std::ios::end);
    std::streamsize fileSize = in_file.tellg();
    in_file.seekg(0, std::ios::beg);  // Reset the file pointer

    // Resize the string to the file size (optional, for efficiency)
    content.resize(static_cast<size_t>(fileSize));

    // Read the entire file into the string
    in_file.read(&content[0], fileSize);

    return FromWkt(content);

  } catch (...) {
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
