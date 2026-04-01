# Test Coverage Plan

Generated from the coverage analysis in `test_coverage_report.md`.
Items are ranked by impact: entire untested classes first, then individual missing methods.

---

## How to measure coverage

```bash
# CMake — Debug build with gcov instrumentation
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="--coverage" \
      -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
      ..
cmake --build .

# Run the test suite
./geompp_tests/geompp_tests

# Collect and display a summary
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '*/build/_deps/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
# Open coverage_html/index.html in a browser
```

---

## Priority 1 — Entire classes with no tests

### 1.1 `Polyline3D` — no test file
**New file:** `geompp_tests/src/test_polyline3d.cpp`

- [ ] `Make` — valid 3-knot polyline; deduplication removes coincident knots; collinear knots are removed; throw on < 2 unique points
- [ ] `Size` — matches expected knot count after deduplication
- [ ] `Length` — axis-aligned segment; multi-segment sum (e.g. L-shape)
- [ ] `Contains` — each knot (true); midpoints of each segment (true); point before start (false); point after end (false)
- [ ] `Location` / `Interpolate` — round-trip: `Location(Interpolate(t)) ≈ t` for t ∈ {0, 0.25, 0.5, 1.0}
- [ ] `DistanceTo` — point on polyline (0); point 1 unit off a segment; point beyond an endpoint
- [ ] `ToSegments` — segment count == `Size() - 1`; each segment endpoints match consecutive knots
- [ ] `Intersects(Line3D)` / `Intersection(Line3D)` — line crosses one segment; line parallel to all segments; line behind
- [ ] `Intersects(Ray3D)` / `Intersection(Ray3D)` — ray crosses segment ahead; ray points away (no intersection)
- [ ] `Intersects(LineSegment3D)` / `Intersection(LineSegment3D)` — segment crosses polyline; segment misses
- [ ] `Intersects(Polyline3D)` / `Intersection(Polyline3D)` — two polylines crossing; non-intersecting
- [ ] `AlmostEquals` — identical polylines; polylines differing in one knot
- [ ] `operator==` — same, different
- [ ] `ToWkt` / `FromWkt` — round-trip; wrong geometry keyword throws; missing bracket throws; missing comma throws
- [ ] `ToFile` / `FromFile` — write to temp file, read back, compare equal; cleanup temp file

---

### 1.2 `Plane` — test file exists but bodies are empty
**File:** `geompp_tests/src/test_plane.cpp`

- [ ] `From3Points` — three XY-plane points → normal is (0,0,1); arbitrary 3D points; throw on collinear input
- [ ] `FromOriginAndNormal` — normal is stored normalized
- [ ] `FromOriginAndAxes` — axes are stored; normal == axis_u × axis_v
- [ ] `XY()` / `YZ()` / `ZX()` — verify origin, normal directions, and axis orientations
- [ ] `AlmostEquals` — same plane; plane with flipped normal (expect ≠)
- [ ] `SignedDistanceTo` — point above plane (+); point below plane (−); point on plane (0)
- [ ] `DistanceTo` — always ≥ 0; equals `abs(SignedDistanceTo)`
- [ ] `Contains` — point on plane (true); point 0.001 off plane (false at DP_THREE)
- [ ] `ProjectOnto` — off-plane point projects to closest point on plane; result passes `Contains`
- [ ] `ProjectInto` — result is a `Point2D`; `Evaluate(ProjectInto(p))` round-trips back to the projected 3D point
- [ ] `Evaluate` — `Evaluate({0,0})` == plane origin; `Evaluate({1,0})` == `origin + axis_u`
- [ ] `Intersects(Line3D)` / `Intersection(Line3D)` — line pierces plane (has value, result lies on plane); line parallel to plane (no value); line contained in plane (edge case — document expected behavior)
- [ ] `operator==` — same plane; different origin; different normal

---

### 1.3 `BBox2D` — no test file
**New file:** `geompp_tests/src/test_bbox2d.cpp`

- [ ] `BBox2D(Point2D, Point2D)` — min/max ordered correctly regardless of argument order
- [ ] `BBox2D(LineSegment2D)` — min/max span both endpoints
- [ ] `BBox2D(Polyline2D)` — min/max span all knots
- [ ] `BBox2D(Triangle2D)` — min/max span all three vertices
- [ ] `BBox2D(Polygon2D)` — min/max span all vertices
- [ ] `min()` / `max()` — values match construction input
- [ ] `Contains(Point2D)` — interior (true); on boundary (true); exterior (false)
- [ ] `AlmostEquals` — same box; box differing by epsilon
- [ ] `operator==` — same, different

---

### 1.4 `BBox3D` — no test file
**New file:** `geompp_tests/src/test_bbox3d.cpp`

- [ ] `BBox3D(Point3D, Point3D)` — min/max ordered in all three axes
- [ ] `min()` / `max()` — values match construction input
- [ ] `Contains(Point3D)` — interior (true); on boundary (true); exterior (false)
- [ ] `AlmostEquals` — same box; box differing by epsilon in z only
- [ ] `operator==` — same, different

---

### 1.5 `Polygon3D` — no test file
**New file:** `geompp_tests/src/test_polygon3d.cpp`

- [ ] `Make` — valid polygon; throw on < 3 points; throw on coincident points
- [ ] `Size` — matches vertex count
- [ ] `AlmostEquals` — same polygon; polygon with one vertex shifted
- [ ] `ToWkt` / `FromWkt` — round-trip; invalid keyword throws
- [ ] `ToFile` / `FromFile` — round-trip via temp file
- [ ] `operator==` — same, different

---

## Priority 2 — Partially tested classes

### 2.1 `Triangle3D` — 8 methods untested
**File:** `geompp_tests/src/test_triangle3d.cpp`

- [ ] `Area` — flat triangle in XY plane at z=2 (compare to 2D result); triangle with non-zero Z spread
- [ ] `Perimeter` — same triangle as `Area` test; verify it equals sum of side lengths
- [ ] `Contains` — each vertex (true); centroid (true); point outside (false); point in same plane but outside (false)
- [ ] `DistanceTo` — point on surface (0); point 1 unit above the plane
- [ ] `Location` — at each vertex; at centroid
- [ ] `Interpolate` — `Interpolate(0,0)` == vertex 0; `Interpolate(1,0)` == vertex 1; `Interpolate(0,1)` == vertex 2; round-trip with `Location(centroid)`
- [ ] `ToAxis` — returned vectors are orthogonal; both lie in the triangle's plane
- [ ] `AlmostEquals` — same triangle; triangle with one vertex shifted

---

### 2.2 `Line2D` — 6 methods untested
**File:** `geompp_tests/src/test_line2d.cpp`

- [ ] `AlmostEquals` — same line; lines with different origin on same infinite line (expect ==); different direction (expect ≠)
- [ ] `ProjectOnto` — point above line projects to foot; point already on line projects to itself; point behind origin projects to negative location
- [ ] `Location` — at origin (0.0); at a known point ahead (+); at a known point behind (−)
- [ ] `Intersects(Ray2D)` / `Intersection(Ray2D)` — ray crosses line (true, has value); ray parallel to line (false); intersection point behind ray origin (false)
- [ ] `Intersects(LineSegment2D)` / `Intersection(LineSegment2D)` — segment crosses line; segment parallel; segment too short to reach line

---

### 2.3 `Line3D` — 3 methods untested
**File:** `geompp_tests/src/test_line3d.cpp`

- [ ] `DistanceTo(Point3D)` — point on line (0); point perpendicular off line; skew point
- [ ] `Intersects(Ray3D)` / `Intersection(Ray3D)` — ray crosses line (has value); ray parallel; intersection behind ray origin
- [ ] `Intersects(LineSegment3D)` / `Intersection(LineSegment3D)` — segment crosses line; segment parallel; segment too short

---

### 2.4 `LineSegment2D` — 2 methods untested
**File:** `geompp_tests/src/test_line_segment2d.cpp`

- [ ] `Length` — axis-aligned (3.0); 3-4-5 hypotenuse (5.0); symmetry: `s.Length() == s_reversed.Length()`
- [ ] `ToLine` — result passes through both endpoints; direction is normalized

---

### 2.5 `LineSegment3D::DistanceTo` — 1 method untested
**File:** `geompp_tests/src/test_line_segment3d.cpp`

- [ ] Point between the endpoints (perpendicular distance only)
- [ ] Point beyond an endpoint (distance to nearest endpoint)
- [ ] Point on the segment (0)

---

### 2.6 `Ray2D::AlmostEquals` / `Ray2D::ToLine` — 2 methods untested
**File:** `geompp_tests/src/test_ray2d.cpp`

- [ ] `AlmostEquals` — same ray; ray with same origin but different direction (≠); ray with same direction but different origin (≠)
- [ ] `ToLine` — result has same origin and direction as the ray

---

### 2.7 `Ray3D` — 5 methods untested
**File:** `geompp_tests/src/test_ray3d.cpp`

- [ ] `Contains` — point on ray (true); point at origin (true); point behind origin (false); point off-axis (false)
- [ ] `DistanceTo` — point on ray (0); point ahead but off-axis; point behind origin
- [ ] `Intersects(Ray3D)` / `Intersection(Ray3D)` — two rays crossing in 3D; parallel; skew (no intersection)
- [ ] `Intersects(LineSegment3D)` / `Intersection(LineSegment3D)` — segment crosses ray; segment behind ray origin

---

### 2.8 `Polygon2D` — serialization untested
**File:** add a `test_polygon2d.cpp` or extend `test_triangle2d.cpp`

- [ ] `FromWkt` — round-trip from `ToWkt`; wrong keyword throws; missing bracket throws
- [ ] `ToFile` / `FromFile` — write to temp file, read back, compare equal
- [ ] `AlmostEquals` — same polygon; polygon with one vertex shifted

---

## Priority 3 — Utility functions (`utils.hpp`)

**File:** new `geompp_tests/src/test_utils.cpp`

- [ ] `trim(" hello ")` → `"hello"`; leading only; trailing only; already trimmed; empty string
- [ ] `to_upper("hElLo")` → `"HELLO"`; already uppercase; empty string
- [ ] `sign(3.5)` → 1; `sign(-2.0)` → -1; `sign(0.0)` → 0
- [ ] `tokenize_to_doubles("1.5 -3.0 0")` → `{1.5, -3.0, 0.0}`; extra spaces; invalid token throws
- [ ] `tokenize_string("a,b,c", ',')` → `{"a","b","c"}`; empty token between delimiters; no delimiter
- [ ] `count_decimal_places(1.23)` → 2; integer (0); negative value
- [ ] `string_join({"a","b","c"}, ", ")` → `"a, b, c"`; single element; empty vector
- [ ] `ToWkt(vector<Point3D>{...})` → `"GEOMETRYCOLLECTION(POINT ..., POINT ...)"` format
- [ ] `linear_combination(vector<Point2D>, weights)` — weights summing to 1 return weighted average; verify with known result

---

## Priority 4 — `Vector2D` gaps
**File:** `geompp_tests/src/test_vector2d.cpp`

- [ ] `Length` — `(1,0)` → 1.0; `(0,0)` → 0.0; `(3,4)` → 5.0
- [ ] `Normalize` — result has length 1.0; direction preserved; zero vector throws or returns zero (document behavior)
- [ ] `AlmostEquals` — same vector; near-equal within DP_THREE; clearly different
- [ ] `operator-()` unary — `-(1,2)` → `(-1,-2)`
- [ ] `operator-(Vector2D, Vector2D)` — `(3,4)-(1,1)` → `(2,3)`
- [ ] `operator/(Vector2D, double)` — `(4,6)/2` → `(2,3)`; divide by zero throws or produces inf (document behavior)

---

## Constants

- [ ] `DP_NINE` — verify `round(x, DP_NINE)` produces 9 decimal places of precision (currently never exercised)
