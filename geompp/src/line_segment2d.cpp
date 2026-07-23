#include "line_segment2d.hpp"

#include "calc_utils2d.hpp"
#include "line2d.hpp"
#include "polyline2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment2D LineSegment2D::Make(Point2D const& p0, Point2D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

LineSegment2D& LineSegment2D::operator=(LineSegment2D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
  }
  return *this;
}

double LineSegment2D::Length() const { return (P1 - P0).Length(); }

LineSegment2D LineSegment2D::Reversed() const { return {P1, P0}; }

bool LineSegment2D::AlmostEquals(LineSegment2D const& other, double epsilon) const {
  return (P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon))  // forward
         ||                                                                          //
         (P0.AlmostEquals(other.P1, epsilon) && P1.AlmostEquals(other.P0, epsilon))  // backward
      ;  // no heading check (needed for edges in a mesh though)
}

Line2D LineSegment2D::ToLine() const { return Line2D::Make(P0, P1); }

#pragma endregion

#pragma region line operations

Point2D LineSegment2D::ProjectOnto(Point2D const& point) const {
  auto dir_unit = (P1 - P0).Normalize();
  double t = (point - P0).Dot(dir_unit);
  // the point is behind the first point of the segment, or beyond the second point of the segment
  if (compare(t, 0) <= 0) {
    return P0;
  }
  // the point is beyond the second point of the segment
  if (compare(t, Length()) >= 0) {
    return P1;
  }
  return P0 + t * dir_unit;
}

double LineSegment2D::Location(Point2D const& point) const {
  if (!ToLine().Contains(point)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length() / Length();
}

Point2D LineSegment2D::Interpolate(double pct) const {
  // the point is behind the polyline
  if (compare(pct, 0) < 0) {
    return P0;
  }

  // the point is beyond the polyline
  if (compare(pct, 1.0) > 0) {
    return P1;
  }

  return lerp(P0, P1, pct);
}

bool LineSegment2D::IsLeft(Point2D const& p) const {
  // elegant use of primitives, but calls many constructors
  // return compare((P1 - P0).Cross(p.ToVector()), 0) > 0;
  return compare((P1.x() - P0.x()) * (p.y() - P0.y()) - (P1.y() - P0.y()) * (p.x() - P0.x()), 0) > 0;
}

double LineSegment2D::DistanceTo(Point2D const& point) const { return (point - ProjectOnto(point)).Length(); }

#pragma endregion

#pragma region Collections Operations

bool has_intersections(std::vector<LineSegment2D> const& segments) { return detail::has_intersections(segments); }

std::vector<Point2D> find_intersections(std::vector<LineSegment2D> const& segments) {
  auto events = detail::find_intersections(segments);
  std::vector<Point2D> pts;
  pts.reserve(events.size());
  for (auto const& ev : events) {
    pts.emplace_back(ev.Point);
  }
  return pts;
}

bool intersect(LineSegment2D const& seg1, LineSegment2D const& seg2) {
  auto l1 = seg1.First();
  auto r1 = seg1.Last();
  if (detail::compare_event_point(l1, r1) > 0) {
    std::swap(l1, r1);
  }

  auto l2 = seg2.First();
  auto r2 = seg2.Last();
  if (detail::compare_event_point(l2, r2) > 0) {
    std::swap(l2, r2);
  }

  // Signed cross-product magnitude, NOT is_left()'s left/not-left bool: multiplying two bools can never
  // produce a negative product, so "both strictly on the right" (false * false = 0) was indistinguishable
  // from "one endpoint exactly on the line" and silently fell through as an undetected same-side case —
  // a false positive whenever the OTHER segment's line happened to straddle seg1/seg2 (see the seg1×seg7
  // repro in a Union() output ring that motivated this fix: both of seg7's endpoints were strictly right
  // of seg1's line, is_left() returned false/false for both, and the same-side rejection never fired).
  double lsign, rsign;
  lsign = (r1 - l1).Cross(l2 - l1);
  rsign = (r1 - l1).Cross(r2 - l1);
  if (compare(lsign * rsign, 0) > 0) {
    return false;  // seg2 is on the same side of seg1
  }

  lsign = (r2 - l2).Cross(l1 - l2);
  rsign = (r2 - l2).Cross(r1 - l2);
  if (compare(lsign * rsign, 0) > 0) {
    return false;  // seg1 is on the same side of seg2
  }

  return true;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, LineSegment2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment2D::Contains(Point2D const& point) const {
  double t = Location(point);
  return compare(t, 0) >= 0 && compare(t, 1.0) <= 0;
}

bool LineSegment2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool LineSegment2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }

bool LineSegment2D::Intersects(LineSegment2D const& other) const { return Intersection(other).has_value(); }

bool LineSegment2D::Intersects(Polyline2D const& polyline) const { return polyline.Intersects(*this); }

std::optional<Point2D> LineSegment2D::Intersection(Line2D const& line) const {
  double sc, tc;
  auto Pc = detail::line_intersection(P0, P1, line.First(), line.Last(), sc, tc);

  // respecting constraints: sc should be in the range [0, 1]
  if (!Pc.has_value() || !is_in_range(sc, 0, 1)) {
    return std::nullopt;
  }

  return Pc;
}

std::optional<Point2D> LineSegment2D::Intersection(Ray2D const& ray) const {
  double sc, tc;
  auto Pc = detail::line_intersection(P0, P1, ray.Origin(), ray.Origin() + ray.Direction(), sc, tc);

  // respecting constraints: sc should be in the range [0, 1] while tc should be in non negative
  if (!Pc.has_value() || !is_in_range(sc, 0, 1) || !is_greater_or_equal(tc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

std::optional<Point2D> LineSegment2D::Intersection(LineSegment2D const& other) const {
  double sc, tc;
  auto Pc = detail::line_intersection(P0, P1, other.P0, other.P1, sc, tc);

  // respecting constraints: sc and tc should be in the range [0, 1]
  if (!Pc.has_value() || !is_in_range(sc, 0, 1) || !is_in_range(tc, 0, 1)) {
    return std::nullopt;
  }

  return Pc;
}

std::optional<std::vector<Point2D>> LineSegment2D::Intersection(Polyline2D const& polyline) const {
  return polyline.Intersection(*this);
}

bool LineSegment2D::Overlaps(Line2D const& line) const { return Overlap(line).has_value(); }
bool LineSegment2D::Overlaps(Ray2D const& ray) const { return Overlap(ray).has_value(); }
bool LineSegment2D::Overlaps(LineSegment2D const& seg) const { return Overlap(seg).has_value(); }
bool LineSegment2D::Overlaps(Polyline2D const& polyline) const { return polyline.Overlaps(*this); }

std::optional<LineSegment2D> LineSegment2D::Overlap(Line2D const& line) const {
  if (!((P1 - P0).IsParallel(line.Direction()) && line.Contains(P0))) {
    return std::nullopt;
  }

  double t0 = (P0 - line.Origin()).Dot(line.Direction());
  double t1 = (P1 - line.Origin()).Dot(line.Direction());
  if (compare(t0, t1) > 0) {
    return Reversed();
  }

  return *this;
}

std::optional<LineSegment2D> LineSegment2D::Overlap(Ray2D const& ray) const {
  // if not parallel, there isn't an overlap
  if (!(P1 - P0).IsParallel(ray.Direction())) {
    return std::nullopt;
  }

  bool ray_has_first = ray.Contains(P0);
  bool ray_has_last = ray.Contains(P1);

  if (!ray_has_first && !ray_has_last) {
    return std::nullopt;
  }
  if (ray_has_first && ray_has_last) {
    return *this;
  }

  // one endpoint on the ray: overlap runs from ray.Origin() to that endpoint
  Point2D const& end_pt = ray_has_first ? P0 : P1;
  if (ray.Origin().AlmostEquals(end_pt)) {
    return std::nullopt;  // single-point touch
  }
  return LineSegment2D::Make(ray.Origin(), end_pt);
}

std::optional<LineSegment2D> LineSegment2D::Overlap(LineSegment2D const& other) const {
  // if not parallel, there isn't an overlap
  if (!(P1 - P0).IsParallel(other.P1 - other.P0)) {
    return std::nullopt;
  }

  // collect all endpoints that lie inside both segments
  std::vector<Point2D> candidates;
  if (other.Contains(P0)) {
    candidates.push_back(P0);
  }
  if (other.Contains(P1)) {
    candidates.push_back(P1);
  }
  if (Contains(other.First())) {
    candidates.push_back(other.First());
  }
  if (Contains(other.Last())) {
    candidates.push_back(other.Last());
  }
  if (candidates.empty()) {
    return std::nullopt;
  }

  // find the two extremes using this segment's parametric axis
  Point2D lo = candidates[0], hi = candidates[0];
  double lo_t = Location(lo), hi_t = lo_t;
  for (auto const& c : candidates) {
    double t = Location(c);
    if (compare(t, lo_t) < 0) {
      lo_t = t;
      lo = c;
    }
    if (compare(t, hi_t) > 0) {
      hi_t = t;
      hi = c;
    }
  }
  if (lo.AlmostEquals(hi)) {
    return std::nullopt;  // single-point touch
  }
  return LineSegment2D::Make(lo, hi);
}

std::optional<std::vector<LineSegment2D>> LineSegment2D::Overlap(Polyline2D const& polyline) const {
  return polyline.Overlap(*this);
}

bool LineSegment2D::Touches(Line2D const& line) const { return Touch(line).has_value(); }
bool LineSegment2D::Touches(Ray2D const& ray) const { return Touch(ray).has_value(); }
bool LineSegment2D::Touches(LineSegment2D const& seg) const { return Touch(seg).has_value(); }
bool LineSegment2D::Touches(Polyline2D const& polyline) const { return polyline.Touches(*this); }

std::optional<Point2D> LineSegment2D::Touch(Line2D const& line) const {
  if (!(P1 - P0).IsParallel(line.Direction())) {
    if (line.Contains(P0)) {
      return P0;
    }
    if (line.Contains(P1)) {
      return P1;
    }
  }
  return std::nullopt;
}

std::optional<Point2D> LineSegment2D::Touch(Ray2D const& ray) const {
  // ray contains first or ray contains second, but not both
  bool r_has_first = ray.Contains(P0);
  bool r_has_last = ray.Contains(P1);
  if (r_has_first ^ r_has_last) {
    return r_has_first ? P0 : P1;
  }

  // otherwise, not parallel and segment contains ray origin
  if (!(P1 - P0).IsParallel(ray.Direction())) {
    if (Contains(ray.Origin())) {
      return ray.Origin();
    }
  }

  return std::nullopt;
}

std::optional<Point2D> LineSegment2D::Touch(LineSegment2D const& seg) const {
  // not parallel and only one point in common
  if (!(P1 - P0).IsParallel(seg.P1 - seg.P0)) {
    bool s_has_p0 = seg.Contains(P0), s_has_p1 = seg.Contains(P1);
    bool t_has_s0 = Contains(seg.First()), t_has_s1 = Contains(seg.Last());
    if (s_has_p0 ^ s_has_p1) {
      return s_has_p0 ? P0 : P1;
    }
    if (t_has_s0 ^ t_has_s1) {
      return t_has_s0 ? seg.First() : seg.Last();
    }
    return std::nullopt;
  }

  // parallel, but not overlap, and one extremity in common
  bool e00 = P0.AlmostEquals(seg.First()), e11 = P1.AlmostEquals(seg.Last());
  bool e01 = P0.AlmostEquals(seg.Last()), e10 = P1.AlmostEquals(seg.First());
  if (e00 ^ e11 ^ e01 ^ e10) {
    if (e00 || e01) {
      return P0;
    }
    return P1;
  }
  return std::nullopt;
}

std::optional<std::vector<Point2D>> LineSegment2D::Touch(Polyline2D const& polyline) const {
  return polyline.Touch(*this);
}

#pragma endregion

#pragma region Formatting

std::string LineSegment2D::ToWkt() const {
  return std::format("LINESTRING ({} {}, {} {})", round(P0.x()), round(P0.y()), round(P1.x()), round(P1.y()));
}

LineSegment2D LineSegment2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINESTRING") {
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

    return Make({nums_p1[0], nums_p1[1]}, {nums_p2[0], nums_p2[1]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void LineSegment2D::ToFile(std::string const& path) const {
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

LineSegment2D LineSegment2D::FromFile(std::string const& path) {
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
