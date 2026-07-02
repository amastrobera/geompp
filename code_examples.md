[← back](./README.md)

## Examples

  ### 1. Creation and I/O operations

  #### 1.1 Create geometries programmatically
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


  #### 1.2 Import geometries from a file
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


  ### 2. Containment

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


  ### 3. Intersections

  Intersection methods return `std::optional<std::variant<...>>` — callers pattern-match on the exact
  geometry without casting. `find_intersections(segments)` (Bentley–Ottmann) reports all crossing
  points across an arbitrary set of 2D segments, sorted left-to-right.

  ```cpp
  #include "triangle3d.hpp"
  #include "line_segment2d.hpp"
  #include "calc_utils2d.hpp"

  namespace g = geompp;

  // Ray3D vs Triangle3D
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
  POINT (1 1 0)
  POINT (2 1)
  POINT (1 2)
  POINT (2 2)
  POINT (3 2)
  ```

  #### 3.1 Split a complex polygon

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


  ### 4. Planar operations

  #### 4.1 Coplanarity, winding order, and polygon with holes
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


  #### 4.2 Projecting points onto a plane

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


  #### 4.3 Planar vs non-planar Polyline3D

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


  #### 4.4 View2D — streaming 3D points to 2D

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


  ### 5. PCA on a 3D point cloud

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


  ### 6. Bounding containers

  #### 6.1 Simple containers for quick rejection 

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

  #### 6.2 Convex hull

  ##### 6.2.1 Convex hull of a point cloud

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


  ##### 6.2.2 Convex hull of a polygon

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


  ##### 6.2.3 Convex hull of a simple polyline

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


  #### 6.3 Oriented Minimum Bounding Rectangle

  `BRect2D` computes the **tightest** axis-aligned-to-input rectangle that encloses a point cloud.
  It is defined by a center point, two orthogonal unit axes (`axis_u`, `axis_v`), and two half-lengths
  (`half_len_u`, `half_len_v`).

  **Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
  1. Compute the convex hull of the input cloud (Andrew's monotone chain, O(n log n)).
  2. For each hull edge, project all hull vertices onto the edge direction and its perpendicular.
  3. The rectangle aligned with that edge has width = max − min along the edge and height = max − min
     along the perpendicular.
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

  The `Contains()` test projects the query point onto the local axes and checks both projections
  against the half-lengths — O(1) per query.
