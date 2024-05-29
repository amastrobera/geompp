#include "vector2d.hpp"

#include "utils.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib


namespace geompp {

Vector2D::Vector2D(double x, double y) : X(x), Y(y) {}

Vector2D& Vector2D::operator=(Vector2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

double Vector2D::Length() const { return sqrt(pow(X, 2) + pow(Y, 2)); }

bool Vector2D::AlmostEquals(Vector2D const& other, int decimal_precision) const {
  return round_to(X - other.X, decimal_precision) == 0 && round_to(Y - other.Y, decimal_precision) == 0;
}

double Vector2D::Dot(Vector2D const& v) const { return (X * v.X + Y * v.Y); }

double Vector2D::Cross(Vector2D const& v) const { return (-Y * v.X + X * v.Y); }

Vector2D Vector2D::Perp() const { return {-Y, X}; }

Vector2D Vector2D::Normalize() const {
  double len = Length();
  return {X / len, Y / len};
}

#pragma region Operator Overloading

Vector2D Vector2D::operator-() { return {-X, -Y}; }

bool operator==(Vector2D const& lhs, Vector2D const& rhs) { return lhs.AlmostEquals(rhs); }

Vector2D operator+(Vector2D const& lhs, Vector2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator-(Vector2D const& lhs, Vector2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }

Point2D operator+(Vector2D const& lhs, Point2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator*(Vector2D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a}; }

Vector2D operator*(double a, Vector2D const& rhs) { return rhs * a; }

double operator*(Vector2D const& lhs, Vector2D const& rhs) { return lhs.Dot(rhs); }

#pragma endregion

#pragma region Formatting

std::string Vector2D::ToWkt(int decimal_precision) const {
  return std::format("VECTOR ({} {})", round_to(X, decimal_precision), round_to(Y, decimal_precision));
}

Vector2D Vector2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_nums;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "VECTOR") {
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

void Vector2D::ToFile(std::string const& path, int decimal_precision) const {
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

Vector2D Vector2D::FromFile(std::string const& path) {
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
