#include "triangle2d.hpp"

#include "bbox2d.hpp"
#include "calc_utils2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace geompp {

namespace {

static bool within_axis_boundary(double s, double t) {
  return (compare(s, 0) >= 0 && compare(s, 1.0) <= 0) &&
         (compare(t, 0) >= 0 && compare(t, 1.0) <= 0 && compare(s + t, 1.0) <= 0);  // including borders
}

}  // namespace

#pragma region Constructors

Triangle2D Triangle2D::Make(Point2D const& p0, Point2D const& p1, Point2D const& p2) {
  auto unique_points = remove_duplicates({p0, p1, p2});

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format("points {}, {}, {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), p2.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1, p2};
}

Triangle2D& Triangle2D::operator=(Triangle2D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
    P2 = other.P2;
  }
  return *this;
}

bool Triangle2D::AlmostEquals(Triangle2D const& other, double epsilon) const {
  return P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon) && P2.AlmostEquals(other.P2, epsilon);
}

Point2D Triangle2D::Centroid() const { return average({P0, P1, P2}); }

Polygon2D Triangle2D::ToPolygon() const { return Polygon2D::Make({P0, P1, P2}); }

double Triangle2D::SignedArea() const {
  return ((P1 - P0).Cross(P2 - P0)) / 2.0;  // same as 1/2 perp-prod
}

double Triangle2D::Area() const { return std::abs(SignedArea()); }

double Triangle2D::Perimeter() const { return (P1 - P0).Length() + (P2 - P1).Length() + (P0 - P2).Length(); }

bool Triangle2D::IsCCW() const { return SignedArea() > 0; }

#pragma endregion

#pragma region line operations

double Triangle2D::DistanceTo(Point2D const& point) const {
  if (Contains(point)) {
    return 0.0;
  }

  // Contains() already ruled out interior and on-perimeter, so the closest point is somewhere on one
  // of the three edges — plain min over each edge's own DistanceTo(), same approach as
  // Polygon2D::DistanceTo (no convex fast path needed at just 3 edges).
  return std::min({LineSegment2D::Make(P0, P1).DistanceTo(point), LineSegment2D::Make(P1, P2).DistanceTo(point),
                   LineSegment2D::Make(P2, P0).DistanceTo(point)});
}

std::tuple<Vector2D, Vector2D> Triangle2D::ToAxis() const { return {P1 - P0, P2 - P0}; }

std::optional<Point2D> Triangle2D::Interpolate(double s, double t) const {
  if (!within_axis_boundary(s, t)) {
    GEOMPP_LOG(ERROR) << "(s, t) = (" << s << ", " << t << ") are not within boundaries [0, 1]";
    return std::nullopt;
  }

  return linear_combination({P0, P1, P2}, {1 - s - t, s, t});
}

std::optional<std::tuple<double, double>> Triangle2D::Location(Point2D const& point) const {
  auto u = P1 - P0;
  auto v = P2 - P0;
  auto w = point - P0;
  auto u_perp = u.Perp();
  auto v_perp = v.Perp();
  double s = w.Dot(v_perp) / u.Dot(v_perp);  // guaranteed non zero (triangle ctor)
  double t = w.Dot(u_perp) / v.Dot(u_perp);  // guaranteed non zero (triangle ctor)

  if (!within_axis_boundary(s, t)) {
    return std::nullopt;
  }
  return std::make_tuple(s, t);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Triangle2D const& lhs, Triangle2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Triangle2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Triangle2D::Contains(Point2D const& point) const {
  if (!BBox2D(*this).Contains(point)) {
    return false;
  }
  return Location(point).has_value();
}

bool Triangle2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }
bool Triangle2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }
bool Triangle2D::Intersects(LineSegment2D const& segment) const { return Intersection(segment).has_value(); }
bool Triangle2D::Intersects(Triangle2D const& other) const { return Intersection(other).has_value(); }

Triangle2D::ReturnSet Triangle2D::Intersection(Line2D const& line) const {
  auto points_view = std::vector<LineSegment2D>{LineSegment2D::Make(P0, P1), LineSegment2D::Make(P1, P2),
                                                LineSegment2D::Make(P2, P0)} |
                     std::views::transform([&](LineSegment2D const& seg) { return seg.Intersection(line); }) |
                     std::views::filter([](std::optional<Point2D> const& res) { return res.has_value(); }) |
                     std::views::transform([](std::optional<Point2D> const& res) { return *res; });

  std::vector<Point2D> intersections(points_view.begin(), points_view.end());

  // all points are the same as the triangle vertices,
  // so we consider it as no intersection but something else (touch, tangency or overlap)
  if (std::ranges::all_of(intersections, [&](Point2D const& p) { return p == P0 || p == P1 || p == P2; })) {
    return std::nullopt;
  }

  // sort them to get a line in the direction of the intersecting line
  std::sort(intersections.begin(), intersections.end(), [&](Point2D const& a, Point2D const& b) {
    return line.Origin().DistanceTo(a) < line.Origin().DistanceTo(b);
  });  // sort intersection points in the direction of the line

  std::vector<Point2D> unique_points = remove_consecutive_duplicates(intersections);

  if (unique_points.empty()) {
    return std::nullopt;
  }

  if (unique_points.size() == 1) {
    return unique_points[0];
  }

  return LineSegment2D::Make(unique_points[0], unique_points[1]);
}

Triangle2D::ReturnSet Triangle2D::Intersection(Ray2D const& ray) const {
  auto line_result = Intersection(ray.ToLine());

  if (!line_result.has_value()) {
    return std::nullopt;
  }

  // all points are the same as the triangle vertices,
  // so we consider it as no intersection but something else (touch, tangency or overlap)
  // --> already tested in line intersection, so we can skip it here

  // case: 1 intersection, a point
  if (std::holds_alternative<Point2D>(*line_result)) {
    Point2D const& p = std::get<Point2D>(*line_result);
    // giving for granted that the point IS on the ray, we only need to check if it's ahead of its origin
    if (ray.IsAhead(p)) {
      return p;
    }
    return std::nullopt;
  }

  // case 2: 2 intersections, a line segment
  if (std::holds_alternative<LineSegment2D>(*line_result)) {
    LineSegment2D const& seg = std::get<LineSegment2D>(*line_result);
    auto line_points = std::vector<Point2D>{seg.First(), seg.Last()};
    // giving for granted that the point IS on the ray, we only need to check if it's ahead of its origin
    line_points.erase(
        std::remove_if(line_points.begin(), line_points.end(), [&](Point2D const& p) { return !ray.IsAhead(p); }),
        line_points.end());

    if (line_points.empty()) {
      return std::nullopt;
    }

    // case 2.1: only 1 point is ahead of the ray, so we consider it as a point intersection
    if (line_points.size() == 1) {
      return line_points[0];
    }

    // case 2.2: both points are ahead of the ray, so we consider it as a line segment intersection (already sorted in
    // the direction of the ray from Line::Intersection)
    return seg;
  }

  // case 3: undefined
  throw std::runtime_error("unexpected result from line intersection");
}

Triangle2D::ReturnSet Triangle2D::Intersection(LineSegment2D const& segment) const {
  auto line_result = Intersection(segment.ToLine());

  if (!line_result.has_value()) {
    return std::nullopt;
  }

  // all points are the same as the triangle vertices,
  // so we consider it as no intersection but something else (touch, tangency or overlap)
  // --> already tested in line intersection, so we can skip it here

  // case: 1 intersection, a point
  if (std::holds_alternative<Point2D>(*line_result)) {
    Point2D const& p = std::get<Point2D>(*line_result);
    // giving for granted that the point IS on the ray, we only need to check if it's ahead of its origin
    if (segment.Contains(p)) {
      return p;
    }
    return std::nullopt;
  }

  // case 2: 2 intersections, a line segment
  if (std::holds_alternative<LineSegment2D>(*line_result)) {
    LineSegment2D const& seg = std::get<LineSegment2D>(*line_result);
    auto line_points = std::vector<Point2D>{seg.First(), seg.Last()};
    // giving for granted that the point IS on the ray, we only need to check if it's ahead of its origin
    line_points.erase(
        std::remove_if(line_points.begin(), line_points.end(), [&](Point2D const& p) { return !segment.Contains(p); }),
        line_points.end());

    if (line_points.empty()) {
      return std::nullopt;
    }

    // case 2.1: only 1 point is ahead of the ray, so we consider it as a point intersection
    if (line_points.size() == 1) {
      return line_points[0];
    }

    // case 2.2: both points are ahead of the ray, so we consider it as a line segment intersection (already sorted in
    // the direction of the ray from Line::Intersection)
    return seg;
  }

  // case 3: undefined
  throw std::runtime_error("unexpected result from line intersection");
}

Triangle2D::ReturnSet Triangle2D::Intersection(Triangle2D const& other) const {
  std::vector<Point2D> this_pts = {P0, P1, P2};
  std::vector<Point2D> other_pts = {other.P0, other.P1, other.P2};

  // clip() runs the general planar-arrangement boolean-intersection engine, no convex special-casing
  // needed (triangles are always convex). It already drops zero-area slivers internally, so a mere
  // touch (shared vertex, or edges meeting without any overlapping area) comes back as an empty result
  // rather than a degenerate 1- or 2-point "ring" — consistent with how Intersection(Line2D) above
  // treats an all-vertices touch as "no intersection" too.
  auto rings = clip(other_pts, this_pts);
  if (rings.empty()) {
    return std::nullopt;
  }

  // the intersection of two convex regions is itself convex and simply connected, so clip() can only
  // ever return a single outer ring here (no holes possible, unlike its general contract for arbitrary
  // loops).
  auto const& ring = rings[0];
  if (ring.size() == 3) {
    return Triangle2D::Make(ring[0], ring[1], ring[2]);
  }
  return Polygon2D::Make(ring);
}

#pragma endregion

#pragma region Formatting

std::string Triangle2D::ToWkt() const {
  // clang-format off
  return std::format("TRIANGLE ({} {}, {} {}, {} {})", 
                     round(P0.x()), round(P0.y()), 
                     round(P1.x()), round(P1.y()),
                     round(P2.x()), round(P2.y())
                     );
  // clang-format on
}

Triangle2D Triangle2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "TRIANGLE") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).rfind(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string mid_part = wkt.substr(end_gtype + 1, end_pn);

    std::vector<Point2D> pt_vec;
    for (std::string const& p_str : geompp::tokenize_string(mid_part, ',')) {
      std::string pt_trimmed = geompp::trim(p_str);
      auto nums = geompp::tokenize_to_doubles(pt_trimmed, ' ');
      if (nums.size() != 2) {
        throw std::runtime_error("numbers");
      }
      pt_vec.emplace_back(nums[0], nums[1]);
    }

    if (pt_vec.size() != 3) {
      throw std::runtime_error("initialized with n != 3 points");
    }

    return Make(pt_vec[0], pt_vec[1], pt_vec[2]);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Triangle2D::ToFile(std::string const& path) const {
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

Triangle2D Triangle2D::FromFile(std::string const& path) {
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
