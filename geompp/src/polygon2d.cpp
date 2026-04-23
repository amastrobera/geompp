#include "polygon2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "ray2d.hpp"
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

Polygon2D Polygon2D::Make(std::vector<Point2D> const& points) {
  auto unique_points = remove_duplicates(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points  are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }
  return {unique_points};
}

Polygon2D::Polygon2D(std::vector<Point2D> const& points) : VERTICES(points) {}

Polygon2D& Polygon2D::operator=(Polygon2D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
  }
  return *this;
}

bool Polygon2D::AlmostEquals(Polygon2D const& other, double epsilon) const {
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

Point2D Polygon2D::Centroid() const { throw std::runtime_error("not implemented"); }

double Polygon2D::SignedArea() const { throw std::runtime_error("not implemented"); }

double Polygon2D::Area() const { throw std::runtime_error("not implemented"); }

double Polygon2D::Perimeter() const { throw std::runtime_error("not implemented"); }

double Polygon2D::DistanceTo(Point2D const& point) const { throw std::runtime_error("not implemented"); }

double Polygon2D::Location(Point2D const& point) const { throw std::runtime_error("not implemented"); }

Point2D Polygon2D::Interpolate(double pct) const { throw std::runtime_error("not implemented"); }

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs) { return lhs.AlmostEquals(rhs); }

Point2D const& Polygon2D::operator[](int i) const {
  if (i >= Size()) {
    throw std::out_of_range("Index out of range");
  }
  return VERTICES[i];
}

std::ostream& operator<<(std::ostream& os, Polygon2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Polygon2D::Contains(Point2D const& point) const { throw std::runtime_error("not implemented"); }

bool Polygon2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool Polygon2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon2D::Intersects(LineSegment2D const& segment) const { return Intersection(segment).has_value(); }

Polygon2D::ReturnSet Polygon2D::Intersection(Line2D const& line) const { throw std::runtime_error("not implemented"); }

Polygon2D::ReturnSet Polygon2D::Intersection(Ray2D const& ray) const { throw std::runtime_error("not implemented"); }

Polygon2D::ReturnSet Polygon2D::Intersection(LineSegment2D const& other) const {
  throw std::runtime_error("not implemented");
}

#pragma endregion

// #pragma region Formatting

std::string Polygon2D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYGON ";
  int num_verts = VERTICES.size();
  if (!num_verts) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "((";
  for (int i = 0; i < num_verts; ++i) {
    buf << std::format("{} {}", round(VERTICES[i].x()), round(VERTICES[i].y()));
    buf << ", ";
  }
  buf << std::format("{} {}", round(VERTICES[0].x()), round(VERTICES[0].y()));
  buf << "))";

  return buf.str();
}

Polygon2D Polygon2D::FromWkt(std::string const& wkt) {
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

  //   std::vector<Point2D> pt_vec;
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

void Polygon2D::ToFile(std::string const& path) const {
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

Polygon2D Polygon2D::FromFile(std::string const& path) {
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
