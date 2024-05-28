#include "line2d.hpp"

#include "line_segment2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <format>
#include <iostream>  // TODO: replace with logger lib
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Line2D Line2D::Make(Point2D const& p0, Point2D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision), decimal_precision));
  }
  return {p0, p1};
}

Line2D Line2D::Make(Point2D const& p0, Vector2D const& dir, int decimal_precision) {
  if (round_to(dir.Length(), decimal_precision) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", decimal_precision));
  }
  return {p0, dir};
}

Line2D::Line2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1), DIR((p1 - p0).Normalize()) {}

Line2D::Line2D(Point2D const& orig, Vector2D const& dir) : P0(orig), DIR(dir.Normalize()), P1(orig + dir) {}

bool Line2D::AlmostEquals(Line2D const& other, int decimal_precision) const {
  return P0.AlmostEquals(other.P0, decimal_precision) && P1.AlmostEquals(other.P1, decimal_precision);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Line2D const& lhs, Line2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Geometrical Operations

bool Line2D::Contains(Point2D const& point, int decimal_precision) const {
  return round_to((point - P0).Cross(DIR), decimal_precision) == 0.0;
}

bool Line2D::Intersects(Line2D const& other, int decimal_precision) const {
  // very easy to verify in 2D plane
  return round_to(DIR.Cross(other.DIR), decimal_precision) != 0.0;
}

bool Line2D::Intersects(Ray2D const& ray, int decimal_precision) const {
  return ray.Intersects(*this, decimal_precision);
}

bool Line2D::Intersects(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersects(*this, decimal_precision);
}

Line2D::ReturnSet Line2D::Intersection(Line2D const& other, int decimal_precision) const {
  auto u = DIR;
  auto v = other.DIR;
  auto vp = v.Perp();
  auto w = (P0 - other.P0);

  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  return P0 + t * u;
}

Line2D::ReturnSet Line2D::Intersection(Ray2D const& ray, int decimal_precision) const {
  return ray.Intersection(*this, decimal_precision);
}

Line2D::ReturnSet Line2D::Intersection(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersection(*this, decimal_precision);
}

#pragma endregion

#pragma region Formatting

std::string Line2D::ToWkt(int decimal_precision) const {
  return std::format("LINE ({} {}, {} {})", round_to(P0.x(), decimal_precision), round_to(P0.y(), decimal_precision),
                     round_to(P1.x(), decimal_precision), round_to(P1.y(), decimal_precision));
}

Line2D Line2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINE") {
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

    return Make(Point2D{nums_p1[0], nums_p1[1]}, Point2D{nums_p2[0], nums_p2[1]});

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
