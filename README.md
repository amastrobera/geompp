  # Geom++

[![Build / Test (Windows)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml)
[![Build / Test (Linux)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml)
[![C++ version](https://img.shields.io/github/v/tag/amastrobera/geompp?filter=v*&label=C%2B%2B&color=blue)](https://github.com/amastrobera/geompp/tags)

[![Publish NuGet](https://github.com/amastrobera/geompp/actions/workflows/nuget-publish.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/nuget-publish.yml)
[![NuGet version](https://img.shields.io/nuget/v/GeomPP.svg)](https://www.nuget.org/packages/GeomPP)

[![Publish PyPI](https://github.com/amastrobera/geompp/actions/workflows/pypi-publish.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/pypi-publish.yml)
[![PyPI version](https://img.shields.io/pypi/v/geompp.svg)](https://pypi.org/project/geompp)

  A modern C++20 geometry library for 2D and 3D spatial computation — fast, mathematically correct,
  thoroughly tested, and usable from C++, C# (.Net 8/9/10 or .Net Framework 4.8), and Python 3.

  This library is a spiritual successor to [GeomSharp](https://github.com/amastrobera/geom_sharp),
  rewritten to produce better algorithms, faster execution, and no dependency on C#/.NET.

  **Language bindings:**
  [![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)](./geompp_python/README.md) 
  [![C# .NET](https://img.shields.io/badge/C%23_.NET-512BD4?logo=dotnet&logoColor=white)](./geompp_csharp/README.md) 

  
  ![geometry picture 3d](etc/intersections_projections_3d.png)

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
  | `BBall`        | Minimum bounding sphere (Ritter's algorithm)             |
  | `BRect2D`      | Minimum oriented bounding rectangle (rotating calipers) |
  | `BPrism3D`     | Minimum oriented bounding prism (PCA + rotating calipers) |
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
  - **Simplicity / self-intersection** — `Polygon2D::IsSimple()` and the free functions `has_intersections(segments)` (Shamos–Hoey, boolean) / `find_intersections(segments)` (Bentley–Ottmann, returns every crossing point)
  - **Convex hull** — `convex_hull(points)` (`point2d.hpp`) — Andrew's monotone chain, returns hull vertices in CCW order
  - **Planar operations** — `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters. Particularly efficient when streaming large containers of 3D points into 2D algorithms: calling `view.x(p)` and `view.y(p)` per element avoids allocating an intermediate `Point2D` container.

    ```cpp
    #include "view2d.hpp"
    using namespace geompp;

    // axis-aligned views (fastest path — just a coordinate read)
    auto v_xy = View2D::XY();   // x→x, y→y (drops z)
    auto v_yz = View2D::YZ();   // y→x, z→y (drops x)
    auto v_zx = View2D::ZX();   // z→x, x→y (drops y)

    // custom view onto any plane
    auto plane = Plane::FromOriginAndNormal({0,0,5}, {0,0,1});
    auto v_custom = View2D::OnPlane(plane);

    std::vector<Point3D> pts3d = {{1,2,5}, {3,4,5}, {5,6,5}};

    // stream 3D points to 2D without building a Point2D container
    for (auto const& p : pts3d) {
        double u = v_xy.x(p);  // 1, 3, 5
        double w = v_xy.y(p);  // 2, 4, 6
    }
    ```

  - **Bounding containers** — tight-fitting containers around point clouds.

    `BRect2D` computes the minimum-area oriented bounding rectangle via rotating calipers (requires ≥ 3 non-collinear points):

    ```cpp
    #include "brect2d.hpp"
    using namespace geompp;

    std::vector<Point2D> pts = {{0,0}, {4,0}, {4,3}, {2,4}, {0,3}};
    BRect2D rect(pts);
    std::cout << rect.center().ToWkt() << "\n";       // (2.0, 1.75)
    std::cout << rect.width() << " × " << rect.height() << "\n";
    std::cout << "area: " << rect.area() << "\n";
    std::cout << "axis_u: " << rect.axis_u().ToWkt() << "\n";
    auto corners = rect.Corners();                    // 4 Point2D corners
    std::cout << rect.Contains({2, 1}) << "\n";       // true
    ```

    `BPrism3D` computes the minimum-volume oriented bounding prism via PCA + rotating calipers (requires ≥ 3 non-collinear points):

    ```cpp
    #include "bprism3d.hpp"
    using namespace geompp;

    std::vector<Point3D> pts = {
      {0,0,0}, {4,0,0}, {4,3,0}, {0,3,0},
      {0,0,2}, {4,0,2}, {4,3,2}, {0,3,2},
    };
    BPrism3D prism(pts);
    std::cout << prism.center().ToWkt() << "\n";       // roughly (2, 1.5, 1)
    std::cout << "U: " << prism.axis_u().ToWkt() << "\n";
    std::cout << "V: " << prism.axis_v().ToWkt() << "\n";
    std::cout << "W: " << prism.axis_w().ToWkt() << "\n";
    std::cout << prism.width() << " × " << prism.height()
              << " × " << prism.depth() << "\n";       // 4 × 3 × 2
    std::cout << "volume: " << prism.volume() << "\n"; // 24.0
    auto corners = prism.Corners();                    // 8 Point3D corners
    std::cout << prism.Contains({2, 1.5, 1}) << "\n"; // true
    ```

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

  There also is a whole [set of code examples](./code_examples.md) in the next page. 


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
  | Done | 2D primitives, operations, tests, WKT/file I/O, GitHub Actions CI, Docker (Linux), basic OpenGL viewer, [C# bindings (NuGet)](./geompp_csharp/README.md), [Python bindings (PyPI)](./geompp_python/README.md); `Triangle2D/3D::Location()` (barycentric coords); `Polygon2D/3D::Contains()` (winding number); `Triangle3D::Contains()` (barycentric, no projection); `Triangle3D::Intersection(×Line/Ray/Seg/Plane/△)` and the symmetric `Plane::Intersection(Triangle3D)`; `Line3D/Ray3D/LineSegment3D::Distance(...)` and `DistanceTo(...)` between every pair of 3D linear primitives + `LineSegment3D::Flip()`; `Polygon2D::IsSimple()` via `has_intersections` (Shamos–Hoey) and `find_intersections` (Bentley–Ottmann) on the new `calc_utils2d` sweep-line module (`EventQueue2D`, `SweepLineComparator`, `SweepLine2D`) plus 2D helpers `is_left` / `is_right` / `intersect(seg, seg)`; `convex_hull(vector<Point2D>)` (Andrew's monotone chain) |
  | Next | TBC |
  | Backlog | overlap/adjacency, polygon clipping, definition of "non-planar polygon" or mesh, triangulation/polygonization |


  I am at improving the test coverage, see how in [test coverage plan](./test_coverage_plan.md).

  | Metric | Count | Notes |
  |--------|-------|-------|
  | Public methods | ~399 | Excl. ctors/dtors/operators |
  | C++ tested | ~355 | ~89% |
  | Python tested | ~210 | ~53% |
  | C# tested | ~215 | ~54% |
  | Stubs (not yet impl.) | 10 | Polygon2D/3D::DistanceTo; Triangle2D::Intersection(△); Triangle2D/3D::DistanceTo |

  More on [test coverage](./test_coverage_report.md).


  ## For developers

  ### Docker Dev Environment

  You can inspect the `Dockerfile.lin` (or `.win`) to see what the system dependencies are. 

  ```bash
  cd docker
  .\build.bat -image Linux    # or -image Windows
  .\run.bat   -image Linux    # same possibilities
  ```

  To allow the graphics app to display from inside Docker on a Linux host:
  xhost -local:root

  ### Linux

  Build the library and (optionally) the Python bindings
  ```bash
  mkdir build && cd build
  cmake .. [-DCMAKE_BUILD_TYPE=Release] [-DBUILD_PYTHON=ON]
  make -j6
  ```
  The `-DBUILD_PYTHON=ON` will build locally the python bindings and be ready to run the smoke tests

  Run tests
  ```bash
  # from the build directory
  ./geompp_tests/geompp_tests
  ./geompp_tests/geompp_tests --gtest_filter="Point2D*"
  ./geompp_tests/geompp_tests --gtest_filter="Point2D.ToFile"

  # or 
  ctest [--build-config Debug]
  ```

  Python bindings and tests (if you did cmake with the flag -DBUILD_PYTHON=ON)
  ```bash
  # from the build directory
  pip install pytest # useful only the first time
  pip install --no-build-isolation -e ./geompp_python # faster iteration while tweaking bindings
  pytest geompp_python/tests/ -v [-k "BBox3D"]
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
  .\build_win\geompp_tests\Debug\geompp_tests.exe [--gtest_filter="Point2D*"]
  [.\build_win\geompp_tests\Release\geompp_tests.exe [--gtest_filter="Point2D*"] ]
  # alternatively
  ctest --test-dir build_win/geompp_tests --build-config Debug
  [ctest --test-dir build_win/geompp_tests --build-config Release]
  ```

  Python bindings and tests (if you did cmake with the flag -DBUILD_PYTHON=ON)
  ```powershell
  # smoke tests on python bindings 
  pip install pytest # useful only the first time
  Remove-Item S:\development\geompp\geompp_python\geompp\_geompp.cp314-win_amd64.pyd # in case you have already built it before
  pip install --no-build-isolation -e ./geompp_python # faster iteration while tweaking bindings
  pytest geompp_python/tests/ -v [-k "BBox3D"]
  ```

  Build the C# DLL
  ```powershell
  # from the main directory, geompp

  # .NET 8 (LTS)
  msbuild geompp_csharp\GeomPP_Net8.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

  # .NET 9 (STS)
  msbuild geompp_csharp\GeomPP_Net9.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

  # .NET 10 (LTS)
  msbuild geompp_csharp\GeomPP.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

  # .NET Framework 4.8
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
