[← back](./README.md)

## Examples

### Creation and I/O operations

#### LineSegment3D intersection

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

#### Load geometries from an .lsv file

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

### Containment

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

### Intersections

Intersection methods return `object` (null when there is no intersection); use C# pattern matching to
extract the result type. `GeomUtil.FindIntersections(segments)` (Bentley–Ottmann) reports all crossing
points across an arbitrary set of 2D segments, sorted left-to-right.

```csharp
using G = GeomPP;

// Ray3D vs Triangle3D
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
POINT (1 1 0)
POINT (2 1)
POINT (1 2)
POINT (2 2)
POINT (3 2)
```

---

### Planar operations

#### Verify coplanarity, winding order, and polygon with holes

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

#### Planar vs non-planar `Polyline3D`

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

#### Projecting points onto a plane

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

### PCA on a 3D point cloud

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

### Bounding containers

#### Simple containers for quick rejection

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

#### Convex hulls

##### `GeomUtil.ConvexHull` — point cloud

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

##### `Polygon3D.ConvexHull`

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

##### `Polyline2D.ConvexHull`

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
