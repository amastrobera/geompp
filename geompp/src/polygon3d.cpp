#include "polygon3d.hpp"

#include "bbox3d.hpp"
#include "calc_utils2d.hpp"
#include "calc_utils3d.hpp"
#include "line3d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Polygon3D Polygon3D::Make(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_coplanar(unique_points)) {
    throw std::runtime_error("cannot create polygon with non-coplanar points");
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }
  auto outer_plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);

  double perimeter = 0;
  int n0 = unique_points.size();
  for (int i = 0; i < n0; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % n0]);
  }

  bool is_poly_convex = is_convex(unique_points, {}, outer_plane.normal());

  return {unique_points, outer_plane, perimeter, is_poly_convex};
}

Polygon3D Polygon3D::Make(std::vector<Point3D> const& points, std::vector<std::vector<Point3D>> const& holes) {
  auto unique_points =
      remove_collinear(remove_consecutive_duplicates(points));  // remove duplicates and collinear points

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_coplanar(unique_points)) {
    throw std::runtime_error("cannot create polygon with non-coplanar points");
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  auto outer_plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);

  std::vector<std::vector<Point3D>> unique_holes_points;
  for (auto const& hole : holes) {
    auto unique_hole_points = remove_collinear(remove_consecutive_duplicates(hole));

    if (unique_hole_points.size() < 3) {
      throw std::runtime_error(std::format(
          "cannot create hole with less than 3 unique points; points are too close with {} decimals precision",
          DECIMAL_PRECISION));
    }

    if (!are_coplanar(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes non-coplanar points");
    }

    if (!are_cw(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes in anti-clock-wise order");
    }

    // verify that holes are on the same plane as the outer loop
    for (int i = 0; i < unique_hole_points.size(); ++i) {
      if (!outer_plane.Contains(unique_hole_points[i])) {
        throw std::runtime_error("cannot create polygon holes that are not on the same plane as the outer loop");
      }
    }

    unique_holes_points.push_back(unique_hole_points);
  }

  double perimeter = 0;
  int nh = unique_points.size();
  for (int i = 0; i < nh; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % nh]);
  }

  bool is_poly_convex = is_convex(unique_points, unique_holes_points, outer_plane.normal());

  return {unique_points, outer_plane, perimeter, unique_holes_points, is_poly_convex};
}

Polygon3D& Polygon3D::operator=(Polygon3D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
    HOLES = other.HOLES;
    PLANE = other.PLANE;
    PERIMETER = other.PERIMETER;
    IS_CONVEX = other.IS_CONVEX;
  }
  return *this;
}

bool Polygon3D::AlmostEquals(Polygon3D const& other, double epsilon) const {
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

SegmentRange3D Polygon3D::ToSegments() const { return SegmentRange3D(VERTICES, true); }

Point3D Polygon3D::Centroid() const {
  Point3D cs = centroid(VERTICES, PLANE);
  double sa = signed_area(VERTICES, PLANE);

  if (HOLES.empty()) {
    return cs;
  }

  // weighted average: c = Σ(aᵢ·cᵢ) / Σ(aᵢ)  — hole areas are negative (CW) so they subtract
  double total_sa = sa;
  double wx = sa * cs.x();
  double wy = sa * cs.y();
  double wz = sa * cs.z();

  for (auto const& hole : HOLES) {
    double sa_h = signed_area(hole, PLANE);
    Point3D c_h = centroid(hole, PLANE);
    total_sa += sa_h;
    wx += sa_h * c_h.x();
    wy += sa_h * c_h.y();
    wz += sa_h * c_h.z();
  }

  return Point3D(wx / total_sa, wy / total_sa, wz / total_sa);
}

double Polygon3D::Area() const {
  double area = signed_area(VERTICES, PLANE);  // guaranteed to be positive by construction
  for (auto const& hole : HOLES) {
    area += signed_area(hole, PLANE);  // guaranteed to be negative by construction, so we add it
  }
  return area;
}

double Polygon3D::Perimeter() const { return PERIMETER; }

bool Polygon3D::IsSimple() const {
  Axis dax = PLANE.normal().DominantAxis();

  auto make_segs = [dax](std::vector<Point3D> const& ring) {
    auto to2d = [dax](Point3D const& p) -> Point2D {
      if (dax == Axis::X) {
        return Point2D(p.y(), p.z());
      }
      if (dax == Axis::Y) {
        return Point2D(p.z(), p.x());
      }
      return Point2D(p.x(), p.y());
    };
    std::vector<LineSegment2D> segs;
    segs.reserve(ring.size());
    int n = (int)ring.size();
    for (int i = 0; i < n; ++i) {
      segs.push_back(LineSegment2D::Make(to2d(ring[i]), to2d(ring[(i + 1) % n])));
    }
    return segs;
  };

  if (has_intersections(make_segs(VERTICES))) {
    return false;
  }
  for (auto const& hole : HOLES) {
    if (has_intersections(make_segs(hole))) {
      return false;
    }
  }
  return true;
}


std::vector<Polygon3D> Polygon3D::Simplify() const {
  if (IsSimple()) {
    return {*this};
  }

  Vector3D n = PLANE.normal();
  Axis dax = n.DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  // Plane equation n·p = d (used for back-projection of intersection points)
  Point3D orig = PLANE.origin();
  double d = n.x() * orig.x() + n.y() * orig.y() + n.z() * orig.z();

  auto from2d = [&n, d, dax](Point2D const& p) -> Point3D {
    if (dax == Axis::X) {
      return Point3D((d - n.y() * p.x() - n.z() * p.y()) / n.x(), p.x(), p.y());
    }
    if (dax == Axis::Y) {
      return Point3D(p.y(), (d - n.z() * p.x() - n.x() * p.y()) / n.y(), p.x());
    }
    return Point3D(p.x(), p.y(), (d - n.x() * p.x() - n.y() * p.y()) / n.z());
  };

  auto rings2d = simplify_rings_impl(VERTICES, HOLES, view);

  // Detect whether the dominant-axis projection reverses chirality.
  // For Y-dominant the mapping (z,x) mirrors the coordinate system, so a CCW 3D polygon
  // projects to CW in 2D.  Check the outer ring's projected signed area to find out.
  std::vector<Point2D> outer2d;
  outer2d.reserve(VERTICES.size());
  for (auto const& v : VERTICES) {
    outer2d.emplace_back(view.x(v), view.y(v));
  }
  bool projection_flips = compare(signed_area(outer2d), 0.0) < 0;

  // Interior faces come out CW (SA < 0) and the outer graph face CCW (SA > 0) — unless
  // the projection flips chirality, in which case the signs are reversed.
  // Either way, candidates must end up CCW in 2D for Polygon2D::Make / hole assignment.
  std::vector<std::vector<Point2D>> candidates2d;
  std::vector<double> candidate_areas;
  for (auto const& ring : rings2d) {
    if (ring.size() < 3) {
      continue;
    }
    try {
      double sa = signed_area(ring);
      bool is_interior = projection_flips ? compare(sa, 0.0) > 0 : compare(sa, 0.0) < 0;
      if (is_interior) {
        if (projection_flips) {
          // Ring is already CCW in 2D (SA > 0 means CCW)
          candidate_areas.push_back(sa);
          candidates2d.push_back(ring);
        } else {
          // Ring is CW in 2D (SA < 0), flip to CCW
          auto ccw = ring;
          std::reverse(ccw.begin(), ccw.end());
          candidate_areas.push_back(-sa);
          candidates2d.push_back(std::move(ccw));
        }
      }
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "Simplify: skipping degenerate ring (" << ring.size() << " pts): " << e.what();
    }
  }

  // Sort candidates by area descending (largest first)
  std::vector<int> order(candidates2d.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(),
            [&](int a, int b) { return candidate_areas[a] > candidate_areas[b]; });
  {
    std::vector<std::vector<Point2D>> sorted_c(candidates2d.size());
    std::vector<double> sorted_a(candidate_areas.size());
    for (int k = 0; k < static_cast<int>(order.size()); ++k) {
      sorted_c[k] = std::move(candidates2d[order[k]]);
      sorted_a[k] = candidate_areas[order[k]];
    }
    candidates2d = std::move(sorted_c);
    candidate_areas = std::move(sorted_a);
  }

  int nc = static_cast<int>(candidates2d.size());
  std::vector<std::optional<Polygon2D>> candidate_polys(nc);
  for (int i = 0; i < nc; ++i) {
    try {
      candidate_polys[i] = Polygon2D::Make(candidates2d[i]);
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "Simplify: could not build polygon from ring " << i << ": " << e.what();
    }
  }

  std::vector<bool> is_hole(nc, false);
  std::vector<std::vector<std::vector<Point2D>>> outer_holes2d(nc);

  for (int i = nc - 1; i >= 0; --i) {
    if (!candidate_polys[i].has_value()) {
      continue;
    }
    Point2D test_pt = centroid(candidates2d[i]);
    for (int j = 0; j < i; ++j) {
      if (!candidate_polys[j].has_value()) {
        continue;
      }
      if (candidate_polys[j]->Contains(test_pt)) {
        auto cw_hole = candidates2d[i];
        std::reverse(cw_hole.begin(), cw_hole.end());
        outer_holes2d[j].push_back(std::move(cw_hole));
        is_hole[i] = true;
        break;
      }
    }
  }

  // Unproject to 3D and assemble.
  // Candidates are CCW in the 2D projection.  Polygon3D::Make computes its own plane
  // and CCW check, so if the back-projected ring is CW in 3D we reverse and retry.
  std::vector<Polygon3D> results;
  for (int i = 0; i < nc; ++i) {
    if (is_hole[i]) {
      continue;
    }

    std::vector<Point3D> outer3d;
    for (auto const& p : candidates2d[i]) {
      outer3d.push_back(from2d(p));
    }

    std::vector<std::vector<Point3D>> holes3d;
    for (auto const& h2d : outer_holes2d[i]) {
      std::vector<Point3D> h3d;
      for (auto const& p : h2d) {
        h3d.push_back(from2d(p));
      }
      holes3d.push_back(std::move(h3d));
    }

    for (int attempt = 0; attempt < 2; ++attempt) {
      try {
        if (holes3d.empty()) {
          results.push_back(Polygon3D::Make(outer3d));
        } else {
          results.push_back(Polygon3D::Make(outer3d, holes3d));
        }
        break;
      } catch (std::runtime_error const& e) {
        if (attempt == 0) {
          std::reverse(outer3d.begin(), outer3d.end());
          for (auto& h : holes3d) {
            std::reverse(h.begin(), h.end());
          }
        } else {
          GEOMPP_LOG(WARNING) << "Simplify: could not assemble 3D polygon from ring " << i << ": " << e.what();
        }
      }
    }
  }
  return results;
}

Polygon3D Polygon3D::ConvexHull() {
  auto cv_indices = convex_hull_indices(VERTICES);
  std::vector<Point3D> cv_points;
  cv_points.reserve(cv_indices.size());
  for (std::size_t i : cv_indices) {
    cv_points.emplace_back(VERTICES[i]);
  }
  return Make(cv_points);
}

std::vector<Point3D> Polygon3D::ToPoints() {
  std::vector<Point3D> points;
  points.reserve(VERTICES.size());
  for (auto pt : VERTICES) {
    points.emplace_back(pt);
  }
  return points;
}

double Polygon3D::DistanceTo(Point3D const& point) const { throw std::runtime_error("not implemented"); }

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

bool Polygon3D::IsOnBoundary(Point3D const& point) const {
  if (!PLANE.Contains(point)) {
    return false;
  }

  auto project_view = VERTICES | std::views::transform([&](auto const& p) { return PLANE.ProjectInto(p); });
  std::vector<Point2D> outer2d(project_view.begin(), project_view.end());

  std::vector<std::vector<Point2D>> inners2d;
  for (auto const& hole : HOLES) {
    auto project_view_h = hole | std::views::transform([&](auto const& p) { return PLANE.ProjectInto(p); });
    inners2d.push_back(std::vector<Point2D>(project_view_h.begin(), project_view_h.end()));
  }

  auto proj_poly = Polygon2D::Make(outer2d, inners2d);
  return proj_poly.IsOnBoundary(PLANE.ProjectInto(point));
}

bool Polygon3D::Contains(Point3D const& point) const {
  // quick rejection with bounding box
  if (!BBox3D(*this).Contains(point)) {
    return false;
  }

  // another quick rejection: if not on the plane, can't belong to the polygon
  if (!PLANE.Contains(point)) {
    return false;
  }

  // final test: project all in 2D, and verify in 2D
  auto project_view = VERTICES | std::views::transform([&](auto const& p) { return PLANE.ProjectInto(p); });
  std::vector<Point2D> outer2d(project_view.begin(), project_view.end());

  std::vector<std::vector<Point2D>> inners2d;
  for (auto const& hole : HOLES) {
    auto project_view_h = hole | std::views::transform([&](auto const& p) { return PLANE.ProjectInto(p); });
    inners2d.push_back(std::vector<Point2D>(project_view_h.begin(), project_view_h.end()));
  }

  auto proj_poly = Polygon2D::Make(outer2d, inners2d);

  return proj_poly.Contains(PLANE.ProjectInto(point));
}

bool Polygon3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Polygon3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon3D::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

Polygon3D::ReturnSet Polygon3D::Intersection(Line3D const& line) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(Ray3D const& ray) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(LineSegment3D const& other) const {
  throw std::runtime_error("not implemented");
}

#pragma endregion

#pragma region Formatting

std::string Polygon3D::ToWkt() const {
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
      buf << std::format("{} {} {}, ", round(VERTICES[i].x()), round(VERTICES[i].y()), round(VERTICES[i].z()));
    }
    buf << std::format("{} {} {}", round(VERTICES[0].x()), round(VERTICES[0].y()), round(VERTICES[0].z()));
    buf << ")";
  }

  // inner loops
  {
    for (auto const& hole : HOLES) {
      buf << ", (";
      std::size_t n = hole.size();
      for (int i = 0; i < n; ++i) {
        buf << std::format("{} {} {}, ", round(hole[i].x()), round(hole[i].y()), round(hole[i].z()));
      }
      buf << std::format("{} {} {}", round(hole[0].x()), round(hole[0].y()), round(hole[0].z()));
      buf << ")";
    }
  }

  buf << ")";

  return buf.str();
}

Polygon3D Polygon3D::FromWkt(std::string const& wkt) {
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

    // Content between outermost parens: e.g. "(x y z, ...), (hx hy hz, ...)"
    std::string content = wkt.substr(end_gtype + 1, outer_close - end_gtype - 1);

    // Parse each ring by scanning for '(' ... ')' pairs
    std::vector<std::vector<Point3D>> rings;
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
      std::vector<Point3D> ring;
      for (std::string const& tok : geompp::tokenize_string(ring_str, ',')) {
        std::string trimmed = geompp::trim(tok);
        auto nums = geompp::tokenize_to_doubles(trimmed);
        if (nums.size() != 3) {
          throw std::runtime_error("numbers");
        }
        ring.emplace_back(nums[0], nums[1], nums[2]);
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

    std::vector<Point3D> outer_ring = rings[0];
    std::vector<std::vector<Point3D>> holes(rings.begin() + 1, rings.end());
    return Make(outer_ring, holes);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

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
