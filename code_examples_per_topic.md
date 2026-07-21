# How to use the library

Here is an example of code. You can also look at the directories of [c++ tests](./geompp_tests/), [python tests](./geompp_python/tests), or [csharp tests](./geompp_csharp/tests), to see more code.

A quick list of code examples per topic is provided here.


<details open>
<summary><b>C++</b></summary>

<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details closed>
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

<details closed>
<summary><b> &nbsp; &nbsp; 2.5 Polyline Overlaps / Touches </b></summary>

  `Polyline2D` and `Polyline3D` iterate over their constituent segments to collect all
  overlapping sub-segments or all touch points. `Overlap(other)` returns
  `std::optional<std::vector<LineSegment2D/3D>>`; `Touch(other)` returns `std::optional<std::vector<Point2D/3D>>`.

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

<details closed>
<summary><b> &nbsp; &nbsp; 5.4 Polygon extreme points </b></summary>

  `find_extreme_points(polygon, line)` returns the two vertices of a polygon that are **extreme** — the
  least and the greatest — when projected onto a line's direction (the "supporting vertices" along that
  axis). It is handy for collision broad-phase (SAT), rotating calipers, and directional clipping.

  The result is an `ExtremePoints<Point2D>` (or `ExtremePoints<Point3D>`) with `.min_point` / `.max_point`.
  When the polygon is **convex** it uses Daniel Sunday's O(log n) binary search; otherwise it falls back to
  an O(n) linear scan. Holes are ignored — only the outer ring participates.

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

</details>


<details open>
<summary><b> &nbsp; 6. Distance </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 6.1 from Point</b></summary>

  Every core primitive implements `DistanceTo(point)` — the perpendicular / nearest distance to a
  point, clamped to the primitive's own domain where relevant (a `Ray` only measures ahead of its
  origin, a `LineSegment`/`Polyline` clamps to its own bounded extent). `Point2D`/`Point3D` themselves
  just measure Euclidean distance to another point.

  `Polygon2D`/`Polygon3D::DistanceTo(Point)` and `Triangle2D`/`Triangle3D::DistanceTo(Point)` are
  declared but not yet implemented — they currently `throw std::runtime_error("not implemented")`,
  so they're omitted from the example below.

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
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 6.2 from Other primitives</b></summary>

  `Line3D`, `Ray3D`, and `LineSegment3D` each expose `DistanceTo(Line3D | Ray3D | LineSegment3D)` —
  pairwise distance between any two of the three (0 if they intersect, overlap, or one contains the
  other). This overload set is 3D-only: two 2D primitives are either parallel (a constant distance,
  rarely useful on its own) or they intersect (0), so `Line2D`/`Ray2D`/`LineSegment2D` don't expose it.

  If you need the actual closest-approach segment instead of just the scalar, use `Distance(...)`
  (note: no `To`) — it returns `std::optional<LineSegment3D>`, `std::nullopt` when the two intersect
  or overlap (matching the zero case of `DistanceTo`).

  For polygon-to-line distance, use the free function `distance_to(polygon, line)` (see section 5.4
  "Polygon extreme points" for `find_extreme_points`, its sibling function) — zero if the line
  crosses the polygon; for `Polygon3D`/`Line3D` it also handles a line coplanar with, parallel to
  (fixed offset from), or skew to the polygon's plane.

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

</details>

<details open>
<summary><b> &nbsp; 7. Polygon tangents</b></summary>

<details closed>
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
<summary><b> &nbsp; &nbsp; 7.2 Polygon to Polygon</b></summary>

  `tangents_to(polygon, other)` returns the two common outer tangent segments between two polygons —
  the "belt around two pulleys" lines that touch both shapes without crossing either. Neither polygon
  needs to be convex: each is independently reduced to its convex hull when needed, same as the
  point overload above. For `Polygon3D`, both polygons must share the same plane (two polygons in
  general 3D position don't have a single well-defined common tangent line), or the call throws
  `std::logic_error`.

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

</details>

<details open>
<summary><b> &nbsp; 8. Polyline operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 8.1 Decimation / reduction</b></summary>

  `Polyline2D::Reduce(settings)` and `Polyline3D::Reduce(settings)` return a copy of the polyline with
  fewer vertices, per a `PolylineDecimationParams` bundling a `PolylineDecimationParams::Strategy` and
  a `threshold`:

  - `RadialDistance` — O(n) brute-force pass: drops a vertex if it's closer than `threshold` to the
    last *kept* vertex. Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as
    the sole strategy when shape fidelity matters.
  - `RamerDouglasPeucker` — O(n log n) to O(n²): recursively drops vertices closer than `threshold`
    to the chord spanning their segment. Given points P1, P2, P3, drops P2 when its perpendicular
    distance from the P1-P3 chord is below `threshold`; otherwise keeps P2 and recurses on both
    halves. Best general-purpose choice — preserves the vertices that most define the polyline's shape.
  - `VisvalingamWhyatt` — O(n log n) to O(n²): repeatedly drops the vertex forming the smallest-area
    triangle with its neighbors, while that area stays below `threshold`, then re-evaluates the
    neighbors. Tends to preserve visually significant features (sharp spikes) better than
    `RadialDistance` while being similarly simple to reason about.

  `settings` is optional and defaults to `{RamerDouglasPeucker, 0.5}`, so `Reduce()` with no arguments
  keeps working.

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
<summary><b> &nbsp; &nbsp; 8.2 Corner smoothing with Bezier curve</b></summary>

  `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance_or_num_segments)` rounds the corner at
  `p1` with a quadratic Bezier arc tangent to `p0-p1` and `p1-p2`. `smoothness` in `[0, 1]` controls
  how much of the shorter adjacent edge is trimmed into the arc's tangent points (`0` leaves the
  corner sharp, `1` trims half of the shorter edge). Two overloads control how densely the arc is
  sampled — resolved by the type of the last argument, not a shared parameter:

  - a `double` **min_distance** samples roughly that far apart, however many points that takes.
  - an `int` **num_segments** samples exactly that many segments (`num_segments + 1` points),
    regardless of the arc's length.

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
<summary><b> &nbsp; &nbsp; 8.3 Expand / smooth a polyline</b></summary>

  `Polyline2D::Expand(settings)` / `Polyline3D::Expand(settings)` — the inverse of `Reduce()` — round
  every *inner* corner with a quadratic Bezier arc, via `PolylineExpansionParams`:

  - `smoothness`, `mode` (`FixedSegments` / `MinDistance`), `segments_per_corner`, `min_distance` —
    same meaning as the matching `bezier_smoothing_2` parameters, applied to every corner.
  - `min_segment_length` — forwarded to `bezier_smoothing_2` for every corner; a corner whose adjacent
    edges are both that short stays sharp instead of being rounded.

  `settings` is optional and defaults to `{0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON}`.

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

</details>

</details>


<details closed>
<summary><b>C# .Net</b></summary>


<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 1.1 Create geometries from classes</b></summary>

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

<details closed>
<summary><b> &nbsp; &nbsp; 1.2 Create geometries from text</b></summary>

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

<details closed>
<summary><b> &nbsp; &nbsp; 1.3 Import geometries from a file</b></summary>

  An `.lsv` file is a plain-text list of WKT geometries, one per line:

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

<details open>
<summary><b> &nbsp; 2. Geometry Operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 2.1 Containment </b></summary>

`Triangle3D.Contains(p)` and `Polygon2D/3D.Contains(p)` test whether a point lies inside a shape
  using barycentric coordinates and the winding number, respectively.
  `LineSegment.Contains(p)` checks whether a point lies on the segment;
  `Location(p)` returns the parameter `t ∈ [0, 1]` for a point already on it, and `Interpolate(t)` reverses the mapping.

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


<details closed>
<summary><b> &nbsp; &nbsp; 2.2 Intersection </b></summary>

  Every 2D primitive (`Line2D`, `Ray2D`, `LineSegment2D`, `Triangle2D`, `Polygon2D`) can intersect any
  other 2D primitive, and the same holds in 3D across `Line3D`, `Ray3D`, `LineSegment3D`, `Triangle3D`,
  and `Plane`. Methods return `object` (null on miss); use C# pattern matching to extract the result
  type. Polygon intersections return `LineSegment2D[]` because a line can produce multiple chords through
  a concave shape. `GeomUtil.FindIntersections(segments)` (Bentley–Ottmann) reports all crossing points
  across an arbitrary set of 2D segments, sorted left-to-right.

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


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 2.2.1 Split a complex polygon </b></summary>

  A **complex polygon** (also called a self-intersecting polygon) is a polygon whose edges cross
  each other. `Simplify()` decomposes it into an array of simple (non-self-intersecting) polygons
  via planar-graph half-edge face tracing. Each returned polygon is guaranteed to satisfy
  `IsSimple() == true`. If the input is already simple, `Simplify()` returns a single-element
  array containing the original polygon.

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

<details closed>
<summary><b> &nbsp; &nbsp; 2.3 Overlap </b></summary>

  `Overlaps(other)` returns `true` when two primitives share a 1D region (more than a single point).
  `Overlap(other)` returns the shared geometry, or `null` when they do not overlap or only touch at
  a single point. The return type mirrors the "smaller" of the two primitives: a `Line × Line`
  overlap yields a `Line`; `Ray × Ray` with opposite directions yields a `LineSegment`; all
  `Segment`-involving pairs yield a `LineSegment`. Use C# pattern matching to extract the concrete type.

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


<details closed>
<summary><b> &nbsp; &nbsp; 2.4 Touch </b></summary>

  `Touches(other)` returns `true` when two primitives share exactly one endpoint-contact point
  (not an interior crossing, not a shared segment). `Touch(other)` returns that contact point,
  or `null` when there is no touch.

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

<details closed>
<summary><b> &nbsp; &nbsp; 2.5 Polyline Overlaps / Touches </b></summary>

  `Polyline2D` and `Polyline3D` iterate over their constituent segments to collect all
  overlapping sub-segments or all touch points. `Overlap()` returns `LineSegment2D[]`
  (or `null`); `Touch()` returns `Point2D[]` (or `null`).

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


<details open>
<summary><b> &nbsp; 3. Planes </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 3.1 Coplanarity, winding order, and polygon with holes </b></summary>

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

<details closed>
<summary><b> &nbsp; &nbsp; 3.2 Projecting points onto a plane </b></summary>

  `Plane.ProjectOnto(p)` returns the perpendicular projection in 3D world coordinates.
  `Plane.ProjectInto(p)` maps the same projected point into the plane's local 2D frame.
  `Plane.Evaluate(p2d)` is the inverse — local 2D coordinates back to world 3D.

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


<details closed>
<summary><b> &nbsp; &nbsp; 3.3 Planar vs non-planar Polyline3D </b></summary>

  `Polyline3D.IsPlanar()` checks whether all knots lie in a common plane. Only planar polylines support
  `IsSimple()`, `IsConvex()`, `ConvexHull()`, and `ToPolygon()` — call `IsPlanar()` first.

  `ConvexHull()` returns a `Polyline3D` (an open path). Call `ToPolygon()` on it to close the boundary into a `Polygon3D` with area.

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

<details closed>
<summary><b> &nbsp; &nbsp; 3.4 View2D — streaming 3D points to 2D </b></summary>

  `View2D` maps 3D points to 2D scalars via `X()` / `Y()` getters without allocating an intermediate
  `Point2D` array. Axis-aligned views (`XY`, `YZ`, `ZX`) are the fastest path — just a direct coordinate
  read. `OnPlane` computes dot products against the plane's local axes.

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


<details closed>
<summary><b> &nbsp; 4. PCA on a 3D point cloud </b></summary>

  `GeomUtil.PrincipalAxes(points)` runs PCA (Jacobi eigendecomposition on the 3×3 covariance matrix) and
  returns a `CoordinateFrame` — three orthonormal axes sorted by variance: `X` is the direction of most
  spread, `Y` the secondary, and `Z` the best-fit plane normal (least variance).

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


<details open>
<summary><b> &nbsp; 5. Bounding containers </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 5.1 Simple containers for quick rejection </b></summary>

  `BBox3D` gives the tight axis-aligned box; `BBall3D` (Ritter 1990) gives an approximate
  minimum enclosing sphere — both accept any cloud of points.

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

<details open>
<summary><b> &nbsp; &nbsp; 5.2 Convex hull </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.1 Convex hull of a point cloud </b></summary>

  `GeomUtil.ConvexHull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

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

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.2 Convex hull of a polygon </b></summary>

  `Polygon2D` and `Polygon3D` expose a `ConvexHull()` method that wraps the free function:

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


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.3 Convex hull of a simple polyline </b></summary>

  `Polyline2D.ConvexHull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `IsSimple()` first.

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

<details closed>
<summary><b> &nbsp; &nbsp; 5.3 Oriented Minimum Bounding Rectangle </b></summary>

  `BRect2D` computes the **tightest** axis-aligned-to-input rectangle that encloses a point cloud.
  It is defined by a center point, two orthogonal unit axes (`AxisU`, `AxisV`), and two half-lengths
  (`HalfLenU`, `HalfLenV`).

  **Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
  1. Compute the convex hull of the input cloud (Andrew's monotone chain, O(n log n)).
  2. For each hull edge, project all hull vertices onto the edge direction and its perpendicular.
  3. The rectangle aligned with that edge has width = max − min along the edge and height = max − min along the perpendicular.
  4. Track the edge orientation that minimises area; the center is the midpoint of the extents.

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

<details closed>
<summary><b> &nbsp; &nbsp; 5.4 Polygon extreme points </b></summary>

  `GeomUtil.FindExtremePoints(polygon, line)` returns the two vertices of a polygon that are **extreme** —
  the least and the greatest — when projected onto a line's direction (the "supporting vertices" along that
  axis). It is handy for collision broad-phase (SAT), rotating calipers, and directional clipping.

  The result is an `ExtremePoints2D` (or `ExtremePoints3D`) with `.MinPoint` / `.MaxPoint`. When the
  polygon is **convex** it uses Daniel Sunday's O(log n) binary search; otherwise it falls back to an
  O(n) linear scan. Holes are ignored — only the outer ring participates.

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


<details open>
<summary><b> &nbsp; 6. Distance </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 6.1 from Point</b></summary>

  Every core primitive implements `DistanceTo(point)` — the perpendicular / nearest distance to a
  point, clamped to the primitive's own domain where relevant (a `Ray` only measures ahead of its
  origin, a `LineSegment`/`Polyline` clamps to its own bounded extent). `Point2D`/`Point3D` themselves
  just measure Euclidean distance to another point.

  `Polygon2D`/`Polygon3D.DistanceTo(Point)` and `Triangle2D`/`Triangle3D.DistanceTo(Point)` are
  exposed but not yet implemented on the native side — calling them throws a
  `System.Runtime.InteropServices.SEHException` ("External component has thrown an exception"), so
  they're omitted from the example below.

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
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 6.2 from Other primitives</b></summary>

  `Line3D`, `Ray3D`, and `LineSegment3D` each expose `DistanceTo(Line3D | Ray3D | LineSegment3D)` —
  pairwise distance between any two of the three (0 if they intersect, overlap, or one contains the
  other). This overload set is 3D-only: two 2D primitives are either parallel (a constant distance,
  rarely useful on its own) or they intersect (0), so `Line2D`/`Ray2D`/`LineSegment2D` don't expose it.

  If you need the actual closest-approach segment instead of just the scalar, use `Distance(...)`
  (note: no `To`) — it returns a `LineSegment3D`, `null` when the two intersect or overlap (matching
  the zero case of `DistanceTo`).

  For polygon-to-line distance, use `GeomUtil.DistanceTo(polygon, line)` (see section 5.4 "Polygon
  extreme points" for `GeomUtil.FindExtremePoints`, its sibling function) — zero if the line crosses
  the polygon; for `Polygon3D`/`Line3D` it also handles a line coplanar with, parallel to (fixed
  offset from), or skew to the polygon's plane.

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

<details open>
<summary><b> &nbsp; 7. Polygon tangents</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 7.1 Point to Polygon</b></summary>

  `GeomUtil.TangentsTo(polygon, point)` returns `PolygonTangents2D^`/`PolygonTangents3D^` (`Left` /
  `Right`, each a `LineSegment`) — the two tangent segments from an external point to a polygon (the
  point's "line of sight" grazing the shape on either side, like a taut string pulled around it).
  Convex polygons use Daniel Sunday's O(log n) binary search; non-convex polygons are reduced to
  their convex hull first (a tangent point can only ever be a hull vertex — a reflex vertex always
  has the polygon on both sides of it, so it can never support a tangent line) and the result is
  mapped back to the original vertex.

  The point must be strictly outside the polygon and not equal to any of its vertices. For
  `Polygon3D`, a tangent is inherently a planar concept — unlike `DistanceTo`, there is no "skew"
  fallback — so the point must lie in the polygon's own plane, or the call throws.

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

<details closed>
<summary><b> &nbsp; &nbsp; 7.2 Polygon to Polygon</b></summary>

  `GeomUtil.TangentsTo(polygon, other)` returns the two common outer tangent segments between two
  polygons — the "belt around two pulleys" lines that touch both shapes without crossing either.
  Neither polygon needs to be convex: each is independently reduced to its convex hull when needed,
  same as the point overload above. For `Polygon3D`, both polygons must share the same plane (two
  polygons in general 3D position don't have a single well-defined common tangent line), or the call
  throws.

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

<details open>
<summary><b> &nbsp; 8. Polyline operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 8.1 Decimation / reduction</b></summary>

  `Polyline2D.Reduce(settings)` and `Polyline3D.Reduce(settings)` return a copy of the polyline with
  fewer vertices, per a `PolylineDecimationParams` bundling a `PolylineDecimationStrategy` and a
  `Threshold`:

  - `RadialDistance` — O(n) brute-force pass: drops a vertex if it's closer than `Threshold` to the
    last *kept* vertex. Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as
    the sole strategy when shape fidelity matters.
  - `RamerDouglasPeucker` — O(n log n) to O(n²): recursively drops vertices closer than `Threshold`
    to the chord spanning their segment. Given points P1, P2, P3, drops P2 when its perpendicular
    distance from the P1-P3 chord is below `Threshold`; otherwise keeps P2 and recurses on both
    halves. Best general-purpose choice — preserves the vertices that most define the polyline's shape.
  - `VisvalingamWhyatt` — O(n log n) to O(n²): repeatedly drops the vertex forming the smallest-area
    triangle with its neighbors, while that area stays below `Threshold`, then re-evaluates the
    neighbors. Tends to preserve visually significant features (sharp spikes) better than
    `RadialDistance` while being similarly simple to reason about.

  `Reduce()` also has a no-argument overload, and `PolylineDecimationParams`'s own parameterless
  constructor defaults to `RamerDouglasPeucker` with `Threshold = 0.5` — so both behave identically.

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

<details closed>
<summary><b> &nbsp; &nbsp; 8.2 Corner smoothing with Bezier curve</b></summary>

  `GeomUtil.BezierSmoothing2(p0, p1, p2, smoothness, minDistanceOrNumSegments)` rounds the corner at
  `p1` with a quadratic Bezier arc tangent to `p0-p1` and `p1-p2`. `smoothness` in `[0, 1]` controls
  how much of the shorter adjacent edge is trimmed into the arc's tangent points (`0` leaves the
  corner sharp, `1` trims half of the shorter edge). Two overloads control how densely the arc is
  sampled — resolved by the type of the last argument, not a shared parameter:

  - a `double` **minDistance** samples roughly that far apart, however many points that takes.
  - an `int` **numSegments** samples exactly that many segments (`numSegments + 1` points),
    regardless of the arc's length.

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

<details closed>
<summary><b> &nbsp; &nbsp; 8.3 Expand / smooth a polyline</b></summary>

  `Polyline2D.Expand(settings)` / `Polyline3D.Expand(settings)` — the inverse of `Reduce()` — round
  every *inner* corner with a quadratic Bezier arc, via `PolylineExpansionParams`:

  - `Smoothness`, `Mode` (`FixedSegments` / `MinDistance`), `SegmentsPerCorner`, `MinDistance` — same
    meaning as the matching `BezierSmoothing2` parameters, applied to every corner.
  - `MinSegmentLength` — forwarded to `BezierSmoothing2` for every corner; a corner whose adjacent
    edges are both that short stays sharp instead of being rounded.

  `Expand()` also has a no-argument overload, matching `PolylineExpansionParams`'s own parameterless
  constructor defaults.

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


<details closed>
<summary><b>Python</b></summary>

<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 1.1 Create geometries from classes</b></summary>

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
<summary><b> &nbsp; &nbsp; 1.2 Create geometries from text</b></summary>

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
<summary><b> &nbsp; &nbsp; 1.3 Import geometries from a file</b></summary>

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

</details>

<details open>
<summary><b> &nbsp; 2. Geometry Operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 2.1 Containment </b></summary>

`Triangle3D.contains(p)` and `Polygon2D/3D.contains(p)` test whether a point lies inside a shape
  using barycentric coordinates and the winding number, respectively.
  `LineSegment.contains(p)` checks whether a point lies on the segment;
  `location(p)` returns the parameter `t ∈ [0, 1]` for a point already on it, and `interpolate(t)` reverses the mapping.

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
<summary><b> &nbsp; &nbsp; 2.2 Intersection </b></summary>

  Every 2D primitive (`Line2D`, `Ray2D`, `LineSegment2D`, `Triangle2D`, `Polygon2D`) can intersect any
  other 2D primitive, and the same holds in 3D across `Line3D`, `Ray3D`, `LineSegment3D`, `Triangle3D`,
  and `Plane`. Results are the geometry object or `None`; polygon intersections return a
  `list[LineSegment2D]` because a line can produce multiple chords through a concave shape.
  `find_intersections(segments)` (Bentley–Ottmann) reports all crossing points across an arbitrary set
  of 2D segments, sorted left-to-right.

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


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 2.2.1 Split a complex polygon </b></summary>

  A **complex polygon** (also called a self-intersecting polygon) is a polygon whose edges cross
  each other. `simplify()` decomposes it into a list of simple (non-self-intersecting) polygons
  via planar-graph half-edge face tracing. Each returned polygon is guaranteed to satisfy
  `is_simple() == True`. If the input is already simple, `simplify()` returns a single-element
  list containing the original polygon.

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

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 2.3 Overlap </b></summary>

  `overlaps(other)` returns `True` when two primitives share a 1D region (more than a single point).
  `overlap(other)` returns the shared geometry, or `None` when they do not overlap or only touch at
  a single point. The return type mirrors the "smaller" of the two primitives: a `Line × Line`
  overlap yields a `Line`; `Ray × Ray` with opposite directions yields a `LineSegment`; all
  `Segment`-involving pairs yield a `LineSegment`.

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
<summary><b> &nbsp; &nbsp; 2.4 Touch </b></summary>

  `touches(other)` returns `True` when two primitives share exactly one endpoint-contact point
  (not an interior crossing, not a shared segment). `touch(other)` returns that contact point,
  or `None` when there is no touch.

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
<summary><b> &nbsp; &nbsp; 2.5 Polyline Overlaps / Touches </b></summary>

  `Polyline2D` and `Polyline3D` iterate over their constituent segments to collect all
  overlapping sub-segments or all touch points. `overlap()` returns a list of
  `LineSegment2D`/`3D` or `None`; `touch()` returns a list of `Point2D`/`3D` or `None`.

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

</details>


<details open>
<summary><b> &nbsp; 3. Planes </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 3.1 Coplanarity, winding order, and polygon with holes </b></summary>

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
<summary><b> &nbsp; &nbsp; 3.2 Projecting points onto a plane </b></summary>

  `plane.project_onto(p)` returns the perpendicular projection in 3D world coordinates.
  `plane.project_into(p)` maps the same projected point into the plane's local 2D frame.
  `plane.evaluate(p2d)` is the inverse — local 2D coordinates back to world 3D.

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
<summary><b> &nbsp; &nbsp; 3.3 Planar vs non-planar Polyline3D </b></summary>

  `Polyline3D.is_planar()` checks whether all knots lie in a common plane. Only planar polylines support
  `is_simple()`, `is_convex()`, `convex_hull()`, and `to_polygon()` — call `is_planar()` first.

  `convex_hull()` returns a `Polyline3D` (an open path). Call `to_polygon()` on it to close the boundary into a `Polygon3D` with area.

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
<summary><b> &nbsp; &nbsp; 3.4 View2D — streaming 3D points to 2D </b></summary>

  `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters without allocating an intermediate
  `Point2D` list. Axis-aligned views (`xy`, `yz`, `zx`) are the fastest path — just a direct coordinate
  read. `on_plane` computes dot products against the plane's local axes.

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

</details>


<details closed>
<summary><b> &nbsp; 4. PCA on a 3D point cloud </b></summary>

  `principal_axes(points)` runs PCA (Jacobi eigendecomposition on the 3×3 covariance matrix) and returns
  a `CoordinateFrame` — three orthonormal axes sorted by variance: `x` is the direction of most spread,
  `y` the secondary, and `z` the best-fit plane normal (least variance).

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


<details open>
<summary><b> &nbsp; 5. Bounding containers </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 5.1 Simple containers for quick rejection </b></summary>

  `BBox3D` gives the tight axis-aligned box; `BBall3D` (Ritter 1990) gives an approximate
  minimum enclosing sphere — both accept any cloud of points.

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

<details open>
<summary><b> &nbsp; &nbsp; 5.2 Convex hull </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.1 Convex hull of a point cloud </b></summary>

  `convex_hull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

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
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.2 Convex hull of a polygon </b></summary>

  `Polygon2D` and `Polygon3D` expose a `convex_hull()` method that wraps the free function:

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
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.3 Convex hull of a simple polyline </b></summary>

  `Polyline2D.convex_hull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `is_simple()` first.

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
<summary><b> &nbsp; &nbsp; 5.4 Polygon extreme points </b></summary>

  `find_extreme_points(polygon, line)` returns the two vertices of a polygon that are **extreme** — the
  least and the greatest — when projected onto a line's direction (the "supporting vertices" along that
  axis). It is handy for collision broad-phase (SAT), rotating calipers, and directional clipping.

  The result is an `ExtremePoints2D` (or `ExtremePoints3D`) with `.min_point` / `.max_point`. When the
  polygon is **convex** it uses Daniel Sunday's O(log n) binary search; otherwise it falls back to an
  O(n) linear scan. Holes are ignored — only the outer ring participates.

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

</details>


<details open>
<summary><b> &nbsp; 6. Distance </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 6.1 from Point</b></summary>

  Every core primitive implements `.distance_to(point)` — the perpendicular / nearest distance to a
  point, clamped to the primitive's own domain where relevant (a `Ray` only measures ahead of its
  origin, a `LineSegment`/`Polyline` clamps to its own bounded extent). `Point2D`/`Point3D` themselves
  just measure Euclidean distance to another point.

  `Polygon2D`/`Polygon3D.distance_to(point)` and `Triangle2D`/`Triangle3D.distance_to(point)` are
  declared but not yet implemented — they currently raise `RuntimeError`, so they're omitted from
  the example below.

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
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 6.2 from Other primitives</b></summary>

  `Line3D`, `Ray3D`, and `LineSegment3D` each expose `.distance_to(Line3D | Ray3D | LineSegment3D)`
  — pairwise distance between any two of the three (0 if they intersect, overlap, or one contains
  the other). This overload set is 3D-only: two 2D primitives are either parallel (a constant
  distance, rarely useful on its own) or they intersect (0), so `Line2D`/`Ray2D`/`LineSegment2D`
  don't expose it.

  If you need the actual closest-approach segment instead of just the scalar, use `.distance(...)`
  (note: no `_to`) — it returns a `LineSegment3D` or `None` when the two intersect or overlap
  (matching the zero case of `distance_to`).

  For polygon-to-line distance, use the free function `distance_to(polygon, line)` (see section 5.4
  "Polygon extreme points" for `find_extreme_points`, its sibling function) — zero if the line
  crosses the polygon; for `Polygon3D`/`Line3D` it also handles a line coplanar with, parallel to
  (fixed offset from), or skew to the polygon's plane.

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

</details>

<details open>
<summary><b> &nbsp; 7. Polygon tangents</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 7.1 Point to Polygon</b></summary>

  `tangents_to(polygon, point)` returns `PolygonTangents2D`/`PolygonTangents3D` (`.left` / `.right`,
  each a `LineSegment`) — the two tangent segments from an external point to a polygon (the point's
  "line of sight" grazing the shape on either side, like a taut string pulled around it). Convex
  polygons use Daniel Sunday's O(log n) binary search; non-convex polygons are reduced to their
  convex hull first (a tangent point can only ever be a hull vertex — a reflex vertex always has the
  polygon on both sides of it, so it can never support a tangent line) and the result is mapped back
  to the original vertex.

  The point must be strictly outside the polygon and not equal to any of its vertices. For
  `Polygon3D`, a tangent is inherently a planar concept — unlike `distance_to`, there is no "skew"
  fallback — so the point must lie in the polygon's own plane, or the call raises `RuntimeError`.

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
<summary><b> &nbsp; &nbsp; 7.2 Polygon to Polygon</b></summary>

  `tangents_to(polygon, other)` returns the two common outer tangent segments between two polygons —
  the "belt around two pulleys" lines that touch both shapes without crossing either. Neither polygon
  needs to be convex: each is independently reduced to its convex hull when needed, same as the
  point overload above. For `Polygon3D`, both polygons must share the same plane (two polygons in
  general 3D position don't have a single well-defined common tangent line), or the call raises
  `RuntimeError`.

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

</details>

<details open>
<summary><b> &nbsp; 8. Polyline operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 8.1 Decimation / reduction</b></summary>

  `Polyline2D.reduce(settings)` and `Polyline3D.reduce(settings)` return a copy of the polyline with
  fewer vertices, per a `PolylineDecimationParams` bundling a `PolylineDecimationStrategy` and a
  `threshold`:

  - `RadialDistance` — O(n) brute-force pass: drops a vertex if it's closer than `threshold` to the
    last *kept* vertex. Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as
    the sole strategy when shape fidelity matters.
  - `RamerDouglasPeucker` — O(n log n) to O(n²): recursively drops vertices closer than `threshold`
    to the chord spanning their segment. Given points P1, P2, P3, drops P2 when its perpendicular
    distance from the P1-P3 chord is below `threshold`; otherwise keeps P2 and recurses on both
    halves. Best general-purpose choice — preserves the vertices that most define the polyline's shape.
  - `VisvalingamWhyatt` — O(n log n) to O(n²): repeatedly drops the vertex forming the smallest-area
    triangle with its neighbors, while that area stays below `threshold`, then re-evaluates the
    neighbors. Tends to preserve visually significant features (sharp spikes) better than
    `RadialDistance` while being similarly simple to reason about.

  `settings` is optional; `reduce()` with no arguments and `reduce(g.PolylineDecimationParams())`
  behave identically, both defaulting to `RamerDouglasPeucker` with `threshold = 0.5`.

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
<summary><b> &nbsp; &nbsp; 8.2 Corner smoothing with Bezier curve</b></summary>

  `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance_or_num_segments)` rounds the corner at
  `p1` with a quadratic Bezier arc tangent to `p0-p1` and `p1-p2`. `smoothness` in `[0, 1]` controls
  how much of the shorter adjacent edge is trimmed into the arc's tangent points (`0` leaves the
  corner sharp, `1` trims half of the shorter edge). Two overloads control how densely the arc is
  sampled — resolved by the type of the last argument, not a shared parameter:

  - a `float` **min_distance** samples roughly that far apart, however many points that takes.
  - an `int` **num_segments** samples exactly that many segments (`num_segments + 1` points),
    regardless of the arc's length.

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
<summary><b> &nbsp; &nbsp; 8.3 Expand / smooth a polyline</b></summary>

  `Polyline2D.expand(settings)` / `Polyline3D.expand(settings)` — the inverse of `reduce()` — round
  every *inner* corner with a quadratic Bezier arc, via `PolylineExpansionParams`:

  - `smoothness`, `mode` (`FixedSegments` / `MinDistance`), `segments_per_corner`, `min_distance` —
    same meaning as the matching `bezier_smoothing_2` parameters, applied to every corner.
  - `min_segment_length` — forwarded to `bezier_smoothing_2` for every corner; a corner whose adjacent
    edges are both that short stays sharp instead of being rounded.

  `settings` is optional; `expand()` with no arguments and `expand(g.PolylineExpansionParams())` behave
  identically, both defaulting to `{0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON}`.

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

</details>

</details>



