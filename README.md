  # Geom++

  [![Build / Test (Windows)](https://github.com/amastrobera/geompp/actions/workflows/build_test_window
  s.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml)
  [![Build / Test (Linux)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.ym
  l/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml)

  A modern C++20 geometry library for 2D and 3D spatial computation — fast, mathematically correct,
  thoroughly tested, and designed to eventually be usable from languages beyond C++ (Python, C#).

  This library is a spiritual successor to [GeomSharp](https://github.com/amastrobera/geom_sharp),
  rewritten to produce better algorithms, faster execution, and no dependency on C#/.NET.


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

  See [WIP.md](./WIP.md) for the full task list. High-level:

  | Status | Area |
  |--------|------|
  | Done | 2D primitives, operations, tests, WKT/file I/O, GitHub Actions CI, Docker (Linux), basic
  OpenGL viewer |
  | Next | Docker (Windows), geom_viewer camera/input/delete |
  | Backlog | Polygon ops, convex hull, overlap/adjacency, 3D polygon & mesh, polygon clipping, Python
   bindings, C# bindings |


  ## Build it

  ### Docker Dev Environment

  cd docker
  .\build.bat -image Linux    # or -image Windows
  .\run.bat   -image Linux    # same possibilities

  To allow the graphics app to display from inside Docker on a Linux host:
  xhost -local:root

  ### Linux

  Install g++13:
  sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
  sudo apt install -y g++-13
  sudo rm -f /usr/bin/g++ /usr/bin/c++
  sudo ln -s /usr/bin/g++-13 /usr/bin/g++
  sudo ln -s /usr/bin/c++-13 /usr/bin/c++

  Build and test:
  mkdir build && cd build
  cmake ..
  make -j6

  ./geompp_tests/geompp_tests
  ./geompp_tests/geompp_tests --gtest_filter="Point2D*"
  ./geompp_tests/geompp_tests --gtest_filter="Point2D.ToFile"

  ![unit test linux](etc/unit_tests_linux.png)

  ### Windows

  Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/), then via the VS
  Installer enable **Desktop Development with C++**.

  Open VS 2022 → _Open Folder_ → select the `geompp` directory.

  - **Ctrl+Shift+B** — build the whole solution
  - **F5** — run all tests

  ![unit test windows](etc/unit_tests_win_vs.png)


