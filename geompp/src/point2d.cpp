#include "point2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib

namespace geompp {

Point2D::Point2D(double x, double y) : X(x), Y(y) {}

Point2D::Point2D(Point2D const& p) : X(p.X), Y(p.Y) {}

bool Point2D::AlmostEquals(Point2D const& other, int decimal_precision) const {
  return round_to(X - other.X, decimal_precision) == 0.0 && round_to(Y - other.Y, decimal_precision) == 0.0;
}

Vector2D Point2D::ToVector() { return {X, Y}; }

double Point2D::DistanceTo(Point2D const& other, int decimal_precision) const {
  return round_to((other - *this).Length(), decimal_precision);
}

Point2D& Point2D::operator=(Point2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

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

Point2D Point2D::FromWkt(std::string const& wkt) {
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

    end_nums = wkt.substr(end_gtype + 1).find(')');
    if (end_nums == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums = wkt.substr(end_gtype + 1, end_nums);

    auto nums = geompp::tokenize_space_separated_string_to_doubles(s_nums);
    if (nums.size() != 2) {
      throw std::runtime_error("numbers");
    }

    return {nums[0], nums[1]};

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Point2D::ToFile(std::string const& path, int decimal_precision) const {
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

Point2D Point2D::FromFile(std::string const& path) {
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
