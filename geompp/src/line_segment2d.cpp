#include "line_segment2d.hpp"

#include "line2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment2D LineSegment2D::Make(Point2D const& p0, Point2D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision), decimal_precision));
  }
  return {p0, p1};
}

LineSegment2D::LineSegment2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1) {}

LineSegment2D& LineSegment2D::operator=(LineSegment2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

double LineSegment2D::Length() const { return (P1 - P0).Length(); }

bool LineSegment2D::AlmostEquals(LineSegment2D const& other, int decimal_precision) const {
  return P0.AlmostEquals(other.P0, decimal_precision) && P1.AlmostEquals(other.P1, decimal_precision);
}

Line2D LineSegment2D::ToLine(int decimal_precision) const { return Line2D::Make(P0, P1, decimal_precision); }

double LineSegment2D::Location(Point2D const& point, int decimal_precision) const {
  if (!ToLine().Contains(point, decimal_precision)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0), decimal_precision) * (point - P0).Length() / Length();
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment2D::Contains(Point2D const& point, int decimal_precision) const {
  double t = Location(point, decimal_precision);
  return round_to(t, decimal_precision) >= 0 && round_to(t - 1, decimal_precision) <= 0;
}

bool LineSegment2D::Intersects(Line2D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}

bool LineSegment2D::Intersects(Ray2D const& ray, int decimal_precision) const {
  return Intersection(ray, decimal_precision).has_value();
}

bool LineSegment2D::Intersects(LineSegment2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(Line2D const& line, int decimal_precision) const {
  auto u = P1 - P0;
  auto v = line.Direction();
  auto vp = v.Perp();
  auto w = (P0 - line.First());

  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = P0 + t * u;
  if (!Contains(inter_p, decimal_precision)) {
    return std::nullopt;
  }

  return inter_p;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(Ray2D const& ray, int decimal_precision) const {
  auto u = P1 - P0;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = ray.Direction();
  auto vp = v.Perp();
  auto w = (P0 - ray.Origin());

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = ray.Origin() + s * v;
  if (!ray.IsAhead(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(LineSegment2D const& other, int decimal_precision) const {
  auto u = P1 - P0;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = (other.P1 - other.P0);
  auto vp = v.Perp();
  auto w = (P0 - other.P0);

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.P0 + s * v;
  if (!other.Contains(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

#pragma endregion

#pragma region Formatting

std::string LineSegment2D::ToWkt(int decimal_precision) const {
  return std::format("LINESTRING ({} {}, {} {})", round_to(P0.x(), decimal_precision),
                     round_to(P0.y(), decimal_precision), round_to(P1.x(), decimal_precision),
                     round_to(P1.y(), decimal_precision));
}

LineSegment2D LineSegment2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINESTRING") {
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

    return Make({nums_p1[0], nums_p1[1]}, {nums_p2[0], nums_p2[1]});

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void LineSegment2D::ToFile(std::string const& path, int decimal_precision) const {
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

LineSegment2D LineSegment2D::FromFile(std::string const& path) {
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
