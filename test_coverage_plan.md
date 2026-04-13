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

## ✅ Completed — previously 0% classes, now covered

| Class | New file | Coverage |
|---|---|---|
| `Plane` | `geompp_tests/src/test_plane.cpp` | ~94% |
| `BBox2D` | `geompp_tests/src/test_bbox2d.cpp` | ~82% |
| `BBox3D` | `geompp_tests/src/test_bbox3d.cpp` | **100%** |
| `Polygon2D` | `geompp_tests/src/test_polygon2d.cpp` | ~91% |
| `Polygon3D` | `geompp_tests/src/test_polygon3d.cpp` | ~91% |

## ✅ Completed — previously partial, now extended

| Class | What was added | Old → New coverage |
|---|---|---|
| `Triangle3D` | `Vertices`, `AlmostEquals`, `Perimeter`, `ToAxis`, `Interpolate`; documented throwing methods (`SignedArea`, `Area`, `DistanceTo`, `Location`, `Contains`, `Intersects/Intersection×Line3D`) | ~35% → ~90% |
| `Polyline2D` | `AlmostEquals`, `ToSegments` | ~90% → ~92% |
| `LineSegment3D` | `DistanceTo`, `Intersects/Intersection×Ray3D`, `Intersects/Intersection×LineSegment3D` | ~81% → ~96% |

---

## ✅ Completed — `Polyline3D` and `utils.hpp`

| Class | New file | Coverage |
|---|---|---|
| `Polyline3D` | `geompp_tests/src/test_polyline3d.cpp` | ~92% |
| `utils.hpp` | `geompp_tests/src/test_utils.cpp` | **100%** |

---

## Priority 1 — `Polyline3D` remaining gaps

- [ ] `Polyline3D::operator=` — assign one polyline to another; self-assign is safe
- [ ] `Polyline3D::operator<<` — stream output matches `ToWkt()`

---

## Priority 3 — `Line2D` (~61%)

**File:** `geompp_tests/src/test_line2d.cpp`

- [ ] `AlmostEquals` — same line; lines with different origin on same infinite line (expect ==); different direction (expect ≠)
- [ ] `ProjectOnto` — point above line projects to foot; point already on line projects to itself
- [ ] `Location` — at origin (0.0); at a known point ahead (+); behind (−)
- [ ] `Intersects(Ray2D)` / `Intersection(Ray2D)` — ray crosses line; ray parallel; behind ray origin
- [ ] `Intersects(LineSegment2D)` / `Intersection(LineSegment2D)` — crosses; parallel; too short to reach

---

## Priority 4 — `Ray3D` (~67%)

**File:** `geompp_tests/src/test_ray3d.cpp`

- [ ] `Contains` — point on ray (true); at origin (true); behind origin (false); off-axis (false)
- [ ] `DistanceTo` — point on ray (0); ahead but off-axis; behind origin
- [ ] `Intersects(Ray3D)` / `Intersection(Ray3D)` — two rays crossing; parallel; skew
- [ ] `Intersects(LineSegment3D)` / `Intersection(LineSegment3D)` — crosses; behind ray origin

---

## Priority 5 — `Vector2D` (~70%)

**File:** `geompp_tests/src/test_vector2d.cpp`

- [ ] `Length` — `(1,0)` → 1.0; `(0,0)` → 0.0; `(3,4)` → 5.0
- [ ] `Normalize` — result has length 1.0; direction preserved; zero vector throws or returns zero (document behavior)
- [ ] `AlmostEquals` — same vector; near-equal within DP_THREE; clearly different
- [ ] `operator-()` unary — `-(1,2)` → `(-1,-2)`
- [ ] `operator-(Vector2D, Vector2D)` — `(3,4)-(1,1)` → `(2,3)`
- [ ] `operator/(Vector2D, double)` — `(4,6)/2` → `(2,3)`; divide by zero (document behavior)

---

## Priority 6 — `Line3D` (~75%)

**File:** `geompp_tests/src/test_line3d.cpp`

- [ ] `DistanceTo(Point3D)` — point on line (0); point perpendicular off line; skew point
- [ ] `Intersects(Ray3D)` / `Intersection(Ray3D)` — ray crosses line; parallel; behind ray origin
- [ ] `Intersects(LineSegment3D)` / `Intersection(LineSegment3D)` — crosses; parallel; too short

---

## Priority 7 — `BBox2D` (~82%)

**File:** `geompp_tests/src/test_bbox2d.cpp`

- [ ] `BBox2D(Polyline2D)` — min/max span all knots
- [ ] `BBox2D(Polygon2D)` — min/max span all vertices

---

## Priority 8 — `LineSegment2D` (~87%)

**File:** `geompp_tests/src/test_line_segment2d.cpp`

- [ ] `Length` — axis-aligned (3.0); 3-4-5 hypotenuse (5.0); symmetry check
- [ ] `ToLine` — result passes through both endpoints; direction is normalized

---

## Priority 9 — `Ray2D` (~89%)

**File:** `geompp_tests/src/test_ray2d.cpp`

- [ ] `AlmostEquals` — same ray; same origin different direction (≠); same direction different origin (≠)
- [ ] `ToLine` — result has same origin and direction as the ray

---

## Priority 10 — `Triangle3D` (~90%), `Polyline2D` (~92%)

These classes have only `operator=` and/or `operator<<` untested — low-value gaps.

- [ ] `Triangle3D::operator=` — assign one triangle to another; self-assign is safe
- [ ] `Triangle3D::operator<<` — stream output matches `ToWkt()`
- [ ] `Polyline2D::operator=` — assign one polyline to another; self-assign is safe
- [ ] `Polyline2D::operator<<` — stream output matches `ToWkt()`

---

## Priority 11 — Minor gaps (single untested item each)

| Class | Missing |
|---|---|
| `Triangle2D` | `AlmostEquals` |
| `Plane` | `operator==` (throws, because `AlmostEquals` throws — document this) |
| `Point2D` | `linear_combination` |
| `LineSegment3D` | `operator<<` |
| `Polygon2D` | `operator<<` |
| `Polygon3D` | `operator<<` |
