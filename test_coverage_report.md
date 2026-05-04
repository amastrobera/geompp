# Test Coverage Report

_Last updated: 2026-05-04_

## Overall

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (total) | ~360 | Excluding copy/move ctors, dtors, `operator<<`, `operator=` |
| Stubs (`throw "not implemented"`) | 21 | Listed per class below |
| C++ explicit tests | ~300 | At least one `TEST_F` exercises the method |
| Python explicit tests | ~160 | At least one `test_*` function calls the method |

---

## By Class

Key: **★** = stub (not yet implemented) · **○** = implemented, no explicit test (C++ or Python)

| Class | C++ tested | Py tested | Stubs ★ | Notable gaps ○ |
|-------|-----------|----------|---------|----------------|
| `Point2D` | ✓ all key | ✓ most | — | `linear_combination` (Py) |
| `Point3D` | ✓ all key | ✓ most | — | — |
| `Vector2D` | ✓ most | ✓ most | — | Many arithmetic operators (both) |
| `Vector3D` | ✓ most | ✓ most | — | Many arithmetic operators (both); `IsParallel` (C++) |
| `Line2D` | ✓ most | ✓ partial | — | `Intersects`/`Intersection` ×`Ray2D`, ×`Segment2D` (Py) |
| `Line3D` | ✓ most | ○ thin | — | `Contains`, `Intersects`/`Intersection` ×`Ray3D`, ×`Segment3D` (both) |
| `Ray2D` | ✓ most | ✓ partial | — | `ProjectOnto`, `ToLine` (Py) |
| `Ray3D` | ✓ most | ○ thin | — | `Contains`, `ProjectOnto`, `Intersects`/`Intersection` ×`Segment3D` (Py) |
| `LineSegment2D` | ✓ most | ✓ most | — | `ProjectOnto`, `ToLine` (Py) |
| `LineSegment3D` | ✓ all key | ○ thin | — | `First`, `Last`, `AlmostEquals`, `ProjectOnto`, `Location`, `Interpolate`, `Contains`, all `Intersects`/`Intersection` (Py) |
| `Polyline2D` | ✓ all key | ✓ partial | — | `ProjectOnto` (both); `DistanceTo`, `Location` (Py) |
| `Polyline3D` | ✓ all key | ○ thin | — | `ProjectOnto` (both); `DistanceTo`, `Location`, `Interpolate`, most `Intersects`/`Intersection` (Py) |
| `Polygon2D` | ✓ core | ✓ core | `Contains` ★ `DistanceTo` ★ `Intersection(×Line/Ray/Seg)` ★ | `ToWkt`/`FromWkt`, `ToFile`/`FromFile`, `AlmostEquals` (Py) |
| `Polygon3D` | ✓ core | ✓ core | `Contains` ★ `DistanceTo` ★ `Intersection(×Line/Ray/Seg)` ★ | Same as Polygon2D (Py) |
| `Triangle2D` | ✓ most | ✓ partial | `DistanceTo` ★ `Intersects(△)` ★ `Intersection(△)` ★ | `AlmostEquals`, `ToPolygon`, `ToAxis`, all `Intersection` (Py) |
| `Triangle3D` | ✓ partial | ✓ partial | `DistanceTo` ★ `Contains` ★ `Intersects(△)` ★ `Intersection(×Line/Ray/Seg/△)` ★ | `Perimeter`, `ToAxis` (Py); most `Intersects`/`Intersection` (Py) |
| `Plane` | ✓ all key | ✓ all key | — | `axis_u`, `axis_v`, `ProjectInto`, `Evaluate`, `FromOriginAndAxes` (Py) |
| `BBox2D` | ✓ most | ✓ partial | — | `BBox2D(Polyline2D)`, `BBox2D(Polygon2D)` (C++); most ctors (Py) |
| `BBox3D` | ✓ all key | ○ thin | — | `min`, `max`, `Contains`, all shape ctors (Py) |

---

## Stub Methods (21 total)

These methods are declared in the public API but `throw std::runtime_error("not implemented")`.
Each has a `EXPECT_ANY_THROW` test confirming the throw.

| Class | Method |
|-------|--------|
| `Polygon2D` | `DistanceTo(Point2D)` |
| `Polygon2D` | `Contains(Point2D)` |
| `Polygon2D` | `Intersection(Line2D)` |
| `Polygon2D` | `Intersection(Ray2D)` |
| `Polygon2D` | `Intersection(LineSegment2D)` |
| `Polygon3D` | `DistanceTo(Point3D)` |
| `Polygon3D` | `Contains(Point3D)` |
| `Polygon3D` | `Intersection(Line3D)` |
| `Polygon3D` | `Intersection(Ray3D)` |
| `Polygon3D` | `Intersection(LineSegment3D)` |
| `Triangle2D` | `DistanceTo(Point2D)` |
| `Triangle2D` | `Intersects(Triangle2D)` |
| `Triangle2D` | `Intersection(Triangle2D)` |
| `Triangle3D` | `DistanceTo(Point3D)` |
| `Triangle3D` | `Contains(Point3D)` |
| `Triangle3D` | `Intersects(Triangle3D)` |
| `Triangle3D` | `Intersection(Line3D)` |
| `Triangle3D` | `Intersection(Ray3D)` |
| `Triangle3D` | `Intersection(LineSegment3D)` |
| `Triangle3D` | `Intersection(Triangle3D)` |
| `Polygon2D::FromWkt` | (parses but returns broken result — fix tracked separately) |

---

## Key Takeaways

- **No class is at 0%** — all geometry classes have at least some test coverage.
- **Python binding tests are the biggest gap**: `LineSegment3D`, `Polyline3D`, `BBox3D`, `Line3D`, `Ray3D` are thin or untested in Python.
- **Stub cluster**: `Polygon2D/3D` and `Triangle2D/3D` all have unimplemented intersection and containment methods. These are the primary next implementation targets.
- **`Plane`, `Point2D`, `Point3D`, `Vector2D`, `Vector3D`** have excellent coverage in both C++ and Python.
- **Operator overloads** (`operator<<`, `operator=`, arithmetic) are implicitly exercised by other tests even when not explicitly targeted.

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
