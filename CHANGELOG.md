# Changelog

All notable changes to geompp are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Each release covers all three packages at the same version:
- **C++ library** — tagged `v{version}`
- **C# / NuGet** — tagged `csharp-v{version}`
- **Python / PyPI** — tagged `python-v{version}`

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
