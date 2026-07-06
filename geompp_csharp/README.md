# GeomPP — C# Bindings

C++/CLI bindings for [geompp](https://github.com/amastrobera/geompp), targeting **Windows x64**.

Available on NuGet as [`GeomPP`](https://www.nuget.org/packages/GeomPP).

Supports **.NET 8**, **.NET 9**, **.NET 10**, and **.NET Framework 4.8**.

**[Changelog](https://github.com/amastrobera/geompp/blob/master/CHANGELOG.md)** — full release notes for every version.

---

## Install

```bash
dotnet add package GeomPP
```

or in your `.csproj`:

```xml
<PackageReference Include="GeomPP" Version="0.8.0" />
```


---

## Supported types

| Type              | 2D | 3D |
|-------------------|----|----|
| `Point`           | ✓  | ✓  |
| `Vector`          | ✓  | ✓  |
| `Line`            | ✓  | ✓  |
| `Ray`             | ✓  | ✓  |
| `LineSegment`     | ✓  | ✓  |
| `Polyline`        | ✓  | ✓  |
| `Triangle`        | ✓  | ✓  |
| `Polygon`         | ✓  | ✓  |
| `BBox`            | ✓  | ✓  |
| `BBall`           | ✓  | ✓  |
| `BRect2D`         | ✓  | —  |
| `BPrism3D`        | —  | ✓  |
| `Plane`           | —  | ✓  |
| `View2D`          | ✓  | —  |
| `GeometryCollection` | ✓ | ✓ |
| `WktParser`       | ✓  | ✓  |

All types expose `ToWkt()`, `FromWkt()`, `ToFile()`, `FromFile()`, `AlmostEquals()`, and the same
operators available in the C++ library.

Decimal precision for all `<=>` or `AlmostEquals()` or `compare(a,b, epsilon)` functions are managed by a global, _thread local_ variable. It is best to set it up at the beginning of your program (in your _main_). The defalt is 3 decimals, or 0.001 tolorance. 

```csharp
G.Precision.DecimalPrecision = 6; // default: G.Precision.DP_THREE
```

Intersection methods return `object` (null when there is no intersection); use C# pattern matching
to extract the result type:

```csharp
var result = line.Intersection(segment);
if (result is G.Point2D p)       { /* point intersection */ }
if (result is G.LineSegment2D s) { /* overlap */ }
```

## Platform note

GeomPP is built with C++/CLI and is **Windows x64 only**. It will not run on Linux, macOS, or
32-bit processes.

---


## How to use it

You can look at the [test suite](./tests/) to see detailed usage. 

A quick list of code examples per topic is provided here.

<details open><summary><b>Code Examples</b></summary>

<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details open>
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

</details>



</details>

</details>



---

## Build on Windows x64 manually

Build the C# DLL
```powershell
# from the main directory, geompp

# .NET 8 (LTS, supported until Nov 2026)
msbuild geompp_csharp\GeomPP_Net8.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET 9 (STS)
msbuild geompp_csharp\GeomPP_Net9.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET 10 (LTS)
msbuild geompp_csharp\GeomPP.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET Framework 4.8
msbuild geompp_csharp\GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# run smoke tests, after build from the main directory geompp
dotnet test geompp_csharp\tests\GeomPPTests.csproj [-p:GeomPPConfiguration=Release]
```
