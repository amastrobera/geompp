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
#include <sstream>
#include <stdexcept>

namespace geompp {

namespace {

// Whether outer_plane's normal agrees with the canonical dominant-axis-positive direction
// are_ccw(unique_points) already validated the polygon's winding against. Cheap (O(1)) rather than a
// second are_ccw(unique_points, outer_plane) call (O(n), a full signed_area recomputation): every point in
// unique_points is already known coplanar (checked earlier in Make()), so ANY local 3-point normal —
// outer_plane's included — is exactly parallel or anti-parallel to that one true plane normal, never some
// other direction. That means outer_plane and the canonical reference always share the same dominant axis;
// only the sign can differ, so checking outer_plane's own dominant-axis component sign is sufficient.
bool plane_matches_canonical_winding(Plane const& outer_plane) {
  Vector3D n = outer_plane.normal();
  Axis dax = n.DominantAxis();
  double dominant_component = (dax == Axis::X) ? n.x() : (dax == Axis::Y) ? n.y() : n.z();
  return compare(dominant_component, 0.0) >= 0;
}

// True only for a genuine STRICT-interior crossing (each segment struck strictly between its own two
// endpoints, not at either one) — see Polygon2D::Make()'s identical helper (polygon2d.cpp) for the full
// rationale: a hole touching the outer boundary at a vertex, a T-junction, or running flush along part of
// it (collinear overlap) is a legitimate polygon-with-hole shape, not a "hole crosses outer" error.
bool strictly_crosses(LineSegment2D const& a, LineSegment2D const& b) {
  double sc, tc;
  auto pt = detail::line_intersection(a.First(), a.Last(), b.First(), b.Last(), sc, tc);
  return pt.has_value() && compare(sc, 0) > 0 && compare(sc, 1) < 0 && compare(tc, 0) > 0 && compare(tc, 1) < 0;
}

}  // namespace

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
  // Plane::From3Points(unique_points[0..2]) is a LOCAL quantity — the turn pivoting at vertex 0 — which
  // can point opposite the polygon's GLOBAL winding the are_ccw() check above just validated, whenever
  // vertex 0 is a reflex corner. Swap which two points go into From3Points when that happens (see
  // plane_matches_canonical_winding) — guarantees outer_plane's sign always matches the already-validated
  // CCW winding, so downstream code (Area(), boolean ops' shared-view projection) never has to work around
  // a possibly-wrong sign.
  auto outer_plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);
  if (!plane_matches_canonical_winding(outer_plane)) {
    outer_plane = Plane::From3Points(unique_points[0], unique_points[2], unique_points[1]);
  }

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
  if (!plane_matches_canonical_winding(outer_plane)) {
    outer_plane = Plane::From3Points(unique_points[0], unique_points[2], unique_points[1]);
  }

  Axis dax = outer_plane.normal().DominantAxis();
  View2D hole_view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  auto project_hole = [&hole_view](std::vector<Point3D> const& ring) {
    std::vector<LineSegment2D> segs;
    int n = static_cast<int>(ring.size());
    segs.reserve(n);
    for (int i = 0; i < n; ++i) {
      segs.push_back(LineSegment2D::Make(Point2D(hole_view.x(ring[i]), hole_view.y(ring[i])),
                                         Point2D(hole_view.x(ring[(i + 1) % n]), hole_view.y(ring[(i + 1) % n]))));
    }
    return segs;
  };

  std::vector<std::vector<Point3D>> unique_holes_points;
  std::vector<std::vector<LineSegment2D>> unique_holes_segs_2d;  // cached for the cross-hole check below
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

    // Checked before CW-ness, same reasoning as Polygon2D::Make(): a self-intersecting ring's winding is
    // a degenerate notion, so it must be rejected outright rather than let a coincidentally-CW
    // self-intersecting ring slip through as a "hole". Projected via outer_plane's dominant axis, same as
    // IsSimple()'s own hole check (PLANE isn't set on this not-yet-constructed instance yet, but
    // outer_plane above is the same plane it will become).
    auto hole_segs = project_hole(unique_hole_points);
    if (has_intersections(hole_segs)) {
      throw std::runtime_error("cannot create polygon hole with self-intersections");
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
    unique_holes_segs_2d.push_back(std::move(hole_segs));
  }

  // Each hole is individually simple (checked above), but nothing yet stops two DIFFERENT holes from
  // crossing each other — see Polygon2D::Make()'s identical check for the full rationale. Reuses the
  // dominant-axis 2D projection already computed per hole above (unique_holes_segs_2d), rather than
  // re-projecting, since all holes share the same outer_plane/dominant axis.
  for (std::size_t i = 0; i < unique_holes_segs_2d.size(); ++i) {
    for (std::size_t j = i + 1; j < unique_holes_segs_2d.size(); ++j) {
      auto combined = unique_holes_segs_2d[i];
      combined.insert(combined.end(), unique_holes_segs_2d[j].begin(), unique_holes_segs_2d[j].end());
      if (has_intersections(combined)) {
        GEOMPP_LOG(ERROR) << "invalid polygon: hole " << i << " intersects hole " << j;
        throw std::runtime_error("cannot create polygon with intersecting holes");
      }
    }
  }

  // Same reasoning as Polygon2D::Make()'s identical check: the outer ring is allowed to self-intersect by
  // design, so has_intersections() on outer+hole combined would flag its own legitimate self-crossing as a
  // false "hole crosses outer" positive, and a plain intersect()+shares_endpoint() combo would flag a hole
  // vertex touching mid-way along an outer edge (a T-junction) or a hole edge running flush along part of
  // the outer boundary — both legitimate polygon-with-hole shapes, not a "strikes through" error.
  // strictly_crosses() (above) only counts a hole edge that punches through an outer edge strictly between
  // both segments' endpoints. Same dominant-axis 2D projection as the hole checks above.
  if (!unique_holes_segs_2d.empty()) {
    auto outer_segs = project_hole(unique_points);
    for (std::size_t h = 0; h < unique_holes_segs_2d.size(); ++h) {
      for (auto const& outer_seg : outer_segs) {
        for (auto const& hole_seg : unique_holes_segs_2d[h]) {
          if (strictly_crosses(outer_seg, hole_seg)) {
            GEOMPP_LOG(ERROR) << "invalid polygon: hole " << h << " intersects the outer loop";
            throw std::runtime_error("cannot create polygon with a hole that intersects the outer loop");
          }
        }
      }
    }
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
  Axis dax = PLANE.normal().DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  auto make_segs = [&view](std::vector<Point3D> const& ring) {
    std::vector<LineSegment2D> segs;
    segs.reserve(ring.size());
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      segs.push_back(LineSegment2D::Make(Point2D(view.x(ring[i]), view.y(ring[i])),
                                         Point2D(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]))));
    }
    return segs;
  };
  bool outer_simple = !has_intersections(make_segs(VERTICES));

  if (outer_simple && HOLES.empty()) {
    // Common case, preserved as an exact shortcut: single region, no weighted average needed.
    return centroid(VERTICES, PLANE);
  }

  // weighted average: c = Σ(aᵢ·cᵢ) / Σ(aᵢ)
  double total_sa = 0.0;
  double wx = 0.0;
  double wy = 0.0;
  double wz = 0.0;

  if (outer_simple) {
    Point3D cs = centroid(VERTICES, PLANE);
    double sa = signed_area(VERTICES, PLANE);
    total_sa = sa;
    wx = sa * cs.x();
    wy = sa * cs.y();
    wz = sa * cs.z();
  } else {
    // Self-intersecting outer ring: same reasoning as Polygon2D::Centroid() — weighting by the whole
    // ring's own raw net signed_area is wrong (opposite-winding lobes can cancel that weight, or land it
    // near zero, making the division unstable). Decompose into the real interior faces instead (same
    // unprojection Area()'s slow path uses — a 2D-projected area/centroid isn't correct on a tilted
    // plane) and weight each one's own centroid by its own (plane-aware) area.
    Point3D orig = PLANE.origin();
    Vector3D n = PLANE.normal();
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

    for (auto const& ring2d : detail::view::simplify_rings(VERTICES, std::vector<std::vector<Point3D>>{}, view)) {
      std::vector<Point3D> ring3d;
      ring3d.reserve(ring2d.size());
      for (auto const& p : ring2d) {
        ring3d.push_back(from2d(p));
      }
      double sa = std::abs(signed_area(ring3d, PLANE));
      Point3D cs = centroid(ring3d, PLANE);
      total_sa += sa;
      wx += sa * cs.x();
      wy += sa * cs.y();
      wz += sa * cs.z();
    }
  }

  // Holes are always individually simple (Make() rejects a self-intersecting hole outright), so this is
  // always a direct, correct subtraction — hole areas are negative (CW), so they subtract.
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
  // Holes are always individually simple now — Make() rejects a self-intersecting hole outright (see its
  // own comment) — so this direct shoelace sum is always correct however the OUTER ring turns out; holes
  // never need decomposing or recursing into, unlike VERTICES below. O(N_holes) total.
  double total_hole_area = 0.0;
  for (auto const& hole : HOLES) {
    total_hole_area += signed_area(hole, PLANE);  // guaranteed CW (negative) by Make()
  }

  Axis dax = PLANE.normal().DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  auto make_segs = [&view](std::vector<Point3D> const& ring) {
    std::vector<LineSegment2D> segs;
    segs.reserve(ring.size());
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      segs.push_back(LineSegment2D::Make(Point2D(view.x(ring[i]), view.y(ring[i])),
                                         Point2D(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]))));
    }
    return segs;
  };

  if (!has_intersections(make_segs(VERTICES))) {
    // FAST PATH: outer ring is simple too — pure O(n) shoelace, no decomposition at all.
    return signed_area(VERTICES, PLANE) + total_hole_area;
  }

  // SLOW PATH: self-intersecting outer ring — decompose it, and ONLY it (holes are already handled
  // above), via the same half-edge tracer Simplify() uses, at O(n log n). Not the full public
  // Simplify()/Make() pipeline — that would also re-run hole assignment and reconstruct Polygon3D objects
  // for no reason here, and recursing into piece.Area() would risk unbounded recursion if Simplify() ever
  // can't fully resolve a degenerate input into simple pieces (a real, if rare, hazard — see git history).
  //
  // simplify_rings() works directly on the 3D VERTICES (projected via `view` internally) and returns just
  // the real interior 2D faces, each already CCW and with the Y-dominant-projection chirality mirroring
  // (see its own doc comment) already accounted for — no need to re-derive that here. A 2D-projected area
  // still isn't the true 3D area on a tilted plane (foreshortening), so each interior loop is unprojected
  // back to 3D and measured with the plane-aware signed_area(..., PLANE) instead of computing the scale
  // factor by hand.
  Point3D orig = PLANE.origin();
  Vector3D n = PLANE.normal();
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

  double outer_solid_area = 0.0;
  for (auto const& ring2d : detail::view::simplify_rings(VERTICES, std::vector<std::vector<Point3D>>{}, view)) {
    std::vector<Point3D> ring3d;
    ring3d.reserve(ring2d.size());
    for (auto const& p : ring2d) {
      ring3d.push_back(from2d(p));
    }
    outer_solid_area += std::abs(signed_area(ring3d, PLANE));
  }

  return outer_solid_area + total_hole_area;  // total_hole_area is already negative — add, don't subtract
}

double Polygon3D::PerimeterSize() const { return PERIMETER; }

bool Polygon3D::IsSimple() const {
  Axis dax = PLANE.normal().DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();

  auto make_segs = [&view](std::vector<Point3D> const& ring) {
    std::vector<LineSegment2D> segs;
    segs.reserve(ring.size());
    int n = (int)ring.size();
    for (int i = 0; i < n; ++i) {
      segs.push_back(LineSegment2D::Make(Point2D(view.x(ring[i]), view.y(ring[i])),
                                         Point2D(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]))));
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

  // simplify_rings() already returns just the real interior faces, each CCW in 2D (positive signed_area)
  // — the single unbounded "outside" face, and the chirality calibration a Y-dominant-axis view needs
  // (a CCW 3D polygon can project to CW in 2D — see simplify_rings()'s own doc comment), are both handled
  // internally now. Candidates are ready to feed into Polygon2D::Make() / hole assignment as-is.
  std::vector<std::vector<Point2D>> candidates2d = detail::view::simplify_rings(VERTICES, HOLES, view);
  std::vector<double> candidate_areas;
  candidate_areas.reserve(candidates2d.size());
  for (auto const& ring : candidates2d) {
    candidate_areas.push_back(signed_area(ring));
  }

  // Sort candidates by area descending (largest first)
  std::vector<int> order(candidates2d.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](int a, int b) { return candidate_areas[a] > candidate_areas[b]; });
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
  auto cv_indices = detail::convex_hull_indices(VERTICES);
  std::vector<Point3D> cv_points;
  cv_points.reserve(cv_indices.size());
  for (std::size_t i : cv_indices) {
    cv_points.emplace_back(VERTICES[i]);
  }
  return Make(cv_points);
}

std::vector<Point3D> const& Polygon3D::Perimeter() const { return VERTICES; }

double Polygon3D::DistanceTo(Point3D const& point) const { throw std::runtime_error("not implemented"); }

#pragma region Operator Overloading

bool operator==(Polygon3D const& lhs, Polygon3D const& rhs) { return lhs.AlmostEquals(rhs); }

Point3D const& Polygon3D::operator[](std::size_t i) const {
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

bool Polygon3D::IsOnPerimeter(Point3D const& point) const {
  if (!PLANE.Contains(point)) {
    return false;
  }
  Axis dax = PLANE.normal().DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  return detail::view::is_on_perimeter(VERTICES, HOLES, view, view.x(point), view.y(point));
}

bool Polygon3D::Contains(Point3D const& point) const {
  if (!BBox3D(*this).Contains(point)) {
    return false;
  }

  if (!PLANE.Contains(point)) {
    return false;
  }

  if (IsOnPerimeter(point)) {
    return true;
  }

  Axis dax = PLANE.normal().DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  return detail::view::polygon_contains(VERTICES, HOLES, view, view.x(point), view.y(point));
}

bool Polygon3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Polygon3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon3D::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

std::optional<Point3D> Polygon3D::Intersection(Line3D const& line) const {
  auto plane_inter = PLANE.Intersection(line);

  if (!(plane_inter.has_value() && std::holds_alternative<Point3D>(*plane_inter))) {
    return std::nullopt;
  }

  auto const& intersection_point = std::get<Point3D>(plane_inter.value());

  if (!Contains(intersection_point)) {
    return std::nullopt;
  }

  return intersection_point;
}

std::optional<Point3D> Polygon3D::Intersection(Ray3D const& ray) const {
  auto plane_inter = PLANE.Intersection(ray);

  if (!(plane_inter.has_value() && std::holds_alternative<Point3D>(*plane_inter))) {
    return std::nullopt;
  }

  auto const& intersection_point = std::get<Point3D>(plane_inter.value());

  if (!Contains(intersection_point)) {
    return std::nullopt;
  }

  return intersection_point;
}

std::optional<Point3D> Polygon3D::Intersection(LineSegment3D const& segment) const {
  auto plane_inter = PLANE.Intersection(segment);

  if (!(plane_inter.has_value() && std::holds_alternative<Point3D>(*plane_inter))) {
    return std::nullopt;
  }

  auto const& intersection_point = std::get<Point3D>(plane_inter.value());

  if (!Contains(intersection_point)) {
    return std::nullopt;
  }

  return intersection_point;
}

namespace {

// Decomposes a polygon into RingPieces (projected to 2D via `view`) for boolean_op_multi: itself (as the
// sole piece) if already simple, or Simplify()'s pieces otherwise — same reasoning as Polygon2D's
// to_ring_pieces. Simplify() resolves a self-intersecting operand's self-crossing into pieces that are
// each genuinely simple and CCW-outer/CW-hole oriented, which classify_and_orient_source_tagged requires
// and a single self-intersecting ring's own traversal cannot guarantee.
//
// `view` is built from GetPlane() directly — Polygon3D::Make() guarantees every stored plane's normal
// sign matches its points' validated CCW winding (checks are_ccw(points, outer_plane) against the very
// plane it's about to store, flipping which two of the first three points go into From3Points if it
// disagrees), so every piece's raw vertex order is guaranteed to project as CCW through it.
detail::RingPieces to_ring_pieces_3d(Polygon3D const& p, View2D const& view) {
  auto project = [&](std::vector<Point3D> const& ring) {
    std::vector<Point2D> out;
    out.reserve(ring.size());
    for (auto const& pt : ring) {
      out.push_back(Point2D(view.x(pt), view.y(pt)));
    }
    return out;
  };
  auto project_holes = [&](std::vector<std::vector<Point3D>> const& holes) {
    std::vector<std::vector<Point2D>> out;
    out.reserve(holes.size());
    for (auto const& hole : holes) {
      out.push_back(project(hole));
    }
    return out;
  };
  auto add_piece = [&](Polygon3D const& piece, detail::RingPieces& out) {
    out.push_back({project(piece.Perimeter()), project_holes(piece.Holes())});
  };

  detail::RingPieces pieces;
  if (p.IsSimple()) {
    add_piece(p, pieces);
    return pieces;
  }
  for (auto const& piece : p.Simplify()) {
    add_piece(piece, pieces);
  }
  return pieces;
}

// Coplanar-only engine shared by Union/Difference/Xor and the coplanar branch of Intersection(): project
// both operands onto their common plane via View2D::OnPlane (exact and invertible, see View2D::xyz()),
// run the same source-tagged 2D engine Polygon2D uses (boolean_op_multi — see its docs and
// Polygon2D::to_ring_pieces in polygon2d.cpp for why the Simplify()-decomposition step matters for
// self-intersecting operands), then lift each result ring's vertices back to 3D.
std::vector<Polygon3D> run_boolean_op_3d(Polygon3D const& a, Polygon3D const& b, detail::BooleanOp op) {
  if (!a.GetPlane().AlmostEquals(b.GetPlane())) {
    throw std::logic_error("Polygon3D boolean operations require both polygons to be coplanar");
  }

  auto view = View2D::OnPlane(a.GetPlane());

  auto groups = detail::boolean_op_multi(to_ring_pieces_3d(a, view), to_ring_pieces_3d(b, view), op);

  auto unproject = [&](std::vector<Point2D> const& ring) {
    std::vector<Point3D> out;
    out.reserve(ring.size());
    for (auto const& p : ring) {
      out.push_back(view.xyz(p));
    }
    return out;
  };

  std::vector<Polygon3D> result;
  result.reserve(groups.size());
  for (auto const& [outer, holes] : groups) {
    std::vector<std::vector<Point3D>> holes3d;
    holes3d.reserve(holes.size());
    for (auto const& hole : holes) {
      holes3d.push_back(unproject(hole));
    }
    // Same defensive retry as Polygon2D::run_boolean_op (polygon2d.cpp) — see its comment. Make() now
    // rejects a self-intersecting hole outright; retry without holes rather than losing the whole piece.
    auto outer3d = unproject(outer);
    try {
      result.push_back(Polygon3D::Make(outer3d, holes3d));
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "run_boolean_op_3d: rejected a result piece's holes (" << e.what()
                          << "); retrying without holes";
      try {
        result.push_back(Polygon3D::Make(outer3d));
      } catch (std::runtime_error const& e2) {
        GEOMPP_LOG(WARNING) << "run_boolean_op_3d: could not assemble result piece at all: " << e2.what();
      }
    }
  }
  return result;
}

}  // namespace

bool Polygon3D::Intersects(Polygon3D const& other) const {
  auto result = Intersection(other);
  if (!result.has_value()) {
    return false;
  }
  return std::visit([](auto const& alternative) { return !alternative.empty(); }, result.value());
}

std::optional<std::variant<std::vector<Polygon3D>, std::vector<LineSegment3D>>> Polygon3D::Intersection(
    Polygon3D const& other) const {
  if (PLANE.AlmostEquals(other.GetPlane())) {
    auto pieces = run_boolean_op_3d(*this, other, detail::BooleanOp::Intersection);
    if (pieces.empty()) {
      return std::nullopt;
    }
    return pieces;
  }

  if (PLANE.normal().IsParallel(other.GetPlane().normal())) {
    return std::nullopt;  // parallel and (per the check above) distinct planes never meet
  }

  // Planes cross along a line — two flat, non-coplanar regions can only share points there. Find each
  // polygon's own parametric intervals along that shared line (each polygon supplies its own plane as
  // the view, since the line is coplanar with both, just not the same plane for both), then intersect
  // the two 1D interval sets.
  auto plane_inter = PLANE.Intersection(other.GetPlane());
  if (!plane_inter.has_value() || !std::holds_alternative<Line3D>(*plane_inter)) {
    return std::nullopt;  // defensive — shouldn't happen once the parallel check above has passed
  }
  auto const& line = std::get<Line3D>(*plane_inter);

  auto intervals_a = detail::view::compute_intersection_intervals_3d(VERTICES, HOLES, IS_CONVEX, line.First(),
                                                                     line.Last(), View2D::OnPlane(PLANE));
  auto intervals_b = detail::view::compute_intersection_intervals_3d(
      other.Perimeter(), other.Holes(), other.IsConvex(), line.First(), line.Last(), View2D::OnPlane(other.GetPlane()));

  std::vector<std::pair<double, double>> merged;
  std::size_t i = 0, j = 0;
  while (i < intervals_a.size() && j < intervals_b.size()) {
    double lo = std::max(intervals_a[i].first, intervals_b[j].first);
    double hi = std::min(intervals_a[i].second, intervals_b[j].second);
    if (compare(lo, hi) < 0) {
      merged.push_back({lo, hi});
    }
    if (compare(intervals_a[i].second, intervals_b[j].second) < 0) {
      ++i;
    } else {
      ++j;
    }
  }

  if (merged.empty()) {
    return std::nullopt;
  }

  Vector3D dir = line.Last() - line.First();
  std::vector<LineSegment3D> segs;
  segs.reserve(merged.size());
  for (auto const& [t0, t1] : merged) {
    segs.push_back(LineSegment3D::Make(line.First() + dir * t0, line.First() + dir * t1));
  }
  return segs;
}

#pragma endregion

#pragma region Boolean Operations

std::vector<Polygon3D> Polygon3D::Union(Polygon3D const& other) const {
  return run_boolean_op_3d(*this, other, detail::BooleanOp::Union);
}

std::vector<Polygon3D> Polygon3D::Difference(Polygon3D const& other) const {
  return run_boolean_op_3d(*this, other, detail::BooleanOp::Difference);
}

std::vector<Polygon3D> Polygon3D::Xor(Polygon3D const& other) const {
  return run_boolean_op_3d(*this, other, detail::BooleanOp::Xor);
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
