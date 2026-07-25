# Test Coverage Report

_Last updated: 2026-07-25_

> **Scope note:** `Mesh2D`, `Mesh3D`, `PolyMesh2D`, `PolyMesh3D`, `GridCell2D`, `GridCell3D` (added in the `meshes` commit) have now been through a full validate-and-expand pass (see below) — tested in C++/Python/C# and bound in both language bindings. The `detail::` internals backing them (`GridCell2DHash`/`GridCell3DHash`, `GridCellMapForMesh2D`/`GridCellMapForMesh3D`) remain C++-only by design, per the same convention as every other `detail::` namespace in this codebase.

## Overall

| Metric | Count | Notes |
|--------|-------|-------|
| C++ public methods (named classes, excl. `calc_utils2d/3d` internals) | ~491 | Excl. copy/move ctors, dtors, `operator<<`, `operator=` |
| C++ methods with ≥1 explicit test | ~469/491 (~95%) | 909 `TEST_F`/`TEST` cases defined, 907 run (2 disabled) — **all pass** |
| Python bound methods (incl. 28 free functions) | 443 | 423 tested (~95%) — all 28 free functions covered |
| Python test functions | 742 | **all pass** (`pytest geompp_python/tests/ -q`) |
| C# bound methods (incl. 20 `GeomUtil` free-function wrappers) | 549 | 482 tested (~88%) — all 20 free functions covered |
| C# tests (custom console harness in `Program.cs`) | 846 | **all pass, 0 failed** — run via `dotnet run` (see note below), *not* `dotnet test`: the project is `OutputType=Exe`, not a Test SDK project |
| Stubs (`throw`/`NotImplementedError`) | **0** | Down from 10 in the last audit — every previously-stubbed method is now fully implemented (see below) |

This is a full re-audit (every class re-checked method-by-method against its test file), not an incremental diff — several counts and gap lists below correct mistakes in the previous version of this report (noted inline).

**2026-07-25 gap-filling pass:** closed the three biggest concrete gaps flagged in this report's first version — C++ `Vector2D` (`ToPoint`/`Length`/`AlmostEquals`/`Normalize`/`BasisY`, +5 tests), C++ `Polygon2D/3D::ConvexHull()` member (+4 tests each), Python `Vector3D` operators (`__add__`/`__sub__`/`__mul__`/`__rmul__`/`__truediv__`/`__neg__`/`__eq__`, +3 tests), and the C# `ToString()`/`operator==` systemic gap across all 26 bound classes (+26 `ToString` tests, +40 `==` tests). While rebuilding to verify, also found and fixed four **build-breaking bugs** unrelated to this task, all pre-existing in the uncommitted WIP mesh work: `mesh2d.hpp`/`mesh3d.hpp`/`polymesh2d.hpp`/`polymesh3d.hpp` each had a missing closing brace on `Faces()` that left `namespace geompp` unclosed (silently masked by a stale cached build — see `feedback_msbuild_stale_rebuild` in project memory), and `Triangle2D`/`Triangle3D`/`Polygon2D`/`Polygon3D` had a duplicate-declaration edit that conflicted with each class's existing custom copy-assignment operator; the actually-needed piece (an explicit `operator=(T&&) = default` move-assignment, required for `std::movable` so the mesh `Faces()` range pipeline compiles) was extracted and kept, the accidental duplicates removed.

**2026-07-25 mesh/gridcell validate-and-expand pass:** ran the full validate → document → test → bind → changelog checklist against the new `Mesh2D/3D`, `PolyMesh2D/3D`, `GridCell2D/3D` classes. Found and fixed a **critical off-by-one bug**: `add_point`/`get_gridcell_hash_index` (vertex-welding helpers in `grid_cell2d.cpp`/`grid_cell3d.cpp`/`polymesh2d.cpp`/`polymesh3d.cpp`) recorded each new vertex's index as the container's post-`push_back` size (a 1-based count) instead of its 0-based position, so every face's vertex indices were shifted by one and the last unique vertex in any mesh was one-past-the-end (out of bounds) — this affected **all four** mesh factories (`Mesh2D/3D::FromTriangles`, `PolyMesh2D/3D::FromPolygons`) and would have corrupted or crashed on every mesh with ≥2 faces sharing a vertex. Also fixed: `GridCell2DHash`/`GridCell3DHash` referenced unqualified (as bare `GridCell2DHash`) in `polymesh2d.cpp`/`polymesh3d.cpp` after the type moved into `namespace detail` (compile error); `add_triangle` declared a `std::tuple<size_t,size_t,size_t>` return type but never returned a value (dead, UB-on-paper code — the return was unused, so simplified to `void`); `PolyMesh2D/3D::FromPolygons` didn't reject polygons with holes despite the class's own "each facet has no holes" invariant (added validation). All four fixes are covered by dedicated regression tests (`*_SharedEdge_Welds*`, `*_WithHoles_Throws`) in all three languages. New tests: +44 C++ (`test_grid_cell2d/3d.cpp`, `test_mesh2d/3d.cpp`, `test_polymesh2d/3d.cpp`), +24 Python, +27 C#. New bindings: `Mesh2D/3D`, `PolyMesh2D/3D`, `GridCell2D/3D` in both `geompp_python` and `geompp_csharp` (their `detail::` internals — `GridCell2DHash`/`GridCell3DHash`, `GridCellMapForMesh2D/3D` — are not bound, per the standing convention that `detail::` stays C++-only).

---

## By Class

Key: **★** = stub (none remain) · **○** = implemented, thin/no explicit test

| Class | C++ tested | Py tested | CS tested | Notable gaps |
|-------|-----------|----------|----------|----------------|
| `Point2D` | ✓ most (19/24) | ✓ most | ✓ most | `are_collinear`, `remove_consecutive_duplicates`, `linear_combination`, `are_ccw`, `are_cw` (C++ free fns); `__rmul__` (Py); `+(Vector2D)` (C#) — `==`/`ToString` now closed |
| `Point3D` | ✓ most | ✓ most | ✓ most | `remove_consecutive_duplicates` (C++); `__rmul__` (Py); `+` (C#) — `==`/`ToString` now closed |
| `Vector2D` | ✓ all key (16/16) | ✓ most | ✓ most | C++: closed — `ToPoint`/`Length`/`AlmostEquals`/`Normalize`/`BasisY` now tested; `__rmul__`, `__eq__` (Py); C#: `ToFile`/`FromFile` still untested, `ToString` now closed (`==` was already covered before this pass) |
| `Vector3D` | ✓ most (18/19) | ✓ all key (21/21) | ✓ most | `IsParallel` (C++); Py: closed — `__add__/__sub__/__mul__/__rmul__/__truediv__/__neg__/__eq__` now tested via `test_arithmetic`/`test_negation`/`test_eq`; C#: `ToWkt`/`ToFile`/`FromFile`/`Perp`/`IsParallel` still untested, but `==`/`ToString` now covered |
| `Line2D` | ✓ all key | ✓ most | ○ partial (16/21) | Py: `first`, `last`; C#: `First/Last/Origin/Direction/ToWkt`, `Overlaps/Overlap/Touches/Touch(Polyline2D)` — `==`/`ToString` now closed |
| `Line3D` | ✓ all key | ○ partial (12/16) | ○ partial (15/22) | Py: `direction/first/last/origin`; C#: same accessors + `Touches/Touch(Segment3D)`, polyline overloads — `==`/`ToString` now closed |
| `Ray2D` | ✓ most | ✓ all key | ○ partial (17/21) | C++: `ToLine`; C#: `Origin/Direction`, polyline overloads — `==`/`ToString` now closed |
| `Ray3D` | ✓ all key | ✓ all key | ○ partial (15/22) | C#: `Overlaps/Overlap` vs Line3D/Segment3D, polyline overloads — `==`/`ToString` now closed |
| `LineSegment2D` | ✓ most | ✓ all key | ✓ most (20/23) | C++: `ToLine` (`has_intersections`/`find_intersections` tested in `test_calc_utils2d.cpp` instead); C#: `Overlaps/Overlap(Ray2D)`, polyline overloads — `==`/`ToString` now closed |
| `LineSegment3D` | ✓ all key | ✓ all key | ✓ most (20/24) | C#: `Overlaps/Overlap(Ray3D)`, polyline overloads — `==`/`ToString` now closed |
| `Polyline2D` | ✓ most | ✓ all key | ✓ most (24/25) | C++: `IsSimple` (Polyline3D's is tested); C#: indexer — `==`/`ToString` now closed, best-covered of the C# polyline family |
| `Polyline3D` | ✓ all key | ✓ all key | ✓ most (26/29) | C#: `Overlaps/Overlap(Segment3D)`, `Touches/Touch(Ray3D)` — `==`/`ToString` now closed |
| `Polygon2D` | ✓ all key (27/27) | ✓ all key | ✓ most | C++: `ConvexHull()` member gap closed (+4 tests: already-convex, concave-drops-dent, hole-ignored, result-IsConvex); Py/C#: `distance_to`/`DistanceTo` implemented but still untested (see Stub correction below); C#: `==`/`ToString` now closed |
| `Polygon3D` | ✓ all key (28/28) | ✓ all key | ✓ most | C++: `ConvexHull()` gap closed (+4 tests, incl. a tilted-plane case); Py/C#: `distance_to`/`DistanceTo` still untested; C#: `Xor` also untested (Union/Difference are), `==`/`ToString` now closed |
| `Triangle2D` | ✓ all key | ✓ most | ○ partial (18/22) | Py: `distance_to`, `to_polygon`; **C#: `DistanceTo` untested, and the C# binding has no `Intersects(Triangle2D)`/`Intersection(Triangle2D)` overload at all** (only vs. `Line2D`, which is itself untested in C#) — C++ has and tests `Intersection(Triangle2D)` (`test_triangle2d.cpp:397`, `IntersectionWTriangle`); `==`/`ToString` now closed |
| `Triangle3D` | ✓ all key | ✓ most | ✓ most (24/25) | Py: `distance_to`; C#: `DistanceTo` untested — but `Intersects/Intersection(Triangle3D)` **are** well tested in C#; `==`/`ToString` now closed |
| `Plane` | ✓ most | ✓ all key | ✓ most | C++: `are_coplanar` has 0 calls in `test_plane.cpp` (only exercised in `test_utils.cpp`); Py: `__eq__`; C#: `Origin()` still untested, `==`/`ToString` now closed |
| `View2D` | ○ partial (7/9) | — | — | `xy()`, `xyz()` (inverse-projection round-trip) never tested — internal/helper type, not bound to Python or C# |
| `BBox2D` | ○ partial | ✓ all key | ✓ all key (11/11) | C++: ctor(Polyline2D), ctor(Polygon2D), ctor(point-range template) untested; C#: `==`/`ToString` now closed, fully tested |
| `BBox3D` | ○ partial | ✓ most | ✓ all key (11/11) | C++: most ctor overloads not individually re-verified; Py: `__eq__`; C#: `==`/`ToString` now closed, fully tested |
| `BBall2D` | ✓ all key | ✓ all key | ✓ all key (8/8) | `ToString` gap closed |
| `BBall3D` | ✓ all key | ✓ all key | ✓ all key (8/8) | `ToString` gap closed |
| `BRect2D` | ✓ all key | ✓ most | ✓ most (12/14) | Py/C#: `half_len_u`/`HalfLenU`, `half_len_v`/`HalfLenV` still untested; C#: `ToString` gap closed |
| `BPrism3D` | ✓ all key | ✓ all key | ○ partial (11/17) | C#: `HalfLenU`, `HalfLenV`, `Width`, `Height`, `Depth` still untested; `ToString` gap closed |
| `WktParser` | ○ thin | ✓ all key | ✓ all key (6/6) | C++: only **1** `TEST_F` in the whole file — `GetFilePath` untested; C#: `ToString` closed, fully tested — note `WktParser` has no `operator==` binding at all (not a gap, just not applicable) |
| `GeometryCollection2D` | ✓ most | ✓ all key | ✓ most | C++: `FromFile`; C#: `Add(nested GeometryCollection2D)` still untested, `==`/`ToString` closed |
| `GeometryCollection3D` | ✓ most | ✓ most | ✓ most | C++: `FromFile`; Py: `__len__`; C#: `Add(nested GeometryCollection3D)` still untested, `==`/`ToString` closed |
| `Mesh2D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Triangle-faced mesh with vertex welding. All of `FromTriangles`/`Size`/`Area`/`operator[]`/`Faces` tested in all three languages, incl. a shared-edge welding regression test (the fixed off-by-one bug) |
| `Mesh3D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Same as `Mesh2D` |
| `PolyMesh2D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Arbitrary-polygon-faced mesh with vertex welding. All of `FromPolygons`/`Size`/`Area`/`operator[]`/`Faces` tested, incl. shared-edge welding, mixed face sizes, and the new hole-rejection validation |
| `PolyMesh3D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Same as `PolyMesh2D` |
| `GridCell2D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Spatial-hash bucket backing mesh vertex welding. `FromPoint`/`operator==` tested incl. a boundary-straddle case documenting the known floor-bucketing vs. `AlmostEquals` limitation |
| `GridCell3D` (WIP) | ✓ all key | ✓ all key | ✓ all key | Same as `GridCell2D` |
| `calc_utils2d` (public: `has_intersections`, `find_intersections`, `dist/rdp/vw_decimation`, `boolean_op[_multi]`, `clip`, `bezier_smoothing_2`, `polyline_expansion`, `tangents_to`, `find_extreme_points`, `distance_to`, `Event2D`/`EventQueue2D`/`SweepLine2D`) | ✓ core | ✓ core (all free fns tested) | ✓ core (all 20 `GeomUtil` free fns tested) | C++: low-level `detail::` internals (`SweepLineComparator`, `line_intersection`, `run_shamos_hoey`, `run_bentley_ottmann`, `winding_number`, `split_segments_at_crossings`, `detail::view::*`) have no *direct* by-name test — only exercised transitively via `Polygon2D`/`BRect2D`/etc. |
| `calc_utils3d` (`convex_hull_indices`, `find_extreme_points`, `distance_to`) | ✓ core | ✓ core | ✓ core | C++: `detail::convex_hull_indices`, `detail::is_convex` — no direct test, only via `Polygon3D::ConvexHull()`/`IsConvex()` |

---

## Stub Correction — 0 stubs remain

The previous report listed 7 stub methods. **All are now fully implemented in C++** (verified by reading `geompp/src/*.cpp` directly — no `throw std::runtime_error("not implemented")` remains anywhere in the named-class source, confirmed independently by all three coverage agents):

| Class | Method | C++ status | Py status | C# status |
|-------|--------|-----------|-----------|-----------|
| `Polygon2D` | `DistanceTo(Point2D)` | ✓ implemented, tested (`test_polygon2d.cpp:383`, `:393`) | implemented, **untested** | implemented, **untested** |
| `Polygon3D` | `DistanceTo(Point3D)` | ✓ implemented, tested (`test_polygon3d.cpp:528`, `:546`) | implemented, **untested** | implemented, **untested** |
| `Triangle2D` | `DistanceTo(Point2D)` | ✓ implemented, tested (`test_triangle2d.cpp:161`) | implemented, **untested** | implemented, **untested** |
| `Triangle2D` | `Intersects`/`Intersection(Triangle2D)` | ✓ implemented, tested (`test_triangle2d.cpp:397`) | tested | **not bound at all** (only vs. `Line2D`) |
| `Triangle3D` | `DistanceTo(Point3D)` | ✓ implemented, tested (`test_triangle3d.cpp:584`) | implemented, **untested** | implemented, **untested** |
| `Polygon2D::FromWkt` | (previously "parses but returns broken result") | ✓ fixed — `test_polygon2d.cpp:82` `"FromWkt round-trips correctly"` passes | — | — |

**Action item, not a correctness bug:** the `DistanceTo(Point)` family (Polygon2D/3D, Triangle2D/3D) is implemented and C++-tested but has zero Python or C# tests — the single biggest concrete, actionable gap in this audit.

---

## Key Takeaways

- **Zero stubs remain.** The prior stub cluster (`Polygon2D/3D::DistanceTo`, `Triangle2D::DistanceTo`/`Intersects(△)`/`Intersection(△)`, `Triangle3D::DistanceTo`) is fully implemented; `Polygon2D::FromWkt`'s broken-result bug is fixed. The remaining action is writing Python/C# tests for the four `DistanceTo(Point)` methods — not yet done, still the single biggest concrete gap left.
- **`Vector2D`'s C++ gap is closed**: `Length`, `Normalize`, `AlmostEquals`, `ToPoint`, `BasisY` are now directly tested in `test_vector2d.cpp`.
- **`Polygon2D/3D::ConvexHull()` (the member method) is now directly tested in C++** — 4 tests each (already-convex, concave-drops-the-dent, hole-ignored, result-`IsConvex()`), closing a gap the previous report had mismarked as already covered.
- **C#'s `ToString()`/`operator==` systemic gap is closed**: all 26 bound classes now have a `ToString()` test, and all 20 classes missing `operator==` (everything except the 5 that already had it: `Vector2D`, `BBall2D/3D`, `BRect2D`, `BPrism3D`) now have one — `WktParser` correctly has neither `==` test added (it has no `operator==` binding, not a gap). C# overall coverage moved from ~78% to ~87%. The polyline-overload family (`Overlaps`/`Touches` against `Polyline2D/3D`) remains the biggest *remaining* C# gap, thin across `Line`/`Ray`/`LineSegment`.
- **`Vector3D` operators are now fully tested in Python** (`__add__`/`__sub__`/`__mul__`/`__rmul__`/`__truediv__`/`__neg__`/`__eq__` via `test_arithmetic`/`test_negation`/`test_eq`, mirroring `Vector2D`'s existing `test_arithmetic`). C# still lacks `ToWkt`/`ToFile`/`FromFile`/`Perp`/`IsParallel` for `Vector3D` (unrelated to the operator gap, out of scope for this pass).
- **Free functions are the best-covered surface across all three languages**: all 28 Python free functions and all 20 C# `GeomUtil` wrappers have ≥1 test; the C++ public wrappers (`has_intersections`, `find_intersections`, decimation, boolean ops, `clip`, Bezier smoothing, polyline expansion, extreme points, `distance_to`, `tangents_to`) are likewise fully covered — only the internal `detail::` sweep-line/hull primitives lack *direct* by-name tests (they're exercised transitively).
- **`WktParser` is thin in C++** — only 1 `TEST_F` in the entire file, with `GetFilePath` untested; Python and C# cover it more thoroughly (C# now fully, 6/6).
- **No class is at 0%.**
- **`Mesh2D/3D`, `PolyMesh2D/3D`, `GridCell2D/3D` are now fully validated, tested, and bound** in all three languages, closing what was previously the only untested/unbound surface in the library. This pass found a **critical off-by-one bug** in the shared vertex-welding logic (every mesh with ≥2 faces sharing a vertex would have wrong-by-one indices and an out-of-bounds read on the last unique vertex) plus three smaller bugs (a namespace-qualification compile error, a dead non-returning function, and a missing hole-rejection check) — see the dated note above for details. All four are covered by dedicated regression tests in C++, Python, and C#.
- **Verifying these passes surfaced build-breaking bugs unrelated to the immediate task** — 4 in the mesh WIP code during the first gap-filling pass (missing braces, duplicate declarations), then the off-by-one and 3 more during the mesh validate-and-expand pass itself — all fixed as a side effect of forcing a real rebuild rather than trusting a stale cache, consistent with the standing "force a rebuild before trusting results" lesson.

---

## How to Reproduce These Numbers

```bash
# C++ (Windows, from repo root) — re-run cmake configure first if test/binding
# source files were added since the last configure (new .cpp files aren't picked
# up by an incremental build alone)
cmake -S . -B build_win
cmake --build build_win --target geompp_tests --config Debug
./build_win/geompp_tests/Debug/geompp_tests.exe   # 907 passed, 2 disabled

# Python (editable install must be rebuilt after any C++ change, AND the built
# .pyd must be copied to the site-packages location Python actually imports
# from — see the geompp_python stale-.pyd note in project memory)
cmake --build build_win --target _geompp --config Debug
python -m pytest geompp_python/tests/ -q           # 742 passed

# C# — this is an Exe-type console harness, NOT a Test SDK project.
# `dotnet test` reports success but runs nothing; use `dotnet run`.
msbuild geompp_csharp/GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD/build_win" /p:Configuration=Debug
cd geompp_csharp/tests && dotnet run -c Debug -p:GeomPPConfiguration=Debug_Net48   # 846 passed, 0 failed
```

### Instrumented coverage (gcov/lcov, C++ only)

```bash
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

_See `test_coverage_plan.md` for the prioritized list of missing test cases (if present — not maintained as of this update; the gap lists above are the current source of truth)._
