#include "vector3d.hpp"

#include "point3d.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>

namespace geompp {

inline namespace geometry {

Vector3D& Vector3D::operator=(Vector3D const& other) {
  if (this != &other) {
    X = other.X;
    Y = other.Y;
    Z = other.Z;
  }
  return *this;
}

Point3D Vector3D::ToPoint() const { return Point3D(X, Y, Z); }

double Vector3D::Length() const { return sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2)); }

Axis Vector3D::DominantAxis() const {
  double absX = std::abs(X);
  double absY = std::abs(Y);
  double absZ = std::abs(Z);

  if (compare(absZ, absX) >= 0 && compare(absZ, absY) >= 0) {
    return Axis::Z;

  } else {
    return compare(absX, absY) >= 0 ? Axis::X : Axis::Y;
  }
}

bool Vector3D::AlmostEquals(Vector3D const& other, double epsilon) const {
  return compare(X, other.X, epsilon) == 0 && compare(Y, other.Y, epsilon) == 0 && compare(Z, other.Z, epsilon) == 0;
}

double Vector3D::Dot(Vector3D const& other) const { return (X * other.X + Y * other.Y + Z * other.Z); }

Vector3D Vector3D::Cross(Vector3D const& other) const {
  return {Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X};
}

Vector3D Vector3D::Perp() const {
  bool is_z_biggest = compare(Z, Y) >= 0 && compare(Z, X) >= 0;
  if (is_z_biggest) {  // yaw around Z axis, so X and Y swap and one is negated (ccw rotation)
    if (compare(X, 0) == 0 && compare(Y, 0) == 0) {  // if the vector is along Z axis provide the X basis
      return {Z, 0, 0};
    }
    return {-Y, X, 0};
  }

  bool is_x_biggest = compare(X, Y) >= 0 && compare(X, Z) >= 0;
  if (is_x_biggest) {  // roll around X axis, so Y and Z swap and one is negated (ccw rotation)
    if (compare(Y, 0) == 0 && compare(Z, 0) == 0) {  // if the vector is along X axis provide the Y basis
      return {0, X, 0};
    }
    return {0, -Z, Y};
  }

  // bool is_y_biggest = compare(Y, Z) >= 0 && compare(Y, X) >= 0;
  // if (is_y_biggest) { // pitch around Y axis, so X and Z swap and one is negated (ccw rotation)
  if (compare(Z, 0) == 0 && compare(X, 0) == 0) {  // if the vector is along Y axis provide the Z basis
    return {0, 0, Y};
  }
  return {Z, 0, -X};
  // }
}

Vector3D Vector3D::Normalize() const {
  double len = Length();
  return {X / len, Y / len, Z / len};
}

bool Vector3D::IsParallel(Vector3D const& other) const {
  auto cross = Cross(other);
  return compare(cross.x(), 0) == 0 && compare(cross.y(), 0) == 0 && compare(cross.z(), 0) == 0;
}

#pragma region Operator Overloading

Vector3D Vector3D::operator-() const { return {-X, -Y, -Z}; }

bool operator==(Vector3D const& lhs, Vector3D const& rhs) { return lhs.AlmostEquals(rhs); }

Vector3D operator+(Vector3D const& lhs, Vector3D const& rhs) {
  return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

Vector3D operator+=(Vector3D& lhs, Vector3D const& vec) {
  lhs = lhs + vec;
  return lhs;
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
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
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
    GEOMPP_LOG(ERROR) << "bad path " << path;
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
    content.resize(static_cast<std::size_t>(fileSize));

    // Read the entire file into the string
    in_file.read(&content[0], fileSize);

    return FromWkt(content);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
