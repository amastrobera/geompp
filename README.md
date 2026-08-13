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

  ### Why do we need one ? 

  You may be a CAD or a Game developer using C#.Net, and you use APIs native to the platform you develop into. These native APIs are easy to get in, but may contain bugs that have not been fixed, or simply lack some functionalities. 
  You may be a Data Scientist using Python on a GIS project, and having to import 3+ libraries, and covert from data-structure to data-structure to use it. 
  You may be a C++ developer who wants to import a more lightweight library than those which already exist, and possibly more user friendly. 
  
  This library was born a few years ago to solve all these problems. It was recently augmented with the aim of using the most modern algorithms to solve a variety of geometrical problems. 

  The sources of these algorithms are to be found in several textbooks, such as 
  - Practical Geometry Algorithms (Danniel Sunday)
  - Computational Geometry in C (Joseph O'Rourke)
  - Computational Geometry (Mark de Berg, Marc van Kreveld, Mark Overmars, Otfried Schwarzkopf)

  Finally, the help of AI was used to validate algorithms (bug-free, guarantee the desired big-O), bind into other languages than C++, add edge cases to achieve a high test coverage, and build documentation. 


  **Language bindings:**
  [![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)](./geompp_python/README.md) 
  [![C# .NET](https://img.shields.io/badge/C%23_.NET-512BD4?logo=dotnet&logoColor=white)](./geompp_csharp/README.md) 

  
  ![geometry picture 3d](images/main_page.png)


  ## Available in multiple languages and platforms

  - C++ 20 and above

  - Python ([view](./geompp_python/README.md))

    | Platform | versions |
    |---|---|
    | Linux x86_64 | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |
    | Windows x64  | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |

  
  - C# ([view](./geompp_csharp/README.md))

    | Platform | .Net | .Net Framework |
    |---|---|---|
    | Windows x64  | 8 · 9 · 10 | 4.8 |


  ## What it provides

  ### Classes

  Where not explicitely specified, both 2D and 3D variants are available for all core types:

  | Primitive        | Description                                              |
  |------------------|----------------------------------------------------------|
  | `Point`          | A coordinate in space                                    |
  | `Vector`         | Direction and magnitude                                  |
  | `Line`           | An infinite line through two points                      |
  | `Ray`            | A semi-infinite line from an origin in one direction     |
  | `LineSegment`    | A finite segment between two endpoints                   |
  | `Polyline`       | A connected chain of segments                            |
  | `Triangle`       | Three non-collinear points forming a closed face         |
  | `Polygon`        | A closed polygon defined by an ordered list of vertices  |
  | `BBox`           | Axis-aligned bounding box                                |
  | `BBall`          | Minimum bounding sphere (Ritter's algorithm)             |
  | `BRect2D`        | Minimum oriented bounding rectangle (rotating calipers) |
  | `BPrism3D`       | Minimum oriented bounding prism (PCA + rotating calipers) |
  | `Plane`          | A flat surface in 3D defined by a point and a normal     |
  | `View2D`         | A class that converts a 3D point into 2D quicker than plane|
  | `Mesh`           | A set of adjacent triangles that together make up a detailed 2D or 3D shape (**a surface or a solid**)|
  | `ConnectedMesh`  | This one keeps track of the neighbors of each triangle, so that going from a facet to its 0-3 neighbors is very quick|
  | `PolyMesh`       | Not just triangles, also polygons are allowed, in order to save on the number of vertices on the same planar regions of the surface|
  

  ### Algorithm overview

  Each primitive supports a consistent set of spatial operations where applicable:

  - **Containment** — does a shape contain a given point?
  - **Intersection** — do two shapes strike through each other, and what is the resulting geometry? It is also possible to use a free function `find_intersections()` on a free set of segments. The definition of this operation changes from 2D to 3D, check it out. 
  - **Overlap** — do two shapes have a portion in common, and what is the resulting geometry? The definition of this operation changes from 2D to 3D, check it out. 
  - **Touch** — do two shapes have a point in common, and which is it? The definition of this operation changes from 2D to 3D, check it out. 
  - **Distance** — closest distance from a point to a shape. 
  - **Plane operations** — projection of a point from 3D to 2D, and re-projection from 2D to 3D. In addition to the `Plane` class, a special class called `View2D` is capable of making quicker transitions from 3D to 2D, of one of the 3 world planes (XY, YZ, ZX) or a custom plane, which comes handy for re-using our algorithms (as well as yours). 
  - **Interpolation / Location** — retrieve a point at parameter `t` along a segment or polyline, and - as opposite operation - find the parameter `t` for a point already on a shape. 
  - **Area / Perimeter / Centroid** — geometric properties for closed shapes. 
  - **Signed area** — encodes orientation (clockwise vs. counter-clockwise in 2D, surface normal direction in 3D)
  - **Simplicity / self-intersection** — `Polygon2D::IsSimple()` and the free functions `has_intersections(segments)` (Shamos–Hoey, boolean) / `find_intersections(segments)` (Bentley–Ottmann, returns every crossing point)
  - **Convex hull** — `convex_hull(points)` (`point2d.hpp`) — Andrew's monotone chain, returns hull vertices in CCW order
  - **Planar operations** — `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters. Particularly efficient when streaming large containers of 3D points into 2D algorithms: calling `view.x(p)` and `view.y(p)` per element avoids allocating an intermediate `Point2D` container.
  - **Bounding containers** — tight-fitting containers around point clouds. Available in different varieties (axis-aligned bounding box, bounding ball, minimal oriented rectangle, convex-hull)
  - **Polyline operations** — given a `Polyline` it is possible to `Reduce()` it (or `Extend()` it) according to several decimation (or smoothing) algorithms. It is also possible to use the free functions `polyline_extension()` and `xxx_decimation()` (different algorithms available) for a list of consecutive points not into a polyline data-structure. 
  - **Polygon boolean operations** — Intersection, Union, Difference and Xor (either or) between two polygons are possible. One polygon clips the other with map-overlay method. 
  - **Point cloud operations** — Principal Component Analysis (PCA) function `principal_axes()` helps you find the empirical 3 directive axis of a list of points in space.
  - **Triangulation** — decomposition of a polygon into n-triangles, using several possible algorithms such as the _Ear Clip_, a _Best Fit Ear Clip_, _Monotone Polygon_ or _Constrained Delaunay_. 

  Return types are often `optional` and sometimes `optional<variant<...>>` so callers can match on the exact geometry produced by an intersection without casting.

  ### Free functions

  In addition to the methods on each class, these free functions in `namespace geompp` operate directly
  on point lists / segment lists (no need to construct a class instance first):

  | Function | Description |
  |---|---|
  | `are_collinear(p1, p2, p3)` | Three points on the same line |
  | `remove_consecutive_duplicates(points)` | Drop consecutive duplicate points |
  | `remove_duplicates(points)` | Drop duplicate points |
  | `remove_collinear(points)` | Drop collinear intermediate points |
  | `linear_combination(points, weights)` | Weighted sum |
  | `average(points)` | Arithmetic mean |
  | `lerp(p0, p1, t)` | Linear interpolation between two points — `P0 + t*(P1-P0)`, not clamped |
  | `centroid(points[, plane])` | Centroid of a polygon (3D: plane auto-detected if omitted) |
  | `signed_area(points[, plane])` | Signed area — positive if CCW, negative if CW |
  | `are_ccw(points[, ref_plane])` | Counter-clockwise winding (2D or 3D) |
  | `are_cw(points[, ref_plane])` | Clockwise winding (2D or 3D) |
  | `are_coplanar(points)` | List of `Point3D` on the same plane |
  | `closest_world_plane_to(points)` | XY / YZ / ZX world plane nearest to the point cloud |
  | `has_intersections(segments)` | Shamos–Hoey: `true` if any two segments in a `LineSegment2D` list cross |
  | `find_intersections(segments)` | Bentley–Ottmann: every crossing point among a set of segments, sorted left-to-right |
  | `convex_hull(points[, normal])` | Andrew's monotone chain: convex hull, returned in CCW order (3D: normal auto-detected if omitted) |
  | `clip(clipper_loop, subject_loop)` | Set intersection of two point loops — 2D natively, 3D if coplanar (map-overlay engine, same as `Polygon::Intersection`) |
  | `dist_decimation(points, threshold)` | O(n) radial-distance point decimation |
  | `rdp_decimation(points, threshold)` | Ramer–Douglas–Peucker point decimation |
  | `vw_decimation(points, threshold)` | Visvalingam–Whyatt point decimation |
  | `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance\|num_segments[, min_segment_length])` | Rounds one polyline corner with a quadratic Bezier arc |
  | `polyline_expansion(points, settings)` | Rounds every inner corner of a point list and works with either fixed number of segmens or fixed min segment length (the engine behind `Polyline::Expand()`) |
  | `principal_axes(points)` | PCA on a point cloud: returns a `CoordinateFrame` (`.x` primary, `.y` secondary, `.z` best-fit normal) |
  | `principal_normal(points)` | Best-fit plane normal (PCA eigenvector with smallest eigenvalue) |
  | `principal_direction(points)` | Dominant direction (PCA eigenvector with largest eigenvalue) |
  | `find_extreme_points(polygon, line)` | The two polygon vertices least/greatest projected along a line's direction |
  | `distance_to(polygon, line)` | Distance from a polygon to a line (zero if they intersect) |
  | `tangents_to(polygon, point_or_polygon)` | Tangent segments from a point to a polygon, or common outer tangents between two polygons |
  | `triangulate(polygons, settings)` | Returns a set of adjacent triangles replacing the surface of 1+ polygons (the engine behind `Polygon::Triangulate()` and `PolyMesh::Triangulate()`), and with a robust input validation |
  
  ### Serialization

  All primitives support:
  - **WKT** (Well-Known Text) — `ToWkt()` / `FromWkt()` for standard text interchange
  - **Binary file I/O** — `ToFile()` / `FromFile()` for compact storage

  ### Precision

  Floating-point comparisons use a thread-local `DECIMAL_PRECISION` constant via `AlmostEquals()` methods, making the library robust against rounding errors while remaining configurable per thread.


  ### How to use it

  Here is an example of code. You can also look at the [test directory](./geompp_tests/) to see more.

  A quick list of code examples per topic is provided here.

  👉 [Visual Documentation and Code Examples](./visual_doc_and_sample_code.md)


  ## Roadmap

  High-level development plan:

  | Status | Area |
  |--------|------|
  | Done | 2D primitives, operations, tests, WKT/file I/O, GitHub Actions CI, Docker (Linux), [C# bindings (NuGet)](./geompp_csharp/README.md), [Python bindings (PyPI)](./geompp_python/README.md); 3D primitives, operations, tests, serialization; Planes and projections; Intersections of Ray/Line/Segments; Intersections of a set of Segments; Simple vs Complex Polygons; Contains(Point); Bounding Containers and Convex Hulls; Polylines; View2D and common algorithms between 2D and 3D; Overlap and Touch concepts; polygon extreme points along a line, polygon tangents; polyline decimation, Bezier corner smoothing, polyline expansion; polygon clipping; boolean operations (union, intersection, difference); Mesh2D/3D and PolyMesh2D/3D (triangle- and polygon-faced meshes with spatial-hash vertex welding and mesh-conformity validation — every edge has at most 1 neighbor, no T-junctions/non-manifold edges — but no queryable adjacency structure); `ConnectedMesh2D`/`ConnectedMesh3D` (triangle mesh with precomputed per-facet edge adjacency, queried via a public `FaceView2D`/`FaceView3D` — `Geometry`/`Neighbor`/`NeighborEntryEdge` — bound in Python/C#); Triangulation (ear-clipping, two strategies: `EarClipping` — fast, first valid ear found; `EarClippingBestFit` — clips the least sliver-prone valid ear each step, default) — a free `triangulate()` function (2D, and 3D assumed flat/planar) plus `Polygon2D/3D.Triangulate()` and `PolyMesh2D/3D.Triangulate()` (per-facet, combined into one `Mesh2D/3D`); `geompp::maths` — fixed-size linear algebra (`Vector2/3/4`, `Matrix2/3/4`), Gauss/Cramer solvers, homogeneous-transform factories, bound in Python/C#; `geompp::transformations` — affine transforms (`translate`/`rotate`/`scale`, `transform(primitive, matrix)`, `TransformBuilder`) for every primitive from `Point2D/3D` to `PolyMesh2D/3D`, bound in Python/C#; |
  | Backlog | adjacency structure for `PolyMesh2D/3D`, polygonization, monotone-polygon and Delaunay triangulation strategies |


  This is the summary of the current test coverage. More on [test coverage](./test_coverage_report.md).

  | Metric | Count | Notes |
  |--------|-------|-------|
  | Public methods (C++) | ~513 | Excl. ctors/dtors/operators. `geompp::maths`/`geompp::transformations` (templated/free-function, header-only) tracked separately, see test_coverage_report.md |
  | C++ methods tested | ~493/513 | ~96% (1097 TEST cases, 1095 run, 2 disabled — incl. 37 for `geompp::maths`, 30 for `geompp::transformations`) |
  | Python methods tested | 454/474 | ~96% (868 pytest cases — incl. 27 for `geompp.maths`, 24 for `geompp.transformations`) |
  | C# methods tested | 514/581 | ~88% (964 harness tests — incl. 24 for `GeomPP.Maths`, 23 for `GeomPP.Transformations`) |
  | Stubs (not yet impl.) | 2 | `TriangulationParams::Strategy::MonotonePolygon`/`Delaunay` — intentional, see test_coverage_report.md |



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
  cmake .. [-DCMAKE_BUILD_TYPE=Debug|Release] [-DBUILD_PYTHON=ON]
  make -j6
  ```
  The `-DBUILD_PYTHON=ON` will build locally the python bindings and be ready to run the smoke tests
  The `-DCMAKE_BUILD_TYPE=Debug` is necessary only if you want to debug the project (test executables) with tools like `gdb` and if you want all the `assert` statements to print something out. 

  Run tests
  ```bash
  # from the build directory
  ./geompp_tests/geompp_tests
  ./geompp_tests/geompp_tests --gtest_filter="Point2D*"
  ./geompp_tests/geompp_tests --gtest_filter="Point2D.ToFile"

  # or 
  ctest [--build-config Debug]
  ```

  If you want to debug this code (cpp tests)
  ```bash
  # build in debug mode
  mkdir build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Debug
  make -j6

  # install GDB if you have to (straightforward)

  # use the LLDB debugger if needed
  gdb --args '.\build_win\geompp_tests\Debug\geompp_tests.exe' '--gtest_filter="Polygon2D*Star*"'
  # use commands as in gdb
  # b test_polygon2d.cpp:1439  # break points
  # r                          # run
  # s                          # step in (function)
  # n                          # next line
  # p poly.ToWkt()            # print a variable / function 
  # c                          # continue until next break point or end of program
  # q                          # quit
  ```

  Python bindings and tests (if you did cmake with the flag -DBUILD_PYTHON=ON)
  ```bash
  # from the build directory
  pip install pytest # useful only the first time
  pip install --no-build-isolation -e ./geompp_python # faster iteration while tweaking bindings
  pytest geompp_python/tests/ -v [-k "BBox3D"]
  ```


  ![unit test linux](images/unit_tests_linux.png)

  ### Windows

  #### via command line

  Provided that you have installed cmake, visual studio, msbuild and nuget

  Build the library and (optionally) the Python bindings
  ```powershell
  # from the main directory, geompp
  mkdir build_win
  cmake -S . -B build_win -G "Visual Studio 18 2026" -A x64 [-DBUILD_PYTHON=ON] [-DCMAKE_BUILD_TYPE=Debug|Release]
  cmake --build build_win --target geompp [--config Release]
  ```  
  The `-DBUILD_PYTHON=ON` will build locally the python bindings and be ready to run the smoke tests
  The `-DCMAKE_BUILD_TYPE=Debug` is necessary only if you want to debug the project (test executables) with tools like `lldb.exe` (install LLVM via winget) and if you want all the `assert` statements to print something out. 

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

  If you want to debug this code (cpp tests)
  ```powershell
  # build in debug mode
  cmake -S . -B build_win -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Debug
  cmake --build build_win --target geompp --target geompp_tests

  # install the LLVM package (for first time only)
  # -> winget install LLVM.LLVM
  # -> save the path C:\Program Files\LLVM\bin in your "Environment Variables" -> User -> Path.  

  # use the LLDB debugger if needed
  lldb .\build_win\geompp_tests\Debug\geompp_tests.exe [-- --gtest_filter="Polygon2D*Star*"]
  # use commands as in gdb
  # b test_polygon2d.cpp:1439  # break points
  # r                          # run
  # s                          # step in (function)
  # n                          # next line
  # p poly.ToWkt()            # print a variable / function 
  # c                          # continue until next break point or end of program
  # q                          # quit
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

  ![unit test windows](images/unit_tests_win_vs.png)
