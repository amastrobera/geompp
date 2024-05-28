#include "point2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>  // TODO: replace with logger lib
#include <sstream>
#include <vector>

namespace geompp {

Point2D::Point2D(double x, double y) : X(x), Y(y) {}

Point2D::Point2D(Point2D const& p) : X(p.X), Y(p.Y) {}

bool Point2D::AlmostEquals(Point2D const& other, int decimal_precision) const {
  return round_to(X - other.X, decimal_precision) == 0.0 && round_to(Y - other.Y, decimal_precision) == 0.0;
}

Vector2D Point2D::ToVector() { return {X, Y}; }

#pragma region Operator Overloading
bool operator==(Point2D const& lhs, Point2D const& rhs) { return lhs.AlmostEquals(rhs); }

Point2D operator+(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator-(Point2D const& lhs, Point2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }
Point2D operator-(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }

Point2D operator*(Point2D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a}; }
Point2D operator*(double a, Point2D const& rhs) { return rhs * a; }

#pragma endregion

#pragma region Formatting

std::string Point2D::ToWkt(int decimal_precision) const {
  return std::format("POINT ({} {})", round_to(X, decimal_precision), round_to(Y, decimal_precision));
}

namespace {
std::vector<double> tokenize_string_to_doubles(const std::string& str) {
  std::istringstream iss(str);
  std::vector<double> tokens;
  double token;

  // Extract numbers using stream iterators
  while (iss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}
}  // namespace

Point2D Point2D::FromWkt(std::string wkt) {
  try {
    std::size_t end_gtype, end_nums;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POINT") {
      throw std::runtime_error("geometry name");
    }

    end_nums = wkt.substr(end_gtype).find(')');
    if (end_nums == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums = wkt.substr(end_gtype + 1, end_nums);

    auto nums = tokenize_string_to_doubles(s_nums);
    if (nums.size() != 2) {
      throw std::runtime_error("numbers");
    }

    return {nums[0], nums[1]};

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
