#include "ray3d.hpp"

#include "calc_utils3d.hpp"
#include "constants.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Ray3D Ray3D::Make(Point3D const& p0, Vector3D const& dir) {
  if (compare(dir.Length(), 0) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Ray3D::Ray3D(Point3D const& orig, Vector3D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

Ray3D& Ray3D::operator=(Ray3D const& other) {
  if (this != &other) {
    ORIGIN = other.ORIGIN;
    DIR = other.DIR;
  }
  return *this;
}

bool Ray3D::IsAhead(Point3D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) >= 0; }

bool Ray3D::IsBehind(Point3D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) < 0; }

Line3D Ray3D::ToLine() const { return Line3D::Make(ORIGIN, DIR); }

bool Ray3D::AlmostEquals(Ray3D const& other, double epsilon) const {
  return ORIGIN.AlmostEquals(other.ORIGIN, epsilon) && DIR.AlmostEquals(other.DIR, epsilon);
}

#pragma endregion

#pragma region line operations

Point3D Ray3D::ProjectOnto(Point3D const& point) const {
  double t = (point - ORIGIN).Dot(DIR);
  if (compare(t, 0) <= 0) {
    return ORIGIN;
  }
  return ORIGIN + t * DIR;
}

double Ray3D::DistanceTo(Point3D const& point) const { return (point - ProjectOnto(point)).Length(); }

std::optional<LineSegment3D> Ray3D::Distance(Line3D const& line) const {
  auto P0 = ORIGIN;
  auto P1 = ORIGIN + DIR;  // point along the ray direction, not necessarily unit distance
  auto Q0 = line.First();
  auto Q1 = line.Last();

  double sc, tc;
  distance_line_to_line(P0, P1, Q0, Q1, sc, tc);

  // boundary check for ray
  if (compare(sc, 0) < 0) {
    sc = 0;
  }

  auto U = P1 - P0;
  auto V = Q1 - Q0;

  // verify if null distance (don't build a line segment)
  auto dseg_P0 = P0 + (U * sc);
  auto dseg_P1 = Q0 + (V * tc);
  if (compare(dseg_P0.DistanceTo(dseg_P1), 0) == 0) {
    return std::nullopt;  // they intersect or overlap
  }

  return LineSegment3D::Make(dseg_P0, dseg_P1);
}

double Ray3D::DistanceTo(Line3D const& line) const {
  auto seg = Distance(line);
  if (seg.has_value()) {
    return seg->Length();
  }
  // nullopt = ray hits the line, or they overlap (collinear)
  return 0.0;
}

std::optional<LineSegment3D> Ray3D::Distance(Ray3D const& ray) const {
  auto P0 = ORIGIN;
  auto P1 = ORIGIN + DIR;  // point along the ray direction, not necessarily unit distance
  auto Q0 = ray.Origin();
  auto Q1 = ray.Origin() + ray.Direction();

  double sc, tc;
  distance_line_to_line(P0, P1, Q0, Q1, sc, tc);

  auto U = P1 - P0;
  auto V = Q1 - Q0;

  // Pre-clamp overlap check: ray clamping below collapses the magic-zero from
  // distance_line_to_line in the collinear-overlap case. If the unclamped closest
  // points already coincide AND the primitives genuinely share a region, short-circuit.
  {
    auto raw_P = P0 + U * sc;
    auto raw_Q = Q0 + V * tc;
    if (compare(raw_P.DistanceTo(raw_Q), 0) == 0 && (Contains(ray.Origin()) || ray.Contains(ORIGIN))) {
      return std::nullopt;
    }
  }

  // boundary check for ray
  if (compare(sc, 0) < 0) {
    sc = 0;
  }
  if (compare(tc, 0) < 0) {
    tc = 0;
  }

  // verify if null distance (don't build a line segment)
  auto dseg_P0 = P0 + (U * sc);
  auto dseg_P1 = Q0 + (V * tc);
  if (compare(dseg_P0.DistanceTo(dseg_P1), 0) == 0) {
    return std::nullopt;  // they intersect or overlap
  }

  return LineSegment3D::Make(dseg_P0, dseg_P1);
}

double Ray3D::DistanceTo(Ray3D const& ray) const {
  auto seg = Distance(ray);
  if (seg.has_value()) {
    return seg->Length();
  }

  return 0.0;
}

std::optional<LineSegment3D> Ray3D::Distance(LineSegment3D const& seg) const { return seg.Distance(*this); }

double Ray3D::DistanceTo(LineSegment3D const& seg) const { return seg.DistanceTo(*this); }

#pragma endregion

#pragma region Operator Overloading

bool operator==(Ray3D const& lhs, Ray3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Ray3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Ray3D::Contains(Point3D const& point) const { return ToLine().Contains(point) && IsAhead(point); }

bool Ray3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Ray3D::Intersects(Ray3D const& other) const { return Intersection(other).has_value(); }

bool Ray3D::Intersects(LineSegment3D const& segment) const { return segment.Intersects(*this); }

Ray3D::ReturnSet Ray3D::Intersection(Line3D const& line) const {
  double sc, tc;

  auto Pc = intersection_line_to_line(ORIGIN, ORIGIN + DIR, line.First(), line.Last(), sc, tc);

  // respecting Ray constraints: sc should be positive
  if (!(Pc.has_value() && is_greater_or_equal(sc, 0))) {
    return std::nullopt;
  }

  return Pc;  // intersection!
}

Ray3D::ReturnSet Ray3D::Intersection(Ray3D const& other) const {
  double sc, tc;
  auto Pc = intersection_line_to_line(ORIGIN, ORIGIN + DIR, other.Origin(), other.Origin() + other.Direction(), sc, tc);

  // respecting Ray constraints: sc and tc should be positive
  if (!(Pc.has_value() && is_greater_or_equal(sc, 0) && is_greater_or_equal(tc, 0))) {
    return std::nullopt;
  }

  return Pc;  // intersection!
}

Ray3D::ReturnSet Ray3D::Intersection(LineSegment3D const& segment) const { return segment.Intersection(*this); }

#pragma endregion

#pragma region Formatting

std::string Ray3D::ToWkt() const {
  return std::format("RAY ({} {} {}, {} {} {})", round(ORIGIN.x()), round(ORIGIN.y()), round(ORIGIN.z()),
                     round(DIR.x()), round(DIR.y()), round(DIR.z()));
}

Ray3D Ray3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "RAY") {
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

    return Make(Point3D{nums_p1[0], nums_p1[1], nums_p1[2]}, Vector3D{nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Ray3D::ToFile(std::string const& path) const {
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

Ray3D Ray3D::FromFile(std::string const& path) {
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

}  // namespace geompp
