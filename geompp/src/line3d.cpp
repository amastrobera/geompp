#include "line3d.hpp"

#include "line_segment3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Line3D Line3D::Make(Point3D const& p0, Point3D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

Line3D Line3D::Make(Point3D const& p0, Vector3D const& dir) {
  if (compare(dir.Length(), 0) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Line3D::Line3D(Point3D const& p0, Point3D const& p1) : P0(p0), P1(p1), DIR((p1 - p0).Normalize()) {}

Line3D::Line3D(Point3D const& orig, Vector3D const& dir) : P0(orig), DIR(dir.Normalize()), P1(orig + dir) {}

Line3D& Line3D::operator=(Line3D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
    DIR = other.DIR;
  }
  return *this;
}

bool Line3D::AlmostEquals(Line3D const& other, double epsilon) const {
  return P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon);
}

double Line3D::DistanceTo(Point3D const& point) const { return (point - ProjectOnto(point)).Length(); }

Point3D Line3D::ProjectOnto(Point3D const& point) const { return P0 + (point - P0).Dot(DIR) * DIR; }

double Line3D::Location(Point3D const& point) const {
  if (!Contains(point)) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length();
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Line3D const& lhs, Line3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Line3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Line3D::Contains(Point3D const& point) const { return compare(DIR.Cross((point - P0)).Length(), 0) == 0; }

bool Line3D::Intersects(Line3D const& other) const { return Intersection(other).has_value(); }

bool Line3D::Intersects(Ray3D const& ray) const { return ray.Intersects(*this); }

bool Line3D::Intersects(LineSegment3D const& segment) const { return segment.Intersects(*this); }

Line3D::ReturnSet Line3D::Intersection(Line3D const& other, double& sc, double& tc) const {
  try {
    // input parameters: this line as P0 + s*DIR, other as Q0 + t*DIR
    //
    //  3D Line-Line Intersection
    //      - minimize the (perpendicular) distance between lines
    //      - and later verify that this distance is nearly zero
    //        (intersection) or not (skew lines)
    //
    //  solving system   | u*u u*v | | s |  =  | u*w0 |
    //                   | u*v v*v | | t |     | v*w0 |
    Point3D Q0 = other.P0;
    Vector3D u = P1 - P0;
    Vector3D v = other.P1 - other.P0;
    Vector3D w0 = P0 - Q0;

    // variables
    //  solving system   | a b | | s |  =  | d |
    //                   | b c | | t |     | e |
    double a = u.Dot(u);
    double b = u.Dot(v);
    double c = v.Dot(v);
    double d = u.Dot(w0);
    double e = v.Dot(w0);
    double D = a * c - b * b;  // Determinant

    // Check if lines are parallel
    if (compare(D, 0) == 0) {
      return std::nullopt;  // Lines are parallel, no intersection
    }

    // Cramer's rule
    sc = (b * e - c * d) / D;
    tc = (a * e - b * d) / D;

    // The point on the Line closest to the Ray
    Point3D Pc = P0 + (u * sc);
    // The point on the Ray closest to the Line
    Point3D Qc = Q0 + (v * tc);

    // Check if they actually intersect (distance is near zero)
    if (compare(Pc.DistanceTo(Qc), 0.0) != 0) {
      return std::nullopt;  // No intersection, the closest points are not the same
    }

    return Pc;  // They intersect!

  } catch (...) {
    GEOMPP_LOG(WARNING) << "unexpected error while computing line intersection";
  }
  sc = tc = std::numeric_limits<double>::quiet_NaN();
  return std::nullopt;
}

Line3D::ReturnSet Line3D::Intersection(Line3D const& other) const {
  double sc, tc;
  return Intersection(other, sc, tc);
}

Line3D::ReturnSet Line3D::Intersection(Ray3D const& ray) const { return ray.Intersection(*this); }

Line3D::ReturnSet Line3D::Intersection(LineSegment3D const& segment) const { return segment.Intersection(*this); }

#pragma endregion

#pragma region Formatting

std::string Line3D::ToWkt() const {
  return std::format("LINE ({} {} {}, {} {} {})", round(P0.x()), round(P0.y()), round(P0.z()), round(P1.x()),
                     round(P1.y()), round(P1.z()));
}

Line3D Line3D::FromWkt(std::string const& wkt) {
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

    end_p1 = wkt.substr(end_gtype).find(',');
    if (end_p1 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1 - 1);

    auto nums_p1 = geompp::tokenize_to_doubles(s_nums_p1);
    if (nums_p1.size() != 3) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2 - 1);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 3) {
      throw std::runtime_error("numbers p2");
    }

    return Make(Point3D{nums_p1[0], nums_p1[1], nums_p1[2]}, Point3D{nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Line3D::ToFile(std::string const& path) const {
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

Line3D Line3D::FromFile(std::string const& path) {
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

#pragma endregion

}  // namespace geompp
