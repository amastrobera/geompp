# How to use the library

Here is an example of code. You can also look at the directories of [c++ tests](./geompp_tests/), [python tests](./geompp_python/tests), or [csharp tests](./geompp_csharp/tests), to see more code.

A quick list of code examples per topic is provided here.


<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 1.1 Create geometries from classes</b></summary>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # This will be the precision used by all functions for this run of the program.
  g.set_decimal_precision(g.DP_THREE)

  # two line segments intersecting at (0,0,1)
  s1 = g.LineSegment3D.make(g.Point3D(1, 0, 0), g.Point3D(-1, 0, 2))
  s2 = g.LineSegment3D.make(g.Point3D(0, 1, 0), g.Point3D(0, -1, 2))

  print(f"s1 = {s1.to_wkt()}")
  print(f"s2 = {s2.to_wkt()}")

  if s1.intersects(s2):
      result = s1.intersection(s2)
      if result is not None:
          print(f"intersection found: {result.to_wkt()}")
          result.to_file("intersection.wkt")
          print("intersection written to intersection.wkt")
  else:
      print("no intersection found")
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // This will be the precision used by all functions for this run of the program.
  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // two line segments intersecting at (0,0,1)
  var s1 = G.LineSegment3D.Make(new G.Point3D(1, 0, 0), new G.Point3D(-1, 0, 2));
  var s2 = G.LineSegment3D.Make(new G.Point3D(0, 1, 0), new G.Point3D(0, -1, 2));

  Console.WriteLine($"s1 = {s1.ToWkt()}");
  Console.WriteLine($"s2 = {s2.ToWkt()}");

  if (s1.Intersects(s2)) {
      var result = s1.Intersection(s2);
      if (result is G.Point3D p) {
          Console.WriteLine($"intersection found: {p.ToWkt()}");
          p.ToFile("intersection.wkt");
          Console.WriteLine("intersection written to intersection.wkt");
      }
  } else {
      Console.WriteLine("no intersection found");
  }
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 1.2 Create geometries from text</b></summary>

  **WKT** (Well-Known Text) is the OGC's plain-text format for geometry — `TAG (coordinates)` — used
  across GIS tools and databases (PostGIS, GDAL, QGIS, ...). Every geompp primitive round-trips through
  it via `ToWkt()` / `FromWkt()`, and through the same text via file I/O (`ToFile()` / `FromFile()`,
  see section 1.3). `RAY` and `TRIANGLE` aren't part of the official OGC spec — they're extensions this
  library adds, following the same `TAG (coordinates)` grammar as the standard ones.

  | Primitive | WKT tag | 2D example | Notes |
  |---|---|---|---|
  | `Point2D` | `POINT` | `POINT (1 2)` | a single coordinate pair |
  | `Line2D` (infinite) | `LINE` | `LINE (0 0, 1 0)` | two points the infinite line passes through |
  | `Ray2D` | `RAY` | `RAY (0 0, 1 0)` | origin point, then a **direction vector** — not a second point |
  | `LineSegment2D` | `LINESTRING` | `LINESTRING (0 0, 5 0)` | always exactly 2 points |
  | `Polyline2D` | `LINESTRING` | `LINESTRING (0 0, 2 3, 5 0, 8 4)` | same tag as `LineSegment2D`, any number of points ≥ 2 |
  | `Triangle2D` | `TRIANGLE` | `TRIANGLE (0 0, 4 0, 2 3)` | exactly 3 points |
  | `Polygon2D` | `POLYGON` | `POLYGON ((0 0, 4 0, 4 4, 0 4, 0 0))` | outer ring closes by repeating its first point (must be CCW); holes append as extra, CW rings: `POLYGON ((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1))` |

  For 3D, every point just gets one more coordinate — same tags, same structure. One example,
  `LineSegment3D`: `LINESTRING (1 0 0, -1 0 2)`.

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # two line segments intersecting at (0,0,1)
  s1 = g.LineSegment3D.from_wkt("LINESTRING (1  0  0, -1 0 2)")
  s2 = g.LineSegment3D.from_wkt("LINESTRING (0 1  0, 0 -1 2)")

  print(f"s1 = {s1.to_wkt()}")
  print(f"s2 = {s2.to_wkt()}")

  if s1.intersects(s2):
      result = s1.intersection(s2)
      if result is not None:
          print(f"intersection found: {result.to_wkt()}")
          result.to_file("intersection.wkt")
          print("intersection written to intersection.wkt")
  else:
      print("no intersection found")
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // two line segments intersecting at (0,0,1)
  var s1 = G.LineSegment3D.FromWkt("LINESTRING (1  0  0, -1 0 2)");
  var s2 = G.LineSegment3D.FromWkt("LINESTRING (0 1  0, 0 -1 2)");

  Console.WriteLine($"s1 = {s1.ToWkt()}");
  Console.WriteLine($"s2 = {s2.ToWkt()}");

  if (s1.Intersects(s2)) {
      var result = s1.Intersection(s2);
      if (result is G.Point3D p) {
          Console.WriteLine($"intersection found: {p.ToWkt()}");
          p.ToFile("intersection.wkt");
          Console.WriteLine("intersection written to intersection.wkt");
      }
  } else {
      Console.WriteLine("no intersection found");
  }
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 1.3 Import geometries from a file</b></summary>

  An `.lsv` file is a plain-text list of WKT geometries, one per line — e.g. `sample_geometries.lsv`:

  ```
  POINT (1 2 3)
  POINT (4 5 6)
  LINESTRING (0 0 0, 1 1 1)
  LINESTRING (2 0 0, 2 3 4)
  LINE (0 0 0, 1 0 0)
  RAY (0 0 0, 0 1 0)
  ```

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  lsv_path = "sample_geometries.lsv"
  #   POINT (1 2 3)
  #   POINT (4 5 6)
  #   LINESTRING (0 0 0, 1 1 1)
  #   LINESTRING (2 0 0, 2 3 4)
  #   LINE (0 0 0, 1 0 0)
  #   RAY (0 0 0, 0 1 0)

  g.set_decimal_precision(g.DP_THREE)

  parser = g.WktParser.open(lsv_path)
  if not parser.has_next():
      print(f"no geometries found in file {lsv_path}")
  else:
      while parser.has_next():
          entry = parser.next()
          if entry is None:
              print("skipped unrecognised line")
              continue
          print(g.WktParser.to_wkt(entry))
  ```

  will print out exactly the list of geometries above.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```
  POINT (1 2 3)
  POINT (4 5 6)
  LINESTRING (0 0 0, 1 1 1)
  LINESTRING (2 0 0, 2 3 4)
  LINE (0 0 0, 1 0 0)
  RAY (0 0 0, 0 1 0)
  ```

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  var parser = G.WktParser.Open("sample_geometries.lsv");

  if (!parser.HasNext()) {
      Console.WriteLine("no geometries found");
      return;
  }

  while (parser.HasNext()) {
      var item = parser.Next();
      if (item == null) {
          Console.WriteLine("skipped unrecognised line");
          continue;
      }
      Console.WriteLine(G.WktParser.ToWkt(item));
  }
  ```

  will print out exactly the list of geometries above.

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 2. Geometry Operations</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 2.1 Interpolation </b></summary>

  Three related operations move between a normalized parameter `t` and a physical point:

  - **`lerp(P0, P1, t)`** — free function, plain linear interpolation: `P0 + t * (P1 - P0)`. `t = 0`
    gives `P0`, `t = 1` gives `P1`. **Not clamped** — `t` outside `[0, 1]` extrapolates past either point.

  <p align="center">
    <img src="./images/img_2-1-lerp.png" width="420" alt="A dashed segment between P0 and P1 with the lerp(P0, P1, 0.25) point marked">
  </p>

  - **`Interpolate(t)`** — the point at parameter `t` along a shape, built on `lerp` internally.
    - `LineSegment2D/3D::Interpolate(t)` **clamps** `t` to `[0, 1]` (below `0` returns `First`, above
      `1` returns `Last`), so it never extrapolates the way the free `lerp()` does.
    - `Polyline2D/3D::Interpolate(t)` uses a single **arc-length** `t ∈ [0, 1]` over the *whole*
      polyline, not a per-segment or vertex-index fraction — `t = 0.5` is the point halfway along the
      total length, whichever segment that lands in. `t` outside `[0, 1]` throws.

  <p align="center">
    <img src="./images/img_2-1-interpolate.png" width="420" alt="A 4-point polyline with its Interpolate(0.5) point marked, and the traveled half of the arc length highlighted">
  </p>

  - **`Location(point)`** — the inverse of `Interpolate`: given a point already on the shape, returns
    its parameter `t`. For `Polyline2D/3D`, that's the same arc-length fraction `Interpolate` consumes.
    The point must actually lie on the shape (within the current `DECIMAL_PRECISION` tolerance) —
    `Location` returns `+infinity` for an off-shape point rather than projecting it to the nearest one.

  <p align="center">
    <img src="./images/img_2-1-location.png" width="420" alt="The same polyline with a query point resolved back to its arc-length parameter via Location()">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "point2d.hpp"
  #include "line_segment2d.hpp"
  #include "polyline2d.hpp"

  namespace g = geompp;

  g::DECIMAL_PRECISION = g::DP_THREE;

  // lerp() — plain linear interpolation, not clamped
  auto p0 = g::Point2D(1, 1);
  auto p1 = g::Point2D(9, 5);
  auto mid = g::lerp(p0, p1, 0.25);
  GEOMPP_LOG(INFO) << "lerp(0.25) = " << mid.ToWkt();

  // LineSegment2D::Interpolate — clamped to [0, 1]
  auto seg = g::LineSegment2D::Make(p0, p1);
  GEOMPP_LOG(INFO) << "seg.Interpolate(0.25) = " << seg.Interpolate(0.25).ToWkt();
  GEOMPP_LOG(INFO) << "seg.Interpolate(1.5)  = " << seg.Interpolate(1.5).ToWkt();  // clamped to P1

  // Polyline2D::Interpolate / Location — a single arc-length t over the WHOLE polyline
  auto pl = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(0, 4), g::Point2D(4, 4), g::Point2D(4, 0)});
  auto at_half = pl.Interpolate(0.5);
  GEOMPP_LOG(INFO) << "pl.Interpolate(0.5) = " << at_half.ToWkt();
  GEOMPP_LOG(INFO) << "pl.Location(2, 4)   = " << pl.Location(g::Point2D(2, 4));
  ```

  will print out

  ```
  I20260403] lerp(0.25) = POINT (3 2)
  I20260403] seg.Interpolate(0.25) = POINT (3 2)
  I20260403] seg.Interpolate(1.5)  = POINT (9 5)
  I20260403] pl.Interpolate(0.5) = POINT (2 4)
  I20260403] pl.Location(2, 4)   = 0.5
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # lerp() — plain linear interpolation, not clamped
  p0 = g.Point2D(1, 1)
  p1 = g.Point2D(9, 5)
  mid = g.lerp(p0, p1, 0.25)
  print(f"lerp(0.25) = {mid.to_wkt()}")

  # LineSegment2D.interpolate — clamped to [0, 1]
  seg = g.LineSegment2D.make(p0, p1)
  print(f"seg.interpolate(0.25) = {seg.interpolate(0.25).to_wkt()}")
  print(f"seg.interpolate(1.5)  = {seg.interpolate(1.5).to_wkt()}")  # clamped to P1

  # Polyline2D.interpolate / location — a single arc-length t over the WHOLE polyline
  pl = g.Polyline2D.make([g.Point2D(0, 0), g.Point2D(0, 4), g.Point2D(4, 4), g.Point2D(4, 0)])
  at_half = pl.interpolate(0.5)
  print(f"pl.interpolate(0.5) = {at_half.to_wkt()}")
  print(f"pl.location(2, 4)   = {pl.location(g.Point2D(2, 4))}")
  ```

  will print out

  ```
  lerp(0.25) = POINT (3 2)
  seg.interpolate(0.25) = POINT (3 2)
  seg.interpolate(1.5)  = POINT (9 5)
  pl.interpolate(0.5) = POINT (2 4)
  pl.location(2, 4)   = 0.5
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // Lerp — plain linear interpolation, not clamped
  var p0 = new G.Point2D(1, 1);
  var p1 = new G.Point2D(9, 5);
  var mid = G.GeomUtil.Lerp(p0, p1, 0.25);
  Console.WriteLine($"Lerp(0.25) = {mid.ToWkt()}");

  // LineSegment2D.Interpolate — clamped to [0, 1]
  var seg = G.LineSegment2D.Make(p0, p1);
  Console.WriteLine($"seg.Interpolate(0.25) = {seg.Interpolate(0.25).ToWkt()}");
  Console.WriteLine($"seg.Interpolate(1.5)  = {seg.Interpolate(1.5).ToWkt()}"); // clamped to P1

  // Polyline2D.Interpolate / Location — a single arc-length t over the WHOLE polyline
  var pl = G.Polyline2D.Make(new[] {
      new G.Point2D(0, 0), new G.Point2D(0, 4), new G.Point2D(4, 4), new G.Point2D(4, 0)
  });
  var atHalf = pl.Interpolate(0.5);
  Console.WriteLine($"pl.Interpolate(0.5) = {atHalf.ToWkt()}");
  Console.WriteLine($"pl.Location(2, 4)   = {pl.Location(new G.Point2D(2, 4))}");
  ```

  will print out

  ```
  Lerp(0.25) = POINT (3 2)
  seg.Interpolate(0.25) = POINT (3 2)
  seg.Interpolate(1.5)  = POINT (9 5)
  pl.Interpolate(0.5) = POINT (2 4)
  pl.Location(2, 4)   = 0.5
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 2.2 Containment </b></summary>

`Triangle3D::Contains(p)` and `Polygon2D/3D::Contains(p)` test whether a point lies inside a shape
  using barycentric coordinates and the winding number, respectively.
  `LineSegment::Contains(p)` checks whether a point lies on the segment;
  `Location(p)` / `Interpolate(t)` map a point on it to/from a `t ∈ [0, 1]` parameter — see section 2.1
  "Interpolation" for the full story, including the `Polyline` equivalents.

  <p align="center">
    <img src="./images/img_2-2-containment.png" width="420" alt="A point inside a polygon and a point outside it, tested with Contains()">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # Triangle3D — barycentric containment
  tri = g.Triangle3D.make(
      g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(0, 4, 0))
  print(tri.contains(g.Point3D(1, 1, 0)))   # True  — inside
  print(tri.contains(g.Point3D(3, 3, 0)))   # False — outside

  # Polygon2D — winding-number containment
  poly = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(6, 0),
      g.Point2D(6, 4), g.Point2D(0, 4),
  ])
  print(poly.contains(g.Point2D(3, 2)))     # True
  print(poly.contains(g.Point2D(7, 2)))     # False

  # LineSegment2D — containment, location, and interpolation
  seg = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(4, 0))
  print(seg.contains(g.Point2D(2, 0)))      # True
  print(seg.contains(g.Point2D(2, 1)))      # False

  t   = seg.location(g.Point2D(2, 0))      # 0.5
  mid = seg.interpolate(0.5)               # Point2D(2, 0)
  print(f"t = {t:.1f}")
  print(f"mid = {mid.to_wkt()}")
  ```

  ```
  True
  False
  True
  False
  True
  False
  t = 0.5
  mid = POINT (2 0)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // Triangle3D — barycentric containment
  var tri = G.Triangle3D.Make(new G.Point3D(0, 0, 0), new G.Point3D(4, 0, 0), new G.Point3D(0, 4, 0));
  Console.WriteLine(tri.Contains(new G.Point3D(1, 1, 0)));   // True  — inside
  Console.WriteLine(tri.Contains(new G.Point3D(3, 3, 0)));   // False — outside

  // Polygon2D — winding-number containment
  var poly = G.Polygon2D.Make(new G.Point2D[] {
      new(0, 0), new(6, 0), new(6, 4), new(0, 4),
  });
  Console.WriteLine(poly.Contains(new G.Point2D(3, 2)));     // True
  Console.WriteLine(poly.Contains(new G.Point2D(7, 2)));     // False

  // LineSegment2D — containment, location, and interpolation
  var seg = G.LineSegment2D.Make(new G.Point2D(0, 0), new G.Point2D(4, 0));
  Console.WriteLine(seg.Contains(new G.Point2D(2, 0)));      // True
  Console.WriteLine(seg.Contains(new G.Point2D(2, 1)));      // False

  double t   = seg.Location(new G.Point2D(2, 0));            // 0.5
  var    mid = seg.Interpolate(0.5);                         // Point2D(2, 0)
  Console.WriteLine($"t = {t:F1}");
  Console.WriteLine($"mid = {mid.ToWkt()}");
  ```

  ```
  True
  False
  True
  False
  True
  False
  t = 0.5
  mid = POINT (2 0)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 2.3 Intersection </b></summary>

  Every 2D primitive (`Line2D`, `Ray2D`, `LineSegment2D`, `Triangle2D`, `Polygon2D`) can intersect any
  other 2D primitive, and the same holds in 3D across `Line3D`, `Ray3D`, `LineSegment3D`, `Triangle3D`,
  and `Plane`. Results are `std::optional<std::variant<...>>` for point-or-segment outcomes, or
  `std::optional<std::vector<LineSegment2D>>` when polygon clipping can produce multiple chords.
  `find_intersections(segments)` (Bentley–Ottmann) reports all crossing points across an arbitrary set
  of 2D segments, sorted left-to-right.

  Methods return `object` (null on miss) in C#; use pattern matching to extract the result type.

  <p align="center">
    <img src="./images/img_2-3-intersection.png" width="420" alt="A vertical line crossing a C-shaped polygon, producing two chord segments">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # LineSegment2D → Polygon2D: segment is clipped to the polygon interior
  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(4, 4), g.Point2D(0, 4),
  ])
  seg = g.LineSegment2D.make(g.Point2D(-1, 2), g.Point2D(5, 2))
  seg_chords = square.intersection(seg)
  for c in (seg_chords or []):
      print(c.to_wkt())   # LINESTRING (0 2, 4 2)

  # Ray2D → Polygon2D: ray entering from outside, clipped at the exit boundary
  ray2d = g.Ray2D.make(g.Point2D(-1, 2), g.Vector2D(1, 0))
  ray_chords = square.intersection(ray2d)
  for c in (ray_chords or []):
      print(c.to_wkt())   # LINESTRING (0 2, 4 2)

  # Line2D → concave Polygon2D: vertical line through a C-shape produces two chords
  cshape = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 1),
      g.Point2D(1, 1), g.Point2D(1, 3), g.Point2D(4, 3),
      g.Point2D(4, 4), g.Point2D(0, 4),
  ])
  line2d     = g.Line2D.make(g.Point2D(2, 0), g.Point2D(2, 1))
  lin_chords = cshape.intersection(line2d)
  for c in (lin_chords or []):
      print(c.to_wkt())
  # LINESTRING (2 0, 2 1)
  # LINESTRING (2 3, 2 4)

  # Ray3D → Triangle3D
  tri = g.Triangle3D.make(
      g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(0, 4, 0))
  ray = g.Ray3D.make(g.Point3D(1, 1, 3), g.Vector3D(0, 0, -1))
  hit = tri.intersection(ray)
  print(hit.to_wkt() if hit else None)      # POINT (1 1 0)

  # LineSegment2D vs LineSegment2D
  s1    = g.LineSegment2D.make(g.Point2D(0, 1), g.Point2D(4, 1))
  s2    = g.LineSegment2D.make(g.Point2D(2, 0), g.Point2D(2, 4))
  cross = s1.intersection(s2)
  print(cross.to_wkt() if cross else None)  # POINT (2 1)

  # find_intersections — all crossing points (Bentley–Ottmann)
  segs = [
      g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(4, 4)),
      g.LineSegment2D.make(g.Point2D(0, 4), g.Point2D(4, 0)),
      g.LineSegment2D.make(g.Point2D(0, 2), g.Point2D(4, 2)),
  ]
  for p in g.find_intersections(segs):
      print(p.to_wkt())
  ```

  ```
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

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // LineSegment2D → Polygon2D: segment is clipped to the polygon interior
  var square = G.Polygon2D.Make(new G.Point2D[] {
      new(0, 0), new(4, 0), new(4, 4), new(0, 4),
  });
  var seg = G.LineSegment2D.Make(new G.Point2D(-1, 2), new G.Point2D(5, 2));
  if (square.Intersection(seg) is G.LineSegment2D[] segChords)
      foreach (var c in segChords)
          Console.WriteLine(c.ToWkt());   // LINESTRING (0 2, 4 2)

  // Ray2D → Polygon2D: ray entering from outside, clipped at the exit boundary
  var ray2d = G.Ray2D.Make(new G.Point2D(-1, 2), new G.Vector2D(1, 0));
  if (square.Intersection(ray2d) is G.LineSegment2D[] rayChords)
      foreach (var c in rayChords)
          Console.WriteLine(c.ToWkt());   // LINESTRING (0 2, 4 2)

  // Line2D → concave Polygon2D: vertical line through a C-shape produces two chords
  var cshape = G.Polygon2D.Make(new G.Point2D[] {
      new(0, 0), new(4, 0), new(4, 1),
      new(1, 1), new(1, 3), new(4, 3),
      new(4, 4), new(0, 4),
  });
  var line2d = G.Line2D.Make(new G.Point2D(2, 0), new G.Point2D(2, 1));
  if (cshape.Intersection(line2d) is G.LineSegment2D[] linChords)
      foreach (var c in linChords)
          Console.WriteLine(c.ToWkt());
  // LINESTRING (2 0, 2 1)
  // LINESTRING (2 3, 2 4)

  // Ray3D → Triangle3D
  var tri = G.Triangle3D.Make(new G.Point3D(0, 0, 0), new G.Point3D(4, 0, 0), new G.Point3D(0, 4, 0));
  var ray = G.Ray3D.Make(new G.Point3D(1, 1, 3), new G.Vector3D(0, 0, -1));
  var hit = tri.Intersection(ray) as G.Point3D;
  Console.WriteLine(hit?.ToWkt() ?? "null");     // POINT (1 1 0)

  // LineSegment2D vs LineSegment2D
  var s1    = G.LineSegment2D.Make(new G.Point2D(0, 1), new G.Point2D(4, 1));
  var s2    = G.LineSegment2D.Make(new G.Point2D(2, 0), new G.Point2D(2, 4));
  var cross = s1.Intersection(s2) as G.Point2D;
  Console.WriteLine(cross?.ToWkt() ?? "null");   // POINT (2 1)

  // GeomUtil.FindIntersections — all crossing points (Bentley–Ottmann)
  var segs = new System.Collections.Generic.List<G.LineSegment2D> {
      G.LineSegment2D.Make(new G.Point2D(0, 0), new G.Point2D(4, 4)),
      G.LineSegment2D.Make(new G.Point2D(0, 4), new G.Point2D(4, 0)),
      G.LineSegment2D.Make(new G.Point2D(0, 2), new G.Point2D(4, 2)),
  };
  foreach (var p in G.GeomUtil.FindIntersections(segs))
      Console.WriteLine(p.ToWkt());
  ```

  ```
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

   </details>

  </details>

<details open>
<summary><b> &nbsp; &nbsp; &nbsp; 2.3.1 Split a complex polygon </b></summary>

  A **complex polygon** (also called a self-intersecting polygon) is a polygon whose edges cross
  each other. `Simplify()` decomposes it into a list of simple (non-self-intersecting) polygons by
  finding every crossing with a Bentley–Ottmann sweep, splitting the ring at each one, then tracing
  the resulting planar graph face-by-face (half-edge walk). Each returned polygon is guaranteed to satisfy
`IsSimple() == true`. If the input is already simple, `Simplify()` returns a single-element
  vector containing the original polygon. The polygon pieces split out by `Simplify()` are guaranteed 
  to be all CCW winding (in 2D), while they will have their own winding (their own normal) in 3D. 

  <p align="center">
    <img src="./images/img_2-3-1-split.png" width="420" alt="A self-crossing bowtie polygon decomposed by Simplify() into two simple triangles">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # 2D: a "bowtie" — edges B→C and D→A cross at (2,2)
  bowtie = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(1, 3), g.Point2D(3, 3)])
  print("is simple:", bowtie.is_simple())   # False

  parts = bowtie.simplify()
  print(f"{len(parts)} simple polygon(s)")
  for p in parts:
      print(f"  {p.to_wkt()}  area={p.area()}")

  # 3D: same bowtie lifted into the XY plane (z = 0)
  bowtie3d = g.Polygon3D.make([
      g.Point3D(0,0,0), g.Point3D(4,0,0),
      g.Point3D(1,3,0), g.Point3D(3,3,0)])
  parts3d = bowtie3d.simplify()
  print(f"{len(parts3d)} simple 3D polygon(s)")
  ```

  ```
  is simple: False
  2 simple polygon(s)
    POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4.0
    POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1.0
  2 simple 3D polygon(s)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // 2D: a "bowtie" — edges B→C and D→A cross at (2,2)
  var bowtie = G.Polygon2D.Make(new[] {
      new G.Point2D(0, 0), new G.Point2D(4, 0),
      new G.Point2D(1, 3), new G.Point2D(3, 3)
  });
  Console.WriteLine($"is simple: {bowtie.IsSimple()}");  // False

  var parts = bowtie.Simplify();
  Console.WriteLine($"{parts.Length} simple polygon(s)");
  foreach (var p in parts)
      Console.WriteLine($"  {p.ToWkt()}  area={p.Area()}");

  // 3D: same bowtie lifted into the XY plane (z = 0)
  var bowtie3d = G.Polygon3D.Make(new[] {
      new G.Point3D(0,0,0), new G.Point3D(4,0,0),
      new G.Point3D(1,3,0), new G.Point3D(3,3,0)
  });
  var parts3d = bowtie3d.Simplify();
  Console.WriteLine($"{parts3d.Length} simple 3D polygon(s)");
  ```

  ```
  is simple: False
  2 simple polygon(s)
    POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4
    POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1
  2 simple 3D polygon(s)
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 2.4 Overlap </b></summary>

  `Overlaps(other)` returns `true` when two primitives share a 1D region (more than a single point).
  `Overlap(other)` returns the shared geometry, or `std::nullopt` when they do not overlap or
  only touch at a single point.  The return type mirrors the "smaller" of the two primitives:
  a `Line × Line` overlap yields a `Line`; `Ray × Ray` with opposite directions yields a
  `LineSegment`; `Segment × Line` / `Segment × Ray` / `Segment × Segment` always yield a `LineSegment`.
  All six primitive types (`Line2D`, `Ray2D`, `LineSegment2D`, and their 3D counterparts) implement
  `Overlaps` and `Overlap` against all three family members.

  Use C# pattern matching to extract the concrete type.

  <p align="center">
    <img src="./images/img_2-4-overlap.png" width="420" alt="Two overlapping collinear segments and their shared Overlap() result">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # ── Line × Line ─────────────────────────────────────────────────────────────
  x_axis = g.Line2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  x_same = g.Line2D.make(g.Point2D(5, 0), g.Point2D(8, 0))   # same infinite line
  x_off  = g.Line2D.make(g.Point2D(0, 1), g.Vector2D(1, 0))  # parallel, offset

  print(x_axis.overlaps(x_same))  # True
  print(x_axis.overlaps(x_off))   # False

  ov_ll = x_axis.overlap(x_same)  # Line2D or None
  if ov_ll is not None:
      print(ov_ll.to_wkt())        # LINE (0 0, 1 0)

  # ── Ray × Ray (same direction) ─────────────────────────────────────────────
  r1 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  r2 = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(1, 0))  # inside r1

  ov_rr = r1.overlap(r2)  # Ray2D or LineSegment2D or None
  if ov_rr is not None:
      print(ov_rr.to_wkt())  # RAY (3 0, 1 0)

  # ── Ray × Ray (anti-parallel) ──────────────────────────────────────────────
  r3 = g.Ray2D.make(g.Point2D(7, 0), g.Vector2D(-1, 0))  # heads toward r1

  ov_anti = r1.overlap(r3)
  if ov_anti is not None:
      print(ov_anti.to_wkt())
  # LINESTRING (0 0, 7 0)

  # ── Segment × Segment ──────────────────────────────────────────────────────
  a = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(5, 0))
  b = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(8, 0))

  print(a.overlaps(b))  # True
  ov_ss = a.overlap(b)  # LineSegment2D or None
  if ov_ss is not None:
      print(ov_ss.to_wkt())  # LINESTRING (3 0, 5 0)

  # touch at a single endpoint → no overlap
  c = g.LineSegment2D.make(g.Point2D(5, 0), g.Point2D(9, 0))
  print(a.overlaps(c))          # False
  print(a.overlap(c) is None)   # True
  ```

  ```
  True
  False
  LINE (0 0, 1 0)
  RAY (3 0, 1 0)
  LINESTRING (0 0, 7 0)
  True
  LINESTRING (3 0, 5 0)
  False
  True
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;
  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // ── Line × Line ─────────────────────────────────────────────────────────────
  var xAxis = G.Line2D.Make(new G.Point2D(0, 0), new G.Vector2D(1, 0));
  var xSame = G.Line2D.Make(new G.Point2D(5, 0), new G.Point2D(8, 0));   // same infinite line
  var xOff  = G.Line2D.Make(new G.Point2D(0, 1), new G.Vector2D(1, 0));  // parallel, offset

  Console.WriteLine(xAxis.Overlaps(xSame));  // True
  Console.WriteLine(xAxis.Overlaps(xOff));   // False

  if (xAxis.Overlap(xSame) is G.Line2D ovLL)
      Console.WriteLine(ovLL.ToWkt());        // LINE (0 0, 1 0)

  // ── Ray × Ray (same direction) ─────────────────────────────────────────────
  var r1 = G.Ray2D.Make(new G.Point2D(0, 0), new G.Vector2D(1, 0));
  var r2 = G.Ray2D.Make(new G.Point2D(3, 0), new G.Vector2D(1, 0));  // inside r1

  if (xAxis.Overlap(r2) is G.Ray2D ovRR)
      Console.WriteLine(ovRR.ToWkt());        // RAY (3 0, 1 0)

  // ── Ray × Ray (anti-parallel) ──────────────────────────────────────────────
  var r3 = G.Ray2D.Make(new G.Point2D(7, 0), new G.Vector2D(-1, 0));  // heads toward r1

  if (r1.Overlap(r3) is G.LineSegment2D ovAnti)
      Console.WriteLine(ovAnti.ToWkt());
  // LINESTRING (0 0, 7 0)

  // ── Segment × Segment ──────────────────────────────────────────────────────
  var a = G.LineSegment2D.Make(new G.Point2D(0, 0), new G.Point2D(5, 0));
  var b = G.LineSegment2D.Make(new G.Point2D(3, 0), new G.Point2D(8, 0));

  Console.WriteLine(a.Overlaps(b));  // True
  if (a.Overlap(b) is G.LineSegment2D ovSS)
      Console.WriteLine(ovSS.ToWkt());  // LINESTRING (3 0, 5 0)

  // touch at a single endpoint → no overlap
  var c = G.LineSegment2D.Make(new G.Point2D(5, 0), new G.Point2D(9, 0));
  Console.WriteLine(a.Overlaps(c));          // False
  Console.WriteLine(a.Overlap(c) == null);   // True
  ```

  ```
  True
  False
  LINE (0 0, 1 0)
  RAY (3 0, 1 0)
  LINESTRING (0 0, 7 0)
  True
  LINESTRING (3 0, 5 0)
  False
  True
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 2.5 Touch </b></summary>

  `Touches(other)` returns `true` when two primitives share exactly one endpoint-contact point
  (not an interior crossing, not a shared segment). `Touch(other)` returns that contact point,
  or `std::nullopt` when there is no touch.

  <p align="center">
    <img src="./images/img_2-5-touch.png" width="420" alt="A segment T-junction touching another segment at a single contact point">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # Ray origin sits on a line → touch at origin
  line = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))   # x-axis
  ray  = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(0, 1))   # vertical at x=3
  print(line.touches(ray))                    # True
  tp = line.touch(ray)                        # Point2D or None
  print(tp.to_wkt())                          # POINT (3 0)

  # Collinear ray → overlap, not touch
  ray_col = g.Ray2D.make(g.Point2D(1, 0), g.Vector2D(1, 0))
  print(line.touches(ray_col))               # False

  # Anti-parallel rays sharing only their common origin → touch
  r1 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D( 1, 0))
  r2 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(-1, 0))
  print(r1.touches(r2))                      # True
  print(r1.touch(r2).to_wkt())              # POINT (0 0)

  # Anti-parallel rays overlapping → touch returns None
  r3 = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(-1, 0))
  print(r1.touches(r3))                      # False

  # Segment T-junction: endpoint of b lies on a (non-collinear)
  a = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(5, 0))
  b = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(3, 3))
  print(a.touches(b))                        # True
  print(a.touch(b).to_wkt())                # POINT (3 0)

  # Collinear segments sharing exactly one endpoint → touch
  c = g.LineSegment2D.make(g.Point2D(5, 0), g.Point2D(8, 0))
  print(a.touches(c))                        # True
  print(a.touch(c).to_wkt())                # POINT (5 0)

  # Overlapping collinear segments → touch returns None
  d = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(7, 0))
  print(a.touches(d))                        # False
  print(a.touch(d) is None)                 # True
  ```

  ```
  True
  POINT (3 0)
  False
  True
  POINT (0 0)
  False
  True
  POINT (3 0)
  True
  POINT (5 0)
  False
  True
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;
  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // Ray origin sits on a line → touch at origin
  var line = G.Line2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 0));  // x-axis
  var ray  = G.Ray2D.Make(new G.Point2D(3, 0), new G.Vector2D(0, 1)); // vertical at x=3
  Console.WriteLine(line.Touches(ray));            // True
  var tp = line.Touch(ray) as G.Point2D;           // Point2D or null
  Console.WriteLine(tp?.ToWkt());                  // POINT (3 0)

  // Collinear ray → overlap, not touch
  var rayCol = G.Ray2D.Make(new G.Point2D(1, 0), new G.Vector2D(1, 0));
  Console.WriteLine(line.Touches(rayCol));         // False

  // Anti-parallel rays sharing only their common origin → touch
  var r1 = G.Ray2D.Make(new G.Point2D(0, 0), new G.Vector2D( 1, 0));
  var r2 = G.Ray2D.Make(new G.Point2D(0, 0), new G.Vector2D(-1, 0));
  Console.WriteLine(r1.Touches(r2));               // True
  Console.WriteLine((r1.Touch(r2) as G.Point2D)?.ToWkt());  // POINT (0 0)

  // Anti-parallel rays overlapping → Touch returns null
  var r3 = G.Ray2D.Make(new G.Point2D(3, 0), new G.Vector2D(-1, 0));
  Console.WriteLine(r1.Touches(r3));               // False

  // Segment T-junction: endpoint of b lies on a (non-collinear)
  var a = G.LineSegment2D.Make(new G.Point2D(0, 0), new G.Point2D(5, 0));
  var b = G.LineSegment2D.Make(new G.Point2D(3, 0), new G.Point2D(3, 3));
  Console.WriteLine(a.Touches(b));                 // True
  Console.WriteLine((a.Touch(b) as G.Point2D)?.ToWkt());    // POINT (3 0)

  // Collinear segments sharing exactly one endpoint → touch
  var c = G.LineSegment2D.Make(new G.Point2D(5, 0), new G.Point2D(8, 0));
  Console.WriteLine(a.Touches(c));                 // True
  Console.WriteLine((a.Touch(c) as G.Point2D)?.ToWkt());    // POINT (5 0)

  // Overlapping collinear segments → Touch returns null
  var d = G.LineSegment2D.Make(new G.Point2D(3, 0), new G.Point2D(7, 0));
  Console.WriteLine(a.Touches(d));                 // False
  Console.WriteLine(a.Touch(d) == null);           // True
  ```

  ```
  True
  POINT (3 0)
  False
  True
  POINT (0 0)
  False
  True
  POINT (3 0)
  True
  POINT (5 0)
  False
  True
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 2.6 Polyline Overlaps / Touches </b></summary>

  `Polyline2D` and `Polyline3D` iterate over their constituent segments to collect all
  overlapping sub-segments or all touch points. `Overlap(other)` returns
  `std::optional<std::vector<LineSegment2D/3D>>`; `Touch(other)` returns `std::optional<std::vector<Point2D/3D>>`.

  <p align="center">
    <img src="./images/img_2-6-polyline-overlap-touch.png" width="420" alt="An L-shaped polyline overlapping the x-axis and a stem polyline touching it at a T-junction">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;

  // L-shaped polyline
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3)});

  // x-axis line overlaps the horizontal leg
  auto x_axis = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  GEOMPP_LOG(INFO) << pl.Overlaps(x_axis);                            // 1
  auto segs = pl.Overlap(x_axis);
  for (auto const& s : *segs) {
    GEOMPP_LOG(INFO) << s.ToWkt();                                    // LINESTRING (0 0, 4 0)
  }

  // T-junction: vertical arm touches a horizontal segment at (3,0)
  auto stem = g::Polyline2D::Make({g::Point2D(3, 0), g::Point2D(3, 3)});
  auto bar  = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(5, 0));
  GEOMPP_LOG(INFO) << stem.Touches(bar);                              // 1
  auto pts = stem.Touch(bar);
  for (auto const& p : *pts) {
    GEOMPP_LOG(INFO) << p.ToWkt();                                    // POINT (3 0)
  }

  // Two polylines sharing an endpoint
  auto pl1 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(3, 0)});
  auto pl2 = g::Polyline2D::Make({g::Point2D(3, 0), g::Point2D(3, 3)});
  GEOMPP_LOG(INFO) << pl1.Touches(pl2);                               // 1
  ```

  ```bash
  1
  LINESTRING (0 0, 4 0)
  1
  POINT (3 0)
  1
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # L-shaped polyline
  pl = g.Polyline2D.make([g.Point2D(0,0), g.Point2D(4,0), g.Point2D(4,3)])

  # x-axis line overlaps the horizontal leg
  x_axis = g.Line2D.make(g.Point2D(0,0), g.Point2D(1,0))
  print(pl.overlaps(line=x_axis))                    # True
  segs = pl.overlap(line=x_axis)
  for s in segs:
      print(s.to_wkt())                              # LINESTRING (0 0, 4 0)

  # T-junction: vertical arm touches a horizontal segment at (3,0)
  stem = g.Polyline2D.make([g.Point2D(3,0), g.Point2D(3,3)])
  bar  = g.LineSegment2D.make(g.Point2D(0,0), g.Point2D(5,0))
  print(stem.touches(segment=bar))                   # True
  pts = stem.touch(segment=bar)
  for p in pts:
      print(p.to_wkt())                              # POINT (3 0)

  # Two polylines sharing an endpoint
  pl1 = g.Polyline2D.make([g.Point2D(0,0), g.Point2D(3,0)])
  pl2 = g.Polyline2D.make([g.Point2D(3,0), g.Point2D(3,3)])
  print(pl1.touches(other=pl2))                      # True
  ```

  ```
  True
  LINESTRING (0 0, 4 0)
  True
  POINT (3 0)
  True
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using GeomPP;

  // L-shaped polyline
  var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,3) });

  // x-axis line overlaps the horizontal leg
  var xAxis = Line2D.Make(new Point2D(0,0), new Vector2D(1,0));
  Console.WriteLine(pl.Overlaps(xAxis));              // True
  var segs = pl.Overlap(xAxis);
  if (segs != null)
      foreach (var s in segs)
          Console.WriteLine(s.ToWkt());               // LINESTRING (0 0, 4 0)

  // T-junction: vertical arm touches a horizontal segment at (3,0)
  var stem = Polyline2D.Make(new Point2D[] { new(3,0), new(3,3) });
  var bar  = LineSegment2D.Make(new Point2D(0,0), new Point2D(5,0));
  Console.WriteLine(stem.Touches(bar));               // True
  var pts = stem.Touch(bar);
  if (pts != null)
      foreach (var p in pts)
          Console.WriteLine(p.ToWkt());               // POINT (3 0)

  // Two polylines sharing an endpoint
  var pl1 = Polyline2D.Make(new Point2D[] { new(0,0), new(3,0) });
  var pl2 = Polyline2D.Make(new Point2D[] { new(3,0), new(3,3) });
  Console.WriteLine(pl1.Touches(pl2));                // True
  ```

  ```
  True
  LINESTRING (0 0, 4 0)
  True
  POINT (3 0)
  True
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 3. Planes </b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 3.1 Coplanarity, winding order, and polygon with holes </b></summary>

  <p align="center">
    <img src="./images/img_3-1-winding-holes.png" width="420" alt="A Polygon3D outer ring wound CCW with a rectangular hole wound CW">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # Four points on the XY plane vs. a set that spans 3D space
  pts_flat = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(1,1,0)]
  pts_3d   = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(0,0,1)]

  print(g.are_coplanar(pts_flat))  # True  — all on the XY plane
  print(g.are_coplanar(pts_3d))    # False — spans 3D space

  # Which world-axis plane is closest to the cloud?
  plane = g.closest_world_plane_to(pts_flat)
  print(plane.normal)              # VECTOR (0 0 1)  → XY plane

  # Winding check
  ring = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(1,1,0), g.Point3D(0,1,0)]
  print(g.are_ccw(ring))           # True

  # Polygon3D with a rectangular hole (outer CCW, hole CW)
  outer = [g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(4,4,0), g.Point3D(0,4,0)]
  hole  = [g.Point3D(1,3,0), g.Point3D(3,3,0), g.Point3D(3,1,0), g.Point3D(1,1,0)]
  poly  = g.Polygon3D.make(outer, [hole])
  print(poly.to_wkt())
  ```

  will print out

  ```
  True
  False
  VECTOR (0 0 1)
  True
  POLYGON ((0 0 0, 4 0 0, 4 4 0, 0 4 0, 0 0 0), (1 3 0, 3 3 0, 3 1 0, 1 1 0, 1 3 0))
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using GeomPP;
  using Geompp.Extensions;

  Precision.DecimalPrecision = Precision.DP_THREE;

  // Four points on the XY plane vs. a set that spans 3D space
  var flat = new List<Point3D> {
      new(0,0,0), new(1,0,0), new(0,1,0), new(1,1,0)
  };
  var skew = new List<Point3D> {
      new(0,0,0), new(1,0,0), new(0,1,0), new(0,0,1)
  };

  Console.WriteLine(flat.AreCoplanar());   // True  — all on the XY plane
  Console.WriteLine(skew.AreCoplanar());   // False — spans 3D space

  // Which world-axis plane is closest to the cloud?
  var plane = flat.ClosestWorldPlaneTo();
  Console.WriteLine(plane.Normal());       // VECTOR (0 0 1)  → XY plane

  // Winding check
  var ring = new List<Point3D> { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) };
  Console.WriteLine(ring.AreCCW());        // True

  // Polygon3D with a rectangular hole (outer CCW, hole CW)
  var outer = new List<Point3D> {
      new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0)
  };
  var hole = new List<Point3D> {
      new(1,3,0), new(3,3,0), new(3,1,0), new(1,1,0)
  };
  var poly = Polygon3D.Make(outer, new List<List<Point3D>> { hole });
  Console.WriteLine(poly.ToWkt());
  ```

  will print out

  ```
  True
  False
  VECTOR (0 0 1)
  True
  POLYGON ((0 0 0, 4 0 0, 4 4 0, 0 4 0, 0 0 0), (1 3 0, 3 3 0, 3 1 0, 1 1 0, 1 3 0))
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 3.2 Projecting points onto a plane </b></summary>

  `Plane::ProjectOnto(p)` returns the perpendicular projection in 3D world coordinates.
  `Plane::ProjectInto(p)` maps the same projected point into the plane's local 2D frame.
  `Plane::Evaluate(p2d)` is the inverse — local 2D coordinates back to world 3D.

  <p align="center">
    <img src="./images/img_3-2-projection.png" width="420" alt="A point above a plane dropped perpendicularly onto it via ProjectOnto">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # XY plane: normal (0, 0, 1), origin (0, 0, 0)
  pl = g.Plane.xy()
  p  = g.Point3D(3.0, 4.0, 7.0)

  on   = pl.project_onto(p)    # Point3D(3, 4, 0) — projection in world 3D
  into = pl.project_into(p)    # Point2D(3, 4)    — in the plane's local frame
  back = pl.evaluate(into)     # Point3D(3, 4, 0) — local 2D back to world 3D

  print(f"on_plane: {on.to_wkt()}")
  print(f"in_plane: {into.to_wkt()}")
  print(f"back:     {back.to_wkt()}")
  ```

  ```
  on_plane: POINT (3 4 0)
  in_plane: POINT (3 4)
  back:     POINT (3 4 0)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // XY plane: normal (0, 0, 1), origin (0, 0, 0)
  var pl = G.Plane.XY();
  var p  = new G.Point3D(3.0, 4.0, 7.0);

  var on   = pl.ProjectOnto(p);    // Point3D(3, 4, 0)
  var into = pl.ProjectInto(p);    // Point2D(3, 4)
  var back = pl.Evaluate(into);    // Point3D(3, 4, 0)

  Console.WriteLine($"on_plane: {on.ToWkt()}");
  Console.WriteLine($"in_plane: {into.ToWkt()}");
  Console.WriteLine($"back:     {back.ToWkt()}");
  ```

  ```
  on_plane: POINT (3 4 0)
  in_plane: POINT (3 4)
  back:     POINT (3 4 0)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 3.3 Planar vs non-planar Polyline3D </b></summary>

  `Polyline3D::IsPlanar()` checks whether all knots lie in a common plane. Only planar polylines support
  `IsSimple()`, `IsConvex()`, `ConvexHull()`, and `ToPolygon()` — call `IsPlanar()` first.

  `ConvexHull()` returns a `Polyline3D` (an open path). Call `ToPolygon()` on it to close the boundary into a `Polygon3D` with area.

  <p align="center">
    <img src="./images/img_3-3-planar.png" width="420" alt="A flat planar polyline next to a non-planar polyline whose z rises at each knot">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # Planar star-like path in the XY plane
  planar = g.Polyline3D.make([
      g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(2, 2, 0),
      g.Point3D(4, 4, 0), g.Point3D(0, 4, 0),
  ])

  print(f"planar: {planar.is_planar()}")     # True

  hull    = planar.convex_hull()             # Polyline3D — open hull
  polygon = hull.to_polygon()               # Polygon3D  — closed region with area

  print(f"hull knots:   {hull.size()}")
  print(f"polygon area: {polygon.area():.3f}")

  # Non-planar path: each point rises out of the XY plane
  rising = g.Polyline3D.make([
      g.Point3D(0, 0, 0), g.Point3D(1, 0, 0),
      g.Point3D(1, 1, 1), g.Point3D(0, 1, 2),
  ])

  print(f"planar: {rising.is_planar()}")     # False

  pts = [rising[i] for i in range(rising.size())]
  direction = g.principal_direction(pts)
  print(f"dominant direction: {direction}")
  ```

  will print out

  ```
  planar: True
  hull knots:   4
  polygon area: 16.000
  planar: False
  dominant direction: VECTOR (...)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // Planar star-like path in the XY plane
  var planar = G.Polyline3D.Make(new G.Point3D[] {
      new(0, 0, 0), new(4, 0, 0), new(2, 2, 0),
      new(4, 4, 0), new(0, 4, 0),
  });

  Console.WriteLine($"planar: {planar.IsPlanar()}");    // True

  var hull    = planar.ConvexHull();   // Polyline3D — open hull
  var polygon = hull.ToPolygon();      // Polygon3D  — closed region with area

  Console.WriteLine($"hull knots:   {hull.Size()}");
  Console.WriteLine($"polygon area: {polygon.Area():F3}");

  // Non-planar path: each point rises out of the XY plane
  var rising = G.Polyline3D.Make(new G.Point3D[] {
      new(0, 0, 0), new(1, 0, 0),
      new(1, 1, 1), new(0, 1, 2),
  });

  Console.WriteLine($"planar: {rising.IsPlanar()}");    // False

  var pts = new System.Collections.Generic.List<G.Point3D>();
  for (int i = 0; i < rising.Size(); i++) pts.Add(rising[i]);
  var direction = G.GeomUtil.PrincipalDirection(pts);
  Console.WriteLine($"dominant direction: {direction}");
  ```

  will print out

  ```
  planar: True
  hull knots:   4
  polygon area: 16.000
  planar: False
  dominant direction: VECTOR (...)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 3.4 View2D — streaming 3D points to 2D </b></summary>

  `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters without allocating an intermediate
  `Point2D` container. Axis-aligned views (`XY`, `YZ`, `ZX`) are the fastest path — just a direct
  coordinate read. `OnPlane` computes dot products against the plane's local axes.

  <p align="center">
    <img src="./images/img_3-4-view2d.png" width="420" alt="A 3D point on a tilted plane streamed to a flat 2D view via View2D::OnPlane">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  from geompp import View2D, Plane, Point3D, Vector3D

  # Axis-aligned views (fast path — single coordinate read)
  v_xy = View2D.xy()   # x→x, y→y (drops z)
  v_yz = View2D.yz()   # y→x, z→y (drops x)
  v_zx = View2D.zx()   # z→x, x→y (drops y)

  # Custom view onto any plane
  plane    = Plane.from_origin_and_normal(Point3D(0, 0, 5), Vector3D(0, 0, 1))
  v_custom = View2D.on_plane(plane)

  pts3d = [Point3D(1, 2, 5), Point3D(3, 4, 5), Point3D(5, 6, 5)]

  # Stream 3D points to 2D without allocating a Point2D list
  for p in pts3d:
      print(f"({v_xy.x(p)}, {v_xy.y(p)})")
  ```

  ```
  (1, 2)
  (3, 4)
  (5, 6)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;
  using System.Linq;

  // Axis-aligned views (fast path — single coordinate read)
  var vXY = G.View2D.XY();   // x→x, y→y (drops z)
  var vYZ = G.View2D.YZ();   // y→x, z→y (drops x)
  var vZX = G.View2D.ZX();   // z→x, x→y (drops y)

  // Custom view onto any plane
  var plane   = G.Plane.FromOriginAndNormal(new G.Point3D(0, 0, 5), new G.Vector3D(0, 0, 1));
  var vCustom = G.View2D.OnPlane(plane);

  var pts3d = new G.Point3D[] { new(1, 2, 5), new(3, 4, 5), new(5, 6, 5) };

  // Stream 3D points to 2D without allocating a Point2D array
  foreach (var p in pts3d)
      Console.WriteLine($"({vXY.X(p)}, {vXY.Y(p)})");
  ```

  ```
  (1, 2)
  (3, 4)
  (5, 6)
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 4. PCA on a 3D point cloud </b></summary>

  `principal_axes(points)` runs PCA (Jacobi eigen decomposition on the 3×3 covariance matrix) and returns
  a `CoordinateFrame` — three orthonormal axes sorted by variance: `X` is the direction of most spread,
  `Y` the secondary, and `Z` the best-fit plane normal (least variance).

  <p align="center">
    <img src="./images/img_4-pca.png" width="420" alt="A 3D point cloud (grey) with its centroid and principal_axes X/Y/Z (red/green/blue), shown against the white world axes">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # 8 points flat in the XY plane, elongated along X
  cloud = [
      g.Point3D(0, 0,   0), g.Point3D(1, 0,   0),
      g.Point3D(2, 0,   0), g.Point3D(3, 0,   0),
      g.Point3D(0, 0.1, 0), g.Point3D(1, 0.1, 0),
      g.Point3D(2, 0.1, 0), g.Point3D(3, 0.1, 0),
  ]

  frame = g.principal_axes(cloud)

  print(f"x (primary):   {frame.x}")   # ≈ (1, 0, 0)
  print(f"y (secondary): {frame.y}")   # ≈ (0, 1, 0)
  print(f"z (normal):    {frame.z}")   # ≈ (0, 0, 1)

  # Convenience wrappers
  normal    = g.principal_normal(cloud)     # == frame.z
  direction = g.principal_direction(cloud)  # == frame.x
  ```

  will print out

  ```
  x (primary):   VECTOR (1 0 0)
  y (secondary): VECTOR (0 1 0)
  z (normal):    VECTOR (0 0 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // 8 points flat in the XY plane, elongated along X
  var cloud = new System.Collections.Generic.List<G.Point3D> {
      new(0, 0,   0), new(1, 0,   0), new(2, 0,   0), new(3, 0,   0),
      new(0, 0.1, 0), new(1, 0.1, 0), new(2, 0.1, 0), new(3, 0.1, 0),
  };

  var frame = G.GeomUtil.PrincipalAxes(cloud);

  Console.WriteLine($"X (primary):   {frame.X}");  // ≈ (1, 0, 0)
  Console.WriteLine($"Y (secondary): {frame.Y}");  // ≈ (0, 1, 0)
  Console.WriteLine($"Z (normal):    {frame.Z}");  // ≈ (0, 0, 1)

  // Convenience wrappers
  var normal    = G.GeomUtil.PrincipalNormal(cloud);     // == frame.Z
  var direction = G.GeomUtil.PrincipalDirection(cloud);  // == frame.X
  ```

  will print out

  ```
  X (primary):   VECTOR (1 0 0)
  Y (secondary): VECTOR (0 1 0)
  Z (normal):    VECTOR (0 0 1)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; 5. Bounding containers </b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 5.1 Simple containers for quick rejection </b></summary>

  `BBox3D` gives the tight axis-aligned box; `BBall3D` (Ritter 1990) gives an approximate
  minimum enclosing sphere — both accept any cloud of points.

  <p align="center">
    <img src="./images/bbox2d.png" width="300" alt="BBox2D — axis-aligned bounding box around a 2D point cloud">
    <img src="./images/bball2d.png" width="300" alt="BBall2D — minimum enclosing circle around a 2D point cloud">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # Vertices of a rough L-shaped structure
  verts = [
      g.Point3D(0, 0, 0), g.Point3D(6, 0, 0), g.Point3D(6, 2, 0),
      g.Point3D(2, 2, 0), g.Point3D(2, 4, 0), g.Point3D(0, 4, 0),
      g.Point3D(0, 0, 3), g.Point3D(6, 0, 3), g.Point3D(6, 2, 3),
      g.Point3D(2, 2, 3), g.Point3D(2, 4, 3), g.Point3D(0, 4, 3),
  ]

  # Axis-aligned bounding box (construct from a polyline spanning all verts)
  box = g.BBox3D(g.Polyline3D.make(verts))
  print(box.min.to_wkt())                      # POINT (0 0 0)
  print(box.max.to_wkt())                      # POINT (6 4 3)
  print(box.contains(g.Point3D(3, 1, 1)))      # True
  print(box.contains(g.Point3D(7, 1, 1)))      # False

  # Bounding ball — one constructor takes the point cloud directly
  ball = g.BBall3D(verts)
  print(ball.center.to_wkt())
  print(f"radius: {ball.radius:.3f}")
  print(ball.contains(g.Point3D(3, 1, 1)))     # True

  # All original vertices must be inside the ball
  assert all(ball.contains(p) for p in verts)
  ```

  will print out

  ```
  POINT (0 0 0)
  POINT (6 4 3)
  True
  False
  POINT (3 ...)
  radius: ...
  True
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // Vertices of a rough L-shaped structure
  var verts = new G.Point3D[] {
      new(0, 0, 0), new(6, 0, 0), new(6, 2, 0),
      new(2, 2, 0), new(2, 4, 0), new(0, 4, 0),
      new(0, 0, 3), new(6, 0, 3), new(6, 2, 3),
      new(2, 2, 3), new(2, 4, 3), new(0, 4, 3),
  };

  // Axis-aligned bounding box (construct from a polyline spanning all verts)
  var box = new G.BBox3D(G.Polyline3D.Make(verts));
  Console.WriteLine(box.Min().ToWkt());                     // POINT (0 0 0)
  Console.WriteLine(box.Max().ToWkt());                     // POINT (6 4 3)
  Console.WriteLine(box.Contains(new G.Point3D(3, 1, 1))); // True
  Console.WriteLine(box.Contains(new G.Point3D(7, 1, 1))); // False

  // Bounding ball — one constructor takes the point array directly
  var ball = new G.BBall3D(verts);
  Console.WriteLine(ball.Center().ToWkt());
  Console.WriteLine($"radius: {ball.Radius():F3}");
  Console.WriteLine(ball.Contains(new G.Point3D(3, 1, 1))); // True

  // All original vertices must be inside the ball
  foreach (var p in verts)
      Console.WriteLine($"{p.ToWkt()} -> {ball.Contains(p)}");  // all True
  ```

  will print out

  ```
  POINT (0 0 0)
  POINT (6 4 3)
  True
  False
  POINT (3 ...)
  radius: ...
  True
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 5.2 Convex hull </b></summary>

<details open>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.1 Convex hull of a point cloud </b></summary>

  `convex_hull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

  <p align="center">
    <img src="./images/convex_hull.png" width="360" alt="convex_hull() wrapping the outermost points of a 2D point cloud">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # An asymmetric 5-pointed star: 5 outer tips + 5 inner concave vertices.
  # The convex hull should be exactly the 5 outer tips.
  star = [
      g.Point2D( 0,  5), g.Point2D( 4,  2),
      g.Point2D( 3, -3), g.Point2D(-2, -4), g.Point2D(-3,  1),
      g.Point2D( 2,  1), g.Point2D( 2, -1),
      g.Point2D( 0, -1), g.Point2D(-1, -1), g.Point2D(-1,  2),
  ]

  hull = g.convex_hull(star)

  print(f"hull has {len(hull)} vertices:")
  for p in hull:
      print(f"  {p.to_wkt()}")
  ```

  ```
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

  ```python
  import geompp as g

  # Nearly-coplanar cloud with small Z jitter
  cloud = [
      g.Point3D(0, 0, 0.1), g.Point3D(4, 0, -0.1),
      g.Point3D(4, 4, 0.05), g.Point3D(0, 4, -0.05),
      g.Point3D(2, 2, 0.02),  # interior
  ]

  hull = g.convex_hull(cloud)  # PCA detects near-XY plane, projects, computes hull
  print(f"3D hull has {len(hull)} vertices")  # 4 — interior point excluded
  ```

  (CCW order, starting from the lexicographically smallest point)

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // An asymmetric 5-pointed star: 5 outer tips + 5 inner concave vertices.
  // The convex hull should be exactly the 5 outer tips.
  var star = new System.Collections.Generic.List<G.Point2D> {
      new G.Point2D( 0,  5), new G.Point2D( 4,  2),
      new G.Point2D( 3, -3), new G.Point2D(-2, -4), new G.Point2D(-3,  1),
      new G.Point2D( 2,  1), new G.Point2D( 2, -1),
      new G.Point2D( 0, -1), new G.Point2D(-1, -1), new G.Point2D(-1,  2),
  };

  var hull = G.GeomUtil.ConvexHull(star);

  int count = 0;
  foreach (var p in hull) { Console.WriteLine($"  {p.ToWkt()}"); count++; }
  Console.WriteLine($"hull has {count} vertices");
  ```

  ```
    POINT (3 -3)
    POINT (4 2)
    POINT (0 5)
    POINT (-3 1)
    POINT (-2 -4)
  hull has 5 vertices
  ```

  For 3D point clouds, `GeomUtil.ConvexHull(points)` also works — points do **not** need to be perfectly
  coplanar. When no explicit normal is provided, the best-fit plane is estimated via PCA
  (Jacobi eigendecomposition), and the hull is computed on the projection onto that plane.
  You can also pass an explicit normal if known: `GeomUtil.ConvexHull(points, normal)`.

  ```csharp
  using G = GeomPP;

  // Nearly-coplanar cloud with small Z jitter
  var cloud = new System.Collections.Generic.List<G.Point3D> {
      new G.Point3D(0, 0, 0.1), new G.Point3D(4, 0, -0.1),
      new G.Point3D(4, 4, 0.05), new G.Point3D(0, 4, -0.05),
      new G.Point3D(2, 2, 0.02),  // interior
  };

  var hull3d = G.GeomUtil.ConvexHull(cloud);  // PCA detects near-XY plane, projects, computes hull
  int n = 0;
  foreach (var p in hull3d) n++;
  Console.WriteLine($"3D hull has {n} vertices");  // 4 — interior point excluded
  ```

  (CCW order, starting from the lexicographically smallest point)

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.2 Convex hull of a polygon </b></summary>

  `Polygon2D` and `Polygon3D` expose a `ConvexHull()` method that wraps the free function:

  <p align="center">
    <img src="./images/img_5-2-2-hull-polygon.png" width="420" alt="A 10-vertex star polygon with its 5-vertex convex hull overlaid">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # 3D star polygon (10 vertices, coplanar, CCW)
  star = g.Polygon3D.make([
      g.Point3D( 0,  5, 0), g.Point3D( 2,  1, 0),
      g.Point3D( 4,  2, 0), g.Point3D( 2, -1, 0),
      g.Point3D( 3, -3, 0), g.Point3D( 0, -1, 0),
      g.Point3D(-2, -4, 0), g.Point3D(-1, -1, 0),
      g.Point3D(-3,  1, 0), g.Point3D(-1,  2, 0),
  ])

  hull = star.convex_hull()   # Polygon3D — 5-vertex pentagon

  print(f"star is convex: {star.is_convex()}")  # False — star has concavities
  print(f"hull is convex: {hull.is_convex()}")  # True

  print(f"hull has {hull.size()} vertices:")
  for i in range(hull.size()):
      print(f"  {hull[i].to_wkt()}")
  ```

  ```
  star is convex: False
  hull is convex: True
  hull has 5 vertices:
    POINT (3 -3 0)
    POINT (4 2 0)
    POINT (0 5 0)
    POINT (-3 1 0)
    POINT (-2 -4 0)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // 3D star polygon (10 vertices, coplanar, CCW)
  var star = G.Polygon3D.Make(new G.Point3D[] {
      new( 0,  5, 0), new( 2,  1, 0),
      new( 4,  2, 0), new( 2, -1, 0),
      new( 3, -3, 0), new( 0, -1, 0),
      new(-2, -4, 0), new(-1, -1, 0),
      new(-3,  1, 0), new(-1,  2, 0),
  });

  var hull = star.ConvexHull();   // Polygon3D — 5-vertex pentagon

  Console.WriteLine($"star is convex: {star.IsConvex()}");  // False — star has concavities
  Console.WriteLine($"hull is convex: {hull.IsConvex()}");  // True

  Console.WriteLine($"hull has {hull.Size()} vertices:");
  for (int i = 0; i < hull.Size(); i++)
      Console.WriteLine($"  {hull[i].ToWkt()}");
  ```

  ```
  star is convex: False
  hull is convex: True
  hull has 5 vertices:
    POINT (3 -3 0)
    POINT (4 2 0)
    POINT (0 5 0)
    POINT (-3 1 0)
    POINT (-2 -4 0)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.3 Convex hull of a simple polyline </b></summary>

  `Polyline2D::ConvexHull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `IsSimple()` first.

  <p align="center">
    <img src="./images/img_5-2-3-hull-polyline.png" width="420" alt="A concave polyline with an inner dip wrapped by its 4-vertex convex hull">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # Simple concave path: outer corners with an inner dip at (2,1)
  path = g.Polyline2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4),
      g.Point2D(2, 1), g.Point2D(0, 4),
  ])

  if path.is_simple():
      hull = path.convex_hull()  # Polygon2D — 4-vertex rectangle
      print(f"hull has {hull.size()} vertices")
  ```

  ```
  hull has 4 vertices
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // Simple concave path: outer corners with an inner dip at (2,1)
  var path = G.Polyline2D.Make(new G.Point2D[] {
      new(0, 0), new(4, 0), new(4, 4),
      new(2, 1), new(0, 4),
  });

  if (path.IsSimple()) {
      var hull = path.ConvexHull();  // Polygon2D — 4-vertex rectangle
      Console.WriteLine($"hull has {hull.Size()} vertices");
  }
  ```

  ```
  hull has 4 vertices
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 5.3 Oriented Minimum Bounding Rectangle </b></summary>

  `BRect2D` computes the **tightest** axis-aligned-to-input rectangle that encloses a point cloud.
  It is defined by a center point, two orthogonal unit axes (`axis_u`, `axis_v`), and two half-lengths
  (`half_len_u`, `half_len_v`).

  **Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
  1. Compute the convex hull of the input cloud (Andrew's monotone chain, O(n log n)).
  2. For each hull edge, project all hull vertices onto the edge direction and its perpendicular.
  3. The rectangle aligned with that edge has width = max − min along the edge and height = max − min along the perpendicular.
  4. Track the edge orientation that minimises area; the center is the midpoint of the extents.

  This guarantees a minimum-area rectangle with one side flush with a hull edge. The 3D analog,
  `BPrism3D`, computes the minimum-volume oriented bounding prism the same way (PCA to find the
  best-fit plane, then rotating calipers within it).

  <p align="center">
    <img src="./images/brect2d.png" width="300" alt="BRect2D — minimum-area oriented bounding rectangle">
    <img src="./images/bprism3d.png" width="300" alt="BPrism3D — minimum-volume oriented bounding prism">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

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

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # An asymmetric pentagon
  pts = [
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(5, 2), g.Point2D(2, 4),
      g.Point2D(-1, 2),
  ]

  r = g.BRect2D(pts)

  print(r.center.to_wkt())      # center of the OBB
  print(r.axis_u.to_wkt())      # primary axis (unit vector, along a hull edge)
  print(r.axis_v.to_wkt())      # secondary axis (perpendicular, CCW rotation of axis_u)
  print(f"half_u: {r.half_len_u:.3f}")
  print(f"half_v: {r.half_len_v:.3f}")
  print(f"area:   {r.area:.3f}")
  print(r.contains(g.Point2D(2, 2)))   # True  — interior point
  print(r.contains(g.Point2D(9, 0)))   # False — outside

  corners = r.corners()         # list of 4 Point2D in CCW order
  for c in corners:
      print(c.to_wkt())
  ```

  `corners()` returns the four corners in CCW order; each is guaranteed to be `contains()`-true.

  The `contains()` test projects the query point onto the local axes and checks both projections against the half-lengths — O(1) per query.

  `BPrism3D` computes the minimum-volume oriented bounding prism via PCA + rotating calipers (requires ≥ 3 non-collinear points):

  ```python
  import geompp as g

  pts = [
      g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(4,3,0), g.Point3D(0,3,0),
      g.Point3D(0,0,2), g.Point3D(4,0,2), g.Point3D(4,3,2), g.Point3D(0,3,2),
  ]
  prism = g.BPrism3D(pts)
  print(prism.center.to_wkt())                 # roughly POINT (2 1.5 1)
  print(f"U: {prism.axis_u.to_wkt()}")
  print(f"V: {prism.axis_v.to_wkt()}")
  print(f"W: {prism.axis_w.to_wkt()}")
  print(f"{prism.width:.1f} × {prism.height:.1f} × {prism.depth:.1f}")   # 4 × 3 × 2
  print(f"volume: {prism.volume:.1f}")         # 24.0
  corners = prism.corners()                    # list of 8 Point3D corners
  print(prism.contains(prism.center))          # True
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  G.Precision.DecimalPrecision = G.Precision.DP_THREE;

  // An asymmetric pentagon
  var pts = new G.Point2D[] {
      new G.Point2D(0, 0), new G.Point2D(4, 0),
      new G.Point2D(5, 2), new G.Point2D(2, 4),
      new G.Point2D(-1, 2),
  };

  var r = new G.BRect2D(pts);

  Console.WriteLine(r.Center().ToWkt());         // center of the OBB
  Console.WriteLine(r.AxisU().ToWkt());          // primary axis (unit vector, along a hull edge)
  Console.WriteLine(r.AxisV().ToWkt());          // secondary axis (perpendicular, CCW rotation of AxisU)
  Console.WriteLine($"half_u: {r.HalfLenU():F3}");
  Console.WriteLine($"half_v: {r.HalfLenV():F3}");
  Console.WriteLine($"area:   {r.Area():F3}");
  Console.WriteLine(r.Contains(new G.Point2D(2, 2)));  // True  — interior point
  Console.WriteLine(r.Contains(new G.Point2D(9, 0)));  // False — outside

  var corners = r.Corners();                     // array of 4 Point2D in CCW order
  foreach (var c in corners)
      Console.WriteLine(c.ToWkt());
  ```

  `Corners()` returns the four corners in CCW order; each is guaranteed to be `Contains()`-true.

  The `Contains()` test projects the query point onto the local axes and checks both projections against the half-lengths — O(1) per query.

  `BPrism3D` computes the minimum-volume oriented bounding prism via PCA + rotating calipers (requires ≥ 3 non-collinear points):

  ```csharp
  using G = GeomPP;

  var pts = new[] {
      new G.Point3D(0, 0, 0), new G.Point3D(4, 0, 0),
      new G.Point3D(4, 3, 0), new G.Point3D(0, 3, 0),
      new G.Point3D(0, 0, 2), new G.Point3D(4, 0, 2),
      new G.Point3D(4, 3, 2), new G.Point3D(0, 3, 2),
  };
  var prism = new G.BPrism3D(pts);
  Console.WriteLine(prism.Center());                  // roughly (2, 1.5, 1)
  Console.WriteLine($"U={prism.AxisU()} V={prism.AxisV()} W={prism.AxisW()}");
  Console.WriteLine($"{prism.Width()} × {prism.Height()} × {prism.Depth()}");  // 4 × 3 × 2
  Console.WriteLine($"volume={prism.Volume()}");      // ~24.0
  var corners = prism.Corners();                      // array of 8 Point3D corners
  Console.WriteLine(prism.Contains(prism.Center()));  // True
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 5.4 Polygon extreme points </b></summary>

  `find_extreme_points(polygon, line)` returns the two vertices of a polygon that are **extreme** — the
  least and the greatest — when projected onto a line's direction (the "supporting vertices" along that
  axis). It is handy for collision broad-phase (SAT), rotating calipers, and directional clipping.

  The result is an `ExtremePoints<Point2D>` (or `ExtremePoints<Point3D>`) with `.min_point` / `.max_point`.
  When the polygon is **convex** it uses Daniel Sunday's O(log n) binary search; otherwise it falls back to
  an O(n) linear scan. Holes are ignored — only the outer ring participates.

  <p align="center">
    <img src="./images/img_5-4-extreme-points.png" width="420" alt="A diamond polygon with its min/max supporting vertices along the x-axis direction marked">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "calc_utils2d.hpp"   // find_extreme_points(Polygon2D, Line2D)
  #include "calc_utils3d.hpp"   // find_extreme_points(Polygon3D, Line3D)
  #include "polygon2d.hpp"
  #include "polygon3d.hpp"
  #include "line2d.hpp"
  #include "line3d.hpp"

  namespace g = geompp;

  // Convex diamond; project onto the X-axis to get the left / right tips
  auto diamond = g::Polygon2D::Make({
      g::Point2D(2, 0), g::Point2D(4, 2), g::Point2D(2, 4), g::Point2D(0, 2)});
  auto x_axis = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));

  auto ext = g::find_extreme_points(diamond, x_axis);   // convex → O(log n)
  GEOMPP_LOG(INFO) << "min: " << ext.min_point.ToWkt();  // POINT (0 2)
  GEOMPP_LOG(INFO) << "max: " << ext.max_point.ToWkt();  // POINT (4 2)

  // Works in 3D too — the polygon may lie in any plane
  auto para = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(2, 0, 2),
      g::Point3D(2, 2, 2), g::Point3D(0, 2, 0)});
  auto dir = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0));
  auto ext3 = g::find_extreme_points(para, dir);
  GEOMPP_LOG(INFO) << ext3.min_point.ToWkt() << " .. " << ext3.max_point.ToWkt();
  ```

  ```bash
  min: POINT (0 2)
  max: POINT (4 2)
  POINT (0 0 0) .. POINT (2 2 2)
  ```

  `distance_to(polygon, line)` is the companion function: the distance between a polygon and an
  infinite line, zero if they cross. It works the same way for `Polygon2D`/`Line2D` and
  `Polygon3D`/`Line3D` — the 3D overload additionally handles a line that is coplanar with,
  parallel to (at a fixed offset from), or skew to the polygon's plane.

  ```cpp
  auto square = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});

  // zero when the line crosses the polygon...
  auto crossing = g::Line2D::Make(g::Point2D(2, -1), g::Point2D(2, 5));
  GEOMPP_LOG(INFO) << "distance (crossing): " << g::distance_to(square, crossing);

  // ...and the perpendicular distance to the nearest edge otherwise
  auto outside = g::Line2D::Make(g::Point2D(6, -1), g::Point2D(6, 5));
  GEOMPP_LOG(INFO) << "distance (outside): " << g::distance_to(square, outside);

  // Polygon3D / Line3D: a line parallel to the polygon's plane, offset by 3 along its normal —
  // combines the in-plane distance (2, same shape as above) with the offset via Pythagoras
  auto square3d = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto offset_line = g::Line3D::Make(g::Point3D(6, 0, 3), g::Point3D(6, 1, 3));
  GEOMPP_LOG(INFO) << "distance (3D, parallel offset): " << g::distance_to(square3d, offset_line);
  ```

  ```bash
  distance (crossing): 0
  distance (outside): 2
  distance (3D, parallel offset): 3.60555
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # Convex diamond; project onto the X-axis to get the left / right tips
  diamond = g.Polygon2D.make([
      g.Point2D(2, 0), g.Point2D(4, 2), g.Point2D(2, 4), g.Point2D(0, 2)])
  x_axis = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))

  ext = g.find_extreme_points(diamond, x_axis)   # convex → O(log n)
  print(f"min: {ext.min_point.to_wkt()}")         # POINT (0 2)
  print(f"max: {ext.max_point.to_wkt()}")         # POINT (4 2)

  # Works in 3D too — the polygon may lie in any plane
  para = g.Polygon3D.make([
      g.Point3D(0, 0, 0), g.Point3D(2, 0, 2),
      g.Point3D(2, 2, 2), g.Point3D(0, 2, 0)])
  d = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 1, 0))
  ext3 = g.find_extreme_points(para, d)
  print(f"{ext3.min_point.to_wkt()} .. {ext3.max_point.to_wkt()}")
  ```

  ```
  min: POINT (0 2)
  max: POINT (4 2)
  POINT (0 0 0) .. POINT (2 2 2)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // Convex diamond; project onto the X-axis to get the left / right tips
  var diamond = G.Polygon2D.Make(new G.Point2D[] {
      new(2, 0), new(4, 2), new(2, 4), new(0, 2) });
  var xAxis = G.Line2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 0));

  var ext = G.GeomUtil.FindExtremePoints(diamond, xAxis);   // convex → O(log n)
  Console.WriteLine($"min: {ext.MinPoint.ToWkt()}");         // POINT (0 2)
  Console.WriteLine($"max: {ext.MaxPoint.ToWkt()}");         // POINT (4 2)

  // Works in 3D too — the polygon may lie in any plane
  var para = G.Polygon3D.Make(new G.Point3D[] {
      new(0, 0, 0), new(2, 0, 2), new(2, 2, 2), new(0, 2, 0) });
  var dir = G.Line3D.Make(new G.Point3D(0, 0, 0), new G.Point3D(1, 1, 0));
  var ext3 = G.GeomUtil.FindExtremePoints(para, dir);
  Console.WriteLine($"{ext3.MinPoint.ToWkt()} .. {ext3.MaxPoint.ToWkt()}");
  ```

  ```
  min: POINT (0 2)
  max: POINT (4 2)
  POINT (0 0 0) .. POINT (2 2 2)
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 6. Distance </b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 6.1 from Point</b></summary>

  Every core primitive implements `DistanceTo(point)` — the perpendicular / nearest distance to a
  point, clamped to the primitive's own domain where relevant (a `Ray` only measures ahead of its
  origin, a `LineSegment`/`Polyline` clamps to its own bounded extent). `Point2D`/`Point3D` themselves
  just measure Euclidean distance to another point.

  `Polygon2D`/`Polygon3D` and `Triangle2D`/`Triangle3D::DistanceTo(Point)` return `0` for an interior or
  on-boundary point (including hole boundaries, for `Polygon2D`/`3D`); otherwise the minimum distance to
  the nearest edge. For `Polygon3D`/`Triangle3D`, an off-plane point is never "inside" regardless of its
  in-plane position, so the result is always the true 3D distance to the nearest edge, never a
  perpendicular-to-the-flat-interior shortcut.

  <p align="center">
    <img src="./images/img_6-1-distance-point.png" width="420" alt="A query point outside a square polygon with a dashed line to the nearest boundary point">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  g::Point2D p2(3, 4);
  g::Point3D p3(3, 4, 5);

  GEOMPP_LOG(INFO) << "Point2D:       " << g::Point2D(0, 0).DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Point3D:       " << g::Point3D(0, 0, 0).DistanceTo(p3);

  auto line2 = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto line3 = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  GEOMPP_LOG(INFO) << "Line2D:        " << line2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Line3D:        " << line3.DistanceTo(p3);

  auto ray2 = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D(1, 0));
  auto ray3 = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  GEOMPP_LOG(INFO) << "Ray2D:         " << ray2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Ray3D:         " << ray3.DistanceTo(p3);

  auto seg2 = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(6, 0));
  auto seg3 = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(6, 0, 0));
  GEOMPP_LOG(INFO) << "LineSegment2D: " << seg2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "LineSegment3D: " << seg3.DistanceTo(p3);

  auto pl2 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(6, 0), g::Point2D(6, 6)});
  auto pl3 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(6, 0, 0), g::Point3D(6, 6, 0)});
  GEOMPP_LOG(INFO) << "Polyline2D:    " << pl2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Polyline3D:    " << pl3.DistanceTo(p3);

  auto plane = g::Plane::XY();
  GEOMPP_LOG(INFO) << "Plane:         " << plane.DistanceTo(p3);

  auto poly2 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2), g::Point2D(0, 2)});
  auto poly3 = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 2, 0), g::Point3D(0, 2, 0)});
  GEOMPP_LOG(INFO) << "Polygon2D:     " << poly2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Polygon3D:     " << poly3.DistanceTo(p3);

  auto tri2 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(0, 2));
  auto tri3 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  GEOMPP_LOG(INFO) << "Triangle2D:    " << tri2.DistanceTo(p2);
  GEOMPP_LOG(INFO) << "Triangle3D:    " << tri3.DistanceTo(p3);
  ```

  ```bash
  Point2D:       5
  Point3D:       7.071
  Line2D:        4
  Line3D:        6.40312
  Ray2D:         4
  Ray3D:         6.40312
  LineSegment2D: 4
  LineSegment3D: 6.40312
  Polyline2D:    3
  Polyline3D:    5.83095
  Plane:         5
  Polygon2D:     2.23607
  Polygon3D:     5.47723
  Triangle2D:    3.53553
  Triangle3D:    6.12372
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  p2 = g.Point2D(3, 4)
  p3 = g.Point3D(3, 4, 5)

  print(f"Point2D:       {g.Point2D(0, 0).distance_to(p2):.3f}")
  print(f"Point3D:       {g.Point3D(0, 0, 0).distance_to(p3):.3f}")

  line2 = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))
  line3 = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 0, 0))
  print(f"Line2D:        {line2.distance_to(p2):.3f}")
  print(f"Line3D:        {line3.distance_to(p3):.3f}")

  ray2 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  ray3 = g.Ray3D.make(g.Point3D(0, 0, 0), g.Vector3D(1, 0, 0))
  print(f"Ray2D:         {ray2.distance_to(p2):.3f}")
  print(f"Ray3D:         {ray3.distance_to(p3):.3f}")

  seg2 = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(6, 0))
  seg3 = g.LineSegment3D.make(g.Point3D(0, 0, 0), g.Point3D(6, 0, 0))
  print(f"LineSegment2D: {seg2.distance_to(p2):.3f}")
  print(f"LineSegment3D: {seg3.distance_to(p3):.3f}")

  pl2 = g.Polyline2D.make([g.Point2D(0, 0), g.Point2D(6, 0), g.Point2D(6, 6)])
  pl3 = g.Polyline3D.make([g.Point3D(0, 0, 0), g.Point3D(6, 0, 0), g.Point3D(6, 6, 0)])
  print(f"Polyline2D:    {pl2.distance_to(p2):.3f}")
  print(f"Polyline3D:    {pl3.distance_to(p3):.3f}")

  plane = g.Plane.xy()
  print(f"Plane:         {plane.distance_to(p3):.3f}")

  poly2 = g.Polygon2D.make([g.Point2D(0, 0), g.Point2D(2, 0), g.Point2D(2, 2), g.Point2D(0, 2)])
  poly3 = g.Polygon3D.make([g.Point3D(0, 0, 0), g.Point3D(2, 0, 0), g.Point3D(2, 2, 0), g.Point3D(0, 2, 0)])
  print(f"Polygon2D:     {poly2.distance_to(p2):.3f}")
  print(f"Polygon3D:     {poly3.distance_to(p3):.3f}")

  tri2 = g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(2, 0), g.Point2D(0, 2))
  tri3 = g.Triangle3D.make(g.Point3D(0, 0, 0), g.Point3D(2, 0, 0), g.Point3D(0, 2, 0))
  print(f"Triangle2D:    {tri2.distance_to(p2):.3f}")
  print(f"Triangle3D:    {tri3.distance_to(p3):.3f}")
  ```

  ```
  Point2D:       5.000
  Point3D:       7.071
  Line2D:        4.000
  Line3D:        6.403
  Ray2D:         4.000
  Ray3D:         6.403
  LineSegment2D: 4.000
  LineSegment3D: 6.403
  Polyline2D:    3.000
  Polyline3D:    5.831
  Plane:         5.000
  Polygon2D:     2.236
  Polygon3D:     5.477
  Triangle2D:    3.536
  Triangle3D:    6.124
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using GeomPP;

  var p2 = new Point2D(3, 4);
  var p3 = new Point3D(3, 4, 5);

  Console.WriteLine($"Point2D:       {new Point2D(0, 0).DistanceTo(p2):F3}");
  Console.WriteLine($"Point3D:       {new Point3D(0, 0, 0).DistanceTo(p3):F3}");

  var line2 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
  var line3 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
  Console.WriteLine($"Line2D:        {line2.DistanceTo(p2):F3}");
  Console.WriteLine($"Line3D:        {line3.DistanceTo(p3):F3}");

  var ray2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var ray3 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
  Console.WriteLine($"Ray2D:         {ray2.DistanceTo(p2):F3}");
  Console.WriteLine($"Ray3D:         {ray3.DistanceTo(p3):F3}");

  var seg2 = LineSegment2D.Make(new Point2D(0, 0), new Point2D(6, 0));
  var seg3 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(6, 0, 0));
  Console.WriteLine($"LineSegment2D: {seg2.DistanceTo(p2):F3}");
  Console.WriteLine($"LineSegment3D: {seg3.DistanceTo(p3):F3}");

  var pl2 = Polyline2D.Make(new Point2D[] { new(0, 0), new(6, 0), new(6, 6) });
  var pl3 = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(6, 0, 0), new(6, 6, 0) });
  Console.WriteLine($"Polyline2D:    {pl2.DistanceTo(p2):F3}");
  Console.WriteLine($"Polyline3D:    {pl3.DistanceTo(p3):F3}");

  var plane = Plane.XY();
  Console.WriteLine($"Plane:         {plane.DistanceTo(p3):F3}");

  var poly2 = Polygon2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2), new(0, 2) });
  var poly3 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(2, 0, 0), new(2, 2, 0), new(0, 2, 0) });
  Console.WriteLine($"Polygon2D:     {poly2.DistanceTo(p2):F3}");
  Console.WriteLine($"Polygon3D:     {poly3.DistanceTo(p3):F3}");

  var tri2 = Triangle2D.Make(new Point2D(0, 0), new Point2D(2, 0), new Point2D(0, 2));
  var tri3 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
  Console.WriteLine($"Triangle2D:    {tri2.DistanceTo(p2):F3}");
  Console.WriteLine($"Triangle3D:    {tri3.DistanceTo(p3):F3}");
  ```

  ```
  Point2D:       5.000
  Point3D:       7.071
  Line2D:        4.000
  Line3D:        6.403
  Ray2D:         4.000
  Ray3D:         6.403
  LineSegment2D: 4.000
  LineSegment3D: 6.403
  Polyline2D:    3.000
  Polyline3D:    5.831
  Plane:         5.000
  Polygon2D:     2.236
  Polygon3D:     5.477
  Triangle2D:    3.536
  Triangle3D:    6.124
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 6.2 from Other primitives</b></summary>

  `Line3D`, `Ray3D`, and `LineSegment3D` each expose `DistanceTo(Line3D | Ray3D | LineSegment3D)` —
  pairwise distance between any two of the three (0 if they intersect, overlap, or one contains the
  other). This overload set is 3D-only: two 2D primitives are either parallel (a constant distance,
  rarely useful on its own) or they intersect (0), so `Line2D`/`Ray2D`/`LineSegment2D` don't expose it.

  If you need the actual closest-approach segment instead of just the scalar, use `Distance(...)`
  (note: no `To`) — it returns `std::optional<LineSegment3D>`, `std::nullopt` when the two intersect
  or overlap (matching the zero case of `DistanceTo`). Under the hood, the closest points on two
  (possibly skew) 3D lines are solved in closed form via Cramer's rule on the 2×2 system that makes
  the connecting vector perpendicular to both directions.

  For polygon-to-line distance, use the free function `distance_to(polygon, line)` (see section 5.4
  "Polygon extreme points" for `find_extreme_points`, its sibling function) — zero if the line
  crosses the polygon; for `Polygon3D`/`Line3D` it also handles a line coplanar with, parallel to
  (fixed offset from), or skew to the polygon's plane.

  <p align="center">
    <img src="./images/img_6-2-distance-primitives.png" width="420" alt="Two parallel 3D lines with the closest-approach connector segment between them">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  auto lineA = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto lineB = g::Line3D::Make(g::Point3D(0, 1, 1), g::Point3D(1, 1, 1));  // parallel, offset sqrt(2)
  GEOMPP_LOG(INFO) << "Line3D x Line3D:        " << lineA.DistanceTo(lineB);

  auto rayB = g::Ray3D::Make(g::Point3D(0, 1, 1), g::Vector3D(1, 0, 0));
  GEOMPP_LOG(INFO) << "Line3D x Ray3D:         " << lineA.DistanceTo(rayB);

  auto segB = g::LineSegment3D::Make(g::Point3D(0, 1, 1), g::Point3D(1, 1, 1));
  GEOMPP_LOG(INFO) << "Line3D x LineSegment3D: " << lineA.DistanceTo(segB);

  // the closest-approach connecting segment, instead of just the scalar
  auto connector = lineA.Distance(lineB);
  GEOMPP_LOG(INFO) << "Line3D.Distance(Line3D): " << connector->ToWkt();

  // Polygon2D / Polygon3D — distance to an infinite line (zero if the line crosses)
  auto square = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto far_line = g::Line2D::Make(g::Point2D(6, -1), g::Point2D(6, 5));
  GEOMPP_LOG(INFO) << "distance_to(Polygon2D, Line2D): " << g::distance_to(square, far_line);
  ```

  ```bash
  Line3D x Line3D:        1.41421
  Line3D x Ray3D:         1.41421
  Line3D x LineSegment3D: 1.41421
  Line3D.Distance(Line3D): LINESTRING (0 0 0, 0 1 1)
  distance_to(Polygon2D, Line2D): 2
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  lineA = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 0, 0))
  lineB = g.Line3D.make(g.Point3D(0, 1, 1), g.Point3D(1, 1, 1))  # parallel, offset sqrt(2)
  print(f"Line3D x Line3D:        {lineA.distance_to(lineB):.3f}")

  rayB = g.Ray3D.make(g.Point3D(0, 1, 1), g.Vector3D(1, 0, 0))
  print(f"Line3D x Ray3D:         {lineA.distance_to(rayB):.3f}")

  segB = g.LineSegment3D.make(g.Point3D(0, 1, 1), g.Point3D(1, 1, 1))
  print(f"Line3D x LineSegment3D: {lineA.distance_to(segB):.3f}")

  # the closest-approach connecting segment, instead of just the scalar
  connector = lineA.distance(lineB)
  print(f"Line3D.distance(Line3D): {connector.to_wkt()}")

  # Polygon2D / Polygon3D — distance to an infinite line (zero if the line crosses)
  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  far_line = g.Line2D.make(g.Point2D(6, -1), g.Point2D(6, 5))
  print(f"distance_to(Polygon2D, Line2D): {g.distance_to(square, far_line):.3f}")
  ```

  ```
  Line3D x Line3D:        1.414
  Line3D x Ray3D:         1.414
  Line3D x LineSegment3D: 1.414
  Line3D.distance(Line3D): LINESTRING (0 0 0, 0 1 1)
  distance_to(Polygon2D, Line2D): 2.000
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  var lineA = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
  var lineB = Line3D.Make(new Point3D(0, 1, 1), new Point3D(1, 1, 1));  // parallel, offset sqrt(2)
  Console.WriteLine($"Line3D x Line3D:        {lineA.DistanceTo(lineB):F3}");

  var rayB = Ray3D.Make(new Point3D(0, 1, 1), new Vector3D(1, 0, 0));
  Console.WriteLine($"Line3D x Ray3D:         {lineA.DistanceTo(rayB):F3}");

  var segB = LineSegment3D.Make(new Point3D(0, 1, 1), new Point3D(1, 1, 1));
  Console.WriteLine($"Line3D x LineSegment3D: {lineA.DistanceTo(segB):F3}");

  // the closest-approach connecting segment, instead of just the scalar
  var connector = lineA.Distance(lineB);
  Console.WriteLine($"Line3D.Distance(Line3D): {connector.ToWkt()}");

  // Polygon2D / Polygon3D — distance to an infinite line (zero if the line crosses)
  var square = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) });
  var farLine = Line2D.Make(new Point2D(6, -1), new Point2D(6, 5));
  Console.WriteLine($"GeomUtil.DistanceTo(Polygon2D, Line2D): {GeomUtil.DistanceTo(square, farLine):F3}");
  ```

  ```
  Line3D x Line3D:        1.414
  Line3D x Ray3D:         1.414
  Line3D x LineSegment3D: 1.414
  Line3D.Distance(Line3D): LINESTRING (0 0 0, 0 1 1)
  GeomUtil.DistanceTo(Polygon2D, Line2D): 2.000
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 7. Polygon tangents</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 7.1 Point to Polygon</b></summary>

  `tangents_to(polygon, point)` returns `PolygonTangents<LineSegment>{left, right}` — the two tangent
  segments from an external point to a polygon (the point's "line of sight" grazing the shape on
  either side, like a taut string pulled around it). Convex polygons use Daniel Sunday's O(log n)
  binary search; non-convex polygons are reduced to their convex hull first (a tangent point can only
  ever be a hull vertex — a reflex vertex always has the polygon on both sides of it, so it can never
  support a tangent line) and the result is mapped back to the original vertex.

  The point must be strictly outside the polygon and not equal to any of its vertices. For
  `Polygon3D`, a tangent is inherently a planar concept — unlike `distance_to`, there is no "skew"
  fallback — so the point must lie in the polygon's own plane, or the call throws `std::logic_error`.

  <p align="center">
    <img src="./images/img_7-1-tangents-point.png" width="420" alt="A point outside a square with its two grazing tangent segments to the square's vertices">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  auto square = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto t2 = g::tangents_to(square, g::Point2D(10, -2));
  GEOMPP_LOG(INFO) << "Polygon2D left:  " << t2.left.ToWkt();
  GEOMPP_LOG(INFO) << "Polygon2D right: " << t2.right.ToWkt();

  // Polygon3D requires the point to be coplanar with the polygon (here, the z=0 plane)
  auto square3 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto t3 = g::tangents_to(square3, g::Point3D(10, -2, 0));
  GEOMPP_LOG(INFO) << "Polygon3D left:  " << t3.left.ToWkt();
  GEOMPP_LOG(INFO) << "Polygon3D right: " << t3.right.ToWkt();
  ```

  ```bash
  Polygon2D left:  LINESTRING (10 -2, 0 0)
  Polygon2D right: LINESTRING (10 -2, 4 4)
  Polygon3D left:  LINESTRING (10 -2 0, 0 0 0)
  Polygon3D right: LINESTRING (10 -2 0, 4 4 0)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  t2 = g.tangents_to(square, g.Point2D(10, -2))
  print(f"Polygon2D left:  {t2.left.to_wkt()}")
  print(f"Polygon2D right: {t2.right.to_wkt()}")

  # Polygon3D requires the point to be coplanar with the polygon (here, the z=0 plane)
  square3 = g.Polygon3D.make([
      g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(4, 4, 0), g.Point3D(0, 4, 0)])
  t3 = g.tangents_to(square3, g.Point3D(10, -2, 0))
  print(f"Polygon3D left:  {t3.left.to_wkt()}")
  print(f"Polygon3D right: {t3.right.to_wkt()}")
  ```

  ```
  Polygon2D left:  LINESTRING (10 -2, 0 0)
  Polygon2D right: LINESTRING (10 -2, 4 4)
  Polygon3D left:  LINESTRING (10 -2 0, 0 0 0)
  Polygon3D right: LINESTRING (10 -2 0, 4 4 0)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  var square = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) });
  var t2 = GeomUtil.TangentsTo(square, new Point2D(10, -2));
  Console.WriteLine($"Polygon2D left:  {t2.Left.ToWkt()}");
  Console.WriteLine($"Polygon2D right: {t2.Right.ToWkt()}");

  // Polygon3D requires the point to be coplanar with the polygon (here, the z=0 plane)
  var square3 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) });
  var t3 = GeomUtil.TangentsTo(square3, new Point3D(10, -2, 0));
  Console.WriteLine($"Polygon3D left:  {t3.Left.ToWkt()}");
  Console.WriteLine($"Polygon3D right: {t3.Right.ToWkt()}");
  ```

  ```
  Polygon2D left:  LINESTRING (10 -2, 0 0)
  Polygon2D right: LINESTRING (10 -2, 4 4)
  Polygon3D left:  LINESTRING (10 -2 0, 0 0 0)
  Polygon3D right: LINESTRING (10 -2 0, 4 4 0)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 7.2 Polygon to Polygon</b></summary>

  `tangents_to(polygon, other)` returns the two common outer tangent segments between two polygons —
  the "belt around two pulleys" lines that touch both shapes without crossing either. Neither polygon
  needs to be convex: each is independently reduced to its convex hull when needed, same as the
  point overload above. For `Polygon3D`, both polygons must share the same plane (two polygons in
  general 3D position don't have a single well-defined common tangent line), or the call throws
  `std::logic_error`.

  <p align="center">
    <img src="./images/img_7-2-tangents-polygon.png" width="420" alt="Two squares with their common outer belt tangent segments">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  auto squareA = g::Polygon2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto squareB = g::Polygon2D::Make(
      {g::Point2D(10, 1), g::Point2D(14, 1), g::Point2D(14, 5), g::Point2D(10, 5)});
  auto t2 = g::tangents_to(squareA, squareB);
  GEOMPP_LOG(INFO) << "Polygon2D left:  " << t2.left.ToWkt();
  GEOMPP_LOG(INFO) << "Polygon2D right: " << t2.right.ToWkt();
  ```

  ```bash
  Polygon2D left:  LINESTRING (0 4, 10 5)
  Polygon2D right: LINESTRING (4 0, 14 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  square_a = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  square_b = g.Polygon2D.make([
      g.Point2D(10, 1), g.Point2D(14, 1), g.Point2D(14, 5), g.Point2D(10, 5)])
  t2 = g.tangents_to(square_a, square_b)
  print(f"Polygon2D left:  {t2.left.to_wkt()}")
  print(f"Polygon2D right: {t2.right.to_wkt()}")
  ```

  ```
  Polygon2D left:  LINESTRING (0 4, 10 5)
  Polygon2D right: LINESTRING (4 0, 14 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  var squareA = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) });
  var squareB = Polygon2D.Make(new Point2D[] { new(10, 1), new(14, 1), new(14, 5), new(10, 5) });
  var t2 = GeomUtil.TangentsTo(squareA, squareB);
  Console.WriteLine($"Polygon2D left:  {t2.Left.ToWkt()}");
  Console.WriteLine($"Polygon2D right: {t2.Right.ToWkt()}");
  ```

  ```
  Polygon2D left:  LINESTRING (0 4, 10 5)
  Polygon2D right: LINESTRING (4 0, 14 1)
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 8. Polyline operations</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 8.1 Decimation / reduction</b></summary>

  `Polyline2D::Reduce(settings)` and `Polyline3D::Reduce(settings)` return a copy of the polyline with
  fewer vertices, per a `PolylineDecimationParams` bundling a `PolylineDecimationParams::Strategy` and
  a `threshold`:

  - `RadialDistance` — O(n) brute-force pass: drops a vertex if it's closer than `threshold` to the
    last *kept* vertex. Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as
    the sole strategy when shape fidelity matters.

  <p align="center">
    <img src="./images/dist_decimation_mechanism.png" width="560" alt="How RadialDistance decides: a point survives only if it is farther than the threshold from the last kept point"><br>
  </p>

  - `RamerDouglasPeucker` — O(n log n) to O(n²): recursively drops vertices closer than `threshold`
    to the chord spanning their segment. Given points P1, P2, P3, drops P2 when its perpendicular
    distance from the P1-P3 chord is below `threshold`; otherwise keeps P2 and recurses on both
    halves. Best general-purpose choice — preserves the vertices that most define the polyline's shape.

  <p align="center">
    <img src="./images/rdp_decimation_mechanism.png" width="560" alt="How RamerDouglasPeucker decides: keeps the point with the largest perpendicular distance from the chord"><br>
  </p>


  - `VisvalingamWhyatt` — O(n log n) to O(n²): repeatedly drops the vertex forming the smallest-area
    triangle with its neighbors, while that area stays below `threshold`, then re-evaluates the
    neighbors. Tends to preserve visually significant features (sharp spikes) better than
    `RadialDistance` while being similarly simple to reason about.


  <p align="center">
    <img src="./images/vw_decimation_mechanism.png" width="560" alt="How VisvalingamWhyatt decides: repeatedly removes the point forming the smallest-area triangle with its neighbors">
  </p>


  `settings` is optional and defaults to `{RamerDouglasPeucker, 0.5}`, so `Reduce()` with no arguments
  keeps working.


  Here is an example done with Jupyter Notebook in 2D of polyline expansion and reduction. 

  <p align="center">
    <img src="./images/polyline_notepad.png" width="560" alt="sample code to expand or reduce a polyline with default settings">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  // a triangular "spike" on an otherwise straight path
  auto spike = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(4, 5), g::Point2D(6, 0), g::Point2D(8, 0)});

  auto rdp = spike.Reduce({.strategy = g::PolylineDecimationParams::Strategy::RamerDouglasPeucker, .threshold = 2.0});
  GEOMPP_LOG(INFO) << "RamerDouglasPeucker (" << spike.Size() << " -> " << rdp.Size() << "): " << rdp.ToWkt();

  auto vw = spike.Reduce({.strategy = g::PolylineDecimationParams::Strategy::VisvalingamWhyatt, .threshold = 6.0});
  GEOMPP_LOG(INFO) << "VisvalingamWhyatt   (" << spike.Size() << " -> " << vw.Size() << "): " << vw.ToWkt();

  // a noisy path: tight clusters of near-duplicate points around two real vertices. Note the clusters
  // aren't collinear with the endpoints — Polyline2D::Make() prunes exactly collinear knots at
  // construction time, so a flat clustered dataset would already collapse before Reduce() runs.
  auto noisy = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(0.1, 0.05), g::Point2D(0.2, -0.05),
       g::Point2D(5, 5), g::Point2D(5.1, 5.05), g::Point2D(10, 0)});
  auto radial = noisy.Reduce({.strategy = g::PolylineDecimationParams::Strategy::RadialDistance, .threshold = 1.0});
  GEOMPP_LOG(INFO) << "RadialDistance      (" << noisy.Size() << " -> " << radial.Size() << "): " << radial.ToWkt();
  ```

  ```bash
  RamerDouglasPeucker (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  VisvalingamWhyatt   (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  RadialDistance      (6 -> 3): LINESTRING (0 0, 5 5, 10 0)
  ```

  `Reduce()` dispatches to three free functions in `calc_utils2d.hpp` that do the actual work —
  `dist_decimation(points, threshold)`, `rdp_decimation(points, threshold)`, and
  `vw_decimation(points, threshold)`. Each is templated over any `PointContainer` of `Point2D` or
  `Point3D` (explicit-instantiated for `std::vector` of either), so they're also available directly
  when you want to decimate a raw point list without constructing a `Polyline` first.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # a triangular "spike" on an otherwise straight path
  spike = g.Polyline2D.make([
      g.Point2D(0, 0), g.Point2D(2, 0), g.Point2D(4, 5), g.Point2D(6, 0), g.Point2D(8, 0)])

  rdp = spike.reduce(g.PolylineDecimationParams(strategy=g.PolylineDecimationStrategy.RamerDouglasPeucker, threshold=2.0))
  print(f"RamerDouglasPeucker ({spike.size()} -> {rdp.size()}): {rdp.to_wkt()}")

  vw = spike.reduce(g.PolylineDecimationParams(strategy=g.PolylineDecimationStrategy.VisvalingamWhyatt, threshold=6.0))
  print(f"VisvalingamWhyatt   ({spike.size()} -> {vw.size()}): {vw.to_wkt()}")

  # a noisy path: tight clusters of near-duplicate points around two real vertices. Note the clusters
  # aren't collinear with the endpoints — Polyline2D.make() prunes exactly collinear knots at
  # construction time, so a flat clustered dataset would already collapse before reduce() runs.
  noisy = g.Polyline2D.make([
      g.Point2D(0, 0), g.Point2D(0.1, 0.05), g.Point2D(0.2, -0.05),
      g.Point2D(5, 5), g.Point2D(5.1, 5.05), g.Point2D(10, 0)])
  radial = noisy.reduce(g.PolylineDecimationParams(strategy=g.PolylineDecimationStrategy.RadialDistance, threshold=1.0))
  print(f"RadialDistance      ({noisy.size()} -> {radial.size()}): {radial.to_wkt()}")
  ```

  ```
  RamerDouglasPeucker (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  VisvalingamWhyatt   (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  RadialDistance      (6 -> 3): LINESTRING (0 0, 5 5, 10 0)
  ```

  `reduce()` dispatches to three free functions that do the actual work —
  `dist_decimation(points, threshold)`, `rdp_decimation(points, threshold)`, and
  `vw_decimation(points, threshold)` — each bound for both `Point2D` and `Point3D` lists. Call them
  directly when you want to decimate a raw point list without constructing a `Polyline` first.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  // a triangular "spike" on an otherwise straight path
  var spike = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(4, 5), new(6, 0), new(8, 0) });

  var rdp = spike.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RamerDouglasPeucker, 2.0));
  Console.WriteLine($"RamerDouglasPeucker ({spike.Size()} -> {rdp.Size()}): {rdp.ToWkt()}");

  var vw = spike.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.VisvalingamWhyatt, 6.0));
  Console.WriteLine($"VisvalingamWhyatt   ({spike.Size()} -> {vw.Size()}): {vw.ToWkt()}");

  // a noisy path: tight clusters of near-duplicate points around two real vertices. Note the clusters
  // aren't collinear with the endpoints — Polyline2D.Make() prunes exactly collinear knots at
  // construction time, so a flat clustered dataset would already collapse before Reduce() runs.
  var noisy = Polyline2D.Make(new Point2D[] {
    new(0, 0), new(0.1, 0.05), new(0.2, -0.05), new(5, 5), new(5.1, 5.05), new(10, 0) });
  var radial = noisy.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RadialDistance, 1.0));
  Console.WriteLine($"RadialDistance      ({noisy.Size()} -> {radial.Size()}): {radial.ToWkt()}");
  ```

  ```
  RamerDouglasPeucker (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  VisvalingamWhyatt   (5 -> 3): LINESTRING (0 0, 4 5, 8 0)
  RadialDistance      (6 -> 3): LINESTRING (0 0, 5 5, 10 0)
  ```

  `Reduce()` dispatches to three static `GeomUtil` methods that do the actual work —
  `GeomUtil.DistDecimation(points, threshold)`, `GeomUtil.RdpDecimation(points, threshold)`, and
  `GeomUtil.VwDecimation(points, threshold)` — each overloaded for `List<Point2D>` and
  `List<Point3D>`. Call them directly when you want to decimate a raw point list without
  constructing a `Polyline` first.

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 8.2 Corner smoothing with Bezier curve</b></summary>

  `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance_or_num_segments)` rounds the corner at
  `p1` with a quadratic Bezier arc tangent to `p0-p1` and `p1-p2`. `smoothness` in `[0, 1]` controls
  how much of the shorter adjacent edge is trimmed into the arc's tangent points (`0` leaves the
  corner sharp, `1` trims half of the shorter edge). Two overloads control how densely the arc is
  sampled — resolved by the type of the last argument, not a shared parameter:

  - a `double` **min_distance** samples roughly that far apart, however many points that takes.
  - an `int` **num_segments** samples exactly that many segments (`num_segments + 1` points),
    regardless of the arc's length.

  <p align="center">
    <img src="./images/img_8-2-bezier-corner.png" width="420" alt="A single right-angle corner rounded into a Bezier arc with its 4 sampled points marked">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  auto p0 = g::Point2D(0, 0);
  auto p1 = g::Point2D(2, 0);
  auto p2 = g::Point2D(2, 2);

  // density-based: samples roughly min_distance apart
  auto by_distance = g::bezier_smoothing_2(p0, p1, p2, /*smoothness=*/1.0, /*min_distance=*/0.5);
  GEOMPP_LOG(INFO) << "by min_distance (" << by_distance.size() << " points)";

  // count-based: samples an exact number of segments, regardless of arc length
  auto by_count = g::bezier_smoothing_2(p0, p1, p2, /*smoothness=*/1.0, /*num_segments=*/3);
  GEOMPP_LOG(INFO) << "by num_segments (" << by_count.size() << " points)";
  for (auto const& pt : by_count) {
    GEOMPP_LOG(INFO) << "  " << pt.ToWkt();
  }
  ```

  will print out

  ```bash
  by min_distance (4 points)
  by num_segments (4 points)
    POINT (1 0)
    POINT (1.556 0.111)
    POINT (1.889 0.444)
    POINT (2 1)
  ```

  The first and last sampled points are always the trimmed tangent points (`(1, 0)` and `(2, 1)`
  here), not `p1` itself — `p1` is replaced by the arc, unless `smoothness` is `0` (every sample
  collapses to `p1`, a sharp corner). `bezier_smoothing_2` is templated over `PointT` and works the
  same way for `Point3D`.

  A 6th, optional `min_segment_length` parameter (default `DOUBLE_EPSILON`) skips trimming on a side
  whose adjacent edge is at or below that length — that tangent point falls back to `p1` instead. If
  *both* adjacent edges are that short, the whole corner collapses to `p1` (no curve), which is the
  mechanism `Polyline::Expand()` (next) uses to leave tiny/noisy corners sharp.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  p0 = g.Point2D(0, 0)
  p1 = g.Point2D(2, 0)
  p2 = g.Point2D(2, 2)

  # density-based: samples roughly min_distance apart
  by_distance = g.bezier_smoothing_2(p0, p1, p2, 1.0, 0.5)
  print(f"by min_distance ({len(by_distance)} points)")

  # count-based: samples an exact number of segments, regardless of arc length
  by_count = g.bezier_smoothing_2(p0, p1, p2, 1.0, 3)
  print(f"by num_segments ({len(by_count)} points)")
  for pt in by_count:
      print(f"  {pt.to_wkt()}")
  ```

  ```
  by min_distance (4 points)
  by num_segments (4 points)
    POINT (1 0)
    POINT (1.556 0.111)
    POINT (1.889 0.444)
    POINT (2 1)
  ```

  The first and last sampled points are always the trimmed tangent points (`(1, 0)` and `(2, 1)`
  here), not `p1` itself — `p1` is replaced by the arc, unless `smoothness` is `0` (every sample
  collapses to `p1`, a sharp corner). `bezier_smoothing_2` is bound for both `Point2D` and `Point3D`.

  A keyword-only `min_segment_length` argument (default `DOUBLE_EPSILON`) skips trimming on a side
  whose adjacent edge is at or below that length — that tangent point falls back to `p1` instead. If
  *both* adjacent edges are that short, the whole corner collapses to `p1` (no curve), which is the
  mechanism `Polyline.expand()` (next) uses to leave tiny/noisy corners sharp.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  var p0 = new Point2D(0, 0);
  var p1 = new Point2D(2, 0);
  var p2 = new Point2D(2, 2);

  // density-based: samples roughly minDistance apart
  var byDistance = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, smoothness: 1.0, minDistance: 0.5));
  Console.WriteLine($"by minDistance ({byDistance.Count} points)");

  // count-based: samples an exact number of segments, regardless of arc length
  var byCount = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, smoothness: 1.0, numSegments: 3));
  Console.WriteLine($"by numSegments ({byCount.Count} points)");
  foreach (var pt in byCount) {
    Console.WriteLine($"  {pt.ToWkt()}");
  }
  ```

  ```
  by minDistance (4 points)
  by numSegments (4 points)
    POINT (1 0)
    POINT (1.556 0.111)
    POINT (1.889 0.444)
    POINT (2 1)
  ```

  The first and last sampled points are always the trimmed tangent points (`(1, 0)` and `(2, 1)`
  here), not `p1` itself — `p1` is replaced by the arc, unless `smoothness` is `0` (every sample
  collapses to `p1`, a sharp corner). `BezierSmoothing2` is overloaded for `Point2D` and `Point3D`.

  A `minSegmentLength` overload (default `DOUBLE_EPSILON` when omitted) skips trimming on a side whose
  adjacent edge is at or below that length — that tangent point falls back to `p1` instead. If *both*
  adjacent edges are that short, the whole corner collapses to `p1` (no curve), which is the mechanism
  `Polyline.Expand()` (next) uses to leave tiny/noisy corners sharp.

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 8.3 Expand / smooth a polyline</b></summary>

  `Polyline2D::Expand(settings)` / `Polyline3D::Expand(settings)` — the inverse of `Reduce()` — round
  every *inner* corner with a quadratic Bezier arc, via `PolylineExpansionParams`:

  - `smoothness`, `mode` (`FixedSegments` / `MinDistance`), `segments_per_corner`, `min_distance` —
    same meaning as the matching `bezier_smoothing_2` parameters, applied to every corner.
  - `min_segment_length` — forwarded to `bezier_smoothing_2` for every corner; a corner whose adjacent
    edges are both that short stays sharp instead of being rounded.

  `settings` is optional and defaults to `{0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON}`.

  <p align="center">
    <img src="./images/polyline_expansion_corner_rounding.png" width="460" alt="A sharp line-peak-line corner replaced by a smooth Bezier crown, tangent to trimmed points on each side">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  namespace g = geompp;
  g::DECIMAL_PRECISION = g::DP_THREE;

  // a right-angle path with two consecutive corners
  auto path = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2), g::Point2D(0, 2)});

  auto rounded = path.Expand({.smoothness = 1.0, .segments_per_corner = 3});
  GEOMPP_LOG(INFO) << "Expand (" << path.Size() << " -> " << rounded.Size() << "): " << rounded.ToWkt();
  ```

  will print out

  ```bash
  Expand (4 -> 9): LINESTRING (0 0, 1 0, 1.556 0.111, 1.889 0.444, 2 1, 1.889 1.556, 1.556 1.889, 1 2, 0 2)
  ```

  Each of the two inner corners (`(2, 0)` and `(2, 2)`) is replaced by its own trimmed-tangent arc; the
  two arcs share a tangent point exactly at the shared edge's midpoint `(2, 1)` (each corner's trim is
  capped at half of its shared edge), collapsed to one point rather than duplicated. `Expand()` delegates
  to the free function `polyline_expansion(points, settings)`, which rounds a raw point list the same
  way without constructing a `Polyline` first.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # a right-angle path with two consecutive corners
  path = g.Polyline2D.make([
      g.Point2D(0, 0), g.Point2D(2, 0), g.Point2D(2, 2), g.Point2D(0, 2)])

  settings = g.PolylineExpansionParams(smoothness=1.0, segments_per_corner=3)
  rounded = path.expand(settings)
  print(f"Expand ({path.size()} -> {rounded.size()}): {rounded.to_wkt()}")
  ```

  ```
  Expand (4 -> 9): LINESTRING (0 0, 1 0, 1.556 0.111, 1.889 0.444, 2 1, 1.889 1.556, 1.556 1.889, 1 2, 0 2)
  ```

  Each of the two inner corners (`(2, 0)` and `(2, 2)`) is replaced by its own trimmed-tangent arc; the
  two arcs share a tangent point exactly at the shared edge's midpoint `(2, 1)` (each corner's trim is
  capped at half of its shared edge), collapsed to one point rather than duplicated. `expand()`
  dispatches to the free function `polyline_expansion(points, settings)`, which rounds a raw point list
  the same way without constructing a `Polyline` first.

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  // a right-angle path with two consecutive corners
  var path = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2), new(0, 2) });

  var settings = new PolylineExpansionParams(1.0, PolylineExpansionMode.FixedSegments, 3, 0.1, 1e-6);
  var rounded = path.Expand(settings);
  Console.WriteLine($"Expand ({path.Size()} -> {rounded.Size()}): {rounded.ToWkt()}");
  ```

  ```
  Expand (4 -> 9): LINESTRING (0 0, 1 0, 1.556 0.111, 1.889 0.444, 2 1, 1.889 1.556, 1.556 1.889, 1 2, 0 2)
  ```

  Each of the two inner corners (`(2, 0)` and `(2, 2)`) is replaced by its own trimmed-tangent arc; the
  two arcs share a tangent point exactly at the shared edge's midpoint `(2, 1)` (each corner's trim is
  capped at half of its shared edge), collapsed to one point rather than duplicated. `Expand()`
  dispatches to the static `GeomUtil.PolylineExpansion(points, settings)` method that does the actual
  work — call it directly when you want to round a raw point list without constructing a `Polyline`.

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 9. Polygon boolean operations </b></summary>

  `Polygon2D`/`Polygon3D` support four set operations against another polygon of the same type, all
  built on the same planar-arrangement engine (`clip()` / `boolean_op()`) that backs `Intersection(Polygon)`:
  a Bentley–Ottmann sweep finds every crossing between the two operands' edges, the rings are split at
  each one, and every surviving segment is classified inside/outside the other operand (by point
  containment) before the kept segments are traced back into closed result rings.
  **Union, Intersection, Difference, and Xor are the exact same algorithm** — only a tiny inside/outside
  truth table differs between them. The sweep and the final retrace are O(n log n), but classification
  probes each split fragment against the other operand with a plain winding-number scan (no spatial
  index), which is O((n+k)·m) — so **O(n²) worst case**, dominated by classification rather than the sweep.
  Each returns zero or more result polygons — a disjoint pair of operands can split into several pieces —
  and holes are respected on both operands throughout.

  - **Union** — `A.Union(B)`: the combined area covered by either polygon.
  - **Intersection** — `A.Intersection(B)`: the area covered by both polygons (the overlap).
  - **Difference** — `A.Difference(B)`: the area covered by `A` but not `B`. **Non-commutative**:
    `A.Difference(B) != B.Difference(A)` in general — subtracting `B` from `A` keeps `A`'s exclusive
    area, while `B.Difference(A)` keeps `B`'s exclusive area instead.
  - **Xor** — `A.Xor(B)`: the symmetric difference, i.e. the area covered by exactly one of the two
    polygons (equivalent to `Union` minus `Intersection`).

  <p align="center">
    <img src="./images/boolean_union.png" width="280" alt="Union of two overlapping 40-gon circle approximations">
    <img src="./images/boolean_intersection.png" width="280" alt="Intersection of two overlapping 40-gon circle approximations">
  </p>
  <p align="center">
    <img src="./images/boolean_difference.png" width="280" alt="Difference (A minus B) of two overlapping 40-gon circle approximations">
    <img src="./images/boolean_xor.png" width="280" alt="Xor (symmetric difference) of two overlapping 40-gon circle approximations">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "polygon2d.hpp"

  namespace g = geompp;

  // two overlapping 4x4 squares, offset diagonally by (2, 2)
  auto a = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4),
  });
  auto b = g::Polygon2D::Make({
      g::Point2D(2, 2), g::Point2D(6, 2), g::Point2D(6, 6), g::Point2D(2, 6),
  });

  for (auto const& p : a.Union(b))
      GEOMPP_LOG(INFO) << "union:        " << p.ToWkt();

  for (auto const& p : a.Intersection(b))
      GEOMPP_LOG(INFO) << "intersection: " << p.ToWkt();

  for (auto const& p : a.Difference(b))
      GEOMPP_LOG(INFO) << "a - b:        " << p.ToWkt();

  // Difference is non-commutative: b - a != a - b
  for (auto const& p : b.Difference(a))
      GEOMPP_LOG(INFO) << "b - a:        " << p.ToWkt();

  for (auto const& p : a.Xor(b))
      GEOMPP_LOG(INFO) << "xor:          " << p.ToWkt();
  ```

  ```bash
  I20260403] union:        POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0))
  I20260403] intersection: POLYGON ((4 2, 4 4, 2 4, 2 2, 4 2))
  I20260403] a - b:        POLYGON ((0 0, 4 0, 4 2, 2 2, 2 4, 0 4, 0 0))
  I20260403] b - a:        POLYGON ((4 2, 6 2, 6 6, 2 6, 2 4, 4 4, 4 2))
  I20260403] xor:          POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0), (4 2, 2 2, 2 4, 4 4, 4 2))
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # two overlapping 4x4 squares, offset diagonally by (2, 2)
  a = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4),
  ])
  b = g.Polygon2D.make([
      g.Point2D(2, 2), g.Point2D(6, 2), g.Point2D(6, 6), g.Point2D(2, 6),
  ])

  for p in a.union(b):
      print(f"union:        {p.to_wkt()}")

  for p in a.intersection(b):
      print(f"intersection: {p.to_wkt()}")

  for p in a.difference(b):
      print(f"a - b:        {p.to_wkt()}")

  # difference is non-commutative: b - a != a - b
  for p in b.difference(a):
      print(f"b - a:        {p.to_wkt()}")

  for p in a.xor(b):
      print(f"xor:          {p.to_wkt()}")
  ```

  ```
  union:        POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0))
  intersection: POLYGON ((4 2, 4 4, 2 4, 2 2, 4 2))
  a - b:        POLYGON ((0 0, 4 0, 4 2, 2 2, 2 4, 0 4, 0 0))
  b - a:        POLYGON ((4 2, 6 2, 6 6, 2 6, 2 4, 4 4, 4 2))
  xor:          POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0), (4 2, 2 2, 2 4, 4 4, 4 2))
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // two overlapping 4x4 squares, offset diagonally by (2, 2)
  var a = G.Polygon2D.Make(new G.Point2D[] {
      new(0, 0), new(4, 0), new(4, 4), new(0, 4),
  });
  var b = G.Polygon2D.Make(new G.Point2D[] {
      new(2, 2), new(6, 2), new(6, 6), new(2, 6),
  });

  foreach (var p in a.Union(b))
      Console.WriteLine($"union:        {p.ToWkt()}");

  foreach (var p in a.Intersection(b))
      Console.WriteLine($"intersection: {p.ToWkt()}");

  foreach (var p in a.Difference(b))
      Console.WriteLine($"a - b:        {p.ToWkt()}");

  // Difference is non-commutative: b - a != a - b
  foreach (var p in b.Difference(a))
      Console.WriteLine($"b - a:        {p.ToWkt()}");

  foreach (var p in a.Xor(b))
      Console.WriteLine($"xor:          {p.ToWkt()}");
  ```

  ```
  union:        POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0))
  intersection: POLYGON ((4 2, 4 4, 2 4, 2 2, 4 2))
  a - b:        POLYGON ((0 0, 4 0, 4 2, 2 2, 2 4, 0 4, 0 0))
  b - a:        POLYGON ((4 2, 6 2, 6 6, 2 6, 2 4, 4 4, 4 2))
  xor:          POLYGON ((0 0, 4 0, 4 2, 6 2, 6 6, 2 6, 2 4, 0 4, 0 0), (4 2, 2 2, 2 4, 4 4, 4 2))
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; 10. Meshes</b></summary>

  `Mesh2D`/`Mesh3D` (triangle faces) and `PolyMesh2D`/`PolyMesh3D` (arbitrary-sided polygon faces) hold
  a set of adjacent facets built from a list of `Triangle`/`Polygon` inputs. No adjacency structure is
  stored (no "neighboring face" query), and `PolyMesh` facets cannot have holes — `FromPolygons()`
  throws if any input polygon does. `ConnectedMesh2D`/`ConnectedMesh3D` (§10.3) are the same idea as
  `Mesh2D`/`Mesh3D` but precompute per-facet edge adjacency internally, queryable via each facet's
  `FaceView2D`/`FaceView3D`.

  #### The problem: equating "the same" point twice

  A triangle/polygon soup — the raw input to `FromTriangles()`/`FromPolygons()` — repeats every shared
  vertex once per facet that touches it: two triangles sharing an edge each carry their own copy of
  that edge's two endpoints. Building a mesh means collapsing those repeats back into one shared
  vertex per physical point, so that adjacent facets actually reference the same index rather than two
  numerically-almost-equal-but-distinct points. The naive way to detect "is this point one we've
  already seen?" is an `AlmostEquals` scan against every previously-registered point — correct, but
  O(n) per insertion and O(n²) overall for an n-vertex mesh, which gets slow fast on any mesh with more
  than a few hundred facets.

  #### The fix: GridCell

  `GridCell2D`/`GridCell3D` turn that O(n) scan into an O(1) hash-map lookup: floor-divide each
  coordinate by a resolution `epsilon` (`floor(x/ε)`, `floor(y/ε)`, ...) to get an integer cell
  address, and use that address as the hash key. Two points that fall in the same cell are assumed to
  be the same vertex and get welded; a point in a new cell is registered as a new unique vertex. This
  is deliberately an *approximation* of `AlmostEquals`, not a replacement for it — it trades a small,
  well-understood inaccuracy at cell boundaries for O(1) average-case welding:

  - Two points **up to `√2·ε`** (2D) or **`√3·ε`** (3D) apart can still land in the **same** cell and
    get welded, even though that's farther apart than `epsilon` alone would suggest (the cell's
    diagonal, not its side, bounds the worst case).
  - Two points **closer than `epsilon`** can land in **different** cells and stay **distinct**, if
    they happen to fall on opposite sides of a cell boundary.

  `epsilon` defaults to `DOUBLE_EPSILON`, which tracks the same thread-local `DECIMAL_PRECISION` every
  `AlmostEquals` call in the library already uses — so tightening/loosening precision globally also
  tightens/loosens mesh vertex welding, even though the comparison *algorithm* (grid bucketing vs.
  direct distance) differs.

  <p align="center">
    <img src="./images/grid_cell.png" width="420" alt="GridCell2D bucketing: near-duplicate points inside one cell weld to a single vertex; points straddling a cell boundary stay distinct even when closer together">
  </p>

  #### The other invariant: every edge has at most 1 neighbor

  Welding coincident points isn't the only thing `FromTriangles()`/`FromPolygons()` guarantee. Every one
  of them (`Mesh2D/3D`, `PolyMesh2D/3D`, `ConnectedMesh2D/3D`) also rejects a **non-conforming** input: a
  facet vertex is never allowed to land in the *interior* of another facet's edge — it may only touch a
  neighboring facet exactly at that edge's own start or end vertex. Equivalently: every edge, across the
  whole set of facets, has at most 1 neighbor (a boundary edge has 0, a normal shared interior edge has
  1). This is the same rule known elsewhere as a "conforming mesh" / no "hanging nodes" (FEM, finite element method), 
  no "T-junctions" (graphics), or a valid PSLG (planar straight line graph) mesh generation — 
  not something invented for this library.

  Two distinct violations get checked for, and only one of them is fixable:
  - A **T-junction** — a vertex partially overlapping an edge (a wall's corner landing halfway along a
    longer neighboring wall instead of meeting it exactly) — is fixable: the missing vertex can be
    spliced into the coarse edge, same shape, same area, one extra flat-180°-angle vertex.
  - A **non-manifold edge** — a *full* edge shared by 3 or more facets, not just 1 — is not fixable:
    there's no principled way to guess which 2 of the 3+ facets are "the real pair" that should share it.

  `validate_adjacency(facets)` reports every violation found (empty = conforming); `fix_adjacency(facets)`
  repairs every T-junction it can and throws on the first non-manifold edge, since that one genuinely has
  no valid automatic fix. `Mesh2D/3D::FromTriangles()`, `PolyMesh2D/3D::FromPolygons()`, and
  `ConnectedMesh2D/3D::FromTriangles()` all call `validate_adjacency()` unconditionally and throw on any
  violation — a non-conforming mesh is treated as invalid caller input at construction time, never
  silently repaired. The batch `triangulate(vector<Polygon2D>, ...)` free function (§11) is the one place
  that repairs instead of rejecting by default — see there for why.

<details open>
<summary><b> &nbsp; &nbsp; 10.1 Triangle mesh (Mesh2D / Mesh3D)</b></summary>

  Every facet is a `Triangle2D`/`Triangle3D` — always convex, always planar (in 3D, a triangle can't
  be non-planar), and cheap to construct and reason about. `FromTriangles()` welds shared vertices via
  `GridCell2D/3D` (see above) and stores each facet as a fixed `array<size_t, 3>` index triple — no
  variable-length bookkeeping.

  **Advantages**: fixed-size face records mean no indirection to find a facet's vertex count, and
  triangulating arbitrary geometry (terrain, scanned surfaces, subdivision output) is a well-trodden
  problem with mature algorithms to draw from. **Trade-off**: representing anything with flat faces
  wider than 3 vertices (a cube's square side, say) means splitting it into 2+ triangles up front —
  extra vertices/edges that convey no extra geometric information, and a seam down the middle of what
  is conceptually one flat face.

  <p align="center">
    <img src="./images/mesh3d.png" width="420" alt="Mesh3D: a triangulated 3D dome, 18 triangular facets welded from 16 unique vertices">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "mesh2d.hpp"

  namespace g = geompp;

  // two triangles sharing an edge — a unit square split along its diagonal
  auto t0 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1));
  auto t1 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t0, t1});

  GEOMPP_LOG(INFO) << "facets: " << mesh.Size() << ", area: " << mesh.Area();
  for (std::size_t i = 0; i < mesh.Size(); ++i)
      GEOMPP_LOG(INFO) << "face " << i << ": " << mesh[i].ToWkt();
  ```

  ```bash
  I20260725] facets: 2, area: 1
  I20260725] face 0: TRIANGLE (0 0, 1 0, 1 1)
  I20260725] face 1: TRIANGLE (0 0, 1 1, 0 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # two triangles sharing an edge — a unit square split along its diagonal
  t0 = g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(1, 0), g.Point2D(1, 1))
  t1 = g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(1, 1), g.Point2D(0, 1))
  mesh = g.Mesh2D.from_triangles([t0, t1])

  print(f"facets: {mesh.size()}, area: {mesh.area()}")
  for i, face in enumerate(mesh):
      print(f"face {i}: {face.to_wkt()}")
  ```

  ```
  facets: 2, area: 1.0
  face 0: TRIANGLE (0 0, 1 0, 1 1)
  face 1: TRIANGLE (0 0, 1 1, 0 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // two triangles sharing an edge — a unit square split along its diagonal
  var t0 = G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 0), new G.Point2D(1, 1));
  var t1 = G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 1), new G.Point2D(0, 1));
  var mesh = G.Mesh2D.FromTriangles(new[] { t0, t1 });

  Console.WriteLine($"facets: {mesh.Size()}, area: {mesh.Area()}");
  for (int i = 0; i < mesh.Size(); ++i)
      Console.WriteLine($"face {i}: {mesh[i].ToWkt()}");
  ```

  ```
  facets: 2, area: 1
  face 0: TRIANGLE (0 0, 1 0, 1 1)
  face 1: TRIANGLE (0 0, 1 1, 0 1)
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 10.2 Polygon mesh (PolyMesh2D / PolyMesh3D)</b></summary>

  Every facet is a `Polygon2D`/`Polygon3D` of whatever vertex count it naturally has — a quad stays a
  quad, a hexagon stays a hexagon. `FromPolygons()` welds shared vertices the same way `Mesh` does, but
  stores each facet as a *run* into a flat, variable-length index buffer (`FACE_INDICES` sliced by
  `FACE_IDX_BEGINS`/`FACE_IDX_OFFSETS`) instead of a fixed `array<size_t, 3>`, since facets no longer
  all have the same vertex count.

  **Advantages**: one facet per real flat surface — no artificial split-triangle seam, and facet count
  stays proportional to the geometry's actual faces rather than 2-3× as many for anything not
  already triangular (a hexagonal prism's 2 caps are 1 facet each here, 4 triangles each as a `Mesh`).
  This matches how CAD/BIM/architectural geometry is usually authored (rooms, panels, walls — genuinely
  flat, multi-sided faces) and is the natural fit for boolean-op or extrusion pipelines built on
  `Polygon2D/3D` already. **Trade-off**: facets aren't guaranteed convex or (in 3D) planar the way a
  triangle always is — `Polygon2D::Make()`/`Polygon3D::Make()` still validate simplicity but a
  near-planar quad from noisy input can be a worse approximation of the "real" flat surface than an
  explicit triangulation would be; the variable-length index buffer also costs one extra indirection
  (begin/offset lookup) per facet access versus `Mesh`'s direct fixed-size array.

  <p align="center">
    <img src="./images/polymesh3d.png" width="420" alt="PolyMesh3D: a hexagonal prism, 8 facets (2 hexagons + 6 quads) welded from 12 unique vertices">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "polymesh2d.hpp"

  namespace g = geompp;

  // two quads sharing an edge — a 2x1 rectangle split down the middle
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  GEOMPP_LOG(INFO) << "facets: " << mesh.Size() << ", area: " << mesh.Area();
  for (std::size_t i = 0; i < mesh.Size(); ++i)
      GEOMPP_LOG(INFO) << "face " << i << ": " << mesh[i].ToWkt();
  ```

  ```bash
  I20260725] facets: 2, area: 2
  I20260725] face 0: POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))
  I20260725] face 1: POLYGON ((1 0, 2 0, 2 1, 1 1, 1 0))
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # two quads sharing an edge — a 2x1 rectangle split down the middle
  p0 = g.Polygon2D.make([g.Point2D(0, 0), g.Point2D(1, 0), g.Point2D(1, 1), g.Point2D(0, 1)])
  p1 = g.Polygon2D.make([g.Point2D(1, 0), g.Point2D(2, 0), g.Point2D(2, 1), g.Point2D(1, 1)])
  mesh = g.PolyMesh2D.from_polygons([p0, p1])

  print(f"facets: {mesh.size()}, area: {mesh.area()}")
  for i, face in enumerate(mesh):
      print(f"face {i}: {face.to_wkt()}")
  ```

  ```
  facets: 2, area: 2.0
  face 0: POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))
  face 1: POLYGON ((1 0, 2 0, 2 1, 1 1, 1 0))
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // two quads sharing an edge — a 2x1 rectangle split down the middle
  var p0 = G.Polygon2D.Make(new G.Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
  var p1 = G.Polygon2D.Make(new G.Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
  var mesh = G.PolyMesh2D.FromPolygons(new[] { p0, p1 });

  Console.WriteLine($"facets: {mesh.Size()}, area: {mesh.Area()}");
  for (int i = 0; i < mesh.Size(); ++i)
      Console.WriteLine($"face {i}: {mesh[i].ToWkt()}");
  ```

  ```
  facets: 2, area: 2
  face 0: POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))
  face 1: POLYGON ((1 0, 2 0, 2 1, 1 1, 1 0))
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 10.3 Connected mesh (ConnectedMesh2D / ConnectedMesh3D)</b></summary>

  Same triangle-faced mesh as `Mesh2D`/`Mesh3D` — `FromTriangles()` welds shared vertices via
  `GridCell2D`/`GridCell3D` the same way — but `ConnectedMesh2D`/`ConnectedMesh3D` additionally
  precompute, for every facet's 3 edges, which other facet (and which of *its* edges) sits across that
  edge, via a compact 4-byte `detail::TriangleCompactNeighborRef` per edge (`{triangle_id,
  local_edge_id}`, or a boundary sentinel if the edge has no twin). That adjacency is built by an
  edge-key hashmap over the welded triangle indices during `FromTriangles()` itself, so it costs
  nothing to query later.

  Indexing/iterating a `ConnectedMesh2D`/`ConnectedMesh3D` hands back a **`FaceView2D`**/**`FaceView3D`**
  rather than a bare `Triangle2D`/`Triangle3D`: a lightweight, chainable handle onto one facet that
  exposes both its geometry and that precomputed adjacency:
  - `Geometry()` — the facet rebuilt as a `Triangle2D`/`Triangle3D` (same as `Mesh2D/3D`'s plain
    accessor).
  - `Neighbor(edge)` — crosses one of the facet's 3 edges (`FIRST`/`SECOND`/`THIRD`, numbered
    `v0`-`v1`, `v1`-`v2`, `v2`-`v0`) and returns the `FaceView2D`/`FaceView3D` on the other side, or
    nothing if that edge is a boundary edge with no twin (`std::nullopt` in C++, `None` in Python,
    `null` in C#). Calls chain: `face.Neighbor(edge)->Geometry()`.
  - `NeighborEntryEdge(edge)` — which edge of `Neighbor(edge)` was entered through (`INVALID` at a
    boundary), so a caller can immediately cross back with `Neighbor(entry_edge)`.
  - `TriangleEdge` (`FIRST`/`SECOND`/`THIRD`/`INVALID`) is one shared enum, used by both the 2D and 3D
    `FaceView`.

  A `FaceView2D`/`FaceView3D` must not outlive the `ConnectedMesh2D`/`ConnectedMesh3D` it came from (in
  Python/C# this is enforced for you — see the bindings' lifetime notes — but it's still your job in
  C++).

  <p align="center">
    <img src="./images/connected_mesh3d.png" width="380" alt="ConnectedMesh3D: the same dome mesh as Mesh3D, with one facet's 3 edges bolded in yellow and the 3 facets across those edges — its precomputed edge-adjacency neighbors — shaded yellow instead of teal">
    &nbsp;&nbsp;
    <img src="./images/connected_mesh2d_navigation.png" width="380" alt="ConnectedMesh2D FaceView navigation: a fan of 4 triangles highlighted gold end to end, with arrows crossing each shared edge labeled THIRD to FIRST, walking from face 0 (start) to face 3 (end, a boundary)">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "connected_mesh2d.hpp"

  namespace g = geompp;
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;  // FIRST=1, SECOND=2, THIRD=3

  // A fan of 4 triangles sharing the origin, each welded to the next along its THIRD edge.
  auto mesh = g::ConnectedMesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(0, 1), g::Point2D(-1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(-1, 1), g::Point2D(-1, 0)),
  });
  GEOMPP_LOG(INFO) << "facets: " << mesh.Size() << ", area: " << mesh.Area();

  // Walk the mesh left to right via Neighbor()/NeighborEntryEdge(), starting from face 0.
  auto face = mesh[0];
  GEOMPP_LOG(INFO) << "start at face " << face.ID() << ": " << face.Geometry().ToWkt();
  while (true) {
      auto next = face.Neighbor(Edge::THIRD);
      if (!next.has_value()) {
          GEOMPP_LOG(INFO) << "face " << face.ID() << ".THIRD is a boundary -- stop";
          break;
      }
      GEOMPP_LOG(INFO) << "cross face " << face.ID() << ".THIRD -> face " << next->ID()
                        << " (entered via " << static_cast<int>(face.NeighborEntryEdge(Edge::THIRD))
                        << "): " << next->Geometry().ToWkt();
      face = *next;
  }
  ```

  ```bash
  I20260729] facets: 4, area: 2
  I20260729] start at face 0: TRIANGLE (0 0, 1 0, 1 1)
  I20260729] cross face 0.THIRD -> face 1 (entered via 1): TRIANGLE (0 0, 1 1, 0 1)
  I20260729] cross face 1.THIRD -> face 2 (entered via 1): TRIANGLE (0 0, 0 1, -1 1)
  I20260729] cross face 2.THIRD -> face 3 (entered via 1): TRIANGLE (0 0, -1 1, -1 0)
  I20260729] face 3.THIRD is a boundary -- stop
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # A fan of 4 triangles sharing the origin, each welded to the next along its THIRD edge.
  mesh = g.ConnectedMesh2D.from_triangles([
      g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(1, 0), g.Point2D(1, 1)),
      g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(1, 1), g.Point2D(0, 1)),
      g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(0, 1), g.Point2D(-1, 1)),
      g.Triangle2D.make(g.Point2D(0, 0), g.Point2D(-1, 1), g.Point2D(-1, 0)),
  ])
  print(f"facets: {mesh.size()}, area: {mesh.area()}")

  # Walk the mesh left to right via neighbor()/neighbor_entry_edge(), starting from face 0.
  face = mesh[0]
  print(f"start at face {face.id()}: {face.geometry().to_wkt()}")
  while True:
      next_face = face.neighbor(g.TriangleEdge.THIRD)
      if next_face is None:
          print(f"face {face.id()}.THIRD is a boundary -- stop")
          break
      entry = face.neighbor_entry_edge(g.TriangleEdge.THIRD)
      print(f"cross face {face.id()}.THIRD -> face {next_face.id()} (entered via {entry.name}): "
            f"{next_face.geometry().to_wkt()}")
      face = next_face
  ```

  ```
  facets: 4, area: 2.0
  start at face 0: TRIANGLE (0 0, 1 0, 1 1)
  cross face 0.THIRD -> face 1 (entered via FIRST): TRIANGLE (0 0, 1 1, 0 1)
  cross face 1.THIRD -> face 2 (entered via FIRST): TRIANGLE (0 0, 0 1, -1 1)
  cross face 2.THIRD -> face 3 (entered via FIRST): TRIANGLE (0 0, -1 1, -1 0)
  face 3.THIRD is a boundary -- stop
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // A fan of 4 triangles sharing the origin, each welded to the next along its Third edge.
  var mesh = G.ConnectedMesh2D.FromTriangles(new[] {
      G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 0), new G.Point2D(1, 1)),
      G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(1, 1), new G.Point2D(0, 1)),
      G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(0, 1), new G.Point2D(-1, 1)),
      G.Triangle2D.Make(new G.Point2D(0, 0), new G.Point2D(-1, 1), new G.Point2D(-1, 0)),
  });
  Console.WriteLine($"facets: {mesh.Size()}, area: {mesh.Area()}");

  // Walk the mesh left to right via Neighbor()/NeighborEntryEdge(), starting from face 0.
  var face = mesh[0];
  Console.WriteLine($"start at face {face.Id()}: {face.Geometry().ToWkt()}");
  while (true) {
      var next = face.Neighbor(G.TriangleEdge.Third);
      if (next == null) {
          Console.WriteLine($"face {face.Id()}.Third is a boundary -- stop");
          break;
      }
      var entry = face.NeighborEntryEdge(G.TriangleEdge.Third);
      Console.WriteLine($"cross face {face.Id()}.Third -> face {next.Id()} (entered via {entry}): "
                       + $"{next.Geometry().ToWkt()}");
      face = next;
  }
  ```

  ```
  facets: 4, area: 2
  start at face 0: TRIANGLE (0 0, 1 0, 1 1)
  cross face 0.Third -> face 1 (entered via First): TRIANGLE (0 0, 1 1, 0 1)
  cross face 1.Third -> face 2 (entered via First): TRIANGLE (0 0, 0 1, -1 1)
  cross face 2.Third -> face 3 (entered via First): TRIANGLE (0 0, -1 1, -1 0)
  face 3.Third is a boundary -- stop
  ```

   </details>

  </details>

</details>

<details open>
<summary><b> &nbsp; &nbsp; 10.4 Converting between mesh representations</b></summary>

  The three mesh types above share the same underlying data (unique welded vertices plus per-facet
  indices), so moving between them doesn't mean rebuilding from scratch:

  - **`Mesh2D`/`Mesh3D` → `ConnectedMesh2D`/`ConnectedMesh3D`**, via `Connect()`: same facets and
    vertices, plus per-facet edge adjacency computed fresh. Use it when a mesh was built for fast
    triangle-soup construction (`FromTriangles()`) and only later turns out to need face-to-face
    navigation (pathfinding, flood-fill/region-growing, adjacency queries) — no need to have gone
    through `ConnectedMesh` from the start.
  - **`PolyMesh2D`/`PolyMesh3D` → `Mesh2D`/`Mesh3D`**, via `Triangulate()` (§11 below): triangulates
    every arbitrary-sided facet independently and combines the results into one triangle mesh. Use it
    when a mesh was authored/edited as flat n-gon panels (§10.2 — rooms, walls, CAD/BIM-style
    geometry) but downstream code (rendering, physics/collision, GPU upload) needs triangle-only
    input.

  Both conversions return a new mesh object — neither mutates the source, and the source stays valid
  and usable afterward.

  <p align="center">
    <img src="./images/polymesh2d_triangulate.png" width="460" alt="A house-shaped PolyMesh2D -- a wide rectangle base, two square facets for the body, a triangle roof (gold) -- next to the Mesh2D produced by Triangulate(): 7 triangles (cyan), same total area">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "polymesh2d.hpp"
  #include "mesh2d.hpp"
  #include "connected_mesh2d.hpp"

  namespace g = geompp;
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  // Start from a PolyMesh2D: two quads sharing an edge (same shape as §10.2's example).
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto poly_mesh = g::PolyMesh2D::FromPolygons({p0, p1});
  GEOMPP_LOG(INFO) << "PolyMesh2D: " << poly_mesh.Size() << " facets, area " << poly_mesh.Area();

  // PolyMesh2D -> Mesh2D: each n-gon facet is triangulated independently (ear clipping by default).
  auto mesh = poly_mesh.Triangulate();
  GEOMPP_LOG(INFO) << "Mesh2D: " << mesh.Size() << " triangular facets, area " << mesh.Area();

  // Mesh2D -> ConnectedMesh2D: same facets/vertices, plus precomputed per-facet edge adjacency.
  auto connected = mesh.Connect();
  GEOMPP_LOG(INFO) << "ConnectedMesh2D: " << connected.Size() << " facets, area " << connected.Area();

  auto neighbor = connected[0].Neighbor(Edge::THIRD);
  GEOMPP_LOG(INFO) << "face 0 neighbor across THIRD: face " << neighbor->ID()
                    << " -- " << neighbor->Geometry().ToWkt();
  ```

  ```bash
  I20260803] PolyMesh2D: 2 facets, area 2
  I20260803] Mesh2D: 4 triangular facets, area 2
  I20260803] ConnectedMesh2D: 4 facets, area 2
  I20260803] face 0 neighbor across THIRD: face 1 -- TRIANGLE (0 1, 1 0, 1 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # Start from a PolyMesh2D: two quads sharing an edge (same shape as 10.2's example).
  p0 = g.Polygon2D.make([g.Point2D(0, 0), g.Point2D(1, 0), g.Point2D(1, 1), g.Point2D(0, 1)])
  p1 = g.Polygon2D.make([g.Point2D(1, 0), g.Point2D(2, 0), g.Point2D(2, 1), g.Point2D(1, 1)])
  poly_mesh = g.PolyMesh2D.from_polygons([p0, p1])
  print(f"PolyMesh2D: {poly_mesh.size()} facets, area {poly_mesh.area()}")

  # PolyMesh2D -> Mesh2D: each n-gon facet is triangulated independently (ear clipping by default).
  mesh = poly_mesh.triangulate()
  print(f"Mesh2D: {mesh.size()} triangular facets, area {mesh.area()}")

  # Mesh2D -> ConnectedMesh2D: same facets/vertices, plus precomputed per-facet edge adjacency.
  connected = mesh.connect()
  print(f"ConnectedMesh2D: {connected.size()} facets, area {connected.area()}")

  neighbor = connected[0].neighbor(g.TriangleEdge.THIRD)
  print(f"face 0 neighbor across THIRD: face {neighbor.id()} -- {neighbor.geometry().to_wkt()}")
  ```

  ```
  PolyMesh2D: 2 facets, area 2.0
  Mesh2D: 4 triangular facets, area 2.0
  ConnectedMesh2D: 4 facets, area 2.0
  face 0 neighbor across THIRD: face 1 -- TRIANGLE (0 1, 1 0, 1 1)
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;

  // Start from a PolyMesh2D: two quads sharing an edge (same shape as 10.2's example).
  var p0 = G.Polygon2D.Make(new G.Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
  var p1 = G.Polygon2D.Make(new G.Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
  var polyMesh = G.PolyMesh2D.FromPolygons(new[] { p0, p1 });
  Console.WriteLine($"PolyMesh2D: {polyMesh.Size()} facets, area {polyMesh.Area()}");

  // PolyMesh2D -> Mesh2D: each n-gon facet is triangulated independently (ear clipping by default).
  var mesh = polyMesh.Triangulate();
  Console.WriteLine($"Mesh2D: {mesh.Size()} triangular facets, area {mesh.Area()}");

  // Mesh2D -> ConnectedMesh2D: same facets/vertices, plus precomputed per-facet edge adjacency.
  var connected = mesh.Connect();
  Console.WriteLine($"ConnectedMesh2D: {connected.Size()} facets, area {connected.Area()}");

  var neighbor = connected[0].Neighbor(G.TriangleEdge.Third);
  Console.WriteLine($"face 0 neighbor across Third: face {neighbor.Id()} -- {neighbor.Geometry().ToWkt()}");
  ```

  ```
  PolyMesh2D: 2 facets, area 2
  Mesh2D: 4 triangular facets, area 2
  ConnectedMesh2D: 4 facets, area 2
  face 0 neighbor across Third: face 1 -- TRIANGLE (0 1, 1 0, 1 1)
  ```

   </details>

  </details>

</details>

</details>

<details open>
<summary><b> &nbsp; 11. Triangulation</b></summary>

  Every `Polygon2D`/`Polygon3D`'s outer ring, and every `PolyMesh2D`/`PolyMesh3D` facet, can be broken
  down into triangles. All of it runs through one free function, `triangulate(points, settings)`: 2D
  is native, and the 3D overload assumes flat/coplanar input, projected via either a caller-supplied
  plane normal or one fitted automatically via PCA (`principal_normal`) when omitted.

  `TriangulationParams` bundles the algorithm choice with how strictly to trust the input:

  - **`Strategy`** — four values, two implemented so far:
    - `EarClipping` walks the ring and clips the *first* valid convex "ear" vertex it finds, in scan
      order, tracking which vertices are reflex as it goes so later ears can't accidentally clip
      through one. O(n²) worst case, but often close to O(n) in practice on well-behaved polygons —
      clipping at one vertex frequently leaves its neighbor immediately clippable too. It has no
      concept of triangle *quality*, though: taking whatever's first in scan order can produce a
      visibly thin sliver triangle purely by luck of vertex ordering, even on an otherwise ordinary
      polygon (not just adversarial input) — see the second picture below.
    - `EarClippingBestFit` (**the default**) clips one ear at a time the same way, but each step first
      does a full lap over the *current* ring to find the best-scoring valid ear — by a cheap,
      scale-invariant shape-quality score, `|cross(prev, cur, next)| / (a² + b² + c²)` (proportional to
      `4√3·Area/(a²+b²+c²)`, 1.0 for an equilateral triangle, → 0 for a sliver) — instead of just
      taking the first one found. It never rejects a geometrically valid ear outright, only reorders
      which one gets preferred, so it keeps the exact same termination guarantee (the Two Ears
      Theorem) `EarClipping` relies on — critically, a strict angle/area floor that *rejects* thin
      candidates instead of just deprioritizing them can't make that same promise, and risks never
      terminating on a polygon with a genuinely sharp (but valid) vertex. The cost of the quality win:
      unconditionally ~O(n²), since the full rescan runs on *every* clip, not only in the worst case —
      so `EarClippingBestFit` is slower than `EarClipping` even on inputs `EarClipping` would finish
      quickly.
    - `MonotonePolygon` and `Delaunay` are declared but **not yet implemented** — both `throw`.
      `MonotonePolygon` would decompose the ring into y-monotone pieces and triangulate each with a
      stack-based sweep, O(n log n) worst case — faster than either ear-clipping strategy, but unlike
      them can't triangulate an arbitrary simple polygon directly; it needs the monotone-decomposition
      step first. `Delaunay` would triangulate a *point set's* convex hull rather than a polygon
      boundary (a different problem — no notion of "outside" the input), O(n log n) worst case, and is
      the only one of the four with a *provable* global shape guarantee: it maximizes the minimum angle
      across the whole triangulation, rather than `EarClippingBestFit`'s local, per-step preference.
  - **`Simplicity` / `Winding` / `Collinearity`** — each independently `Guaranteed` (skip the check,
    run at your own risk), `Assert` (throw if violated), or `Enforce` (fix it in place — decompose
    into simple rings, reverse to CCW, or strip collinear/duplicate points — before triangulating).

  `Polygon2D/3D::Triangulate(strategy)` and `PolyMesh2D/3D::Triangulate(strategy)` (§10.4) are thin
  wrappers around the same free function: since `Make()`/`FromPolygons()` already validated
  simplicity/winding/collinearity at construction time, they pass `Guaranteed` for all three checks
  and only expose the `Strategy` choice. Calling `triangulate()` directly on a raw point list is the
  more general entry point — no `Polygon2D/3D` required, and full control over how much to trust the
  input via `TriangulationParams`.

  A third overload, `triangulate(vector<Polygon2D>, conformity, settings)`, batches this across a whole
  set of polygon facets at once — the free-function equivalent of
  `PolyMesh2D::FromPolygons(polygons).Triangulate()` for callers who just want triangles without
  constructing/keeping a full `PolyMesh2D`. Unlike `PolyMesh2D::FromPolygons()` (§10), which always
  rejects a non-conforming set of facets outright, this overload defaults `conformity` to `Enforce`:
  since the caller isn't building a persistent mesh object here, it's more useful to auto-repair
  whatever's fixable (splice a stray T-junction vertex back in — see `fix_adjacency()`, §10) than to
  simply refuse the input. It still throws on a non-manifold edge either way, since that one has no
  valid automatic fix.

  The picture below triangulates a 5-pointed star — a classic concave shape with 5 reflex vertices
  at its inner corners — using `EarClippingBestFit`, the default. The first lap around the ring clips
  each of the star's 5 points off as its own ear; what's left is the inner pentagon, which the second
  lap then fans from one of its vertices. That two-stage split is a property of this particular vertex
  ordering (and of how close a triple is to collinear), not something the algorithm guarantees in
  general. Alongside it, a 3-tooth "comb" — the classic *adversarial* shape for naive ear-clipping: its
  deep, narrow notches mean some vertices get checked, rejected, and only clipped later once an
  unrelated clip elsewhere in the ring shrinks the set of blocking reflex vertices, so the algorithm
  needs more than one pass around it to finish.

  <p align="center">
    <img src="./images/triangulation.png" width="420" alt="A 5-pointed star polygon before and after Triangulate() with EarClippingBestFit: 8 triangles (every edge in gold) -- the 5 point-ears clipped first, the remaining pentagon fanned from one of its vertices">
    &nbsp;&nbsp;
    <img src="./images/comb_triangulation.png" width="270" alt="A 3-tooth comb polygon before and after Triangulate() with EarClippingBestFit: 10 triangles fanning from the base, the classic adversarial case that needs multiple traversal laps to fully clip">
  </p>

  The same two shapes again below, but with plain `EarClipping` (fast, first-found) instead of the
  default. Same polygons, same triangle counts, same total area — both are valid triangulations — but
  scan order alone produces two visibly thin sliver triangles in the star (flagged red) that
  `EarClippingBestFit` avoids entirely, simply by preferring a fatter ear when one's available. The
  comb's sliver, also flagged red, shows up under *both* strategies: that one is forced by the notch's
  own geometry, not by which ear got picked first, and no re-triangulation of a *fixed* vertex set can
  fix a triangle whose thinness is inherited from a genuinely sharp input angle — only inserting new
  points (Steiner refinement, which neither strategy does) could.

  <p align="center">
    <img src="./images/triangulation_ear_clipping.png" width="420" alt="The same 5-pointed star triangulated with plain EarClipping: 8 triangles, gold = healthy edge, red = two sliver triangles produced purely by scan order">
    &nbsp;&nbsp;
    <img src="./images/comb_triangulation_ear_clipping.png" width="270" alt="The same 3-tooth comb triangulated with plain EarClipping: 10 triangles, red = the one sliver forced by the notch geometry itself, present under EarClippingBestFit too">
  </p>

  <details closed>
  <summary><b> &nbsp; &nbsp; Samples</b></summary>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C++</b></summary>

  ```cpp
  #include "polygon2d.hpp"
  #include "calc_utils2d.hpp"

  namespace g = geompp;

  // A 5-pointed star -- concave, with a reflex vertex at each of its 5 inner corners.
  auto poly = g::Polygon2D::Make({
      g::Point2D(3.0, 6.0), g::Point2D(2.29, 3.97), g::Point2D(0.15, 3.93), g::Point2D(1.86, 2.63),
      g::Point2D(1.24, 0.57), g::Point2D(3.0, 1.8), g::Point2D(4.76, 0.57), g::Point2D(4.14, 2.63),
      g::Point2D(5.85, 3.93), g::Point2D(3.71, 3.97),
  });

  // As a method: Polygon2D/3D::Triangulate() takes strategy explicitly (no default at this layer --
  // EarClippingBestFit is only TriangulationParams' own default, see the free function below). All
  // TriangulationParams checks are Guaranteed -- Make() already validated
  // simplicity/winding/collinearity, so there's nothing left to check.
  for (auto const& t : poly.Triangulate(g::TriangulationParams::Strategy::EarClippingBestFit))
      GEOMPP_LOG(INFO) << t.ToWkt();

  // Same algorithm as a free function on a raw point list -- 2D here, but a Point3D overload works
  // the same way on flat/planar 3D input. settings defaults to EarClippingBestFit + Enforce for all
  // three input-quality checks, so it can be omitted entirely.
  auto triangles = g::triangulate(poly.Perimeter());
  GEOMPP_LOG(INFO) << triangles.size() << " triangles";
  ```

  ```bash
  I20260804] TRIANGLE (3.71 3.97, 3 6, 2.29 3.97)
  I20260804] TRIANGLE (2.29 3.97, 0.15 3.93, 1.86 2.63)
  I20260804] TRIANGLE (1.86 2.63, 1.24 0.57, 3 1.8)
  I20260804] TRIANGLE (3 1.8, 4.76 0.57, 4.14 2.63)
  I20260804] TRIANGLE (4.14 2.63, 5.85 3.93, 3.71 3.97)
  I20260804] TRIANGLE (4.14 2.63, 3.71 3.97, 2.29 3.97)
  I20260804] TRIANGLE (4.14 2.63, 2.29 3.97, 1.86 2.63)
  I20260804] TRIANGLE (4.14 2.63, 1.86 2.63, 3 1.8)
  I20260804] 8 triangles
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; Python</b></summary>

  ```python
  import geompp as g

  # A 5-pointed star -- concave, with a reflex vertex at each of its 5 inner corners.
  poly = g.Polygon2D.make([
      g.Point2D(3.0, 6.0), g.Point2D(2.29, 3.97), g.Point2D(0.15, 3.93), g.Point2D(1.86, 2.63),
      g.Point2D(1.24, 0.57), g.Point2D(3.0, 1.8), g.Point2D(4.76, 0.57), g.Point2D(4.14, 2.63),
      g.Point2D(5.85, 3.93), g.Point2D(3.71, 3.97),
  ])

  # As a method: strategy defaults to EarClippingBestFit. All TriangulationParams checks are
  # Guaranteed -- make() already validated simplicity/winding/collinearity, so there's nothing left
  # to check.
  for t in poly.triangulate():
      print(t.to_wkt())

  # Same algorithm as a free function on a raw point list -- 2D here, but a Point3D overload works
  # the same way on flat/planar 3D input. settings defaults to EarClippingBestFit + Enforce for all
  # three input-quality checks, so it can be omitted entirely.
  triangles = g.triangulate(poly.perimeter())
  print(f"{len(triangles)} triangles")
  ```

  ```
  TRIANGLE (3.71 3.97, 3 6, 2.29 3.97)
  TRIANGLE (2.29 3.97, 0.15 3.93, 1.86 2.63)
  TRIANGLE (1.86 2.63, 1.24 0.57, 3 1.8)
  TRIANGLE (3 1.8, 4.76 0.57, 4.14 2.63)
  TRIANGLE (4.14 2.63, 5.85 3.93, 3.71 3.97)
  TRIANGLE (4.14 2.63, 3.71 3.97, 2.29 3.97)
  TRIANGLE (4.14 2.63, 2.29 3.97, 1.86 2.63)
  TRIANGLE (4.14 2.63, 1.86 2.63, 3 1.8)
  8 triangles
  ```

   </details>

   <details closed>
   <summary><b> &nbsp; &nbsp; &nbsp; C#</b></summary>

  ```csharp
  using G = GeomPP;
  using System.Collections.Generic;
  using System.Linq;

  // A 5-pointed star -- concave, with a reflex vertex at each of its 5 inner corners.
  var poly = G.Polygon2D.Make(new G.Point2D[] {
      new(3.0, 6.0), new(2.29, 3.97), new(0.15, 3.93), new(1.86, 2.63), new(1.24, 0.57),
      new(3.0, 1.8), new(4.76, 0.57), new(4.14, 2.63), new(5.85, 3.93), new(3.71, 3.97),
  });

  // As a method: strategy defaults to EarClippingBestFit. All TriangulationParams checks are
  // Guaranteed -- Make() already validated simplicity/winding/collinearity, so there's nothing left
  // to check.
  foreach (var t in poly.Triangulate())
      Console.WriteLine(t.ToWkt());

  // Same algorithm via GeomUtil.Triangulate() on a raw point list -- 2D here, but 3D overloads work
  // the same way on flat/planar input. Unlike the method above, GeomUtil.Triangulate() takes an
  // explicit TriangulationParams (defaults to EarClippingBestFit + Enforce for all three checks).
  var points = new List<G.Point2D>(poly.Perimeter());
  var triangles = G.GeomUtil.Triangulate(points, new G.TriangulationParams());
  Console.WriteLine($"{triangles.Count()} triangles");
  ```

  ```
  TRIANGLE (3.71 3.97, 3 6, 2.29 3.97)
  TRIANGLE (2.29 3.97, 0.15 3.93, 1.86 2.63)
  TRIANGLE (1.86 2.63, 1.24 0.57, 3 1.8)
  TRIANGLE (3 1.8, 4.76 0.57, 4.14 2.63)
  TRIANGLE (4.14 2.63, 5.85 3.93, 3.71 3.97)
  TRIANGLE (4.14 2.63, 3.71 3.97, 2.29 3.97)
  TRIANGLE (4.14 2.63, 2.29 3.97, 1.86 2.63)
  TRIANGLE (4.14 2.63, 1.86 2.63, 3 1.8)
  8 triangles
  ```

   </details>

  </details>

</details>
