# Changelog

All notable changes to geompp are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Each release covers all three packages at the same version:
- **C++ library** — tagged `v{version}`
- **C# / NuGet** — tagged `csharp-v{version}`
- **Python / PyPI** — tagged `python-v{version}`

---

## [0.7.0] - 2026-05-06

> C++ library — tagged `v0.7.0` · C# / NuGet — tagged `csharp-v0.7.0` · Python / PyPI — tagged `python-v0.7.0`

> Touches `Triangle2D`, `Triangle3D`, `Polygon2D`, `Polygon3D`.

### Added

**C++ core**
- `Triangle2D::Location(Point2D const&)` → `std::optional<std::tuple<double, double>>` — returns barycentric coordinates `(s, t)` where `P = P0 + s·(P1−P0) + t·(P2−P0)`, if the point is inside or on the boundary; `nullopt` if outside. Inverse of `Interpolate`. Implemented via 2D perpendicular dot products (`u.Perp()` / `v.Perp()`).
- `Triangle3D::Location(Point3D const&)` → `std::optional<std::tuple<double, double>>` — same semantics; returns `nullopt` also when the point is off the triangle's plane. Implemented via 3D cross-product isolating each barycentric coordinate without any 2D projection.
- `Polygon2D::IsOnBoundary(Point2D const&) const` — returns `true` if the point lies exactly on an edge (outer ring or any hole boundary); uses `LineSegment2D::Contains` per edge, which tolerates floating-point rounding up to `DECIMAL_PRECISION` digits.
- `Polygon3D::IsOnBoundary(Point3D const&) const` — same semantics; rejects off-plane points immediately, then projects to 2D and delegates to `Polygon2D::IsOnBoundary`.

**Python / PyPI**
- `Triangle2D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Triangle3D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Polygon2D.is_on_boundary(point)` → `bool` — Python binding for the new method.
- `Polygon3D.is_on_boundary(point)` → `bool` — Python binding for the new method.

**C# / NuGet**
- `Triangle2D.Location(Point2D^ point)` → `Tuple<double, double>^` (or `null` if outside) — C# binding for the new `Location` method.
- `Triangle3D.Location(Point3D^ point)` → `Tuple<double, double>^` (or `null` if off-plane or outside) — C# binding for the new `Location` method.
- `Polygon2D.IsOnBoundary(Point2D^ point)` → `bool` — C# binding for the new method.
- `Polygon3D.IsOnBoundary(Point3D^ point)` → `bool` — C# binding for the new method.

### Changed

**C++ core**
- `Triangle2D::Contains(Point2D const&)` — rewritten to delegate entirely to `Location(point).has_value()`. Behavior is unchanged; implementation is now consistent and symmetric with `Interpolate`.
- `Triangle3D::Contains(Point3D const&)` — was an unimplemented stub (`throw std::runtime_error("not implemented")`); now fully implemented. Rejects off-plane points via `BBox3D` and plane check, then delegates to `Location(point).has_value()`. No 2D projection is performed.
- `Polygon2D::Contains(Point2D const&)` — was an unimplemented stub; now implemented using a winding-number algorithm with boundary-inclusive semantics: calls `IsOnBoundary` first, then falls back to winding number for strictly interior points.
- `Polygon3D::Contains(Point3D const&)` — was an unimplemented stub; now implemented. Returns `false` immediately for off-plane points; projects to 2D and applies the winding-number algorithm for in-plane points.

### Fixed

**C++ core**
- `Polygon2D::Contains(Point2D const&)` — fixed inverted boundary logic: `IsOnBoundary` is called first and short-circuits to `true`; previously the order was reversed, causing interior points to return `false`.
- `Polygon2D::FromWkt` / `Polygon3D::FromWkt` — were throwing for valid WKT strings in C++ tests (leftover `EXPECT_ANY_THROW` from when the function was a stub); tests updated to expect successful parse and verify vertex count and first point.
- `Triangle3DTest::Contains_OnBoundary` — off-plane assertion changed from `z=0.001` to `z=0.01`; with `DECIMAL_PRECISION=3` the epsilon is exactly `0.001`, so the old value was within tolerance and the point was classified as on-plane.

### Tests

**C++ (`geompp_tests`)**
- `test_triangle2d.cpp`: `Location` test rewritten — `check_inside` / `check_outside` lambdas that assert `Location` value AND `Contains` status together; round-trip A (`Interpolate(Location(p)) == p`) and round-trip B (`Location(Interpolate(s,t)) == (s,t)`).
- `test_triangle3d.cpp`: `Location` test added — same `check_inside` / `check_outside` / round-trip structure; off-plane point asserts both `Location == nullopt` and `Contains == false`.
- `test_triangle3d.cpp`: `Contains_OnBoundary` — off-plane assertion fixed to `z=0.01`.
- `test_polygon2d.cpp`: `Contains` test completed — interior, near-corner, exterior, and polygon-with-hole cases. `Contains_OnBoundary` — vertices, edge midpoints, and hole boundary. `IsOnBoundary_True` / `IsOnBoundary_False` — explicit standalone tests. `Wkt` and `FromFile` updated to verify successful round-trip.
- `test_polygon3d.cpp`: same coverage as 2D plus off-plane and YZ-plane cases.

**Python (`geompp_python/tests`)**
- `TestTriangle2D.test_location`: rewritten with `check_inside`/`check_outside` helpers and both round-trips.
- `TestTriangle3D.test_location`: added — same structure, including off-plane case.
- `TestPolygon2D.test_contains`: completed — interior, near-corner, exterior, polygon-with-hole, and boundary cases.
- `TestPolygon3D.test_contains`: completed — same plus off-plane assertion.
- `TestPolygon2D.test_is_on_boundary`: vertices, edge midpoints, interior/exterior false cases, hole boundary true and false cases.
- `TestPolygon3D.test_is_on_boundary`: same in 3D plus off-plane false case.

**C# (`geompp_csharp/tests`)**
- `Triangle2D` — `Location_Vertices_ReturnExpectedCoords_2D`, `Location_Centroid_OneThirdEach_2D`, `Location_NullImpliesNotContained_2D`, `Location_RoundTrip_A_And_B_2D`: each asserts `Location` value AND paired `Contains` call; round-trips A and B included.
- `Triangle3D` — `Location_Vertices_ReturnExpectedCoords`, `Location_Centroid_OneThirdEach`, `Location_NullImpliesNotContained`, `Location_RoundTrip_A_And_B`: same relationship-focused structure.
- `Polygon2D` — `Contains_Interior_True`, `Contains_Exterior_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnBoundary_OnEdge_True`, `IsOnBoundary_Interior_False` added.
- `Polygon3D` — `Contains_Interior_True`, `Contains_OffPlane_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnBoundary_OnEdge_True`, `IsOnBoundary_Interior_False` added.
- `Triangle3D` — `Contains_Interior_True`, `Contains_OffPlane_False` added (delegating to `Location`).

---

## [0.6.0] - 2026-05-04

> C++ library — tagged `v0.6.0` · C# / NuGet — tagged `csharp-v0.6.0` · Python / PyPI — tagged `python-v0.6.0`

> Touches `Polyline2D`, `Polyline3D`, `Polygon2D`, `Polygon3D`, `Triangle2D`, `Triangle3D`.

### Changed

**C++ core / Python bindings**
- `Polyline2D::Make` and `Polyline3D::Make` — fixed: now call `remove_collinear(points)` instead of `remove_collinear(remove_duplicates(points))`. `remove_duplicates` removed all duplicate points regardless of position, silently corrupting self-intersecting or backtracking paths; `remove_collinear` already handles consecutive duplicates as a degenerate collinear triplet, so the extra pass was both wrong and redundant.
- `Polygon2D::Make` and `Polygon3D::Make` (no-holes overload) — fixed: now call `remove_collinear(points)`, consistent with the with-holes overload. Previously the no-holes path only called `remove_duplicates`, skipping collinear simplification.
- `remove_duplicates_from_sorted_list()` renamed to `remove_consecutive_duplicates()` (Point2D and Point3D overloads). The old name was misleading — the function removes consecutive equal elements, not all duplicates from a sorted container. Affects `geompp::remove_consecutive_duplicates`, `geompp.remove_consecutive_duplicates` (Python), and all internal callers (`Polygon2D::Make`, `Polygon3D::Make`, `Triangle2D::Intersection`).

**C++ core**
- `Polyline2D::Length()` and `Polyline3D::Length()` — now inline cached getters; value is pre-computed in `Make()` and stored as a private member. No change to public API.
- `Polygon2D::Perimeter()` and `Polygon3D::Perimeter()` — same inline-cached pattern. No change to public API.
- `Polyline2D::operator=` and `Polyline3D::operator=` — now copy the cached `LENGTH` member.
- `Polygon2D::operator=` and `Polygon3D::operator=` — now copy the cached `PERIMETER` member (and `PLANE` for 3D).
- `Polyline2D::AlmostEquals()` and `Polyline3D::AlmostEquals()` — added fast-rejection on `LENGTH` difference before comparing vertices.
- `Polyline2D::Contains()` and `Polyline3D::Contains()` — rewritten with `std::ranges::any_of` over `ToSegments()`.
- `Polyline2D::DistanceTo()` and `Polyline3D::DistanceTo()` — rewritten with `std::ranges::min` + `views::transform` over `ToSegments()`; no intermediate allocation.
- `Polyline2D::ProjectOnto()` and `Polyline3D::ProjectOnto()` — fixed double segment construction; now carries the best projected point directly through the loop.
- `Polyline2D::Location()` and `Polyline3D::Location()` — fixed double `segs[i]` construction per iteration; each segment is now materialized once with `auto seg = segs[i]`.
- `Polyline2D::Interpolate()` and `Polyline3D::Interpolate()` — now throws `std::invalid_argument` when `pct` is outside `[0, 1]`; previously clamped silently.
- `Triangle2D::Contains()` — inlines the removed `Location()` math directly; behaviour unchanged.
- `Triangle2D::DistanceTo()` — reverted to `throw std::runtime_error("not implemented")`; previous implementation was incorrect.

### Added

**Python / PyPI**
- `LineSegment2D.project_onto()`, `LineSegment3D.project_onto()`, `Ray2D.project_onto()`, `Ray3D.project_onto()`, `Polyline2D.project_onto()`, `Polyline3D.project_onto()` — bound from the existing C++ `ProjectOnto(Point)` method; was callable from C++ and C# but missing from Python bindings.

### Fixed

**C++ core**
- `Polyline2D::Location()` and `Polyline3D::Location()` — corrected unit mismatch: `seg.Location(point)` returns a [0, 1] fraction but was treated as a raw length accumulator; result is now `(tot_len + seg.Location(point) * seg.Length()) / LENGTH`.
- `Polyline2D::Interpolate()` and `Polyline3D::Interpolate()` — corrected unit mismatch: `pct` ∈ [0, 1] was compared directly against cumulative segment lengths; fixed by computing `target = pct * LENGTH` before the selection loop.
- `WktParser::FromWkt()` — `LINESTRING` WKT strings were silently parsed as `Line2D`/`Line3D` because the `LINE` prefix check ran before the `LINESTRING` check; fixed by moving the `LINESTRING` branch first.

**C# / NuGet**
- `Line2D` binding — removed spurious `Location(Point2D^)` declaration and implementation; the backing `geompp::Line2D` has no such method, causing a build failure.

### Removed (breaking)

**C++ core**
- `Polygon2D::Location()` and `Polygon3D::Location()` — removed; function had no meaningful geometric definition for a polygon.
- `Polygon2D::Interpolate()` and `Polygon3D::Interpolate()` — removed; function had no meaningful geometric definition for a polygon.
- `Triangle2D::Location()` — removed; returned parametric `(s,t)` coordinates as a `Point2D`, which is a type misuse and leaked an internal implementation detail.
- `Triangle3D::Location()` — removed (was unimplemented stub).

**C# / NuGet (breaking)**
- `Polygon2D.Location()`, `Polygon3D.Location()`, `Polygon2D.Interpolate()`, `Polygon3D.Interpolate()` — removed.
- `Triangle2D.Location()` and `Triangle3D.Location()` — removed.

**Python / PyPI (breaking)**
- `Polygon2D.location()`, `Polygon3D.location()`, `Polygon2D.interpolate()`, `Polygon3D.interpolate()` — removed.
- `Triangle2D.location()` and `Triangle3D.location()` — removed.

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: added `ToSegments`.
- `test_polygon3d.cpp`: added `ToSegments`.
- `test_polyline2d.cpp`: updated `Interpolate` — replaced clamp-assertions with `EXPECT_ANY_THROW` for out-of-range `pct`.
- `test_polyline3d.cpp`: same as above.
- `test_triangle2d.cpp`: replaced `Location`-based roundtrip assertion in `Interpolate` test with `Contains(Centroid())`; added outside-returns-nullopt cases; replaced `DistanceTo` test body with `EXPECT_ANY_THROW`.
- `test_triangle3d.cpp`: removed `Location` test.

**Python (`geompp_python/tests`)**
- `TestPolygon2D`, `TestPolygon3D`, `TestPolyline2D`, `TestPolyline3D`: added `test_to_segments`.
- `TestPolyline2D`, `TestPolyline3D`: updated `test_interpolate` — added `pytest.raises` for out-of-range `pct`.
- `TestTriangle2D`: removed `test_location`; expanded `test_interpolate` (vertex checks, outside returns `None`).
- `TestTriangle3D`: added `test_interpolate` (vertices, centroid, outside returns `None`).

**C# (`geompp_csharp/tests`)**
- `Vector2D`: added ~12 tests covering `AlmostEquals`, arithmetic operators, `Length`, `Dot`, `Cross`, `Normalize`, `Perp`, `IsParallel`, `IsPerpendicular`.
- `WktParser`: added 10 tests (`Open_ValidFile_NoThrow`, `Next_Returns*` for Point2D/3D, LineSegment2D/3D, Polyline2D/3D, `Next_SkipsComments`, `FromWkt_Point2D`, `ToWkt_Point2D`); all file-using tests wrapped in `using (var parser = ...)` blocks to ensure `Dispose()` closes the `std::ifstream` before `File.Delete`.
- `GeometryCollection2D` and `GeometryCollection3D`: added ~15 tests each covering `Add`, `Get`, `Size`, `AlmostEquals`, `ToWkt`, `FromWkt`, `ToFile`/`FromFile`.
- `Polyline2D` / `Polyline3D`: corrected `DistanceTo_PointAboveSegment` — expected distance changed from `3.0` to `2.0`: for polyline `(0,0)→(4,0)→(4,4)` and point `(2,3)`, the nearest point is on the vertical segment at `(4,3)`, distance 2.

**Python (`geompp_python/tests`)**
- `TestPolyline2D` / `TestPolyline3D`: corrected `test_project_onto` — expected `project_onto(Point(2,3))` updated to `Point(3,3)` (nearest, distance 1) from `Point(2,0)` (distance 3).

---

## [0.5.0] - 2026-05-01

> C++ library — tagged `v0.5.0` · C# / NuGet — tagged `csharp-v0.5.0` · Python / PyPI — tagged `python-v0.5.0`

### Added

**C++ core**
- `Axis` enum (`X`, `Y`, `Z`) in `vector3d.hpp` — axis identifier used by the dominant-axis algorithm.
- `Vector3D::DominantAxis()` — returns the `Axis` whose absolute component is largest; used internally by the 3D shoelace formula and available as a public API.
- `Point2D::operator+=(Vector2D const&)` and `Point3D::operator+=(Vector3D const&)` — in-place point translation.
- `Point2D operator/(Point2D, double)` and `Point3D operator/(Point3D, double)` — scalar division (throws on division by zero).
- `centroid(std::vector<Point2D> const&)` free function (declared in `point2d.hpp`) — shoelace-formula centroid of a 2D polygon ring.
- `centroid(std::vector<Point3D> const&, std::optional<Plane>)` free function (declared in `plane.hpp`) — 3D centroid; auto-detects the plane if not supplied.
- `Polygon2D::Perimeter()` — sum of outer-ring edge lengths (holes excluded).
- `Polygon3D::Perimeter()` — sum of outer-ring edge lengths.
- `Polygon3D::Centroid()` — area-weighted centroid; hole areas contribute with negative weight.
- `Polygon3D::GetPlane()` — returns the `Plane` stored on construction.

**C# / NuGet**
- `Polygon2D.Perimeter()` and `Polygon3D.Perimeter()` exposed.
- `Polygon3D.Centroid()` and `Polygon3D.GetPlane()` exposed.

**Python / PyPI**
- `Axis` enum exposed (`geompp.Axis.X / .Y / .Z`).
- `Vector3D.dominant_axis()` exposed.
- `Point2D.__iadd__(Vector2D)` and `Point3D.__iadd__(Vector3D)` exposed (`p += v` syntax).
- `Polygon2D.perimeter()` and `Polygon3D.perimeter()` exposed.
- `Polygon3D.centroid()` and `Polygon3D.get_plane()` exposed.
- `centroid(points)` (2D list) and `centroid(points, plane)` (3D list) free functions exposed.
- `signed_area(points, plane)` (3D) free function exposed.

### Fixed

**C++ core**
- `Polygon2D::Centroid()` with holes: the weighted sum was computing `hole_area²` instead of `hole_area`, producing wrong centroids whenever holes were present. Also fixed an MSVC `C2672: std::construct_at` build error caused by pre-sizing a `std::vector<std::pair<Point2D, double>>` over a type with no default constructor — rewritten to use `reserve()` + range iteration.
- `signed_area(std::vector<Point3D>, Plane)`: now throws `std::runtime_error` for fewer than 3 unique points (previously returned 0 silently).

### Removed

**C++ core (breaking)**
- `Polygon2D::SignedArea()` — removed. Use the free function `signed_area(points)` (2D) instead.
- `Polygon3D::SignedArea()` — removed. Use the free function `signed_area(points, plane)` (3D) instead.

**C# / NuGet (breaking)**
- `Polygon2D.SignedArea()` and `Polygon3D.SignedArea()` removed to match the C++ API change above.

### Tests

**C++ (`geompp_tests`)**
- `test_point2d.cpp`: added `AddVectorInPlace` (3 cases).
- `test_point3d.cpp`: added `AddVectorInPlace` (3 cases).
- `test_plane.cpp`: added 9 `Centroid_*` tests for the 3D `centroid()` free function.
- `test_polygon2d.cpp`: added `Centroid_SquareWithCenteredHole`, `Centroid_SquareWithOffCenterHole`; added `Perimeter_Square`, `Perimeter_Rectangle`, `Perimeter_Triangle`.
- `test_polygon3d.cpp`: added `Centroid_Square`, `Centroid_ElevatedSquare`, `Centroid_Triangle`, `Centroid_NonXYPlane`, `Centroid_SquareWithCenteredHole`, `Centroid_SquareWithOffCenterHole`; added `Perimeter_Square`, `Perimeter_NonXYPlane`, `Perimeter_Triangle`.

**C# (`geompp_csharp/tests`)**
- `Polygon2D`: added `Perimeter_Square`, `Perimeter_Rectangle`, `Centroid_Square`, `Centroid_Rectangle`.
- `Polygon3D`: added `Perimeter_Square`, `Perimeter_NonXYPlane`, `Centroid_Square`, `Centroid_ElevatedSquare`, `GetPlane_XYPlane_NormalPointsInZ`, `GetPlane_ContainsAllVertices`.

**Python (`geompp_python/tests`)**
- `TestVector3D`: added `test_dominant_axis`.
- `TestPoint2D`: added `test_iadd_vector`, `test_iadd_zero_vector_unchanged`.
- `TestPoint3D`: added `test_iadd_vector`, `test_iadd_zero_vector_unchanged`.
- `TestPolygon2D`: added `test_perimeter_square`, `test_perimeter_rectangle`, `test_perimeter_triangle`.
- `TestPolygon3D`: added `test_perimeter_square`, `test_perimeter_non_xy_plane`, `test_perimeter_triangle`, `test_get_plane_returns_plane`, `test_get_plane_contains_all_vertices`, `test_get_plane_with_holes`.
- `TestCentroid3D` (new class): 6 tests for `centroid(points)` and `centroid(points, plane)`.

---

## [0.4.0] - 2026-04-25

> C++ library — tagged `v0.4.0` · C# / NuGet — tagged `csharp-v0.4.0` · Python / PyPI — tagged `python-v0.4.0`

### Changed

**All packages**
- `LVSParser` renamed to `WktParser` everywhere: C++ class, header (`wkt_parser.hpp`), source (`wkt_parser.cpp`), C# wrapper (`WktParser.hpp/.cpp`), Python binding (`bind_wktparser.cpp`, exposed as `geompp.WktParser`), all CMakeLists, vcxproj files, tests, and documentation. The old name is gone entirely — update any call sites.

### Added

**C++ core**
- `GeometryCollection2D` / `GeometryCollection3D`: heterogeneous container holding any mix of 2D (or 3D) primitives. Supports `Add()`, `Get(index)`, `Size()`, `ToWkt()`, `FromWkt()`, `ToFile()`, `FromFile()`, `AlmostEquals()`, and `operator==`.
- `Polygon2D::Make(points, holes)`: new overload that accepts an outer ring and a list of hole rings. Validates that the outer ring is CCW and each hole is CW; throws on violation.
- `Polygon3D::Make(points, holes)`: same, additionally validates that outer ring and all hole rings are coplanar.
- `are_ccw(points)` / `are_cw(points)` (2D, declared in `point2d.hpp`): free functions returning `true` when the ordered point list has the specified winding.
- `are_coplanar(points)` / `are_ccw(points)` / `are_cw(points)` (3D, declared in `plane.hpp`): 3D winding and coplanarity checks on point lists.
- `WktParser::FromWkt(wkt)`: static method — parse any WKT string into a `ReturnSet` (already existed as `Get()`, renamed to `FromWkt()` for consistency with the serialization API).
- `WktParser::ToWkt(shape)`: static method — serialize a `ReturnSet` back to its WKT string; throws on `nullopt`.

**C# / NuGet**
- `Polygon2D.Make(array<Point2D^>^ points, array<array<Point2D^>^>^ holes)` exposed.
- `Polygon3D.Make(array<Point3D^>^ points, array<array<Point3D^>^>^ holes)` exposed.

**Python / PyPI**
- `GeometryCollection2D` and `GeometryCollection3D` exposed with full `add()`, `get()`, `size()`, WKT, file I/O, and equality support.
- `Polygon2D.make(points)` and `Polygon2D.make(points, holes)` exposed.
- `Polygon3D.make(points)` and `Polygon3D.make(points, holes)` exposed.
- `are_ccw(points)` / `are_cw(points)` exposed for `list[Point2D]`.
- `are_coplanar(points)` / `are_ccw(points)` / `are_cw(points)` exposed for `list[Point3D]`.
- `WktParser.from_wkt(wkt)` — parse a WKT string; returns a geometry object or `None`.
- `WktParser.to_wkt(shape)` — serialize any geometry object to its WKT string; raises on `None` or unsupported type.

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: added `WktWithHoles`, `WithHoles_Valid`, `WithHoles_PerimeterCW_Throws`, `WithHoles_HoleCCW_Throws`, `WithHoles_HoleTooFewPoints_Throws`.
- `test_polygon3d.cpp`: added same suite plus `WithHoles_NonCoplanar_Throws`.
- `test_utils.cpp`: added `AreCCW_2D`, `AreCoplanar_3D`, `AreCCW_3D`.
- `test_wkt_parser.cpp` (renamed from `test_lsv_parser.cpp`): existing `FromFile` test retained.

**Python (`geompp_python/tests`)**
- `TestWktParser` (renamed from `TestLVSParser`): added `test_get_returns_geometry`, `test_get_returns_none_on_unknown`, `test_to_wkt_point2d`, `test_to_wkt_linesegment2d`, `test_to_wkt_point3d`, `test_to_wkt_none_raises`, `test_to_wkt_roundtrip`, `test_to_wkt_unsupported_type_raises`.
- `TestPolygon2D`: added holes construction, CCW-outer throw, CW-hole throw, and too-few-points throw cases.
- `TestPolygon3D`: added same suite plus non-coplanar throw.
- `TestFreeFunctions`: added `are_ccw`/`are_cw` (2D), `are_coplanar`/`are_ccw`/`are_cw` (3D).

---

## [0.3.0] - 2026-04-24

> C++ library — tagged `v0.3.0`

### Changed

**C++ core**
- `Line2D::AlmostEquals` / `operator==`: now geometric equality — two lines are equal if they lie on the same infinite line (parallel directions, collinear origins). The old P0/P1 coordinate comparison is replaced. Anti-parallel lines on the same infinite line are equal; parallel but offset lines are not. **Breaking for callers that relied on the old point-coordinate comparison.**
- `Line3D::AlmostEquals` / `operator==`: same geometric-equality change.
- `LineSegment2D::AlmostEquals` / `operator==`: now order-agnostic — `Make(A, B) == Make(B, A)`. **Breaking for callers that relied on direction-sensitive equality.**
- `LineSegment3D::AlmostEquals` / `operator==`: same order-agnostic change.

### Fixed

**C++ core**
- `Ray2D::Contains`: replaced ad-hoc cross-product collinearity check with `ToLine().Contains(point) && IsAhead(point)` for consistency with `Line2D::Contains`.
- `Triangle2D::Intersects(Triangle2D const&)`: corrected return type from `ReturnSet` to `bool`, matching the pattern of all other `Intersects` overloads.
- `Triangle3D::Intersects(Triangle3D const&)`: same return-type correction.

### Added

**C++ core**
- `Triangle2D::Intersection(Ray2D const&)`: implemented. Returns a `Point2D` if the ray clips one edge, a `LineSegment2D` if it crosses two edges, or `nullopt` if it misses or is entirely behind the triangle.
- `Triangle2D::Intersection(LineSegment2D const&)`: implemented. Returns a `Point2D` if the segment crosses one boundary edge, a `LineSegment2D` if it crosses two, or `nullopt` if entirely outside or entirely inside.
- `Triangle2D::Intersects(Ray2D const&)` and `Triangle2D::Intersects(LineSegment2D const&)`: now delegate to the corresponding `Intersection` overloads (were stubs that threw).

### Tests
- `test_line2d.cpp`: added `Location`, `ProjectOnto`, `AlmostEquals` (geometric-equality and epsilon cases).
- `test_line3d.cpp`: extended `AlmostEquals` with same-infinite-line and reversed-direction cases; added `DistanceTo`.
- `test_line_segment2d.cpp`: added `AlmostEquals` including reversed-segment equality.
- `test_line_segment3d.cpp`: extended `AlmostEquals` with reversed-segment case.
- `test_ray3d.cpp`: added `DistanceTo`, `IntersectionWithRay3D`.
- `test_polyline3d.cpp`: added `Location`.
- `test_triangle2d.cpp`: added `IntersectionWRay` and `IntersectionWSegment`.

---

## [0.2.0] - 2026-04-24

> C++ library — tagged `v0.2.0` · C# / NuGet — tagged `csharp-v0.2.0` · Python / PyPI — tagged `python-v0.2.0`

### Changed

**C++ core**
- `Triangle3D::SignedArea(Vector3D const& ref_normal)`: signature changed — now requires an explicit reference normal. Result is `ref_normal.Dot(AreaVector())`; sign depends on which side of the plane `ref_normal` points to. Existing no-argument callers must be updated.
- `Triangle3D::IsCCW(Vector3D const& ref_normal)`: same — now requires explicit reference normal.

**C# / NuGet**
- `Triangle3D.SignedArea(Vector3D^ refNormal)` and `Triangle3D.IsCCW(Vector3D^ refNormal)`: updated to match new C++ signatures.

**Python / PyPI**
- `Triangle3D.signed_area(ref_normal)` and `Triangle3D.is_ccw(ref_normal)`: updated to match new C++ signatures.

### Added

**C++ core**
- `Triangle2D::IsCCW()`: returns `true` if winding is counter-clockwise (`SignedArea() > 0`).
- `Triangle3D::Normal()`: returns the unit normal vector (`AreaVector().Normalize()`).
- `Triangle3D::ToPolygon()`: converts the triangle to a `Polygon3D` with the same three vertices.

**C# / NuGet**
- `Triangle2D.IsCCW()`, `Triangle3D.Normal()`, `Triangle3D.ToPolygon()` newly exposed.

**Python / PyPI**
- `Triangle2D.is_ccw()`, `Triangle3D.normal()`, `Triangle3D.to_polygon()` newly exposed.

### Fixed

**C++ core**
- `triangle3d.hpp`: typo `book IsCCW()` corrected to `bool IsCCW()` — function was syntactically invalid before this fix.
- `test_plane.cpp`: `Plane::AlmostEquals` was already implemented but the test was a stale placeholder expecting a throw; replaced with real assertions.

### Tests
- `test_plane.cpp`: added `AlmostEquals` cases — equal planes, distinct planes, anti-parallel normals at same offset, different offsets.
- `test_triangle3d.cpp`: added `ToPolygon`, `Normal`, `IsCCW`; updated `SignedArea` to pass `ref_normal`.
- `test_triangle2d.cpp`: added `IsCCW`.

---

## [0.1.3] - 2026-04-13

> C++ library — tagged `v0.1.3`

### Fixed

- `Polygon2D::AlmostEquals` and `Polygon2D::ToWkt`: bit-shift (`<<`) typo in loop
  conditions corrected to less-than (`<`); loops now iterate over all vertices.
- `Polygon2D::ToWkt`: ring now closes correctly — first vertex repeated at the end,
  producing valid WKT (`POLYGON ((x0 y0, …, x0 y0))`).
- `Polygon3D`: same loop and ring-closure fixes; `ToWkt` now also includes the z
  coordinate (`"{} {} {}"` format instead of `"{} {}"`).
- `BBox3D` copy constructor: was initialising `MIN` from `b.MAX` instead of `b.MIN`;
  all `BBox3D` copies were silently corrupt.
- `BBox3D::Contains`: z-axis bounds check was missing; added.
- `BBox2D::operator=` and `BBox3D::operator=`: declared but never defined; linker
  error on any code using assignment; implementations added.
- `Line3D::Contains` and `Polyline3D::Contains`: used `Vector3D::Perp()` for the
  collinearity check, which is incorrect in 3D — a single perpendicular dot product
  being zero does not guarantee the point lies on the geometry. Both functions now
  use the cross-product magnitude check. `Contains` and `Location` are public API
  exposed in the C# and Python bindings, so callers may have received wrong results.

### Tests
- Added full test suites for `Plane`, `BBox2D`, `BBox3D`, `Polygon2D`, `Polygon3D`.
- Extended coverage for `Triangle3D`, `Polyline2D`, `LineSegment3D`.

---

## [0.1.2] - 2026-04-13

> C# / NuGet — tagged `csharp-v0.1.2` · Python / PyPI — tagged `python-v0.1.2`

### Fixed

**C# / NuGet**
- Rebuilt against geompp core 0.1.3. Inherits all C++ bugfixes: `Polygon2D`/`Polygon3D`
  loop and ring-closure corrections, `BBox3D` copy-constructor MIN/MAX fix,
  `BBox3D::Contains` z-check, `BBox2D`/`BBox3D` assignment operator.
- `Line3D.Contains`, `Line3D.Location`, `Polyline3D.Contains`, `Polyline3D.Location`:
  fixed incorrect 3D collinearity check (see C++ entry above).

**Python / PyPI**
- Rebuilt against geompp core 0.1.3. Same bugfixes as C# above.
- `line3d.contains()`, `line3d.location()`, `polyline3d.contains()`,
  `polyline3d.location()`: fixed incorrect 3D collinearity check (see C++ entry above).
- `pypi-publish.yml` now stamps `pyproject.toml` version from the git tag at build
  time — no manual file edits needed before releasing.

---

## [0.1.1] - 2026-04-13

### Fixed

**C++ core**
- `Line3D::Intersection(sc, tc)`: parameter `tc` was passed by value instead of
  by reference, leaving the caller's variable uninitialized in Debug builds
  (MSVC Run-Time Check Failure #3).
- `Line2D::Intersection(Line2D const&, double& sc, double& tc)` and
  `Line3D::Intersection(Line3D const&, double& sc, double& tc)` now wrap their
  computation in a `try/catch(...)` block; unexpected exceptions log a warning
  and return `std::nullopt` with `sc = tc = NaN` instead of propagating.

**C# (GeomPP NuGet)**
- Updated to geompp core 0.1.1. Exposes the corrected `Line3D.IntersectionWithParams`
  which now correctly returns the `tc` out-parameter.
- Fixed `0xC0000005` crash when loading `GeomPP.dll` under .NET 5+: geompp core
  is now recompiled inside the C++/CLI project with `GEOMPP_NO_THREAD_LOCAL`,
  replacing `thread_local` globals with plain globals (the .NET 5+ CLR does not
  call `DllMain` for C++/CLI assemblies, preventing TLS slot allocation).
- Added C# smoke tests (`geompp_csharp/tests/`) covering `Precision`, `Point2D`,
  `Point3D`, `Vector3D`, `Line2D`, `Line3D`, and `LineSegment2D`; run via
  `dotnet run --project geompp_csharp\tests\GeomPPTests.csproj`.

**Python (geompp PyPI)**
- Updated to geompp core 0.1.1. `line3d.intersection_with_params()` now correctly
  returns the `tc` value.

### Build / CI
- `enable_testing()` added to root `CMakeLists.txt` so `ctest` finds tests from
  the root build directory on all generators.
- CI `ctest` steps now point at `build/geompp_tests` to avoid running glog's own
  internal test suite alongside geompp's tests.

---

## [0.1.0] - 2026-04-08

### Added

**C++ core**
- Core 2D primitives: `Point2D`, `Vector2D`, `Line2D`, `Ray2D`, `LineSegment2D`,
  `Polyline2D`, `Triangle2D`, `Polygon2D`, `BBox2D`.
- Core 3D primitives: `Point3D`, `Vector3D`, `Line3D`, `Ray3D`, `LineSegment3D`,
  `Polyline3D`, `Triangle3D`, `Polygon3D`, `BBox3D`, `Plane`.
- Intersection queries among `Line`, `Ray`, and `LineSegment` in 2D and 3D.
- `ToWkt()` / `FromWkt()` / `ToFile()` / `FromFile()` serialization for all types.
- Static library release archives for Linux x86_64 and Windows x64.

**C# (GeomPP NuGet)**
- C++/CLI bindings targeting .NET 10 (`net10.0-windows7.0`) and .NET Framework 4.8.
- Full wrapper for all 2D/3D primitives with operator support and WKT serialization.

**Python (geompp PyPI)**
- pybind11 bindings for all 2D/3D primitives.
- Available on PyPI: `pip install geompp`.
- Wheels for Linux x86_64 and Windows AMD64, Python 3.8–3.12.
