#include "vector3d.hpp"

#include "point3d.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib

namespace geompp {

Vector3D::Vector3D(double x, double y, double z) : X(x), Y(y), Z(z) {}

Vector3D& Vector3D::operator=(Vector3D const& other) {
  if (this != &other) {
    X = other.X;
    Y = other.Y;
    Z = other.Z;
  }
  return *this;
}

Point3D Vector3D::ToPoint() { return Point3D(X, Y, Z); }

double Vector3D::Length() const { return sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2)); }

bool Vector3D::AlmostEquals(Vector3D const& other) const {
  return round(X - other.X) == 0 && round(Y - other.Y) == 0 && round(Z - other.Z) == 0;
}

double Vector3D::Dot(Vector3D const& v) const { return (X * v.X + Y * v.Y + Z * v.Z); }

Vector3D Vector3D::Cross(Vector3D const& v) const { return {Y * v.Z - Z * v.Y, Z * v.X - X * v.Z, X * v.Y - Y * v.X}; }

Vector3D Vector3D::Perp() const {
  bool is_z_biggest = round(Z - Y) >= 0 && round(Z - X) >= 0;
  bool is_x_biggest = round(X - Y) >= 0 && round(X - Z) >= 0;
  // bool is_y_biggest = round(Y - Z) >= 0 && round(Y - X) >= 0;

  if (is_z_biggest) {
    return {-Y, X, 0};
  }

  if (is_x_biggest) {
    return {0, -Z, Y};
  }

  // if (is_y_biggest) { }
  return {Z, 0, -X};
}

Vector3D Vector3D::Normalize() const {
  double len = Length();
  return {X / len, Y / len};
}

#pragma region Operator Overloading

Vector3D Vector3D::operator-() { return {-X, -Y, -Z}; }

bool operator==(Vector3D const& lhs, Vector3D const& rhs) { return lhs.AlmostEquals(rhs); }

Vector3D operator+(Vector3D const& lhs, Vector3D const& rhs) {
  return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

Vector3D operator-(Vector3D const& lhs, Vector3D const& rhs) {
  return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

Point3D operator+(Vector3D const& lhs, Point3D const& rhs) {
  return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

Vector3D operator*(Vector3D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a, lhs.z() * a}; }

Vector3D operator*(double a, Vector3D const& rhs) { return rhs * a; }

double operator*(Vector3D const& lhs, Vector3D const& rhs) { return lhs.Dot(rhs); }

Vector3D operator/(Vector3D const& lhs, double a) { return Vector3D(lhs.x() / a, lhs.y() / a, lhs.z() / a); }

std::ostream& operator<<(std::ostream& os, Vector3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Formatting

std::string Vector3D::ToWkt() const { return std::format("VECTOR ({} {} {})", round(X), round(Y), round(Z)); }

Vector3D Vector3D::FromWkt(std::string const& wkt) {
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

    auto nums = geompp::tokenize_to_doubles(s_nums);
    if (nums.size() != 3) {
      throw std::runtime_error("numbers");
    }

    return {nums[0], nums[1], nums[2]};

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Vector3D::ToFile(std::string const& path) const {
  try {
    std::string content = ToWkt();

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

Vector3D Vector3D::FromFile(std::string const& path) {
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
