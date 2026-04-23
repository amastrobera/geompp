#include "polygon3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Polygon3D Polygon3D::Make(std::vector<Point3D> const& points) {
  auto unique_points = remove_duplicates(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }
  return {unique_points};
}

Polygon3D::Polygon3D(std::vector<Point3D> const& points) : VERTICES(points) {}

Polygon3D& Polygon3D::operator=(Polygon3D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
  }
  return *this;
}

bool Polygon3D::AlmostEquals(Polygon3D const& other, double epsilon) const {
  if (Size() != other.Size()) {
    return false;
  }
  for (size_t i = 0; i < VERTICES.size(); ++i) {
    if (!VERTICES[i].AlmostEquals(other[i], epsilon)) {
      return false;
    }
  }
  return true;
}

Point3D Polygon3D::Centroid() const { throw std::runtime_error("not implemented"); }

double Polygon3D::SignedArea() const { throw std::runtime_error("not implemented"); }

double Polygon3D::Area() const { throw std::runtime_error("not implemented"); }

double Polygon3D::Perimeter() const { throw std::runtime_error("not implemented"); }

double Polygon3D::DistanceTo(Point3D const& point) const { throw std::runtime_error("not implemented"); }

double Polygon3D::Location(Point3D const& point) const { throw std::runtime_error("not implemented"); }

Point3D Polygon3D::Interpolate(double pct) const { throw std::runtime_error("not implemented"); }

#pragma region Operator Overloading

bool operator==(Polygon3D const& lhs, Polygon3D const& rhs) { return lhs.AlmostEquals(rhs); }

Point3D const& Polygon3D::operator[](int i) const {
  if (i >= Size()) {
    throw std::out_of_range("Index out of range");
  }
  return VERTICES[i];
}

std::ostream& operator<<(std::ostream& os, Polygon3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Polygon3D::Contains(Point3D const& point) const { throw std::runtime_error("not implemented"); }

bool Polygon3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Polygon3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon3D::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

Polygon3D::ReturnSet Polygon3D::Intersection(Line3D const& line) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(Ray3D const& ray) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(LineSegment3D const& other) const {
  throw std::runtime_error("not implemented");
}

#pragma endregion

// #pragma region Formatting

std::string Polygon3D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYGON ";
  int num_verts = VERTICES.size();
  if (!num_verts) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "((";
  for (int i = 0; i < num_verts; ++i) {
    buf << std::format("{} {} {}", round(VERTICES[i].x()), round(VERTICES[i].y()), round(VERTICES[i].z()));
    buf << ", ";
  }
  buf << std::format("{} {} {}", round(VERTICES[0].x()), round(VERTICES[0].y()), round(VERTICES[0].z()));
  buf << "))";

  return buf.str();
}

Polygon3D Polygon3D::FromWkt(std::string const& wkt) {
  // try {
  //   std::size_t end_gtype, end_pi, end_pn;

  //   end_gtype = wkt.find('(');
  //   if (end_gtype == std::string::npos) {
  //     throw std::runtime_error("brakets");
  //   }

  //   std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
  //   if (g_type != "POLYGON") {
  //     throw std::runtime_error("geometry name");
  //   }

  //   end_pn = wkt.substr(end_gtype + 1).find(')');
  //   if (end_pn == std::string::npos) {
  //     throw std::runtime_error("brakets");
  //   }

  //   std::string mid_part = wkt.substr(end_gtype + 1, wkt.size() - (end_gtype + 1 + 1));

  //   std::vector<Point3D> pt_vec;
  //   int decimal_precision = 0;
  //   int num_dec = 0;
  //   std::string pt_trimmed;
  //   for (std::string const& p_str : geompp::tokenize_string(mid_part, ',')) {
  //     pt_trimmed = geompp::trim(p_str);

  //     auto nums = geompp::tokenize_to_doubles(pt_trimmed, ' ');
  //     if (nums.size() != 2) {
  //       throw std::runtime_error("numbers");
  //     }

  //     num_dec = count_decimal_places(nums[0]);
  //     if (num_dec > decimal_precision) {
  //       decimal_precision = num_dec;
  //     }
  //     num_dec = count_decimal_places(nums[1]);
  //     if (num_dec > decimal_precision) {
  //       decimal_precision = num_dec;
  //     }

  //     pt_vec.push_back({nums[0], nums[1]});
  //   }

  //   if (pt_vec.size() != 3) {
  //     throw std::runtime_error("initialized with n != 3 points");
  //   }

  //   return Make(pt_vec[0], pt_vec[1], pt_vec[2]);

  // } catch (...) {
  //   std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  // }

  throw std::runtime_error("failed to parse WKT");
}

void Polygon3D::ToFile(std::string const& path) const {
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

Polygon3D Polygon3D::FromFile(std::string const& path) {
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
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }

  throw std::runtime_error("failed to parse WKT");
}

// #pragma endregion

}  // namespace geompp
