# Test Coverage Report

_Last updated: 2026-07-07_

## Overall

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (total) | ~414 | Excluding copy/move ctors, dtors, `operator<<`, `operator=` |
| Stubs (`throw "not implemented"`) | 13 | Listed per class below |
| C++ explicit tests | ~490 | At least one `TEST_F` exercises the method |
| Python explicit tests | ~216 | At least one `test_*` function calls the method |
| C# explicit tests | ~222 | At least one test in `Program.cs` exercises the method |

---

## By Class

Key: **★** = stub (not yet implemented) · **○** = implemented, no explicit test (C++ or Python)

| Class | C++ tested | Py tested | CS tested | Stubs ★ | Notable gaps ○ |
|-------|-----------|----------|----------|---------|----------------|
| `Point2D` | ✓ all key | ✓ most | ✓ most | — | `linear_combination` (Py); new `Point2D(Vector2D)` ctor covered all three; new `is_left` / `is_right` / `convex_hull` free fns tested in all three languages |
| `Point3D` | ✓ all key | ✓ most | ✓ most | — | new `Point3D(Vector3D)` ctor covered all three |
| `Vector2D` | ✓ most | ✓ most | ✓ partial | — | Many arithmetic operators (both) |
| `Vector3D` | ✓ most | ✓ most | ✓ most | — | Many arithmetic operators (both); `IsParallel` (C++) |
| `Line2D` | ✓ most | ✓ partial | ✓ most | — | `Intersects`/`Intersection` ×`Ray2D`, ×`Segment2D` (Py) |
| `Line3D` | ✓ most | ✓ partial | ✓ most | — | `Contains`, `Intersects`/`Intersection` ×`Ray3D`, ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `Ray2D` | ✓ most | ✓ partial | — | — | `ToLine` (Py) |
| `Ray3D` | ✓ most | ✓ partial | ✓ partial | — | `Contains`, `Intersects`/`Intersection` ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `LineSegment2D` | ✓ most | ✓ most | ✓ partial | — | `ToLine` (Py); free `intersect(seg, seg)` tested (C++); new `has_intersections` / `find_intersections` public free fns tested in all three languages |
| `LineSegment3D` | ✓ all key | ○ thin | ○ thin | — | `First`, `Last`, `AlmostEquals`, `Location`, `Interpolate`, `Contains`, all `Intersects`/`Intersection` (Py) |
| `Polyline2D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location` (Py); new `ConvexHull()` (Melkman) tested in all three |
| `Polyline3D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location`, `Interpolate`, most `Intersects`/`Intersection` (Py) |
| `Polygon2D` | ✓ all key | ✓ all key | ✓ most | `DistanceTo` ★ | `ToWkt`/`FromWkt`, `ToFile`/`FromFile`, `AlmostEquals` (Py); `IsSimple()` and `ConvexHull()` tested in all three; `Intersection(×Line/Ray/Seg)` now implemented and tested in all three; new `HasHoles()`/`Holes()` tested in all three; `ToPoints()` now returns `const&` |
| `Polygon3D` | ✓ core | ✓ core | ✓ partial | `DistanceTo` ★ `Intersection(×Line/Ray/Seg)` ★ | Same as Polygon2D (Py); new `ConvexHull()` and `IsSimple()` tested in all three; new `HasHoles()`/`Holes()` tested in all three; `ToPoints()` now returns `const&` |
| `Triangle2D` | ✓ most | ✓ partial | ✓ partial | `DistanceTo` ★ `Intersects(△)` ★ `Intersection(△)` ★ | `AlmostEquals`, `ToPolygon`, `ToAxis`, `Location`, all `Intersection` (Py) |
| `Triangle3D` | ✓ most | ✓ most | ✓ most | `DistanceTo` ★ | new `Intersection(×Plane/△)` and the existing `Intersection(×Line/Ray/Seg)` are covered in all three languages |
| `Plane` | ✓ all key | ✓ all key | ✓ most | — | `Intersection(Triangle3D)` now delegates to the symmetric `Triangle3D::Intersection(Plane)` (no stub remaining) |
| `BBox2D` | ✓ most | ✓ partial | — | — | `BBox2D(Polyline2D)`, `BBox2D(Polygon2D)` (C++); most ctors (Py) |
| `BBox3D` | ✓ all key | ○ thin | — | — | `min`, `max`, `Contains`, all shape ctors (Py) |
| `WktParser` | ✓ core | ✓ partial | ✓ most | — | Multi-geometry `FromWkt` round-trip (Py) |
| `GeometryCollection2D` | ✓ core | ✓ partial | ✓ most | — | — |
| `GeometryCollection3D` | ✓ core | ✓ partial | ✓ most | — | — |
| `calc_utils2d` (`Event2D`, `EventQueue2D`, `SweepLineComparator`, `SweepLine2D`, `has_intersections_impl`, `find_intersections_impl`, `convex_hull_indices`, `extreme_points_impl`) | ✓ all key | ✓ core | ✓ core | — | C++ sweep module (internal); `Add`/`Get`/`Remove`/`SetX`/`GetX` + `EventQueue2D` ordering/`Contains` tested in C++; public wrappers `has_intersections` / `find_intersections` (in `line_segment2d.hpp`) + `Polygon2D::IsSimple` tested in all three languages; `convex_hull_indices` exercised via `convex_hull` tests; new `find_extreme_points(Polygon2D, Line2D)` + `extreme_points_impl` (Sunday convex O(log n) + brute-force) tested in all three languages and cross-checked vs brute force over 200k random convex polygons |
| `calc_utils3d` (`convex_hull_indices`, `find_extreme_points`) | ✓ all key | ✓ core | ✓ core | — | Internal 3D hull solver; exercised via `convex_hull(list[Point3D])` free function and `Polygon3D::ConvexHull()` in all three languages; covers XY, YZ, and ZX plane projections; new `find_extreme_points(Polygon3D, Line3D)` tested in all three languages (XY and tilted-plane polygons) |

---

## Stub Methods (10 total)

These methods are declared in the public API but `throw std::runtime_error("not implemented")`.
Each has a `EXPECT_ANY_THROW` test confirming the throw.

| Class | Method |
|-------|--------|
| `Polygon2D` | `DistanceTo(Point2D)` |
| `Polygon3D` | `DistanceTo(Point3D)` |
| `Polygon3D` | `Intersection(Line3D)` |
| `Polygon3D` | `Intersection(Ray3D)` |
| `Polygon3D` | `Intersection(LineSegment3D)` |
| `Triangle2D` | `DistanceTo(Point2D)` |
| `Triangle2D` | `Intersects(Triangle2D)` |
| `Triangle2D` | `Intersection(Triangle2D)` |
| `Triangle3D` | `DistanceTo(Point3D)` |
| `Polygon2D::FromWkt` | (parses but returns broken result — fix tracked separately) |

---

## Key Takeaways

- **No class is at 0%** — all geometry classes have at least some test coverage.
- **Python binding tests are the biggest gap**: `LineSegment3D`, `Polyline3D`, `BBox3D`, `Line3D`, `Ray3D` are thin or untested in Python.
- **Stub cluster**: remaining unimplemented methods: `Polygon2D/3D::DistanceTo`, `Triangle2D::Intersects(△)` and `Triangle2D::Intersection(△)`, and `Triangle2D/3D::DistanceTo`. `Polygon2D::Intersection(×Line/Ray/Seg)` is now fully implemented and tested.
- **`Plane`, `Point2D`, `Point3D`, `Vector2D`, `Vector3D`** have excellent coverage across C++, Python, and (newly for Plane) C#.
- **Operator overloads** (`operator<<`, `operator=`, arithmetic) are implicitly exercised by other tests even when not explicitly targeted.
- **`calc_utils2d` / `Polygon2D::IsSimple`**: both algorithms (`has_intersections_impl` / `find_intersections_impl`) are fully sound. `SweepLineComparator` uses y-at-sweep-x ordering with an id tiebreaker; `EventQueue2D` is a min-heap (left-to-right sweep). Tests in all three languages cover the normal case (simple ring, self-intersecting ring) and edge cases (parallel segments, T-intersections, star case). Public `has_intersections` / `find_intersections` wrappers (now in `line_segment2d.hpp`) tested via the existing suite.
- **`convex_hull`**: Andrew's monotone chain implemented in `calc_utils2d` (`convex_hull_indices`), exposed via `point2d.hpp`. Tested in all three languages including an asymmetric star whose hull must be exactly the 5 outer tips.
- **`find_extreme_points`**: `detail::extreme_points_impl` is a dimension-agnostic template (compares only scalar projections). The convex fast-path is Daniel Sunday's O(log n) binary search; non-convex polygons use an O(n) scan. Tested in all three languages for convex (diamond/square/hexagon), concave (dart), hole-bearing (holes ignored), and 3D (XY + tilted-plane) polygons, plus a C++ convex-vs-brute-force equality check. The convex search was additionally cross-validated against brute force over 200k randomized convex polygons and directions (0 mismatches).

---

## How to Measure Coverage Instrumentally

```bash
# CMake — Debug build with gcov instrumentation
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="--coverage" \
      -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
      ..
cmake --build .
./geompp_tests/geompp_tests
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '*/build/_deps/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

---

_See `test_coverage_plan.md` for the prioritized list of missing test cases._
