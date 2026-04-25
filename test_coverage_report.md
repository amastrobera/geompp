# Test Coverage Report

## Overall

| Status | Count | % |
|---|---|---|
| Tested | ~323 | ~90% |
| Untested | ~37 | ~10% |
| **Total** | **~360** | |

---

## By Class (sorted lowest → highest coverage)

| Class | Tested | Untested | Coverage |
|---|---|---|---|
| `Line2D` | 11 | 7 (`AlmostEquals`, `ProjectOnto`, `Location`, `Intersects`/`Intersection` ×`Ray2D`, `Intersects`/`Intersection` ×`LineSegment2D`) | ~61% |
| `Ray3D` | 12 | 6 (`Contains`, `DistanceTo`, `Intersects`/`Intersection` ×`Ray3D`, `Intersects`/`Intersection` ×`LineSegment3D`) | ~67% |
| `Vector2D` | 14 | 6 (`Length`, `Normalize`, `AlmostEquals`, unary `-`, `operator-`, `operator/`) | ~70% |
| `Line3D` | 14 | 5 (`DistanceTo`, `Intersects`/`Intersection` ×`Ray3D`, `Intersects`/`Intersection` ×`LineSegment3D`) | ~75% |
| `BBox2D` | 9 | 2 (`BBox2D(Polyline2D)`, `BBox2D(Polygon2D)`) | ~82% |
| `LineSegment2D` | 14 | 2 (`Length`, `ToLine`) | ~87% |
| `Ray2D` | 16 | 2 (`AlmostEquals`, `ToLine`) | ~89% |
| `Triangle3D` | 19 | 2 (`operator=`, `operator<<`) | ~90% |
| `Polygon2D` | 10 | 1 (`operator<<`) | ~91% |
| `Polygon3D` | 10 | 1 (`operator<<`) | ~91% |
| `Polyline2D` | 23 | 2 (`operator=`, `operator<<`) | ~92% |
| `Polyline3D` | 23 | 2 (`operator=`, `operator<<`) | ~92% |
| `Triangle2D` | 21 | 1 (`AlmostEquals`) | ~94% |
| `Plane` | 16 | 1 (`operator==`) | ~94% |
| `Point2D` | 19 | 1 (`linear_combination`) | ~95% |
| `LineSegment3D` | 22 | 1 (`operator<<`) | ~96% |
| `BBox3D` | 12 | 0 | **100%** |
| `Point3D` | 20 | 0 | **100%** |
| `Vector3D` | 22 | 0 | **100%** |
| `utils.hpp` | 12 | 0 | **100%** |
| `WktParser` | 4 | 0 | **100%** |

---

## Key Takeaways

- **No class is at 0%** — all geometry classes and utilities have at least some test coverage.
- **`Line2D`, `Ray3D`, `Vector2D`, `Line3D`** remain the weakest — all lack several geometric operations (intersections, distance, normalization).
- **`operator<<` and `operator=`** are the most common "last mile" gaps — they appear across `Triangle3D`, `Polygon2D`, `Polygon3D`, `Polyline2D`, `Polyline3D`, and `LineSegment3D`.
- **`BBox3D`, `Point3D`, `Vector3D`, `utils.hpp`, `WktParser`** are at **100%**.
- **2D side** continues to be strong overall; the biggest remaining gaps are in the 2D intersection/distance helpers (`Line2D`, `Vector2D`, `Ray3D`).

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
