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
  - **Intersection** — do two shapes strike through each other, and what is the resulting geometry?
  - **Overlap** — do two shapes have a portion in common, and what is the resulting geometry?
  - **Touch** — do two shapes have a point in common, and which is it?w
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
  - **Bounding containers** — tight-fitting containers around point clouds.

  Return types are often `std::optional<std::variant<...>>` so callers can match on the exact geometry produced by an intersection without casting.

  ### Available in multiple languages

  - C++ 20

  - Python ([view](./geompp_python/README.md))

    | Platform | versions |
    |---|---|
    | Linux x86_64 | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |
    | Windows x64  | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |

  
  - C# ([view](./geompp_csharp/README.md))

    | Platform | .Net | .Net Framework |
    |---|---|---|
    | Windows x64  | 8 · 9 · 10 | 4.8 |
  

  ### Serialization

  All primitives support:
  - **WKT** (Well-Known Text) — `ToWkt()` / `FromWkt()` for standard text interchange
  - **Binary file I/O** — `ToFile()` / `FromFile()` for compact storage

  ### Precision

  Floating-point comparisons use a thread-local `DECIMAL_PRECISION` constant via `AlmostEquals()`
  methods, making the library robust against rounding errors while remaining configurable per thread.


  ### How to use it

  Here is an example of code. You can also look at the [test directory](./geompp_tests/) or [sample code](./geompp_sample/sample.cpp) to see more.

  A quick list of code examples per topic is provided here.

  <details open><summary><b>Code Examples</b></summary>

<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 1.1 Create geometries from classes</b></summary>

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

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 1.2 Create geometries from text</b></summary>

  ```cpp
  // This will be the precision used by all functions, in all threads, for this
  // run of the program, and it can be modified in later code anytime.
  g::DECIMAL_PRECISION = g::DP_THREE;

  // two line segments intersecting at (0,0,1)
  auto s1 = g::LineSegment3D::FromWkt("LINESTRING (1  0  0, -1 0 2)");
  auto s2 = g::LineSegment3D::FromWkt("LINESTRING (0 1  0, 0 -1 2)");

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

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 1.3 Import geometries from a file</b></summary>

  ```cpp
  std::string const lsv_path = "sample_geometries.lsv";
  //   POINT (1 2 3)
  //   POINT (4 5 6)
  //   LINESTRING (0 0 0, 1 1 1)
  //   LINESTRING (2 0 0, 2 3 4)
  //   LINE (0 0 0, 1 0 0)
  //   RAY (0 0 0, 0 1 0)

  g::DECIMAL_PRECISION = g::DP_THREE;

  auto parser = g::WktParser::Open(lsv_path);
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

    GEOMPP_LOG(INFO) << g::WktParser::ToWkt(entry.value());
  }
  ```

  will print out exactly the list of geometries above.

</details>

</details>

<details open>
<summary><b> &nbsp; 2. Geometry Operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 2.1 Containment </b></summary>

`Triangle3D::Contains(p)` and `Polygon2D/3D::Contains(p)` test whether a point lies inside a shape
  using barycentric coordinates and the winding number, respectively.
  `LineSegment::Contains(p)` checks whether a point lies on the segment;
  `Location(p)` returns the parameter `t ∈ [0, 1]` for a point already on it, and `Interpolate(t)` reverses the mapping.

  ```cpp
  #include "triangle3d.hpp"
  #include "polygon2d.hpp"
  #include "line_segment2d.hpp"

  namespace g = geompp;

  // Triangle3D — barycentric containment
  auto tri = g::Triangle3D::Make(
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 4, 0));
  GEOMPP_LOG(INFO) << tri.Contains(g::Point3D(1, 1, 0));   // 1 — inside
  GEOMPP_LOG(INFO) << tri.Contains(g::Point3D(3, 3, 0));   // 0 — outside

  // Polygon2D — winding-number containment
  auto poly = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(6, 0),
      g::Point2D(6, 4), g::Point2D(0, 4),
  });
  GEOMPP_LOG(INFO) << poly.Contains(g::Point2D(3, 2));     // 1
  GEOMPP_LOG(INFO) << poly.Contains(g::Point2D(7, 2));     // 0

  // LineSegment2D — containment, location, and interpolation
  auto seg = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 0));
  GEOMPP_LOG(INFO) << seg.Contains(g::Point2D(2, 0));      // 1
  GEOMPP_LOG(INFO) << seg.Contains(g::Point2D(2, 1));      // 0

  double t   = seg.Location(g::Point2D(2, 0));   // 0.5
  auto   mid = seg.Interpolate(0.5);              // Point2D(2, 0)
  GEOMPP_LOG(INFO) << "t = " << t;
  GEOMPP_LOG(INFO) << "mid = " << mid.ToWkt();
  ```

  ```bash
  1
  0
  1
  0
  1
  0
  t = 0.5
  mid = POINT (2 0)
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; 2.2 Intersection </b></summary>

  Every 2D primitive (`Line2D`, `Ray2D`, `LineSegment2D`, `Triangle2D`, `Polygon2D`) can intersect any
  other 2D primitive, and the same holds in 3D across `Line3D`, `Ray3D`, `LineSegment3D`, `Triangle3D`,
  and `Plane`. Results are `std::optional<std::variant<...>>` for point-or-segment outcomes, or
  `std::optional<std::vector<LineSegment2D>>` when polygon clipping can produce multiple chords.
  `find_intersections(segments)` (Bentley–Ottmann) reports all crossing points across an arbitrary set
  of 2D segments, sorted left-to-right.

  ```cpp
  #include "polygon2d.hpp"
  #include "line2d.hpp"
  #include "ray2d.hpp"
  #include "line_segment2d.hpp"
  #include "triangle3d.hpp"
  #include "calc_utils2d.hpp"

  namespace g = geompp;

  // LineSegment2D → Polygon2D: segment is clipped to the polygon interior
  auto square = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0),
      g::Point2D(4, 4), g::Point2D(0, 4),
  });
  auto seg = g::LineSegment2D::Make(g::Point2D(-1, 2), g::Point2D(5, 2));
  auto seg_chords = square.Intersection(seg);
  if (seg_chords)
      for (auto const& c : *seg_chords)
          GEOMPP_LOG(INFO) << c.ToWkt();  // LINESTRING (0 2, 4 2)

  // Ray2D → Polygon2D: ray entering from outside, clipped at the exit boundary
  auto ray2d = g::Ray2D::Make(g::Point2D(-1, 2), g::Vector2D(1, 0));
  auto ray_chords = square.Intersection(ray2d);
  if (ray_chords)
      for (auto const& c : *ray_chords)
          GEOMPP_LOG(INFO) << c.ToWkt();  // LINESTRING (0 2, 4 2)

  // Line2D → concave Polygon2D: vertical line through a C-shape produces two chords
  auto cshape = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 1),
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(4, 3),
      g::Point2D(4, 4), g::Point2D(0, 4),
  });
  auto line2d     = g::Line2D::Make(g::Point2D(2, 0), g::Point2D(2, 1));
  auto lin_chords = cshape.Intersection(line2d);
  if (lin_chords)
      for (auto const& c : *lin_chords)
          GEOMPP_LOG(INFO) << c.ToWkt();
  // LINESTRING (2 0, 2 1)
  // LINESTRING (2 3, 2 4)

  // Ray3D → Triangle3D
  auto tri = g::Triangle3D::Make(
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 4, 0));
  auto ray = g::Ray3D::Make(g::Point3D(1, 1, 3), g::Vector3D(0, 0, -1));
  auto hit = tri.Intersection(ray);
  if (hit)
      GEOMPP_LOG(INFO) << std::get<g::Point3D>(*hit).ToWkt();  // POINT (1 1 0)

  // LineSegment2D vs LineSegment2D
  auto s1    = g::LineSegment2D::Make(g::Point2D(0, 1), g::Point2D(4, 1));
  auto s2    = g::LineSegment2D::Make(g::Point2D(2, 0), g::Point2D(2, 4));
  auto cross = s1.Intersection(s2);
  if (cross)
      GEOMPP_LOG(INFO) << std::get<g::Point2D>(*cross).ToWkt();  // POINT (2 1)

  // find_intersections — all crossing points (Bentley–Ottmann)
  std::vector<g::LineSegment2D> segs = {
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 4)),
      g::LineSegment2D::Make(g::Point2D(0, 4), g::Point2D(4, 0)),
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(4, 2)),
  };
  auto crossings = g::find_intersections(segs);   // sorted left-to-right
  for (auto const& p : crossings)
      GEOMPP_LOG(INFO) << p.ToWkt();
  ```

  ```bash
  LINESTRING (0 2, 4 2)
  LINESTRING (0 2, 4 2)
  LINESTRING (2 0, 2 1)
  LINESTRING (2 3, 2 4)
  POINT (1 1 0)
  POINT (2 1)
  POINT (1 2)
  POINT (2 2)
  POINT (3 2)
  ```


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 2.2.1 Split a complex polygon </b></summary>

  A **complex polygon** (also called a self-intersecting polygon) is a polygon whose edges cross
  each other. `Simplify()` decomposes it into a list of simple (non-self-intersecting) polygons
  via planar-graph half-edge face tracing. Each returned polygon is guaranteed to satisfy
  `IsSimple() == true`. If the input is already simple, `Simplify()` returns a single-element
  vector containing the original polygon.

  ```cpp
  #include "polygon2d.hpp"
  #include "polygon3d.hpp"

  namespace g = geompp;

  g::DECIMAL_PRECISION = g::DP_THREE;

  // 2D: a "bowtie" — edges B→C and D→A cross at (2,2)
  auto bowtie = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(1, 3), g::Point2D(3, 3)});
  GEOMPP_LOG(INFO) << "is simple: " << bowtie.IsSimple();  // 0

  auto parts = bowtie.Simplify();
  GEOMPP_LOG(INFO) << parts.size() << " simple polygon(s)";
  for (auto const& p : parts) {
      GEOMPP_LOG(INFO) << "  " << p.ToWkt() << "  area=" << p.Area();
  }

  // 3D: same bowtie lifted into the XY plane (z = 0)
  auto bowtie3d = g::Polygon3D::Make(
      {g::Point3D(0,0,0), g::Point3D(4,0,0), g::Point3D(1,3,0), g::Point3D(3,3,0)});
  auto parts3d = bowtie3d.Simplify();
  GEOMPP_LOG(INFO) << parts3d.size() << " simple 3D polygon(s)";
  ```

  ```bash
  is simple: 0
  2 simple polygon(s)
    POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4
    POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1
  2 simple 3D polygon(s)
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 2.3 Overlap </b></summary>

  `Overlaps(other)` returns `true` when two primitives share a 1D region (more than a single point).
  `Overlap(other)` returns the shared geometry, or `std::nullopt` when they do not overlap or
  only touch at a single point.  The return type mirrors the "smaller" of the two primitives:
  a `Line × Line` overlap yields a `Line`; `Ray × Ray` with opposite directions yields a
  `LineSegment`; `Segment × Line` / `Segment × Ray` / `Segment × Segment` always yield a `LineSegment`.
  All six primitive types (`Line2D`, `Ray2D`, `LineSegment2D`, and their 3D counterparts) implement
  `Overlaps` and `Overlap` against all three family members.

  ```cpp
  #include "line2d.hpp"
  #include "ray2d.hpp"
  #include "line_segment2d.hpp"

  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  // ── Line × Line ────────────────────────────────────────────────────────────
  auto x_axis = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto x_same = g::Line2D::Make(g::Point2D(5, 0), g::Point2D(8, 0));   // same infinite line
  auto x_off  = g::Line2D::Make(g::Point2D(0, 1), g::Vector2D::BasisX());  // parallel, offset

  GEOMPP_LOG(INFO) << x_axis.Overlaps(x_same);  // 1
  GEOMPP_LOG(INFO) << x_axis.Overlaps(x_off);   // 0 (no shared region)

  auto ov_ll = x_axis.Overlap(x_same);           // std::optional<Line2D>
  if (ov_ll)
      GEOMPP_LOG(INFO) << ov_ll->ToWkt();         // LINE (0 0, 1 0)

  // ── Ray × Ray (same direction) ─────────────────────────────────────────────
  auto r1 = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D::BasisX());
  auto r2 = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D::BasisX());  // inside r1

  auto ov_rr = r1.Overlap(r2);  // std::optional<std::variant<Ray2D, LineSegment2D>>
  if (ov_rr && std::holds_alternative<g::Ray2D>(*ov_rr))
      GEOMPP_LOG(INFO) << std::get<g::Ray2D>(*ov_rr).ToWkt();  // RAY (3 0, 1 0)

  // ── Ray × Ray (anti-parallel) ──────────────────────────────────────────────
  auto r3 = g::Ray2D::Make(g::Point2D(7, 0), g::Vector2D(-1, 0));  // heads toward r1

  auto ov_anti = r1.Overlap(r3);
  if (ov_anti && std::holds_alternative<g::LineSegment2D>(*ov_anti))
      GEOMPP_LOG(INFO) << std::get<g::LineSegment2D>(*ov_anti).ToWkt();
  // LINESTRING (0 0, 7 0)

  // ── Segment × Segment ──────────────────────────────────────────────────────
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(5, 0));
  auto b = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(8, 0));

  GEOMPP_LOG(INFO) << a.Overlaps(b);  // 1
  auto ov_ss = a.Overlap(b);          // std::optional<LineSegment2D>
  if (ov_ss)
      GEOMPP_LOG(INFO) << ov_ss->ToWkt();  // LINESTRING (3 0, 5 0)

  // touch at a single endpoint → no overlap
  auto c = g::LineSegment2D::Make(g::Point2D(5, 0), g::Point2D(9, 0));
  GEOMPP_LOG(INFO) << a.Overlaps(c);   // 0
  GEOMPP_LOG(INFO) << (a.Overlap(c) == std::nullopt);  // 1
  ```

  ```bash
  1
  0
  LINE (0 0, 1 0)
  RAY (3 0, 1 0)
  LINESTRING (0 0, 7 0)
  1
  LINESTRING (3 0, 5 0)
  0
  1
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; 2.4 Touch </b></summary>

  `Touches(other)` returns `true` when two primitives share exactly one endpoint-contact point
  (not an interior crossing, not a shared segment). `Touch(other)` returns that contact point,
  or `std::nullopt` when there is no touch.

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  // Ray origin sits on a line → touch at origin
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));   // x-axis
  auto ray  = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D(0, 1));   // vertical at x=3
  GEOMPP_LOG(INFO) << line.Touches(ray);                              // 1
  auto tp = line.Touch(ray);                                          // std::optional<Point2D>
  GEOMPP_LOG(INFO) << tp->ToWkt();                                    // POINT (3 0)

  // Collinear ray → overlap, not touch
  auto ray_col = g::Ray2D::Make(g::Point2D(1, 0), g::Vector2D(1, 0));
  GEOMPP_LOG(INFO) << line.Touches(ray_col);                          // 0

  // Anti-parallel rays sharing only their common origin → touch
  auto r1 = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D( 1, 0));
  auto r2 = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D(-1, 0));
  GEOMPP_LOG(INFO) << r1.Touches(r2);                                 // 1
  GEOMPP_LOG(INFO) << r1.Touch(r2)->ToWkt();                          // POINT (0 0)

  // Anti-parallel rays overlapping → Touch returns nullopt
  auto r3 = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D(-1, 0));
  GEOMPP_LOG(INFO) << r1.Touches(r3);                                 // 0

  // Segment T-junction: endpoint of b lies on a (non-collinear)
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(5, 0));
  auto b = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(3, 3));
  GEOMPP_LOG(INFO) << a.Touches(b);                                   // 1
  GEOMPP_LOG(INFO) << a.Touch(b)->ToWkt();                            // POINT (3 0)

  // Collinear segments sharing exactly one endpoint → touch
  auto c = g::LineSegment2D::Make(g::Point2D(5, 0), g::Point2D(8, 0));
  GEOMPP_LOG(INFO) << a.Touches(c);                                   // 1
  GEOMPP_LOG(INFO) << a.Touch(c)->ToWkt();                            // POINT (5 0)

  // Overlapping collinear segments → Touch returns nullopt
  auto d = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(7, 0));
  GEOMPP_LOG(INFO) << a.Touches(d);                                   // 0
  GEOMPP_LOG(INFO) << (a.Touch(d) == std::nullopt);                   // 1
  ```

  ```bash
  1
  POINT (3 0)
  0
  1
  POINT (0 0)
  0
  1
  POINT (3 0)
  1
  POINT (5 0)
  0
  1
  ```


</details>

</details>


<details open>
<summary><b> &nbsp; 3. Planes </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 3.1 Coplanarity, winding order, and polygon with holes </b></summary>

```cpp
  g::DECIMAL_PRECISION = g::DP_THREE;

  // Four points on the XY plane vs. a set that spans 3D space
  std::vector<g::Point3D> flat = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}};
  std::vector<g::Point3D> skew = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}};

  GEOMPP_LOG(INFO) << "flat coplanar: " << g::are_coplanar(flat);  // 1
  GEOMPP_LOG(INFO) << "skew coplanar: " << g::are_coplanar(skew);  // 0

  // Which world-axis plane is closest to the cloud?
  auto plane = g::closest_world_plane_to(flat);
  GEOMPP_LOG(INFO) << "closest plane normal: " << plane.normal().ToWkt();  // (0, 0, 1)

  // Winding check
  std::vector<g::Point3D> ring = {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}};
  GEOMPP_LOG(INFO) << "ring CCW: " << g::are_ccw(ring);  // 1

  // Polygon3D with a rectangular hole (outer CCW, hole CW)
  std::vector<g::Point3D> outer = {{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  std::vector<g::Point3D> hole  = {{1,3,0}, {3,3,0}, {3,1,0}, {1,1,0}};
  auto poly = g::Polygon3D::Make(outer, {hole});
  GEOMPP_LOG(INFO) << poly.ToWkt();
  ```

  will print out

  ```bash
  I20260403] flat coplanar: 1
  I20260403] skew coplanar: 0
  I20260403] closest plane normal: VECTOR (0 0 1)
  I20260403] ring CCW: 1
  I20260403] POLYGON ((0 0 0, 4 0 0, 4 4 0, 0 4 0, 0 0 0), (1 3 0, 3 3 0, 3 1 0, 1 1 0, 1 3 0))
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 3.2 Projecting points onto a plane </b></summary>

  `Plane::ProjectOnto(p)` returns the perpendicular projection in 3D world coordinates.
  `Plane::ProjectInto(p)` maps the same projected point into the plane's local 2D frame.
  `Plane::Evaluate(p2d)` is the inverse — local 2D coordinates back to world 3D.

  ```cpp
  #include "plane.hpp"

  namespace g = geompp;

  // XY plane: normal (0, 0, 1), origin (0, 0, 0)
  auto pl = g::Plane::XY();
  g::Point3D p(3.0, 4.0, 7.0);

  // perpendicular projection onto the plane (world 3D coords)
  auto on   = pl.ProjectOnto(p);    // Point3D(3, 4, 0)
  // same projection expressed in the plane's local 2D frame
  auto into = pl.ProjectInto(p);    // Point2D(3, 4)
  // inverse — local 2D frame coords back to world 3D
  auto back = pl.Evaluate(into);    // Point3D(3, 4, 0)

  GEOMPP_LOG(INFO) << "on_plane: " << on.ToWkt();
  GEOMPP_LOG(INFO) << "in_plane: " << into.ToWkt();
  GEOMPP_LOG(INFO) << "back:     " << back.ToWkt();
  ```

  ```bash
  on_plane: POINT (3 4 0)
  in_plane: POINT (3 4)
  back:     POINT (3 4 0)
  ```


</details>


<details closed>
<summary><b> &nbsp; &nbsp; 3.3 Planar vs non-planar Polyline3D </b></summary>

  `Polyline3D::IsPlanar()` checks whether all knots lie in a common plane. Only planar polylines support
  `IsSimple()`, `IsConvex()`, `ConvexHull()`, and `ToPolygon()` — call `IsPlanar()` first.

  `ConvexHull()` returns a `Polyline3D` (an open path). Call `ToPolygon()` on it to close the boundary into a `Polygon3D` with area.

  ```cpp
  #include "polyline3d.hpp"
  #include "calc_utils3d.hpp"

  namespace g = geompp;

  // Planar star-like path in the XY plane
  auto planar = g::Polyline3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(2, 2, 0),
      g::Point3D(4, 4, 0), g::Point3D(0, 4, 0),
  });

  GEOMPP_LOG(INFO) << "planar: " << planar.IsPlanar();    // true

  auto hull    = planar.ConvexHull();   // Polyline3D — open hull
  auto polygon = hull.ToPolygon();      // Polygon3D  — closed region with area

  GEOMPP_LOG(INFO) << "hull knots:    " << hull.Size();
  GEOMPP_LOG(INFO) << "polygon area:  " << polygon.Area();

  // Non-planar path: each point rises out of the XY plane
  auto rising = g::Polyline3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0),
      g::Point3D(1, 1, 1), g::Point3D(0, 1, 2),
  });

  GEOMPP_LOG(INFO) << "planar: " << rising.IsPlanar();    // false

  // Dominant direction of the non-planar path via PCA
  std::vector<g::Point3D> pts;
  for (int i = 0; i < rising.Size(); ++i) { pts.push_back(rising[i]); }
  auto dir = g::principal_direction(pts);
  GEOMPP_LOG(INFO) << "dominant direction: " << dir;
  ```

  will print out

  ```bash
  planar: 1
  hull knots:    4
  polygon area:  16
  planar: 0
  dominant direction: VECTOR (...)
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 3.4 View2D — streaming 3D points to 2D </b></summary>

  `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters without allocating an intermediate
  `Point2D` container. Axis-aligned views (`XY`, `YZ`, `ZX`) are the fastest path — just a direct
  coordinate read. `OnPlane` computes dot products against the plane's local axes.

  ```cpp
  #include "view2d.hpp"
  #include "plane.hpp"

  namespace g = geompp;

  // Axis-aligned views (fast path — single coordinate read)
  auto v_xy = g::View2D::XY();   // x→x, y→y (drops z)
  auto v_yz = g::View2D::YZ();   // y→x, z→y (drops x)
  auto v_zx = g::View2D::ZX();   // z→x, x→y (drops y)

  // Custom view onto any plane
  auto plane    = g::Plane::FromOriginAndNormal({0, 0, 5}, {0, 0, 1});
  auto v_custom = g::View2D::OnPlane(plane);

  std::vector<g::Point3D> pts3d = {{1, 2, 5}, {3, 4, 5}, {5, 6, 5}};

  // Stream 3D points to 2D without allocating a Point2D container
  for (auto const& p : pts3d) {
      double u = v_xy.x(p);   // 1.0 / 3.0 / 5.0
      double w = v_xy.y(p);   // 2.0 / 4.0 / 6.0
      GEOMPP_LOG(INFO) << "(" << u << ", " << w << ")";
  }
  ```

  ```bash
  (1, 2)
  (3, 4)
  (5, 6)
  ```

</details>

</details>


<details closed>
<summary><b> &nbsp; 4. PCA on a 3D point cloud </b></summary>

  `principal_axes(points)` runs PCA (Jacobi eigen decomposition on the 3×3 covariance matrix) and returns
  a `CoordinateFrame` — three orthonormal axes sorted by variance: `X` is the direction of most spread,
  `Y` the secondary, and `Z` the best-fit plane normal (least variance).

  ```cpp
  #include "calc_utils3d.hpp"

  namespace g = geompp;

  // 8 points flat in the XY plane, elongated along X
  std::vector<g::Point3D> cloud = {
      {0, 0,   0}, {1, 0,   0}, {2, 0,   0}, {3, 0,   0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0},
  };

  auto frame = g::principal_axes(cloud);

  GEOMPP_LOG(INFO) << "X (primary):    " << frame.X;   // ≈ (1, 0, 0)
  GEOMPP_LOG(INFO) << "Y (secondary):  " << frame.Y;   // ≈ (0, 1, 0)
  GEOMPP_LOG(INFO) << "Z (normal):     " << frame.Z;   // ≈ (0, 0, 1)

  // Convenience wrappers
  auto normal    = g::principal_normal(cloud);     // == frame.Z
  auto direction = g::principal_direction(cloud);  // == frame.X
  ```

  will print out

  ```bash
  X (primary):    VECTOR (1 0 0)
  Y (secondary):  VECTOR (0 1 0)
  Z (normal):     VECTOR (0 0 1)
  ```

</details>


<details open>
<summary><b> &nbsp; 5. Bounding containers </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 5.1 Simple containers for quick rejection </b></summary>

  `BBox3D` gives the tight axis-aligned box; `BBall3D` (Ritter 1990) gives an approximate
  minimum enclosing sphere — both accept any cloud of points.

  ```cpp
  #include "bbox3d.hpp"
  #include "bball3d.hpp"
  #include "polyline3d.hpp"

  namespace g = geompp;

  g::DECIMAL_PRECISION = g::DP_THREE;

  // Vertices of a rough L-shaped structure
  std::vector<g::Point3D> verts = {
      {0, 0, 0}, {6, 0, 0}, {6, 2, 0}, {2, 2, 0}, {2, 4, 0}, {0, 4, 0},
      {0, 0, 3}, {6, 0, 3}, {6, 2, 3}, {2, 2, 3}, {2, 4, 3}, {0, 4, 3},
  };

  // Axis-aligned bounding box (construct from a polyline spanning all verts)
  auto box = g::BBox3D(g::Polyline3D::Make(verts));
  GEOMPP_LOG(INFO) << "box min: " << box.min().ToWkt();  // POINT (0 0 0)
  GEOMPP_LOG(INFO) << "box max: " << box.max().ToWkt();  // POINT (6 4 3)
  GEOMPP_LOG(INFO) << "box contains (3 1 1): " << box.Contains(g::Point3D(3, 1, 1));  // 1
  GEOMPP_LOG(INFO) << "box contains (7 1 1): " << box.Contains(g::Point3D(7, 1, 1));  // 0

  // Bounding ball — one constructor takes the point cloud directly
  auto ball = g::BBall3D(verts);
  GEOMPP_LOG(INFO) << "ball center: " << ball.center().ToWkt();
  GEOMPP_LOG(INFO) << "ball radius: " << ball.radius();
  GEOMPP_LOG(INFO) << "ball contains (3 1 1): " << ball.Contains(g::Point3D(3, 1, 1));  // 1

  // All original vertices must be inside the ball
  for (auto const& p : verts)
      GEOMPP_LOG(INFO) << p.ToWkt() << " -> " << ball.Contains(p);  // all 1
  ```

  will print out

  ```bash
  box min: POINT (0 0 0)
  box max: POINT (6 4 3)
  box contains (3 1 1): 1
  box contains (7 1 1): 0
  ball center: POINT (3 ...)
  ball radius: ...
  ball contains (3 1 1): 1
  ```

</details>

<details open>
<summary><b> &nbsp; &nbsp; 5.2 Convex hull </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.1 Convex hull of a point cloud </b></summary>

  `convex_hull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

  ```cpp
  #include "point2d.hpp"

  namespace g = geompp;

  g::DECIMAL_PRECISION = g::DP_THREE;

  // An asymmetric 5-pointed star: 5 outer tips + 5 inner concave vertices.
  // The convex hull should be exactly the 5 outer tips.
  std::vector<g::Point2D> star = {
      g::Point2D( 0,  5), g::Point2D( 4,  2),
      g::Point2D( 3, -3), g::Point2D(-2, -4), g::Point2D(-3,  1),
      g::Point2D( 2,  1), g::Point2D( 2, -1),
      g::Point2D( 0, -1), g::Point2D(-1, -1), g::Point2D(-1,  2),
  };

  auto hull = g::convex_hull(star);  // Andrew's monotone chain

  GEOMPP_LOG(INFO) << "hull has " << hull.size() << " vertices:";
  for (auto const& p : hull)
      GEOMPP_LOG(INFO) << "  " << p.ToWkt();
  ```

  ```bash
  hull has 5 vertices:
    POINT (3 -3)
    POINT (4 2)
    POINT (0 5)
    POINT (-3 1)
    POINT (-2 -4)
  ```

  For 3D point clouds, `convex_hull(points)` also works — points do **not** need to be perfectly
  coplanar. When no explicit normal is provided, the best-fit plane is estimated via PCA
  (Jacobi eigendecomposition), and the hull is computed on the projection onto that plane.
  You can also pass an explicit normal if known: `convex_hull(points, normal)`.

  ```cpp
  #include "point3d.hpp"

  namespace g = geompp;

  // Nearly-coplanar cloud with small Z jitter
  std::vector<g::Point3D> cloud = {
      g::Point3D(0, 0, 0.1), g::Point3D(4, 0, -0.1),
      g::Point3D(4, 4, 0.05), g::Point3D(0, 4, -0.05),
      g::Point3D(2, 2, 0.02),  // interior
  };

  auto hull = g::convex_hull(cloud);  // PCA detects near-XY plane, projects, computes hull
  GEOMPP_LOG(INFO) << "3D hull has " << hull.size() << " vertices";  // 4 — interior point excluded
  ```

  (CCW order, starting from the lexicographically smallest point)

</details>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.2 Convex hull of a polygon </b></summary>

  `Polygon2D` and `Polygon3D` expose a `ConvexHull()` method that wraps the free function:

  ```cpp
  #include "polygon3d.hpp"

  namespace g = geompp;

  // 3D star polygon (10 vertices, coplanar, CCW)
  auto star = g::Polygon3D::Make({
      g::Point3D( 0,  5, 0), g::Point3D( 2,  1, 0),
      g::Point3D( 4,  2, 0), g::Point3D( 2, -1, 0),
      g::Point3D( 3, -3, 0), g::Point3D( 0, -1, 0),
      g::Point3D(-2, -4, 0), g::Point3D(-1, -1, 0),
      g::Point3D(-3,  1, 0), g::Point3D(-1,  2, 0),
  });

  auto hull = star.ConvexHull();   // Polygon3D — 5-vertex pentagon

  GEOMPP_LOG(INFO) << "star is convex: " << star.IsConvex();  // 0 — star has concavities
  GEOMPP_LOG(INFO) << "hull is convex: " << hull.IsConvex();  // 1

  GEOMPP_LOG(INFO) << "hull has " << hull.Size() << " vertices:";
  for (int i = 0; i < (int)hull.Size(); ++i)
      GEOMPP_LOG(INFO) << "  " << hull[i].ToWkt();
  ```

  ```bash
  star is convex: 0
  hull is convex: 1
  hull has 5 vertices:
    POINT (3 -3 0)
    POINT (4 2 0)
    POINT (0 5 0)
    POINT (-3 1 0)
    POINT (-2 -4 0)
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.3 Convex hull of a simple polyline </b></summary>

  `Polyline2D::ConvexHull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `IsSimple()` first.

  ```cpp
  #include "polyline2d.hpp"

  namespace g = geompp;

  // Simple concave path: outer corners with an inner dip at (2,1)
  auto path = g::Polyline2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4),
      g::Point2D(2, 1), g::Point2D(0, 4),
  });

  if (path.IsSimple()) {
      auto hull = path.ConvexHull();  // Polygon2D — 4-vertex rectangle
      GEOMPP_LOG(INFO) << "hull has " << hull.Size() << " vertices";
  }
  ```

  ```bash
  hull has 4 vertices
  ```

</details>

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 5.3 Oriented Minimum Bounding Rectangle </b></summary>

  `BRect2D` computes the **tightest** axis-aligned-to-input rectangle that encloses a point cloud.
  It is defined by a center point, two orthogonal unit axes (`axis_u`, `axis_v`), and two half-lengths
  (`half_len_u`, `half_len_v`).

  **Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
  1. Compute the convex hull of the input cloud (Andrew's monotone chain, O(n log n)).
  2. For each hull edge, project all hull vertices onto the edge direction and its perpendicular.
  3. The rectangle aligned with that edge has width = max − min along the edge and height = max − min along the perpendicular.
  4. Track the edge orientation that minimises area; the center is the midpoint of the extents.

  This guarantees a minimum-area rectangle with one side flush with a hull edge.

  ```cpp
  #include "brect2d.hpp"

  namespace g = geompp;

  g::DECIMAL_PRECISION = g::DP_THREE;

  // An asymmetric pentagon
  std::vector<g::Point2D> pts = {
      g::Point2D(0, 0), g::Point2D(4, 0),
      g::Point2D(5, 2), g::Point2D(2, 4),
      g::Point2D(-1, 2),
  };

  auto r = g::BRect2D(pts);

  GEOMPP_LOG(INFO) << "center:     " << r.center().ToWkt();
  GEOMPP_LOG(INFO) << "axis_u:     " << r.axis_u().ToWkt();
  GEOMPP_LOG(INFO) << "half_u:     " << r.half_len_u();
  GEOMPP_LOG(INFO) << "half_v:     " << r.half_len_v();
  GEOMPP_LOG(INFO) << "area:       " << r.area();
  GEOMPP_LOG(INFO) << "contains (2 2): " << r.Contains(g::Point2D(2, 2));  // 1
  GEOMPP_LOG(INFO) << "contains (9 0): " << r.Contains(g::Point2D(9, 0));  // 0

  auto corners = r.Corners();
  GEOMPP_LOG(INFO) << "corners (" << corners.size() << "):";
  for (auto const& c : corners)
      GEOMPP_LOG(INFO) << "  " << c.ToWkt();
  ```

  `Corners()` returns the four corners in CCW order; each is guaranteed to be `Contains()`-true.

  The `Contains()` test projects the query point onto the local axes and checks both projections against the half-lengths — O(1) per query.

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

  
</details>

</details>



</details>

</details>



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
  | Backlog | adjacency, polygon clipping, definition of "non-planar polygon" or mesh, triangulation/polygonization, graphic geometry viewer |


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
