#include "line_segment3d.hpp"

#include "calc_utils3d.hpp"
#include "line3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment3D LineSegment3D::Make(Point3D const& p0, Point3D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", DECIMAL_PRECISION,
                                         p0.ToWkt(), p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

LineSegment3D::LineSegment3D(Point3D const& p0, Point3D const& p1) : P0(p0), P1(p1) {}

LineSegment3D& LineSegment3D::operator=(LineSegment3D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
  }
  return *this;
}

double LineSegment3D::Length() const { return (P1 - P0).Length(); }

bool LineSegment3D::AlmostEquals(LineSegment3D const& other, double epsilon) const {
  return (P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon))  // forward
         ||                                                                          //
         (P0.AlmostEquals(other.P1, epsilon) && P1.AlmostEquals(other.P0, epsilon))  // backward
      ;  // no heading check (needed for edges in a mesh though)
}

Line3D LineSegment3D::ToLine() const { return Line3D::Make(P0, P1); }

#pragma endregion

#pragma region line operations

Point3D LineSegment3D::ProjectOnto(Point3D const& point) const {
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

double LineSegment3D::Location(Point3D const& point) const {
  if (!ToLine().Contains(point)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length() / Length();
}

Point3D LineSegment3D::Interpolate(double pct) const {
  // the point is behind the polyline
  if (compare(pct, 0) < 0) {
    return P0;
  }

  // the point is beyond the polyline
  if (compare(pct, 1.0) > 0) {
    return P1;
  }

  return P0 + pct * (P1 - P0);
}

LineSegment3D LineSegment3D::Flip() const { return {P1, P0}; }

double LineSegment3D::DistanceTo(Point3D const& point) const { return (point - ProjectOnto(point)).Length(); }

std::optional<LineSegment3D> LineSegment3D::Distance(Line3D const& line) const {
  // auto P0 = P0;
  // auto P1 = P1;
  auto Q0 = line.First();
  auto Q1 = line.Last();

  double sc, tc;
  distance_line_to_line(P0, P1, Q0, Q1, sc, tc);

  // boundary check for segment
  if (compare(sc, 0) < 0) {
    sc = 0;
  }
  if (compare(sc, 1.0) > 0) {
    sc = 1.0;
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

double LineSegment3D::DistanceTo(Line3D const& line) const {
  auto seg = Distance(line);
  if (seg.has_value()) {
    return seg->Length();
  }

  return 0.0;
}

std::optional<LineSegment3D> LineSegment3D::Distance(Ray3D const& ray) const {
  // auto P0 = P0;
  // auto P1 = P1;
  auto Q0 = ray.Origin();
  auto Q1 = ray.Origin() + ray.Direction();

  double sc, tc;
  distance_line_to_line(P0, P1, Q0, Q1, sc, tc);

  auto U = P1 - P0;
  auto V = Q1 - Q0;

  // Pre-clamp overlap check: segment/ray clamping below collapses the magic-zero
  // from distance_line_to_line in the collinear-overlap case.
  {
    auto raw_P = P0 + U * sc;
    auto raw_Q = Q0 + V * tc;
    if (compare(raw_P.DistanceTo(raw_Q), 0) == 0 && (Contains(ray.Origin()) || ray.Contains(P0) || ray.Contains(P1))) {
      return std::nullopt;
    }
  }

  // boundary check for segment + ray
  if (compare(sc, 0) < 0) {
    sc = 0;
  }
  if (compare(sc, 1.0) > 0) {
    sc = 1.0;
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

double LineSegment3D::DistanceTo(Ray3D const& ray) const {
  auto seg = Distance(ray);
  if (seg.has_value()) {
    return seg->Length();
  }

  return 0.0;
}

std::optional<LineSegment3D> LineSegment3D::Distance(LineSegment3D const& seg) const {
  // auto P0 = P0;
  // auto P1 = P1;
  auto Q0 = seg.First();
  auto Q1 = seg.Last();

  double sc, tc;
  distance_line_to_line(P0, P1, Q0, Q1, sc, tc);

  auto U = P1 - P0;
  auto V = Q1 - Q0;

  // Pre-clamp overlap check: segment clamping below collapses the magic-zero
  // from distance_line_to_line in the collinear-overlap case.
  {
    auto raw_P = P0 + U * sc;
    auto raw_Q = Q0 + V * tc;
    if (compare(raw_P.DistanceTo(raw_Q), 0) == 0 &&
        (Contains(seg.First()) || Contains(seg.Last()) || seg.Contains(P0) || seg.Contains(P1))) {
      return std::nullopt;
    }
  }

  // boundary check for both segments
  if (compare(sc, 0) < 0) {
    sc = 0;
  }
  if (compare(sc, 1.0) > 0) {
    sc = 1.0;
  }
  if (compare(tc, 0) < 0) {
    tc = 0;
  }
  if (compare(tc, 1.0) > 0) {
    tc = 1.0;
  }

  // verify if null distance (don't build a line segment)
  auto dseg_P0 = P0 + (U * sc);
  auto dseg_P1 = Q0 + (V * tc);
  if (compare(dseg_P0.DistanceTo(dseg_P1), 0) == 0) {
    return std::nullopt;  // they intersect or overlap
  }

  return LineSegment3D::Make(dseg_P0, dseg_P1);
}

double LineSegment3D::DistanceTo(LineSegment3D const& seg) const {
  auto dseg = Distance(seg);
  if (dseg.has_value()) {
    return dseg->Length();
  }

  return 0.0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment3D const& lhs, LineSegment3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, LineSegment3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment3D::Contains(Point3D const& point) const {
  double t = Location(point);
  return compare(t, 0) >= 0 && compare(t, 1.0) <= 0;
}

bool LineSegment3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool LineSegment3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool LineSegment3D::Intersects(LineSegment3D const& other) const { return Intersection(other).has_value(); }

LineSegment3D::ReturnSet LineSegment3D::Intersection(Line3D const& line) const {
  double sc, tc;
  auto Pc = intersection_line_to_line(P0, P1, line.First(), line.Last(), sc, tc);

  // respecting LineSegment and Ray constraints: sc should be between 0 and 1
  if (!(Pc.has_value() && is_in_range(sc, 0, 1))) {
    return std::nullopt;
  }

  return Pc;  // intersection!
}

LineSegment3D::ReturnSet LineSegment3D::Intersection(Ray3D const& ray) const {
  double sc, tc;
  auto Pc = intersection_line_to_line(P0, P1, ray.Origin(), ray.Origin() + ray.Direction(), sc, tc);

  // respecting LineSegment and Ray constraints: sc should be between 0 and 1, tc should be greater than 0
  if (!(Pc.has_value() && is_in_range(sc, 0, 1) && is_greater_or_equal(tc, 0))) {
    return std::nullopt;
  }

  return Pc;  // intersection!
}

LineSegment3D::ReturnSet LineSegment3D::Intersection(LineSegment3D const& other) const {
  double sc, tc;
  auto Pc = intersection_line_to_line(P0, P1, other.First(), other.Last(), sc, tc);

  // respecting LineSegment constraints: sc and tc should be between 0 and 1
  if (!(Pc.has_value() && is_in_range(sc, 0, 1) && is_in_range(tc, 0, 1))) {
    return std::nullopt;
  }

  return Pc;  // intersection!
}

#pragma endregion

#pragma region Formatting

std::string LineSegment3D::ToWkt() const {
  return std::format("LINESTRING ({} {} {}, {} {} {})", round(P0.x()), round(P0.y()), round(P0.z()), round(P1.x()),
                     round(P1.y()), round(P1.z()));
}

LineSegment3D LineSegment3D::FromWkt(std::string const& wkt) {
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

    return Make({nums_p1[0], nums_p1[1], nums_p1[2]}, {nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void LineSegment3D::ToFile(std::string const& path) const {
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

LineSegment3D LineSegment3D::FromFile(std::string const& path) {
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
