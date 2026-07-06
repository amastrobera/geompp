#include "polygon2d.hpp"

#include "bbox2d.hpp"
#include "calc_utils2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Polygon2D Polygon2D::Make(std::vector<Point2D> const& points) {
  auto unique_points = remove_collinear(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points  are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  double perimeter = 0;
  int n0 = unique_points.size();
  for (int i = 0; i < n0; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % n0]);
  }

  bool is_poly_convex = detail::is_convex(unique_points, {});

  return {unique_points, perimeter, is_poly_convex};
}

Polygon2D Polygon2D::Make(std::vector<Point2D> const& points, std::vector<std::vector<Point2D>> const& holes) {
  auto unique_points =
      remove_collinear(remove_consecutive_duplicates(points));  // remove duplicates and collinear points

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  std::vector<std::vector<Point2D>> unique_holes_points;
  for (auto const& hole : holes) {
    auto unique_hole_points = remove_collinear(remove_consecutive_duplicates(hole));

    if (unique_hole_points.size() < 3) {
      throw std::runtime_error(std::format(
          "cannot create hole with less than 3 unique points; points are too close with {} decimals precision",
          DECIMAL_PRECISION));
    }

    if (!are_cw(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes in anti-clock-wise order");
    }

    unique_holes_points.push_back(unique_hole_points);
  }

  double perimeter = 0;
  int nh = unique_points.size();
  for (int i = 0; i < nh; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % nh]);
  }

  bool is_poly_convex = detail::is_convex(unique_points, unique_holes_points);

  return {unique_points, perimeter, unique_holes_points, is_poly_convex};
}

Polygon2D& Polygon2D::operator=(Polygon2D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
    HOLES = other.HOLES;
    PERIMETER = other.PERIMETER;
    IS_CONVEX = other.IS_CONVEX;
  }
  return *this;
}

bool Polygon2D::AlmostEquals(Polygon2D const& other, double epsilon) const {
  // size comparison of loops
  if (Size() != other.Size() || HOLES.size() != other.HOLES.size()) {
    return false;
  }
  for (std::size_t i = 0; i < HOLES.size(); ++i) {
    if (HOLES[i].size() != other.HOLES[i].size()) {
      return false;
    }
  }

  // outer loop vertices comparison
  for (std::size_t i = 0; i < VERTICES.size(); ++i) {
    if (!VERTICES[i].AlmostEquals(other[i], epsilon)) {
      return false;
    }
  }

  // inner loops vertices comparison
  for (std::size_t i = 0; i < HOLES.size(); ++i) {
    for (std::size_t j = 0; j < HOLES[i].size(); ++j) {
      if (!HOLES[i][j].AlmostEquals(other.HOLES[i][j], epsilon)) {
        return false;
      }
    }
  }
  return true;
}

SegmentRange2D Polygon2D::ToSegments() const { return SegmentRange2D(VERTICES, true); }

Point2D Polygon2D::Centroid() const {
  Point2D cs = centroid(VERTICES);
  double sa = signed_area(VERTICES);

  if (HOLES.empty()) {
    return cs;
  }

  // weighted average: c = Σ(aᵢ·cᵢ) / Σ(aᵢ)  — hole areas are negative (CW) so they subtract
  double total_sa = sa;
  double wx = sa * cs.x();
  double wy = sa * cs.y();

  for (auto const& hole : HOLES) {
    double sa_h = signed_area(hole);
    Point2D c_h = centroid(hole);
    total_sa += sa_h;
    wx += sa_h * c_h.x();
    wy += sa_h * c_h.y();
  }

  return Point2D(wx / total_sa, wy / total_sa);
}

double Polygon2D::Area() const {
  // outer loop
  double area = signed_area(VERTICES);  // this is guaranteed to be positive by the constructor

  // remove the areas of holes (inner loops)
  for (auto const& hole : HOLES) {
    area += signed_area(
        hole);  // holes are checked to be CW (guaranteed by constructor), therefore this area WILL be negative
  }

  return area;
}

double Polygon2D::Perimeter() const { return PERIMETER; }

double Polygon2D::DistanceTo(Point2D const& point) const { throw std::runtime_error("not implemented"); }

bool Polygon2D::IsSimple() const {
  if (detail::has_intersections_impl(ToSegments())) {
    return false;
  }

  for (auto const& hole : HOLES) {
    if (detail::has_intersections_impl(SegmentRange2D(hole, true))) {
      return false;
    }
  }

  return true;
}

Polygon2D Polygon2D::ConvexHull() {
  auto cv_indices = detail::convex_hull_indices(VERTICES);
  std::vector<Point2D> cv_points;
  cv_points.reserve(cv_indices.size());
  for (std::size_t i : cv_indices) {
    cv_points.emplace_back(VERTICES[i]);
  }
  return Make(cv_points);
}

std::vector<Point2D> const& Polygon2D::ToPoints() const { return VERTICES; }

std::vector<Polygon2D> Polygon2D::Simplify() const {
  if (IsSimple()) {
    return {*this};
  }

  auto rings = detail::simplify_rings_impl(VERTICES, HOLES, View2D::XY());

  // The half-edge walk traces interior faces with CW orientation (SA < 0) and the outer
  // (unbounded) graph face with CCW orientation (SA > 0).  Flip each CW interior ring to
  // CCW to get valid outer-ring candidates; discard CCW rings (outer graph face).
  std::vector<std::vector<Point2D>> candidates;
  std::vector<double> candidate_areas;
  for (auto const& ring : rings) {
    if (ring.size() < 3) {
      continue;
    }
    try {
      double sa = signed_area(ring);
      if (compare(sa, 0.0) < 0) {
        // CW interior face → flip to CCW
        auto ccw = ring;
        std::reverse(ccw.begin(), ccw.end());
        candidate_areas.push_back(-sa);
        candidates.push_back(std::move(ccw));
      }
      // CCW rings (SA > 0): outer graph face → discard
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "Simplify: skipping degenerate ring (" << ring.size() << " pts): " << e.what();
    }
  }

  // Sort candidates by area descending so larger rings come first
  std::vector<int> order(candidates.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](int a, int b) { return candidate_areas[a] > candidate_areas[b]; });
  {
    std::vector<std::vector<Point2D>> sorted_c(candidates.size());
    std::vector<double> sorted_a(candidate_areas.size());
    for (int k = 0; k < static_cast<int>(order.size()); ++k) {
      sorted_c[k] = std::move(candidates[order[k]]);
      sorted_a[k] = candidate_areas[order[k]];
    }
    candidates = std::move(sorted_c);
    candidate_areas = std::move(sorted_a);
  }

  // Pre-build polygons once so hole assignment doesn't reconstruct them per pair.
  int nc = static_cast<int>(candidates.size());
  std::vector<std::optional<Polygon2D>> candidate_polys(nc);
  for (int i = 0; i < nc; ++i) {
    try {
      candidate_polys[i] = Polygon2D::Make(candidates[i]);
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "Simplify: could not build polygon from ring " << i << ": " << e.what();
    }
  }

  // Hole assignment: if a smaller candidate is entirely inside a larger one it becomes a
  // hole of the larger one (and is reversed back to CW for Polygon2D::Make).
  std::vector<bool> is_hole(nc, false);
  std::vector<std::vector<std::vector<Point2D>>> outer_holes(nc);

  for (int i = nc - 1; i >= 0; --i) {
    if (!candidate_polys[i].has_value()) {
      continue;
    }
    Point2D test_pt = centroid(candidates[i]);
    for (int j = 0; j < i; ++j) {
      if (!candidate_polys[j].has_value()) {
        continue;
      }
      if (candidate_polys[j]->Contains(test_pt)) {
        auto cw_hole = candidates[i];
        std::reverse(cw_hole.begin(), cw_hole.end());
        outer_holes[j].push_back(std::move(cw_hole));
        is_hole[i] = true;
        break;
      }
    }
  }

  // Assemble result polygons — reuse pre-built polygons when there are no holes.
  std::vector<Polygon2D> results;
  for (int i = 0; i < nc; ++i) {
    if (is_hole[i] || !candidate_polys[i].has_value()) {
      continue;
    }
    if (outer_holes[i].empty()) {
      results.push_back(std::move(*candidate_polys[i]));
    } else {
      try {
        results.push_back(Polygon2D::Make(candidates[i], outer_holes[i]));
      } catch (std::runtime_error const& e) {
        GEOMPP_LOG(WARNING) << "Simplify: could not assemble polygon from ring " << i << ": " << e.what();
      }
    }
  }
  return results;
}

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

bool Polygon2D::IsOnPerimeter(Point2D const& point) const {
  return detail::is_on_perimeter_with_view(VERTICES, HOLES, View2D::XY(), point.x(), point.y());
}

// winding number method — boundary-inclusive (matches Triangle behaviour)
bool Polygon2D::Contains(Point2D const& point) const {
  // quick rejection (outside of bounding box)
  if (!BBox2D(*this).Contains(point)) {
    return false;
  }

  // containment on boundaries (slow?)
  if (IsOnPerimeter(point)) {
    return true;
  }

  return detail::polygon_contains_with_view(VERTICES, HOLES, View2D::XY(), point.x(), point.y());
}

bool Polygon2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool Polygon2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon2D::Intersects(LineSegment2D const& segment) const { return Intersection(segment).has_value(); }

std::optional<std::vector<LineSegment2D>> Polygon2D::Intersection(Line2D const& line) const {
  auto const& p0 = line.First();
  auto const& p1 = line.Last();

  auto intervals = detail::compute_intersection_intervals_2d(VERTICES, HOLES, IS_CONVEX, p0, p1, View2D::XY());

  if (intervals.empty()) {
    return std::nullopt;
  }
  // if they exist, they are guaranteed to be of even number, sorted, non duplicated
  // and also te < tl, for each pair te = t(i), tl = t(i+1)

  auto eval = [&](double t) { return Point2D(p0.x() + t * (p1.x() - p0.x()), p0.y() + t * (p1.y() - p0.y())); };

  std::vector<LineSegment2D> segs;
  for (auto const& [te, tl] : intervals) {
    segs.push_back(LineSegment2D::Make(eval(te), eval(tl)));
  }
  return segs;
}

std::optional<std::vector<LineSegment2D>> Polygon2D::Intersection(Ray2D const& ray) const {
  Point2D const p0 = ray.Origin();
  Point2D const p1(p0.x() + ray.Direction().x(), p0.y() + ray.Direction().y());

  auto intervals = detail::compute_intersection_intervals_2d(VERTICES, HOLES, IS_CONVEX, p0, p1, View2D::XY());

  if (intervals.empty()) {
    return std::nullopt;
  }
  // if they exist, they are guaranteed to be of even number, sorted, non duplicated
  // and also te < tl, for each pair te = t(i), tl = t(i+1)

  auto eval = [&](double t) { return Point2D(p0.x() + t * (p1.x() - p0.x()), p0.y() + t * (p1.y() - p0.y())); };

  std::vector<LineSegment2D> intersection_list;

  // Clip to ray domain [0, +inf)
  for (auto const& [te, tl] : intervals) {
    auto is_te_valid = compare(te, 0.0) >= 0;
    auto is_tl_valid = compare(tl, 0.0) >= 0;

    // case 1: both intersections are behind the ray
    if (!is_te_valid && !is_tl_valid) {
      continue;
    }

    // case 2: only the last point of the pair is valid for the ray
    //         the ray is inside the polygon
    if (!is_te_valid && is_tl_valid) {
      if (compare(tl, 0) > 0) {
        intersection_list.push_back(LineSegment2D::Make(eval(0.0), eval(tl)));
      }
      continue;
    }

    // case 3: both te and tl are valid points, make a segment
    // compute_parametric_intersection_intervals guarantees te < t1
    intersection_list.push_back(LineSegment2D::Make(eval(te), eval(tl)));
  }

  if (!intersection_list.empty()) {
    return intersection_list;
  }

  return std::nullopt;
}

std::optional<std::vector<LineSegment2D>> Polygon2D::Intersection(LineSegment2D const& other) const {
  auto const& p0 = other.First();
  auto const& p1 = other.Last();

  auto intervals = detail::compute_intersection_intervals_2d(VERTICES, HOLES, IS_CONVEX, p0, p1, View2D::XY());

  if (intervals.empty()) {
    return std::nullopt;
  }
  // if they exist, they are guaranteed to be of even number, sorted, non duplicated
  // and also te < tl, for each pair te = t(i), tl = t(i+1)

  auto eval = [&](double t) { return Point2D(p0.x() + t * (p1.x() - p0.x()), p0.y() + t * (p1.y() - p0.y())); };

  std::vector<LineSegment2D> intersection_list;

  // Clip to segment domain [0, 1]
  for (auto const& [te, tl] : intervals) {
    auto te_ge_0 = compare(te, 0.0) >= 0;
    auto te_se_1 = compare(te, 1.0) <= 0;
    auto tl_ge_0 = compare(tl, 0.0) >= 0;
    auto tl_se_1 = compare(tl, 1.0) <= 0;

    // case 1: the whole segment is inside the polygon
    if ((!te_ge_0 && te_se_1) && (tl_ge_0 && !tl_se_1)) {
      intersection_list.push_back(LineSegment2D::Make(eval(0.0), eval(1.0)));
      continue;
    }

    // case 2: the segment finishes inside the polygon
    if ((te_ge_0 && te_se_1) && (tl_ge_0 && !tl_se_1)) {
      if (compare(te, 1.0) < 0) {
        intersection_list.push_back(LineSegment2D::Make(eval(te), eval(1.0)));
      }
      continue;
    }

    // case 3: the segment starts inside the polygon
    if ((!te_ge_0 && te_se_1) && (tl_ge_0 && tl_se_1)) {
      if (compare(tl, 0.0) > 0) {
        intersection_list.push_back(LineSegment2D::Make(eval(0.0), eval(tl)));
      }
      continue;
    }

    // case 4: proper intersection
    if ((te_ge_0 && te_se_1) && (tl_ge_0 && tl_se_1)) {
      // compute_parametric_intersection_intervals guarantees te < t1
      intersection_list.push_back(LineSegment2D::Make(eval(te), eval(tl)));
      continue;
    }

    // no intersection to report
  }

  if (!intersection_list.empty()) {
    return intersection_list;
  }

  return std::nullopt;
}

#pragma endregion

#pragma region Formatting

std::string Polygon2D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYGON ";
  int num_verts = VERTICES.size();
  if (!num_verts) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "(";

  // outer loop
  {
    buf << "(";
    for (int i = 0; i < num_verts; ++i) {
      buf << std::format("{} {}, ", round(VERTICES[i].x()), round(VERTICES[i].y()));
    }
    buf << std::format("{} {}", round(VERTICES[0].x()), round(VERTICES[0].y()));
    buf << ")";
  }

  // inner loops
  {
    for (auto const& hole : HOLES) {
      buf << ", (";
      std::size_t n = hole.size();
      for (int i = 0; i < n; ++i) {
        buf << std::format("{} {}, ", round(hole[i].x()), round(hole[i].y()));
      }
      buf << std::format("{} {}", round(hole[0].x()), round(hole[0].y()));
      buf << ")";
    }
  }

  buf << ")";

  return buf.str();
  ;
}

Polygon2D Polygon2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POLYGON") {
      throw std::runtime_error("geometry name");
    }

    std::size_t outer_close = wkt.rfind(')');
    if (outer_close == std::string::npos) {
      throw std::runtime_error("brakets (outer close)");
    }

    // Content between outermost parens: e.g. "(x y, ...), (hx hy, ...)"
    std::string content = wkt.substr(end_gtype + 1, outer_close - end_gtype - 1);

    // Parse each ring by scanning for '(' ... ')' pairs
    std::vector<std::vector<Point2D>> rings;
    std::size_t pos = 0;
    while (pos < content.size()) {
      std::size_t ring_open = content.find('(', pos);
      if (ring_open == std::string::npos) {
        break;
      }
      std::size_t ring_close = content.find(')', ring_open);
      if (ring_close == std::string::npos) {
        throw std::runtime_error("brakets (ring close)");
      }

      std::string ring_str = content.substr(ring_open + 1, ring_close - ring_open - 1);
      std::vector<Point2D> ring;
      for (std::string const& tok : geompp::tokenize_string(ring_str, ',')) {
        std::string trimmed = geompp::trim(tok);
        auto nums = geompp::tokenize_to_doubles(trimmed);
        if (nums.size() != 2) {
          throw std::runtime_error("numbers");
        }
        ring.emplace_back(nums[0], nums[1]);
      }
      // WKT rings close by repeating the first point — drop it before passing to Make
      if (ring.size() > 1 && ring.back().AlmostEquals(ring.front())) {
        ring.pop_back();
      }
      rings.push_back(ring);
      pos = ring_close + 1;
    }

    if (rings.empty()) {
      throw std::runtime_error("no rings");
    }

    std::vector<Point2D> outer_ring = rings[0];
    std::vector<std::vector<Point2D>> holes(rings.begin() + 1, rings.end());
    return Make(outer_ring, holes);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

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
