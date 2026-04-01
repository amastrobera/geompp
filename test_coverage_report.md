# Test Coverage Report

## Overall

| Status | Count | % |
|---|---|---|
| Tested | ~220 | 63% |
| Untested | ~110 | 31% |
| Commented-only | ~20 | 6% |
| **Total** | **~350** | |

---

## By Class

| Class | Tested | Untested | Commented-only | Coverage |
|---|---|---|---|---|
| `Point2D` | 19 | 1 (`linear_combination`) | 0 | ~95% |
| `Point3D` | 20 | 0 | 0 | 100% |
| `Vector2D` | 14 | 6 (`Length`, `Normalize`, `AlmostEquals`, unary `-`, `operator-`, `operator/`) | 0 | ~70% |
| `Vector3D` | 22 | 0 | 0 | 100% |
| `Line2D` | 11 | 7 (`AlmostEquals`, `ProjectOnto`, `Location`, `Intersects/Intersection` ×Ray2D + ×LineSegment2D) | 0 | ~60% |
| `Line3D` | 14 | 5 (`DistanceTo`, `Intersects/Intersection` ×Ray3D + ×LineSegment3D) | 0 | ~75% |
| `LineSegment2D` | 14 | 2 (`Length`, `ToLine`) | 0 | ~87% |
| `LineSegment3D` | 13 | 3 (`DistanceTo`, `Intersects/Intersection` ×Ray3D + ×LineSegment3D) | 0 | ~81% |
| `Ray2D` | 16 | 2 (`AlmostEquals`, `ToLine`) | 0 | ~89% |
| `Ray3D` | 12 | 6 (`Contains`, `DistanceTo`, `Intersects/Intersection` ×Ray3D + ×LineSegment3D) | 0 | ~67% |
| `Triangle2D` | 16 | 1 (`AlmostEquals`) | 6 (Ray/Segment/Triangle intersections) | ~94% active |
| `Triangle3D` | 5 | 9 (`Area`, `Perimeter`, `Contains`, `DistanceTo`, `ToAxis`, `Location`, `Interpolate`, `AlmostEquals`, `Intersects/Intersection ×Line3D`) | 9 (Ray/Segment/Triangle intersections) | ~35% active |
| `Polyline2D` | 19 | 1 (`AlmostEquals`, `ToSegments`) | 0 | ~90% |
| `Polyline3D` | 0 | 20 | 0 | **0%** |
| `Polygon2D` | 4 | 4 (`AlmostEquals`, `FromWkt`, `ToFile`, `FromFile`) | 0 | ~50% |
| `Polygon3D` | 0 | 10 | 0 | **0%** |
| `BBox2D` | 0 | 8 | 0 | **0%** |
| `BBox3D` | 0 | 8 | 0 | **0%** |
| `Plane` | 0 | 18 | 0 | **0%** |
| `LVSParser` | 4 | 0 | 0 | 100% |
| `utils.hpp` | 1 (`round`) | 8 | 0 | ~11% |

---

## Key Takeaways

- **5 classes at 0%**: `Polyline3D`, `Polygon3D`, `BBox2D`, `BBox3D`, `Plane` — these alone account for most of the gap.
- **`Triangle3D`** is the worst among partially-tested classes — only the constructor, WKT, and file I/O work; the geometry methods are all dark.
- **`utils.hpp`** is nearly untested despite being called internally by every `FromWkt` parser.
- **2D side** is in good shape overall — most classes are ≥ 87% with only isolated gaps.
- **3D side** is systematically weaker, mirroring the fact that the `feature/geom_3d` branch is still in progress.

---

## How to Measure Coverage Instrumentally

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

*See `test_coverage_plan.md` for the full prioritized list of missing test cases.*
