#include "ray2d.hpp"

#include "calc_utils2d.hpp"
#include "geompp_log.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "polyline2d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

#pragma region Constructors

Ray2D Ray2D::Make(Point2D const& p0, Vector2D const& dir) {
  if (compare(dir.Length(), 0) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Ray2D& Ray2D::operator=(Ray2D const& other) {
  if (this != &other) {
    ORIGIN = other.ORIGIN;
    DIR = other.DIR;
  }
  return *this;
}

bool Ray2D::IsAhead(Point2D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) >= 0; }

bool Ray2D::IsBehind(Point2D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) < 0; }

Line2D Ray2D::ToLine() const { return Line2D::Make(ORIGIN, DIR); }

bool Ray2D::AlmostEquals(Ray2D const& other, double epsilon) const {
  return ORIGIN.AlmostEquals(other.ORIGIN, epsilon) && DIR.AlmostEquals(other.DIR, epsilon);
}

#pragma endregion

#pragma region line operations

Point2D Ray2D::ProjectOnto(Point2D const& point) const {
  double t = (point - ORIGIN).Dot(DIR);
  if (compare(t, 0) <= 0) {
    return ORIGIN;
  }
  return ORIGIN + t * DIR;
}

double Ray2D::DistanceTo(Point2D const& point) const { return (point - ProjectOnto(point)).Length(); }

#pragma endregion

#pragma region Operator Overloading

bool operator==(Ray2D const& lhs, Ray2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Ray2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Ray2D::Contains(Point2D const& point) const { return ToLine().Contains(point) && IsAhead(point); }

bool Ray2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool Ray2D::Intersects(Ray2D const& other) const { return Intersection(other).has_value(); }

bool Ray2D::Intersects(LineSegment2D const& segment) const { return segment.Intersects(*this); }

bool Ray2D::Intersects(Polyline2D const& polyline) const { return polyline.Intersects(*this); }

std::optional<Point2D> Ray2D::Intersection(Line2D const& line) const {
  double sc, tc;
  auto Pc = detail::line_intersection(ORIGIN, ORIGIN + DIR, line.First(), line.Last(), sc, tc);

  // respecting constraints: sc should be positive
  if (!Pc.has_value() || !is_greater_or_equal(sc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

std::optional<Point2D> Ray2D::Intersection(Ray2D const& other) const {
  double sc, tc;
  auto Pc = detail::line_intersection(ORIGIN, ORIGIN + DIR, other.ORIGIN, other.ORIGIN + other.DIR, sc, tc);

  // respecting constraints: sc and tc should be positive
  if (!Pc.has_value() || !is_greater_or_equal(sc, 0) || !is_greater_or_equal(tc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

std::optional<Point2D> Ray2D::Intersection(LineSegment2D const& segment) const { return segment.Intersection(*this); }
std::optional<std::vector<Point2D>> Ray2D::Intersection(Polyline2D const& polyline) const {
  return polyline.Intersection(*this);
}

bool Ray2D::Overlaps(Line2D const& line) const { return Overlap(line).has_value(); }
bool Ray2D::Overlaps(Ray2D const& ray) const { return Overlap(ray).has_value(); }
bool Ray2D::Overlaps(LineSegment2D const& seg) const { return seg.Overlaps(*this); }
bool Ray2D::Overlaps(Polyline2D const& polyline) const { return polyline.Overlaps(*this); }

std::optional<Ray2D> Ray2D::Overlap(Line2D const& line) const {
  if (!DIR.IsParallel(line.Direction()) || !line.Contains(ORIGIN)) {
    return std::nullopt;
  }
  return *this;
}

std::optional<std::variant<Ray2D, LineSegment2D>> Ray2D::Overlap(Ray2D const& ray) const {
  // are rays parallel? if not, they cannot overlap
  if (!DIR.IsParallel(ray.DIR)) {
    return std::nullopt;
  }

  // verify whether the rays contain each other's origin and deduce the common segment or ray
  bool this_has_ray_origin = Contains(ray.ORIGIN);
  bool ray_has_this_origin = ray.Contains(ORIGIN);

  // case 1: the rays are disjoint
  if (!this_has_ray_origin && !ray_has_this_origin) {
    return std::nullopt;
  }

  // case 2: the ray is contained in this
  if (this_has_ray_origin && !ray_has_this_origin) {
    return ray;
  }

  // case 3: this is contained in ray
  if (!this_has_ray_origin && ray_has_this_origin) {
    return *this;
  }

  // case 4: both origins are on the other ray
  if (ORIGIN.AlmostEquals(ray.ORIGIN)) {
    // case 4.1: anti-parallel rays meeting at one point — touch, not overlap
    if (compare(DIR.Dot(ray.DIR), 0) < 0) {
      return std::nullopt;
    }
    // case 4.2: same-direction rays with same origin — identical rays, full overlap
    return *this;
  }

  // case 4.3: anti-parallel rays that overlap — segment between the two origins
  return LineSegment2D::Make(ORIGIN, ray.ORIGIN);
}

std::optional<LineSegment2D> Ray2D::Overlap(LineSegment2D const& seg) const {
  auto result = seg.Overlap(*this);  // returns nothing or a segment in the direction of the LineSegment2D

  // flip the segment in the direction of the Ray2D if a segment exists
  if (result.has_value() && compare(DIR.Dot(seg.Last() - seg.First()), 0) < 0) {
    return result->Reversed();
  }

  return result;
}

std::optional<std::vector<LineSegment2D>> Ray2D::Overlap(Polyline2D const& polyline) const {
  return polyline.Overlap(*this);
}

bool Ray2D::Touches(Line2D const& line) const { return Touch(line).has_value(); }
bool Ray2D::Touches(Ray2D const& ray) const { return Touch(ray).has_value(); }
bool Ray2D::Touches(LineSegment2D const& seg) const { return seg.Touches(*this); }
bool Ray2D::Touches(Polyline2D const& polyline) const { return polyline.Touches(*this); }

std::optional<Point2D> Ray2D::Touch(Line2D const& line) const {
  if (!DIR.IsParallel(line.Direction()) && line.Contains(ORIGIN)) {
    return ORIGIN;
  }
  return std::nullopt;
}

std::optional<Point2D> Ray2D::Touch(Ray2D const& ray) const {
  // not parallel -> either ray contains the other's origin, it's a touch, or not, it's a miss
  if (!DIR.IsParallel(ray.DIR)) {
    if (ray.Contains(ORIGIN)) {
      return ORIGIN;
    }
    if (Contains(ray.ORIGIN)) {
      return ray.ORIGIN;
    }
    return std::nullopt;
  }

  // parallel but opposite heading and the origin matches
  if (DIR.AlmostEquals(-ray.DIR) && ORIGIN.AlmostEquals(ray.ORIGIN)) {
    return ORIGIN;
  }

  return std::nullopt;
}

std::optional<Point2D> Ray2D::Touch(LineSegment2D const& seg) const { return seg.Touch(*this); }
std::optional<std::vector<Point2D>> Ray2D::Touch(Polyline2D const& polyline) const { return polyline.Touch(*this); }

#pragma endregion

#pragma region Formatting

std::string Ray2D::ToWkt() const {
  return std::format("RAY ({} {}, {} {})", round(ORIGIN.x()), round(ORIGIN.y()), round(DIR.x()), round(DIR.y()));
}

Ray2D Ray2D::FromWkt(std::string const& wkt) {
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
    if (nums_p1.size() != 2) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1, end_p2);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 2) {
      throw std::runtime_error("numbers p2");
    }

    return Make(Point2D{nums_p1[0], nums_p1[1]}, Vector2D{nums_p2[0], nums_p2[1]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Ray2D::ToFile(std::string const& path) const {
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

Ray2D Ray2D::FromFile(std::string const& path) {
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
