#include "constants.hpp"
#include "geompp_log.hpp"
#include "line_segment3d.hpp"
#include "lsv_parser.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

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
      auto p = std::get<g::Point3D>(*result);
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

  auto parser = g::LVSParser::Open(lsv_path);

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

    GEOMPP_LOG(INFO) << g::LVSParser::ToWkt(entry.value());
  }
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
  example_1();

  example_2();

  return 0;
}
