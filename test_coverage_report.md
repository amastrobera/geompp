# Test Coverage Report

_Last updated: 2026-05-18_

## Overall

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (total) | ~399 | Excluding copy/move ctors, dtors, `operator<<`, `operator=` |
| Stubs (`throw "not implemented"`) | 13 | Listed per class below |
| C++ explicit tests | ~343 | At least one `TEST_F` exercises the method |
| Python explicit tests | ~195 | At least one `test_*` function calls the method |
| C# explicit tests | ~200 | At least one test in `Program.cs` exercises the method |

---

## By Class

Key: **★** = stub (not yet implemented) · **○** = implemented, no explicit test (C++ or Python)

| Class | C++ tested | Py tested | CS tested | Stubs ★ | Notable gaps ○ |
|-------|-----------|----------|----------|---------|----------------|
| `Point2D` | ✓ all key | ✓ most | ✓ most | — | `linear_combination` (Py); new `Point2D(Vector2D)` ctor covered all three |
| `Point3D` | ✓ all key | ✓ most | ✓ most | — | new `Point3D(Vector3D)` ctor covered all three |
| `Vector2D` | ✓ most | ✓ most | ✓ partial | — | Many arithmetic operators (both) |
| `Vector3D` | ✓ most | ✓ most | ✓ most | — | Many arithmetic operators (both); `IsParallel` (C++) |
| `Line2D` | ✓ most | ✓ partial | ✓ most | — | `Intersects`/`Intersection` ×`Ray2D`, ×`Segment2D` (Py) |
| `Line3D` | ✓ most | ✓ partial | ✓ most | — | `Contains`, `Intersects`/`Intersection` ×`Ray3D`, ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `Ray2D` | ✓ most | ✓ partial | — | — | `ToLine` (Py) |
| `Ray3D` | ✓ most | ✓ partial | ✓ partial | — | `Contains`, `Intersects`/`Intersection` ×`Segment3D` (Py); new `Distance`/`DistanceTo` ×`Line/Ray/Seg` covered in all three |
| `LineSegment2D` | ✓ most | ✓ most | ✓ partial | — | `ToLine` (Py) |
| `LineSegment3D` | ✓ all key | ○ thin | ○ thin | — | `First`, `Last`, `AlmostEquals`, `Location`, `Interpolate`, `Contains`, all `Intersects`/`Intersection` (Py) |
| `Polyline2D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location` (Py) |
| `Polyline3D` | ✓ all key | ✓ partial | ✓ partial | — | `ProjectOnto` (C++); `DistanceTo`, `Location`, `Interpolate`, most `Intersects`/`Intersection` (Py) |
| `Polygon2D` | ✓ core | ✓ core | ✓ partial | `DistanceTo` ★ `Intersection(×Line/Ray/Seg)` ★ | `ToWkt`/`FromWkt`, `ToFile`/`FromFile`, `AlmostEquals` (Py) |
| `Polygon3D` | ✓ core | ✓ core | ✓ partial | `DistanceTo` ★ `Intersection(×Line/Ray/Seg)` ★ | Same as Polygon2D (Py) |
| `Triangle2D` | ✓ most | ✓ partial | ✓ partial | `DistanceTo` ★ `Intersects(△)` ★ `Intersection(△)` ★ | `AlmostEquals`, `ToPolygon`, `ToAxis`, `Location`, all `Intersection` (Py) |
| `Triangle3D` | ✓ most | ✓ most | ✓ most | `DistanceTo` ★ | new `Intersection(×Plane/△)` and the existing `Intersection(×Line/Ray/Seg)` are covered in all three languages |
| `Plane` | ✓ all key | ✓ all key | ✓ most | — | `Intersection(Triangle3D)` now delegates to the symmetric `Triangle3D::Intersection(Plane)` (no stub remaining) |
| `BBox2D` | ✓ most | ✓ partial | — | — | `BBox2D(Polyline2D)`, `BBox2D(Polygon2D)` (C++); most ctors (Py) |
| `BBox3D` | ✓ all key | ○ thin | — | — | `min`, `max`, `Contains`, all shape ctors (Py) |
| `WktParser` | ✓ core | ✓ partial | ✓ most | — | Multi-geometry `FromWkt` round-trip (Py) |
| `GeometryCollection2D` | ✓ core | ✓ partial | ✓ most | — | — |
| `GeometryCollection3D` | ✓ core | ✓ partial | ✓ most | — | — |

---

## Stub Methods (13 total)

These methods are declared in the public API but `throw std::runtime_error("not implemented")`.
Each has a `EXPECT_ANY_THROW` test confirming the throw.

| Class | Method |
|-------|--------|
| `Polygon2D` | `DistanceTo(Point2D)` |
| `Polygon2D` | `Intersection(Line2D)` |
| `Polygon2D` | `Intersection(Ray2D)` |
| `Polygon2D` | `Intersection(LineSegment2D)` |
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
- **Stub cluster**: remaining unimplemented intersection/distance methods live in `Polygon2D/3D` (DistanceTo + Intersection × Line/Ray/Seg), `Triangle2D::Intersects(△)` and `Triangle2D::Intersection(△)`, and `Triangle2D/3D::DistanceTo`.
- **`Plane`, `Point2D`, `Point3D`, `Vector2D`, `Vector3D`** have excellent coverage across C++, Python, and (newly for Plane) C#.
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
