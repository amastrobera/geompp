# Test Coverage Report

_Last updated: 2026-07-21_

## Overall

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (total) | ~437 | Excluding copy/move ctors, dtors, `operator<<`, `operator=`. `clip()` gained a `Point3D` instantiation (same free function, not a new method) |
| Stubs (`throw "not implemented"`) | 10 | Listed per class below |
| C++ explicit tests | ~585 | At least one `TEST_F` exercises the method. +5: `clip<Point3D>` (coplanar, non-axis-aligned plane, disjoint, non-coplanar throw, too-few-points throw) |
| Python explicit tests | ~257 | At least one `test_*` function calls the method. +17: `Polygon2D`/`Polygon3D` boolean ops + `clip` — **written but unverified**, see per-class notes |
| C# explicit tests | ~254 | At least one test in `Program.cs` exercises the method. +8: `Polygon2D`/`Polygon3D` boolean ops + `clip` — **verified, 735/735 passing** |

---

## By Class

Key: **★** = stub (not yet implemented) · **○** = implemented, no explicit test (C++ or Python)

| Class | C++ tested | Py tested | CS tested | Stubs ★ | Notable gaps ○ |
|-------|-----------|----------|----------|---------|----------------|
| `Point2D` | ✓ all key | ✓ most | ✓ most | — | `linear_combination` (Py); new `Point2D(Vector2D)` ctor covered all three; new `is_left` / `is_right` / `convex_hull` free fns tested in all three languages; new `lerp` free fn tested in all three |
| `Point3D` | ✓ all key | ✓ most | ✓ most | — | new `Point3D(Vector3D)` ctor covered all three; new `lerp` free fn tested in all three |
| `Vector2D` | ✓ most | ✓ most | ✓ partial | — | Many arithmetic operators (both) |
| `Vector3D` | ✓ most | ✓ most | ✓ most | — | Many arithmetic operators (both); `IsParallel` (C++) |
| `Line2D` | ✓ most | ✓ partial | ✓ most | — | `Intersects`/`Intersection` ×`Ray2D`, ×`Segment2D` (Py) |
| `Line3D` | ✓ most | ✓ partial | ✓ most | — | `Contains`, `Intersects`/`Intersection` ×`Ray3D`, ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `Ray2D` | ✓ most | ✓ partial | — | — | `ToLine` (Py) |
| `Ray3D` | ✓ most | ✓ partial | ✓ partial | — | `Contains`, `Intersects`/`Intersection` ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `LineSegment2D` | ✓ most | ✓ most | ✓ partial | — | `ToLine` (Py); free `intersect(seg, seg)` tested (C++); new `has_intersections` / `find_intersections` public free fns tested in all three languages |
| `LineSegment3D` | ✓ all key | ○ thin | ○ thin | — | `First`, `Last`, `AlmostEquals`, `Location`, `Interpolate`, `Contains`, all `Intersects`/`Intersection` (Py) |
| `Polyline2D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location` (Py); new `ConvexHull()` (Melkman) tested in all three; new `Reduce(strategy, threshold)` (all 3 strategies + default-params equivalence) tested in all three |
| `Polyline3D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location`, `Interpolate`, most `Intersects`/`Intersection` (Py); new `Reduce(strategy, threshold)` (all 3 strategies) tested in all three; default-params equivalence has a C++ test only |
| `Polygon2D` | ✓ all key | ✓ all key | ✓ most | `DistanceTo(Point2D)` ★ | `ToWkt`/`FromWkt`, `ToFile`/`FromFile`, `AlmostEquals` (Py); `IsSimple()` and `ConvexHull()` tested in all three; `Intersection(×Line/Ray/Seg)` now implemented and tested in all three; new `HasHoles()`/`Holes()` tested in all three; `ToPoints()` now returns `const&`; free `distance_to(Polygon2D, Line2D)` (convex O(log n) + non-convex O(n), holes ignored) now implemented and tested in all three languages; new `Union`/`Intersection(Polygon2D)`/`Difference`/`Xor`/`Intersects(Polygon2D)` (general map-overlay boolean ops, holes + self-intersection + collinear-overlapping-edges tolerant) bound and tested in C++ and C# (735/735 passing); Python bindings/tests written but unverified in the authoring sandbox (unrelated network restriction on a fresh dependency fetch) |
| `Polygon3D` | ✓ core | ✓ core | ✓ partial | `DistanceTo(Point3D)` ★ | Same as Polygon2D (Py); new `ConvexHull()` and `IsSimple()` tested in all three; new `HasHoles()`/`Holes()` tested in all three; `ToPoints()` now returns `const&`; `Intersection(×Line/Ray/Seg)` is implemented (not a stub — corrected from a stale mark) and tested in Python/C#, but has no dedicated C++ `TEST_F` yet; free `distance_to(Polygon3D, Line3D)` (coplanar, parallel-offset via Pythagorean combination, and skew via an exact per-edge quadratic boundary scan) now implemented and tested in all three languages; new `Union`/`Difference`/`Xor`/`Intersects(Polygon3D)` (coplanar-only, throws otherwise) and `Intersection(Polygon3D)` (returns polygons when coplanar, chord segments when the planes cross, `nullopt` when parallel-distinct) bound and tested in C++ and C# (735/735 passing); Python bindings/tests written but unverified (same sandbox network restriction) |
| `Triangle2D` | ✓ most | ✓ partial | ✓ partial | `DistanceTo` ★ `Intersects(△)` ★ `Intersection(△)` ★ | `AlmostEquals`, `ToPolygon`, `ToAxis`, `Location`, all `Intersection` (Py) |
| `Triangle3D` | ✓ most | ✓ most | ✓ most | `DistanceTo` ★ | new `Intersection(×Plane/△)` and the existing `Intersection(×Line/Ray/Seg)` are covered in all three languages |
| `Plane` | ✓ all key | ✓ all key | ✓ most | — | `Intersection(Triangle3D)` now delegates to the symmetric `Triangle3D::Intersection(Plane)` (no stub remaining); fixed `AlmostEquals` comparing the coplanarity dot product against `epsilon` itself instead of against zero with `epsilon` as tolerance — regression-tested with a custom epsilon |
| `View2D` | ✓ all key | — | — | — | `XY`/`YZ`/`ZX` gained an optional plane-offset parameter; new `xyz()` (inverse projection, exact for `OnPlane`/`Custom` and the offset-taking axis views) tested in C++ only — internal/helper type, not bound to Python or C# |
| `BBox2D` | ✓ most | ✓ partial | — | — | `BBox2D(Polyline2D)`, `BBox2D(Polygon2D)` (C++); most ctors (Py) |
| `BBox3D` | ✓ all key | ○ thin | — | — | `min`, `max`, `Contains`, all shape ctors (Py) |
| `WktParser` | ✓ core | ✓ partial | ✓ most | — | Multi-geometry `FromWkt` round-trip (Py) |
| `GeometryCollection2D` | ✓ core | ✓ partial | ✓ most | — | — |
| `GeometryCollection3D` | ✓ core | ✓ partial | ✓ most | — | — |
| `calc_utils2d` (`Event2D`, `EventQueue2D`, `SweepLineComparator`, `SweepLine2D`, `detail::has_intersections`, `detail::find_intersections`, `detail::extreme_points`, `detail::view::*`, `dist_decimation`, `rdp_decimation`, `vw_decimation`) | ✓ all key | ✓ core | ✓ core | — | C++ sweep module (internal); `Add`/`Get`/`Remove`/`SetX`/`GetX` + `EventQueue2D` ordering/`Contains` tested in C++; public wrappers `has_intersections` / `find_intersections` (in `line_segment2d.hpp`) + `Polygon2D::IsSimple` tested in all three languages; `convex_hull_indices` exercised via `convex_hull` tests; `find_extreme_points(Polygon2D, Line2D)` + `detail::extreme_points` (Sunday convex O(log n) + brute-force) tested in all three languages and cross-checked vs brute force over 200k random convex polygons; the `detail::view` namespace (renamed from the old `*_with_view` names: `is_convex`, `is_on_perimeter`, `polygon_contains`, `simplify_rings`, `convex_hull_monotone_chain`, `min_bounding_rect`, `compute_parametric_intersection_intervals`, `distance_to`) groups everything that projects through a `View2D`, shared verbatim between the native-2D and projected-3D call sites; new `detail::view::distance_to` (line-to-polygon distance, projected-scalar O(log n) convex / O(n) non-convex, no `Point2D` materialization) backs the public `distance_to(Polygon2D, Line2D)`, tested in all three languages; new `dist_decimation`/`rdp_decimation`/`vw_decimation` (dimension-agnostic templates, explicit-instantiated for `Point2D`/`Point3D`, backing `Polyline2D/3D::Reduce()`) tested directly on raw point vectors in all three languages, independent of the `Polyline::Reduce()` tests above |
| `calc_utils3d` (`convex_hull_indices`, `find_extreme_points`, `distance_to`) | ✓ all key | ✓ core | ✓ core | — | Internal 3D hull solver; exercised via `convex_hull(list[Point3D])` free function and `Polygon3D::ConvexHull()` in all three languages; covers XY, YZ, and ZX plane projections; `find_extreme_points(Polygon3D, Line3D)` tested in all three languages (XY and tilted-plane polygons); new `distance_to(Polygon3D, Line3D)` covers all three geometric cases — coplanar and parallel-offset (reduced to `detail::view::distance_to` plus a `sqrt(h^2 + d2d^2)` Pythagorean combination for the offset), and skew (crossing-point-in-polygon fast path, else an exact per-edge quadratic boundary scan — correct for convex and non-convex rings alike since point-to-line distance has convex nested level sets) — tested in all three languages, including a dedicated regression case for the oblique-crossing anisotropic metric |

---

## Stub Methods (7 total)

These methods are declared in the public API but `throw std::runtime_error("not implemented")`.
Each has a `EXPECT_ANY_THROW` test confirming the throw.

| Class | Method |
|-------|--------|
| `Polygon2D` | `DistanceTo(Point2D)` |
| `Polygon3D` | `DistanceTo(Point3D)` |
| `Triangle2D` | `DistanceTo(Point2D)` |
| `Triangle2D` | `Intersects(Triangle2D)` |
| `Triangle2D` | `Intersection(Triangle2D)` |
| `Triangle3D` | `DistanceTo(Point3D)` |
| `Polygon2D::FromWkt` | (parses but returns broken result — fix tracked separately) |

Note: `Polygon3D::Intersection(×Line3D/Ray3D/LineSegment3D)` were previously listed here but are
implemented (not stubs) — corrected in this update. They still lack a dedicated C++ `TEST_F`,
though Python and C# exercise them.

---

## Key Takeaways

- **No class is at 0%** — all geometry classes have at least some test coverage.
- **Python binding tests are the biggest gap**: `LineSegment3D`, `Polyline3D`, `BBox3D`, `Line3D`, `Ray3D` are thin or untested in Python.
- **Stub cluster**: remaining unimplemented methods: `Polygon2D/3D::DistanceTo(Point)`, `Triangle2D::Intersects(△)` and `Triangle2D::Intersection(△)`, and `Triangle2D/3D::DistanceTo(Point)`. `Polygon2D::Intersection(×Line/Ray/Seg)` and `Polygon3D::Intersection(×Line/Ray/Seg)` are both fully implemented and tested (the latter was previously mismarked as a stub in this report). Note the free functions `distance_to(Polygon2D/3D, Line2D/3D)` are a *different* capability from the still-stubbed `Polygon2D/3D::DistanceTo(Point)` — distance to an infinite line vs. distance to a point — and are now fully implemented and tested.
- **`Plane`, `Point2D`, `Point3D`, `Vector2D`, `Vector3D`** have excellent coverage across C++, Python, and (newly for Plane) C#.
- **Operator overloads** (`operator<<`, `operator=`, arithmetic) are implicitly exercised by other tests even when not explicitly targeted.
- **`calc_utils2d` / `Polygon2D::IsSimple`**: both algorithms (`detail::has_intersections` / `detail::find_intersections`, renamed from the old `*_impl` names) are fully sound. `SweepLineComparator` uses y-at-sweep-x ordering with an id tiebreaker; `EventQueue2D` is a min-heap (left-to-right sweep). Tests in all three languages cover the normal case (simple ring, self-intersecting ring) and edge cases (parallel segments, T-intersections, star case). Public `has_intersections` / `find_intersections` wrappers (now in `line_segment2d.hpp`) tested via the existing suite.
- **`convex_hull`**: Andrew's monotone chain implemented in `calc_utils2d` (`convex_hull_indices`), exposed via `point2d.hpp`. Tested in all three languages including an asymmetric star whose hull must be exactly the 5 outer tips.
- **`find_extreme_points`**: `detail::extreme_points` is a dimension-agnostic template (compares only scalar projections). The convex fast-path is Daniel Sunday's O(log n) binary search; non-convex polygons use an O(n) scan. Tested in all three languages for convex (diamond/square/hexagon), concave (dart), hole-bearing (holes ignored), and 3D (XY + tilted-plane) polygons, plus a C++ convex-vs-brute-force equality check. The convex search was additionally cross-validated against brute force over 200k randomized convex polygons and directions (0 mismatches).
- **`distance_to(Polygon, Line)`**: new free function, distance between a polygon and an infinite line (zero if they cross); holes ignored, same as `find_extreme_points`. The shared 2D core (`detail::view::distance_to`) reimplements the convex O(log n) search and non-convex O(n) edge scan directly on `View2D`-projected scalars rather than reusing `detail::extreme_points`, since the latter needs a native-dimension direction vector that a `View2D` projection can't reconstruct (its basis is private). The 3D entry point (`calc_utils3d::distance_to`) dispatches on coplanar/parallel (delegates to the 2D core, with a `sqrt(h^2 + d2d^2)` correction for a nonzero plane offset) vs. skew (crossing-point-in-polygon fast path, else an exact per-edge quadratic scan over the 3D boundary — valid for convex and non-convex rings alike). Tested in all three languages, including a dedicated case proving the oblique-crossing skew distance is the anisotropic-metric value, not the naive isotropic one. Writing the first tests for the 2D core caught a real pre-existing bug: the non-convex branch's `min_d = -1` sentinel could never be updated by `std::min`, since `-1` is smaller than any real distance — fixed to use `+infinity` as the sentinel instead.
- **Polyline decimation** (`Polyline2D/3D::Reduce()`, `dist_decimation`/`rdp_decimation`/`vw_decimation`): three vertex-reduction strategies selected via `PolylineDecimationStrategy`. `RadialDistance` is a single O(n) pass; `RamerDouglasPeucker` is an iterative (explicit-stack, no recursion) chord-distance search; `VisvalingamWhyatt` is a doubly-linked-list + lazy-deletion min-heap keyed on triangle area. All three are dimension-agnostic templates shared between `Point2D`/`Point3D`. Writing these tests surfaced two real bugs before they shipped: (1) `rdp_decimation`'s max-distance tracking self-compared a variable against itself (`compare(dist2_max, dist2_max)`), so it never updated and the function silently collapsed every input to just its two endpoints regardless of threshold; (2) `Reduce()`'s three branches returned `{decimated_points}` via brace-init, which doesn't match any `Polyline2D`/`3D` constructor (only copy/move — construction goes through the private-ctor `Make()` factory) — a compile error that only surfaced once a test actually exercised the code path. Separately, the `Reduce_RadialDistance_*` tests (all three languages) initially used a perfectly straight clustered-point dataset; `Polyline2D/3D::Make()` prunes exactly-collinear knots at construction time (`remove_collinear`), so that dataset collapsed to its 2 endpoints before `Reduce()` ever ran, making the test pass for the wrong reason. Fixed by using a dataset with a genuine bend (a "peak" shape) so the decimation logic under test is what actually produces the reduction.

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
