# Changelog

All notable changes to geompp are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Each release covers all three packages at the same version:
- **C++ library** — tagged `v{version}`
- **C# / NuGet** — tagged `csharp-v{version}`
- **Python / PyPI** — tagged `python-v{version}`

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
