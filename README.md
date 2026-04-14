  # Geom++

[![Build / Test (Windows)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml)
[![Build / Test (Linux)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml)
[![C++ version](https://img.shields.io/github/v/tag/amastrobera/geompp?filter=v*&label=C%2B%2B&color=blue)](https://github.com/amastrobera/geompp/tags)

[![Publish NuGet](https://github.com/amastrobera/geompp/actions/workflows/nuget-publish.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/nuget-publish.yml)
[![NuGet version](https://img.shields.io/nuget/v/GeomPP.svg)](https://www.nuget.org/packages/GeomPP)

[![Publish PyPI](https://github.com/amastrobera/geompp/actions/workflows/pypi-publish.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/pypi-publish.yml)
[![PyPI version](https://img.shields.io/pypi/v/geompp.svg)](https://pypi.org/project/geompp)

  A modern C++20 geometry library for 2D and 3D spatial computation — fast, mathematically correct,
  thoroughly tested, and usable from C++, C# (.Net 10 or .Net Framework 4.8), and Python 3.

  This library is a spiritual successor to [GeomSharp](https://github.com/amastrobera/geom_sharp),
  rewritten to produce better algorithms, faster execution, and no dependency on C#/.NET.

  **Language bindings:**
  [![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)](./geompp_python/README.md)
  [![C# .NET](https://img.shields.io/badge/C%23_.NET-512BD4?logo=dotnet&logoColor=white)](./geompp_csharp/README.md)


  ## What it provides

  ### Primitives

  Both 2D and 3D variants are available for all core types:

  | Primitive      | Description                                              |
  |----------------|----------------------------------------------------------|
  | `Point`        | A coordinate in space                                    |
  | `Vector`       | Direction and magnitude                                  |
  | `Line`         | An infinite line through two points                      |
  | `Ray`          | A semi-infinite line from an origin in one direction     |
  | `LineSegment`  | A finite segment between two endpoints                   |
  | `Polyline`     | A connected chain of segments                            |
  | `Triangle`     | Three non-collinear points forming a closed face         |
  | `Polygon`      | A closed polygon defined by an ordered list of vertices  |
  | `BBox`         | Axis-aligned bounding box                                |
  | `Plane`        | A flat surface in 3D defined by a point and a normal     |

  ### Operations

  Each primitive supports a consistent set of spatial operations where applicable:

  - **Containment** — does a shape contain a given point?
  - **Intersection** — do two shapes intersect, and what is the resulting geometry?
  - **Distance** — closest distance from a point to a shape
  - **Projection** — project a point onto a line, segment, or plane
  - **Interpolation** — retrieve a point at parameter `t` along a segment or polyline
  - **Location** — find the parameter `t` for a point already on a shape
  - **Area / Perimeter / Centroid** — geometric properties for closed shapes
  - **Signed area** — encodes orientation (clockwise vs. counter-clockwise in 2D, surface normal
  direction in 3D)

  Return types are `std::optional<std::variant<...>>` so callers can match on the exact geometry
  produced by an intersection without casting.

  ### Serialization

  All primitives support:
  - **WKT** (Well-Known Text) — `ToWkt()` / `FromWkt()` for standard text interchange
  - **Binary file I/O** — `ToFile()` / `FromFile()` for compact storage

  ### Precision

  Floating-point comparisons use a thread-local `DECIMAL_PRECISION` constant via `AlmostEquals()`
  methods, making the library robust against rounding errors while remaining configurable per thread.


  ### How to use it

  Here is an example of code. You can also look at the [test directory](./geompp_tests/) or [sample code](./geompp_sample/sample.cpp) to see more.

  #### Create geometries programmatically
  ```cpp
  // This will be the precision used by all functions, in all threads, for this
  // run of the program, and it can be modified in later code anytime.
  g::DECIMAL_PRECISION = g::DP_THREE;

  // two line segments intersecting at (0,0,1)
  auto s1 = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(-1, 0, 2));
  auto s2 = g::LineSegment3D::Make(g::Point3D(0, 1, 0), g::Point3D(0, -1, 2));

  GEOMPP_LOG(INFO) << "s1 = " << s1.ToWkt();
  GEOMPP_LOG(INFO) << "s2 = " << s2.ToWkt();

  if (s1.Intersects(s2)) { 
    auto result = s1.Intersection(s2);
    if (result.has_value()) {
      auto p = std::get<g::Point3D>(*result);
      GEOMPP_LOG(INFO) << "intersection found: " << p.ToWkt();

      p.ToFile("intersection.wkt");
      GEOMPP_LOG(INFO) << "intersection written to intersection.wkt";
    }
  } else {
    GEOMPP_LOG(INFO) << "no intersection found";
  }
  ```

  will print out 

  ```bash
  I20260403] s1 = LINESTRING (1 0 0, -1 0 2)
  I20260403] s2 = LINESTRING (0 1 0, 0 -1 2)
  I20260403] intersection found: POINT (0 0 1)
  I20260403] intersection written to intersection.wkt
  ```


  #### Import geometries from a file
  ```cpp
  std::string const lsv_path = "sample_geometries.lsv";
  //   POINT (1 2 3)
  //   POINT (4 5 6)
  //   LINESTRING (0 0 0, 1 1 1)
  //   LINESTRING (2 0 0, 2 3 4)
  //   LINE (0 0 0, 1 0 0)
  //   RAY (0 0 0, 0 1 0)

  g::DECIMAL_PRECISION = g::DP_THREE;

  auto parser = g::LVSParser::Open(lsv_path);
  if (!parser.HasNext()) {
    GEOMPP_LOG(WARNING) << "no geometries found in file " << lsv_path;
    return;
  }

  while (parser.HasNext()) {
    auto entry = parser.Next();

    if (!entry.has_value()) {
      GEOMPP_LOG(WARNING) << "skipped unrecognised line";
      continue;
    }

    GEOMPP_LOG(INFO) << g::LVSParser::ToWkt(entry.value());
  }
  ```

  will print out exactly the list of geometries above.


  ## geom_viewer — interactive geometry visualizer (WIP)

  `geom_viewer` is a companion OpenGL application intended to let you see and interact with geometric
  data produced by the library.

  **Current state:**
  - Opens a window and renders 2D points and line segments loaded from `.lsv` geometry files
  - Coordinates are normalized to the viewport automatically

  **Planned features:**
  - Camera controls (pan / zoom via keyboard)
  - An input box to type in new geometry on the fly (e.g. paste a WKT string)
  - Key binding to delete selected geometry from the scene

  The goal is a lightweight debugging and demonstration tool — not a full-featured CAD viewer, but
  enough to visually inspect what the library computes.

  **Stack:** GLFW 3, GLEW, OpenGL 3.3 Core Profile.


  ## Roadmap

  See [development plan](./development_plan.md) for the full task list. High-level:

  | Status | Area |
  |--------|------|
  | Done | 2D primitives, operations, tests, WKT/file I/O, GitHub Actions CI, Docker (Linux), basic OpenGL viewer, [C# bindings (NuGet)](./geompp_csharp/README.md), [Python bindings (PyPI)](./geompp_python/README.md) |
  | Next | Docker (Windows), geom_viewer camera/input/delete |
  | Backlog | Polygon ops, convex hull, overlap/adjacency, 3D polygon & mesh, polygon clipping |


  I am at improving the test coverage, see how in [test coverage plan](./test_coverage_plan.md).

  | Status | Count | % |
  |---|---|---|
  | Tested | ~323 | ~90% |
  | Untested | ~37 | ~10% |
  | **Total** | **~360** | |

  More on [test coverage](./test_coverage_report.md).


  ## For developers

  ### Docker Dev Environment
  ```bash
  cd docker
  .\build.bat -image Linux    # or -image Windows
  .\run.bat   -image Linux    # same possibilities
  ```

  To allow the graphics app to display from inside Docker on a Linux host:
  xhost -local:root

  ### Linux

  Install g++13:
  ```bash
  sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
  sudo apt install -y g++-13
  sudo rm -f /usr/bin/g++ /usr/bin/c++
  sudo ln -s /usr/bin/g++-13 /usr/bin/g++
  sudo ln -s /usr/bin/c++-13 /usr/bin/c++
  ```

  Build the library and (optionally) the Python bindings
  ```bash
  mkdir build && cd build
  cmake .. [-DCMAKE_BUILD_TYPE=Release] [-DBUILD_PYTHON=ON]
  make -j6
  ```
  The `-DBUILD_PYTHON=ON` will build locally the python bindings and be ready to run the smoke tests

  Run tests
  ```bash
  ./geompp_tests/geompp_tests
  ./geompp_tests/geompp_tests --gtest_filter="Point2D*"
  ./geompp_tests/geompp_tests --gtest_filter="Point2D.ToFile"
  ```


  ![unit test linux](etc/unit_tests_linux.png)

  ### Windows

  #### via command line

  Provided that you have installed cmake, visual studio, msbuild and nuget

  Build the library and (optionally) the Python bindings
  ```powershell
  # from the main directory, geompp
  mkdir build_win
  cmake -S . -B build_win -G "Visual Studio 18 2026" -A x64 [-DBUILD_PYTHON=ON]
  cmake --build build_win --target geompp [--config Release]
  ```  
  The `-DBUILD_PYTHON=ON` will build locally the python bindings and be ready to run the smoke tests


  Build and run the tests
  ```powershell
  # tests on c++ library

  # from the main directory, geompp
  cmake --build build_win --target geompp_tests [--config Release]
  .\build_win\geompp_tests\Debug[|Release]\geompp_tests.exe [--gtest_filter="Point2D*"]
  # alternatively
  ctest --test-dir build_win/geompp_tests --build-config Debug [|Release]

  # smoke tests on python bindings 
  pip install pytest # useful only the first time
  pip install --no-build-isolation -e ./geompp_python # faster iteration while tweaking bindings
  pytest geompp_python/tests/ -v [-k "BBox3D"]
  ```

  Build the C# DLL
  ```powershell
  # from the main directory, geompp

  # if you want to build for .Net 10
  msbuild geompp_csharp\GeomPP.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

  # if you want to build for .Net Framework 4.8
  msbuild geompp_csharp\GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

  # run smoke tests, after build from the main directory geompp
  dotnet run --project geompp_csharp\tests\GeomPPTests.csproj [-c Release]
  ```

  #### via Visual Studio

  Install [Visual Studio 2026](https://visualstudio.microsoft.com/downloads/), then via the VS
  Installer enable **Desktop Development with C++**.

  Open VS 2022+ → _Open Folder_ → select the `geompp` directory.

  - **Ctrl+Shift+B** — build the whole solution
  - **F5** — run all tests

  ![unit test windows](etc/unit_tests_win_vs.png)


## Versioning 

I maintain three versions, one for each language. Tagging and pushing to github triggers the deployment of several packages. 

```bash
# C++ release archive
git tag v0.1.1
git push origin v0.1.1

# NuGet
git tag csharp-v0.1.1
git push origin csharp-v0.1.1

# PyPI
git tag python-v0.1.1
git push origin python-v0.1.1
```
