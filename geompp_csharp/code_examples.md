[← back](./README.md)

## Examples

### 1. Creation and I/O operations

#### 1.1 LineSegment3D intersection

```csharp
using G = GeomPP;

G.Precision.DecimalPrecision = G.Precision.DP_THREE;

var s1 = G.LineSegment3D.Make(new G.Point3D(1, 0, 0), new G.Point3D(-1, 0, 2));
var s2 = G.LineSegment3D.Make(new G.Point3D(0, 1, 0), new G.Point3D(0, -1, 2));

Console.WriteLine($"s1 = {s1}");
Console.WriteLine($"s2 = {s2}");

var result = s1.Intersection(s2);
if (result is G.Point3D p) {
    Console.WriteLine("OK: " + p.ToWkt());  // expects POINT (0 0 1)
} else {
    Console.WriteLine("FAIL: no intersection");
}
```

Output:
```
s1 = LINESTRING (1 0 0, -1 0 2)
s2 = LINESTRING (0 1 0, 0 -1 2)
intersection found: POINT (0 0 1)
intersection written to intersection.wkt
```

---

#### 1.2 Load geometries from an .lsv file

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

Output:
```
POINT (1 2 3)
POINT (4 5 6)
LINESTRING (0 0 0, 1 1 1)
LINESTRING (2 0 0, 2 3 4)
LINE (0 0 0, 1 0 0)
RAY (0 0 0, 0 1 0)
```

---

### 2. Containment

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

Output:
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

---

### 3. Intersections

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

Output:
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

#### 3.1 Split a complex polygon

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

Output:
```
is simple: False
2 simple polygon(s)
  POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4
  POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1
2 simple 3D polygon(s)
```

---

### 4. Planar operations

#### 4.1 Verify coplanarity, winding order, and polygon with holes

```csharp
using GeomPP;
using Geompp.Extensions;

Precision.DecimalPrecision = Precision.DP_THREE;

var flat = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(1,0,0),
    new Point3D(0,1,0), new Point3D(1,1,0)
};
var skew = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(1,0,0),
    new Point3D(0,1,0), new Point3D(0,0,1)
};

Console.WriteLine(flat.AreCoplanar());   // True  — all on the XY plane
Console.WriteLine(skew.AreCoplanar());   // False — spans 3D space

// Closest world-axis plane and winding check
var plane = flat.ClosestWorldPlaneTo();
Console.WriteLine(plane.Normal());       // (0, 0, 1)  → XY plane

Console.WriteLine(flat.AreCCW());        // True  — CCW on the XY plane
Console.WriteLine(flat.AreCW());         // False

// Polygon3D requires CCW outer ring and CW holes
var outer = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(4,0,0),
    new Point3D(4,4,0), new Point3D(0,4,0)
};
var hole = new List<Point3D> {
    new Point3D(1,3,0), new Point3D(3,3,0),
    new Point3D(3,1,0), new Point3D(1,1,0)
};
var poly = Polygon3D.Make(outer, new List<List<Point3D>> { hole });
Console.WriteLine(poly.Size());          // 4
```

Output:
```
True
False
VECTOR (0 0 1)
True
False
4
```

---

#### 4.2 Planar vs non-planar `Polyline3D`

`Polyline3D.IsPlanar()` checks whether all knots lie in a common plane. Only planar polylines support
`IsSimple()`, `IsConvex()`, `ConvexHull()`, and `ToPolygon()`.

`ConvexHull()` returns a `Polyline3D` (open path). Call `ToPolygon()` on it to close the boundary into a `Polygon3D` with area.

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

Output:
```
planar: True
hull knots:   4
polygon area: 16.000
planar: False
dominant direction: VECTOR (...)
```

#### 4.3 Projecting points onto a plane

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

Output:
```
on_plane: POINT (3 4 0)
in_plane: POINT (3 4)
back:     POINT (3 4 0)
```

---

#### 4.4 View2D — streaming 3D points to 2D

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
var xs = pts3d.Select(p => vXY.X(p)).ToArray();   // [1, 3, 5]
var ys = pts3d.Select(p => vXY.Y(p)).ToArray();   // [2, 4, 6]

Console.WriteLine(vXY.Type());                    // XY
Console.WriteLine(string.Join(", ", xs));         // 1, 3, 5
Console.WriteLine(string.Join(", ", ys));         // 2, 4, 6
```

Output:
```
XY
1, 3, 5
2, 4, 6
```

---

### 5. PCA on a 3D point cloud

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

Output:
```
X (primary):   VECTOR (1 0 0)
Y (secondary): VECTOR (0 1 0)
Z (normal):    VECTOR (0 0 1)
```

---

### 6. Bounding containers

#### 6.1 Simple containers for quick rejection

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

// Axis-aligned bounding box
var box = new G.BBox3D(G.Polyline3D.Make(verts));
Console.WriteLine(box.Min().ToWkt());                     // POINT (0 0 0)
Console.WriteLine(box.Max().ToWkt());                     // POINT (6 4 3)
Console.WriteLine(box.Contains(new G.Point3D(3, 1, 1))); // True
Console.WriteLine(box.Contains(new G.Point3D(7, 1, 1))); // False

// Bounding ball — constructor takes the point array directly
var ball = new G.BBall3D(verts);
Console.WriteLine(ball.Center().ToWkt());
Console.WriteLine($"radius: {ball.Radius():F3}");
Console.WriteLine(ball.Contains(new G.Point3D(3, 1, 1))); // True

// All original vertices must be inside the ball
foreach (var p in verts)
    Console.WriteLine($"{p.ToWkt()} -> {ball.Contains(p)}");  // all True
```

Output:
```
POINT (0 0 0)
POINT (6 4 3)
True
False
POINT (3 ...)
radius: ...
True
```

#### 6.2 Convex hulls

##### 6.2.1 `GeomUtil.ConvexHull` — point cloud

`GeomUtil.ConvexHull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

```csharp
using G = GeomPP;

G.Precision.DecimalPrecision = G.Precision.DP_THREE;

var star = new System.Collections.Generic.List<G.Point2D> {
    new G.Point2D( 0,  5), new G.Point2D( 4,  2),
    new G.Point2D( 3, -3), new G.Point2D(-2, -4), new G.Point2D(-3,  1),
    new G.Point2D( 2,  1), new G.Point2D( 2, -1),
    new G.Point2D( 0, -1), new G.Point2D(-1, -1), new G.Point2D(-1,  2),
};

var hull = G.GeomUtil.ConvexHull(star);
int count = 0;
foreach (var p in hull) { Console.WriteLine(p.ToWkt()); count++; }
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

(CCW order, starting from the lexicographically smallest point)

For 3D point clouds, `GeomUtil.ConvexHull(points)` also works — points do **not** need to be
perfectly coplanar. When no explicit normal is available, the best-fit plane is estimated via PCA
(Jacobi eigendecomposition), and the hull is computed on the projection onto that plane.

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

##### 6.2.2 `Polygon3D.ConvexHull`

`Polygon2D` and `Polygon3D` expose a `ConvexHull()` instance method:

```csharp
using G = GeomPP;

var star = G.Polygon3D.Make(new G.Point3D[] {
    new( 0,  5, 0), new( 2,  1, 0), new( 4,  2, 0), new( 2, -1, 0),
    new( 3, -3, 0), new( 0, -1, 0), new(-2, -4, 0), new(-1, -1, 0),
    new(-3,  1, 0), new(-1,  2, 0),
});

Console.WriteLine($"star is convex: {star.IsConvex()}");   // False
var hull = star.ConvexHull();   // Polygon3D with 5 vertices
Console.WriteLine($"hull is convex: {hull.IsConvex()}");   // True
Console.WriteLine($"hull has {hull.Size()} vertices");     // hull has 5 vertices
```

##### 6.2.3 `Polyline2D.ConvexHull`

`Polyline2D.ConvexHull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `IsSimple()` first.

```csharp
using G = GeomPP;

var path = G.Polyline2D.Make(new G.Point2D[] {
    new(0, 0), new(4, 0), new(4, 4), new(2, 1), new(0, 4),
});

if (path.IsSimple()) {
    var hull = path.ConvexHull();   // Polygon2D with 4 vertices
    Console.WriteLine($"hull has {hull.Size()} vertices");  // hull has 4 vertices
}
```


#### 6.3 Oriented Minimum Bounding Rectangle

`BRect2D` computes the **tightest** rectangle that encloses a point cloud, with axes aligned to the
geometry rather than the coordinate frame. Members: `Center()`, `AxisU()`, `AxisV()`,
`HalfLenU()`, `HalfLenV()`, `Width()`, `Height()`, `Area()`.

**Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
1. Compute the convex hull (Andrew's monotone chain, O(n log n)).
2. For each hull edge, project all hull vertices onto the edge direction and its CCW perpendicular.
3. Track the orientation that minimises the rectangle area; the center is the midpoint of the extents.

```csharp
using G = GeomPP;

var pts = new G.Point2D[] {
    new G.Point2D(0, 0), new G.Point2D(4, 0),
    new G.Point2D(5, 2), new G.Point2D(2, 4),
    new G.Point2D(-1, 2),
};

var r = new G.BRect2D(pts);

Console.WriteLine(r.Center().ToWkt());         // center of the OBB
Console.WriteLine(r.AxisU().ToWkt());          // primary axis (unit vector, along a hull edge)
Console.WriteLine(r.AxisV().ToWkt());          // secondary axis (CCW perpendicular of AxisU)
Console.WriteLine($"half_u: {r.HalfLenU():F3}");
Console.WriteLine($"half_v: {r.HalfLenV():F3}");
Console.WriteLine($"area:   {r.Area():F3}");
Console.WriteLine(r.Contains(new G.Point2D(2, 2)));  // True  — interior
Console.WriteLine(r.Contains(new G.Point2D(9, 0)));  // False — outside

var corners = r.Corners();                     // array<Point2D^> of 4 points, CCW order
foreach (var c in corners)
    Console.WriteLine(c.ToWkt());
```

`Corners()` returns the four corners in CCW order; each satisfies `Contains()`.
`Contains()` is O(1) — it projects the query point onto `AxisU`/`AxisV` and checks
both projections against the half-lengths.
