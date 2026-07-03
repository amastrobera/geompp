#include "calc_utils2d.hpp"
#include "constants.hpp"
#include "geompp_log.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "ray2d.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"
#include "wkt_parser.hpp"

#include <fstream>
#include <iostream>
#include <variant>

namespace g = geompp;

// ─────────────────────────────────────────────────────────────────────────────
// Example 1 — Two crossing LineSegment3D forming an X, compute intersection
// ─────────────────────────────────────────────────────────────────────────────
void example_1() {
  std::cout << "=== Example 1: LineSegment3D intersection ===\n";

  // This will be the precision used by all functions, in all threads, for this
  // run of the program, and it can be modified in later code anytime.
  g::DECIMAL_PRECISION = g::DP_THREE;

  // Two segments intersecting on the Z axis:

  auto s1 = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(-1, 0, 2));
  auto s2 = g::LineSegment3D::Make(g::Point3D(0, 1, 0), g::Point3D(0, -1, 2));

  GEOMPP_LOG(INFO) << "s1 = " << s1.ToWkt();
  GEOMPP_LOG(INFO) << "s2 = " << s2.ToWkt();

  if (s1.Intersects(s2)) {
    auto result = s1.Intersection(s2);
    if (result.has_value()) {
      auto p = *result;
      GEOMPP_LOG(INFO) << "intersection found: " << p.ToWkt();

      p.ToFile("intersection.wkt");
      GEOMPP_LOG(INFO) << "intersection written to intersection.wkt";
    }
  } else {
    GEOMPP_LOG(INFO) << "no intersection found";
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// Example 2 — Load geometries from an .lsv file and iterate with Next()
// ─────────────────────────────────────────────────────────────────────────────

// Sample .lsv file content (one WKT geometry per line):
//
//   POINT (1 2 3)
//   POINT (4 5 6)
//   LINESTRING (0 0 0, 1 1 1)
//   LINESTRING (2 0 0, 2 3 4)
//   LINE (0 0 0, 1 0 0)
//   RAY (0 0 0, 0 1 0)
//
// To create the sample file, run write_sample_lsv() below, then load it.
void example_2() {
  std::cout << "\n=== Example 2: load geometries from .lsv file ===\n";

  // always definie the precision level you want to use in this run of the program
  // it will be common to all threads
  g::DECIMAL_PRECISION = g::DP_THREE;

  // write file
  std::string const lsv_path = "sample_geometries.lsv";
  std::ofstream f(lsv_path);
  {
    f << "POINT (1 2 3)\n";
    f << "POINT (4 5 6)\n";
    f << "LINESTRING (0 0 0, 1 1 1)\n";
    f << "LINESTRING (2 0 0, 2 3 4)\n";
    f << "LINE (0 0 0, 1 0 0)\n";
    f << "RAY (0 0 0, 0 1 0)";  // last geom doesn't need a newline
    f.close();
  }

  auto parser = g::WktParser::Open(lsv_path);

  if (!parser.HasNext()) {
    GEOMPP_LOG(WARNING) << "no geometries found in file " << lsv_path;
    return;
  }

  while (parser.HasNext()) {
    auto entry = parser.Next();

    if (!entry.has_value()) {
      GEOMPP_LOG(WARNING) << "skipped unrecognised line";
      continue;
    }

    GEOMPP_LOG(INFO) << g::WktParser::ToWkt(entry.value());
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// Example 3 — are_coplanar, closest_world_plane_to, are_ccw, Polygon3D holes
// ─────────────────────────────────────────────────────────────────────────────
void example_3() {
  std::cout << "\n=== Example 3: coplanarity, winding order, polygon with holes ===\n";

  g::DECIMAL_PRECISION = g::DP_THREE;

  // Four points on the XY plane
  std::vector<g::Point3D> flat = {
      {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}};

  // Same four but one point lifted off the plane
  std::vector<g::Point3D> skew = {
      {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  GEOMPP_LOG(INFO) << "flat coplanar: " << g::are_coplanar(flat);   // 1
  GEOMPP_LOG(INFO) << "skew coplanar: " << g::are_coplanar(skew);   // 0

  // Which world-axis plane is closest to the flat cloud?
  auto plane = g::closest_world_plane_to(flat);
  GEOMPP_LOG(INFO) << "closest plane normal: " << plane.normal().ToWkt();  // (0, 0, 1)

  // Winding order of a CCW square on the XY plane
  std::vector<g::Point3D> ring = {
      {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  GEOMPP_LOG(INFO) << "ring is CCW: " << g::are_ccw(ring);   // 1
  GEOMPP_LOG(INFO) << "ring is CW:  " << g::are_cw(ring);    // 0

  // Build a Polygon3D with a rectangular hole (outer ring CCW, hole CW)
  std::vector<g::Point3D> outer = {
      {0, 0, 0}, {4, 0, 0}, {4, 4, 0}, {0, 4, 0}};
  std::vector<g::Point3D> hole = {
      {1, 3, 0}, {3, 3, 0}, {3, 1, 0}, {1, 1, 0}};

  auto poly = g::Polygon3D::Make(outer, {hole});
  GEOMPP_LOG(INFO) << "polygon: " << poly.ToWkt();
}

// ─────────────────────────────────────────────────────────────────────────────
// Example 4 — find all intersection points among a set of 2D segments
// ─────────────────────────────────────────────────────────────────────────────
void example_4() {
  std::cout << "\n=== Example 4: find_intersections (Bentley–Ottmann) ===\n";

  g::DECIMAL_PRECISION = g::DP_THREE;

  // Three 2D segments: two diagonals of a unit square (cross at (0.5, 0.5))
  // plus a horizontal segment that touches neither (parallel, no crossing).
  std::vector<g::LineSegment2D> segments = {
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)),   // seg 0: bottom-left → top-right
      g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(0, 1)),   // seg 1: bottom-right → top-left
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(1, 2)),   // seg 2: horizontal, no crossing
  };

  // Quick boolean check (Shamos–Hoey)
  GEOMPP_LOG(INFO) << "any intersections? " << g::has_intersections(segments);  // 1

  // Full report (Bentley–Ottmann)
  auto hits = g::find_intersections(segments);
  GEOMPP_LOG(INFO) << hits.size() << " crossing(s) found:";
  for (auto const& p : hits) {
    GEOMPP_LOG(INFO) << "  point=" << p.ToWkt();
  }
  // expected output:
  //   any intersections? 1
  //   1 crossing(s) found:
  //     point=POINT (0.5 0.5)

  // Polygon simplicity — delegates to has_intersections internally
  auto simple_square = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto self_intersecting = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(1, 3), g::Point2D(3, 3)});

  GEOMPP_LOG(INFO) << "square is simple: "          << simple_square.IsSimple();    // 1
  GEOMPP_LOG(INFO) << "self-intersecting is simple: " << self_intersecting.IsSimple(); // 0
}

// ─────────────────────────────────────────────────────────────────────────────
// Example 5 — split a complex (self-intersecting) polygon into simple polygons
// ─────────────────────────────────────────────────────────────────────────────
void example_5() {
  std::cout << "\n=== Example 5: Polygon2D/3D::Simplify — split a complex polygon ===\n";

  g::DECIMAL_PRECISION = g::DP_THREE;

  // A complex polygon is one whose edges cross each other.
  // "Bowtie" shape: A(0,0), B(4,0), C(1,3), D(3,3) — edges B→C and D→A cross at X(2,2).
  auto bowtie = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(1, 3), g::Point2D(3, 3)});

  GEOMPP_LOG(INFO) << "bowtie is simple: " << bowtie.IsSimple();  // 0

  // Simplify() decomposes it into simple (non-self-intersecting) polygons.
  auto parts = bowtie.Simplify();
  GEOMPP_LOG(INFO) << parts.size() << " simple polygon(s):";
  for (auto const& p : parts) {
    GEOMPP_LOG(INFO) << "  " << p.ToWkt() << "  area=" << p.Area()
                     << "  simple=" << p.IsSimple();
  }
  // expected output:
  //   bowtie is simple: 0
  //   2 simple polygon(s):
  //     POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4  simple=1
  //     POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1  simple=1

  // Same works in 3D — the polygon is projected onto its dominant plane internally.
  auto bowtie3d = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto parts3d = bowtie3d.Simplify();
  GEOMPP_LOG(INFO) << parts3d.size() << " simple 3D polygon(s)";
}

// ─────────────────────────────────────────────────────────────────────────────
// Example 6 — Polygon2D intersection with Line, Ray, and LineSegment
// ─────────────────────────────────────────────────────────────────────────────
void example_6() {
  std::cout << "\n=== Example 6: Polygon2D::Intersection (Line / Ray / Segment) ===\n";

  g::DECIMAL_PRECISION = g::DP_THREE;

  // Unit square (CCW)
  auto sq = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});

  // ── Line2D through the square at y = 0.5 ──────────────────────────────────
  auto line = g::Line2D::Make(g::Point2D(-1, 0.5), g::Point2D(2, 0.5));

  if (sq.Intersects(line)) {
    auto result = sq.Intersection(line);
    if (result.has_value()) {
      std::visit([](auto const& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, g::Point2D>) {
          GEOMPP_LOG(INFO) << "line touches at point: " << v.ToWkt();
        } else {
          GEOMPP_LOG(INFO) << v.size() << " chord segment(s):";
          for (auto const& s : v) {
            GEOMPP_LOG(INFO) << "  " << s.ToWkt();
          }
        }
      }, result.value());
    }
  }
  // expected: 1 chord segment: LINESTRING (0 0.5, 1 0.5)

  // ── Ray2D from inside the square ──────────────────────────────────────────
  auto ray = g::Ray2D::Make(g::Point2D(0.5, 0.5), g::Vector2D(1, 0));

  if (sq.Intersects(ray)) {
    auto result = sq.Intersection(ray);
    if (result.has_value()) {
      std::visit([](auto const& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, g::Point2D>) {
          GEOMPP_LOG(INFO) << "ray touches at point: " << v.ToWkt();
        } else {
          GEOMPP_LOG(INFO) << v.size() << " chord segment(s) from ray:";
          for (auto const& s : v) {
            GEOMPP_LOG(INFO) << "  " << s.ToWkt();
          }
        }
      }, result.value());
    }
  }
  // expected: 1 chord from (0.5,0.5) to (1,0.5)

  // ── LineSegment2D that pierces the square ─────────────────────────────────
  auto seg = g::LineSegment2D::Make(g::Point2D(-0.5, 0.5), g::Point2D(1.5, 0.5));

  if (sq.Intersects(seg)) {
    auto result = sq.Intersection(seg);
    if (result.has_value()) {
      std::visit([](auto const& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, g::Point2D>) {
          GEOMPP_LOG(INFO) << "segment touches at point: " << v.ToWkt();
        } else {
          GEOMPP_LOG(INFO) << v.size() << " chord segment(s) from segment:";
          for (auto const& s : v) {
            GEOMPP_LOG(INFO) << "  " << s.ToWkt();
          }
        }
      }, result.value());
    }
  }
  // expected: 1 chord segment: LINESTRING (0 0.5, 1 0.5)
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
  example_1();

  example_2();

  example_3();

  example_4();

  example_5();

  example_6();

  return 0;
}
