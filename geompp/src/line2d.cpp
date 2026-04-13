#include "line2d.hpp"

#include "line_segment2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Line2D Line2D::Make(Point2D const& p0, Point2D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

Line2D Line2D::Make(Point2D const& p0, Vector2D const& dir) {
  if (compare(dir.Length(), 0) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Line2D::Line2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1), DIR((p1 - p0).Normalize()) {}

Line2D::Line2D(Point2D const& orig, Vector2D const& dir) : P0(orig), DIR(dir.Normalize()), P1(orig + dir) {}

Line2D& Line2D::operator=(Line2D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
    DIR = other.DIR;
  }
  return *this;
}

bool Line2D::AlmostEquals(Line2D const& other, double epsilon) const {
  return P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon);
}

double Line2D::DistanceTo(Point2D const& point) const { return round(std::abs(DIR.Cross(point - P0))); }

Point2D Line2D::ProjectOnto(Point2D const& point) const { return P0 + (point - P0).Dot(DIR) * DIR; }

double Line2D::Location(Point2D const& point) const {
  if (!Contains(point)) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length();
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Line2D const& lhs, Line2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Line2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Line2D::Contains(Point2D const& point) const { return compare((point - P0).Cross(DIR), 0) == 0; }

bool Line2D::Intersects(Line2D const& other) const {
  // very easy to verify in 2D plane
  return compare(DIR.Cross(other.DIR), 0) != 0;
}

bool Line2D::Intersects(Ray2D const& ray) const { return ray.Intersects(*this); }

bool Line2D::Intersects(LineSegment2D const& segment) const { return segment.Intersects(*this); }

Line2D::ReturnSet Line2D::Intersection(Line2D const& other, double& sc, double& tc) const {
  try {
    // 2D intersection algorithm based on the perp-product
    //   Given
    //    L(s) = P0 + s * u
    //    L(t) = Q0 + t * v
    //  let w0 = P0 - Q0, then we can set up the equations
    //    (w0 + s*u) * perp-v = 0     => s = -(w0 * perp-v) / (u * perp-v)
    //    (-w0 + t*v) * perp-u = 0    => t = (w0 * perp-u) / (v * perp-u)
    auto u = P1 - P0;
    auto v = other.P1 - other.P0;
    auto vp = v.Perp();
    auto up = u.Perp();
    auto Q0 = other.P0;
    auto w0 = (P0 - other.P0);

    // parallel lines
    if (compare(u.Dot(vp), 0) == 0 || compare(v.Dot(up), 0) == 0) {
      sc = tc = std::numeric_limits<double>::quiet_NaN();
      return std::nullopt;  // Lines are parallel, no intersection
    }

    // perp-prod approach (yields the same closed form as Cramer's rule in 2D set of equations - since the determinant,
    // the cross product and the perp-product are the same thing in 2D)
    sc = -w0.Dot(vp) / u.Dot(vp);
    tc = w0.Dot(up) / v.Dot(up);

    // no need to check whether Pc(sc) and Qc(tc) are the same
    // because in 2D the lines are either parallel or they intersect in a single point
    // (there is no skew line)
    auto Pc = P0 + (u * sc);

    return Pc;

  } catch (...) {
    GEOMPP_LOG(WARNING) << "unexpected error while computing line intersection";
  }
  sc = tc = std::numeric_limits<double>::quiet_NaN();
  return std::nullopt;
}

Line2D::ReturnSet Line2D::Intersection(Line2D const& other) const {
  double sc, tc;
  return Intersection(other, sc, tc);
}

Line2D::ReturnSet Line2D::Intersection(Ray2D const& ray) const { return ray.Intersection(*this); }

Line2D::ReturnSet Line2D::Intersection(LineSegment2D const& segment) const { return segment.Intersection(*this); }

#pragma endregion

#pragma region Formatting

std::string Line2D::ToWkt() const {
  return std::format("LINE ({} {}, {} {})", round(P0.x()), round(P0.y()), round(P1.x()), round(P1.y()));
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

    end_p1 = wkt.substr(end_gtype).find(',');
    if (end_p1 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1 - 1);

    auto nums_p1 = geompp::tokenize_to_doubles(s_nums_p1);
    if (nums_p1.size() != 2) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2 - 1);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 2) {
      throw std::runtime_error("numbers p2");
    }

    return Make(Point2D{nums_p1[0], nums_p1[1]}, Point2D{nums_p2[0], nums_p2[1]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Line2D::ToFile(std::string const& path) const {
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

Line2D Line2D::FromFile(std::string const& path) {
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
