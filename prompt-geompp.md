
What's actually in road_plugins today

Three parallel geometry stacks, all doing overlapping work:

1. RevitAPI native (DB.XYZ, DB.Plane, DB.Line) — hand-rolled math in PPRoadLib/Geometry/ComputationalGeometry.cs (3,199 lines) and Extensions.cs (3,877 lines): point-in-triangle, point-in-polygon, line/line and ray/segment intersection, polygon orientation, segment-point distance, centroid, area, convex/concave hull, Bezier curves — essentially a second geometry library reimplemented by hand.
2. Clipper2Lib (CP alias) — used for 2D polygon clipping/point-in-polygon/containment (IsPointInPolygon2D, GetPolygonRelationship, etc.).
3. GeomSharp (G alias, v1.0.21) — imported in ~15 files but barely used. In the two biggest files it's basically dead weight.

Concrete evidence this is already a pain point

- Extensions.cs:2214 and :2316: // TODO: replace with GeomSharp (when it will be upgraded!) — right above a hand-rolled Centroid() the author didn't trust GeomSharp to do correctly.
- ComputationalGeometry.cs:297: // TODO: replace with geompp once it's translatable in c# — sitting directly in PolylineIntersectsPolygon2D. The migration intent is already written into the code.
- ComputationalGeometry.cs:786: // this works in python, fails in C#. big WTF on the MIConvexHull call — a real cross-platform inconsistency in a third-party hull library, exactly the class of bug geompp is designed to avoid (same algorithm, same test suite, verified identical behavior across C++/C#/Python).
- Extensions.cs:310: // emergency hack: Revit intersection with face does not work ? project on approx plane — a workaround for a native RevitAPI geometry bug.
- A real bug I found: ComputationalGeometry.cs:1077, XYZLineCoef:
return (p2.Y - p1.Y) / p2.X - p1.X;   // should be / (p2.X - p1.X)
- Operator-precedence bug — this isn't a slope at all. Low blast radius today (it only feeds a debug-log branch in LinesIntersection2D), but it's the kind of silent error a tested library's Line.Direction/AlmostEquals would never let through.

Why geompp specifically fits

- It's already built for this: C++/CLI bindings targeting Windows x64, .NET Framework 4.8 + .NET 8/9/10 — road-plugins is a Revit add-in on net48/Windows, so there's no platform mismatch.
- It already covers nearly everything hand-rolled here: Point/Polygon/Triangle containment, intersection, distance, centroid, signed area, convex hull, polygon boolean ops (Intersection/Union/Difference/Xor), GeomUtil.Clip() for raw point loops — i.e. a drop-in replacement for both the ComputationalGeometry.cs hand-rolled math and most of the Clipper2Lib usage.
- It's actually tested (88-96% method coverage across C++/C#/Python with a shared test suite) vs. the current code, which has zero cross-validation between the RevitAPI math, Clipper2Lib, and GeomSharp paths — three implementations of "is this point in this polygon" that could disagree and nobody would notice.
- Since you're the author, bugs surfaced by this migration get fixed upstream once, in a library with tests, instead of patched locally three times.

What I'd leave alone

Anything touching Revit's actual BRep model — DB.Solid/DB.Face boolean ops (MergeDownSolids, GetIntersectingElements, DB.BooleanOperationsUtils) — geompp doesn't model Revit's solid/face types, so that stays RevitAPI-native. The migration candidates are the pure point/vector/polygon math functions, which is most of ComputationalGeometry.cs and Extensions.cs.

Bottom line

Yes, worth doing — and incrementally: start with the two spots already TODO-tagged for it, then the isolated pure-math functions (point-in-polygon, centroid, area, orientation, convex hull), building a TypeConverter-style DB.XYZ ↔ geompp::Point3D layer (there's already precedent for this — Utils/TypeConverter.cs does the same for GeomSharp today). GeomSharp itself is nearly free to drop since it's mostly unused already.

I have a background agent still surveying the remaining GeomSharp call sites (RvtRampModel.cs, FloorShape.cs, CurveCreator.cs, AssociationStrategy.cs, etc.) for anything actually load-bearing there — I'll report back if it turns up something that changes this picture.



The background survey of GeomSharp's real usage came back, and it changes the picture slightly — GeomSharp is more load-bearing than the empty imports in ComputationalGeometry.cs suggested, but it turns out to be a flakier dependency than I'd assumed. Updates to the assessment:

GeomSharp is actually used for real geometry in 8 files — RvtRampModel.cs, FloorShape.cs, CurveCreator.cs, Checkup.cs, PlanarElement.cs, FloorMarking.cs, AssociationStrategy.cs, ButtonJunctionFromModelLines.cs — for polyline/polygon intersection, concave/convex hull, ray-vs-polygon location tests, and plane projection. So it's not dead weight everywhere, just dead weight in the one 3,199-line file I read first.

But the comments around it are worse than I expected — concrete evidence it's an unreliable dependency:
- FloorShape.cs:572-573: "there is a new function .ToSegments() in GeomSharp v1.0.9+, idk why, the compiler doesn't see it!" — a packaging bug forcing a hand-rolled workaround.
- AssociationStrategy.cs:315,642: "TODO: this function should exist in GeomSharp and Bbox should be a class" — missing functionality, worked around locally, twice.
- Extensions.cs:2395,2444: "TODO: replace with 'Contains' function from the bugfix/intersection_function branch" — referencing a known, unmerged bugfix branch of GeomSharp's Contains/intersection logic. That's a dependency with an outstanding correctness bug that hasn't shipped.
- FloorMarking.cs: PointSet3D.ConvexHull() is called with the comment "can throw!" — an acknowledged-unstable API surface, called anyway.

The TypeConverter.cs conversion layer is exactly the adapter I described earlier, already built and battle-tested: DB (feet) ↔ G ↔ PMOD (meters) ↔ CP, with unit scaling and precision rounding handled consistently. That's the seam migration would reuse — swap the G.* conversions to geompp equivalents, keep the DB/PMOD sides untouched.

This reinforces rather than undercuts the earlier recommendation: GeomSharp isn't just underused, it's a third-party package with known missing features and an unmerged bugfix you're waiting on — while geompp is yours, has parity with GeomSharp's actual usage here (intersection, containment, concave/convex hull, ray location, plane projection), and is already tested across the same C++/C#/Python targets. The migration path is: build a geompp-flavored sibling to TypeConverter.cs, retire GeomSharp file-by-file starting with the ones citing its bugs (FloorShape.cs, AssociationStrategy.cs, Extensions.cs), then tackle the hand-rolled math in ComputationalGeometry.cs.



❯ what functions of geompp could replace the current ones ? also, what other geompp functions could be useful in this repo ?

  Searched for 15 patterns, read 1 file

Now I have concrete, verified API surface to work with. Let me lay out the mapping.

Direct replacements — existing code → geompp

Current code: IsPointInPolygon2D, IsPolylineInsideOrIntersectsPolygon2D (Clipper2
PointInPolygon)
File:line: ComputationalGeometry.cs:214,262
geompp replacement: Polygon2D.Contains(Point2D) / Polygon2D.Intersects(Polyline2D)
Why it's better, not just equivalent: Same winding-number check, no Clipper conversion
round-trip needed
────────────────────────────────────────
Current code: PolylineIntersectsPolygon2D — already TODO-tagged for geompp
File:line: ComputationalGeometry.cs:286-348
geompp replacement: Polygon2D.Intersects(LineSegment2D) / Polygon2D.Intersection(...) per
edge
Why it's better, not just equivalent: Deletes ~60 lines of hand-rolled segment-vs-segment
math
────────────────────────────────────────
Current code: IsPointOnSegment
File:line: ComputationalGeometry.cs:187
geompp replacement: LineSegment2D.Contains(Point2D)
Why it's better, not just equivalent: —
────────────────────────────────────────
Current code: PointToLineLocation (LEFT/RIGHT/ON_SEGMENT/ON_LINE via Determinant3Points2D)
File:line: ComputationalGeometry.cs:358,386
geompp replacement: LineSegment2D.Location(Point2D) / .IsLeft(Point2D)
Why it's better, not just equivalent: This is already used —
ButtonJunctionFromModelLines.cs:127 calls G.Line2D.Location(...) today via GeomSharp.
geompp has the identical concept, so this is a low-risk swap.
────────────────────────────────────────
Current code: LinesIntersection2D (Cramer's rule + manual parallel/collinear branch)
File:line: ComputationalGeometry.cs:1087-1155
geompp replacement: geompp::line_intersection(p0,p1,other_p0,other_p1) free function, or
Line2D.Intersects/Intersection
Why it's better, not just equivalent: Also fixes the XYZLineCoef bug
(ComputationalGeometry.cs:1077, (p2.Y-p1.Y)/p2.X-p1.X missing parens) since that
hand-written slope check disappears entirely
────────────────────────────────────────
Current code: SegmentToRayIntersects2D + RayToSegmentIntersectPoint2D (two separate
hand-rolled functions, duplicated math)
File:line: ComputationalGeometry.cs:970-1064
geompp replacement: Ray2D.Intersection(LineSegment2D) → std::optional<Point2D>
Why it's better, not just equivalent: One call replaces both; optional return means no
more sentinel-null handling
────────────────────────────────────────
Current code: IsPointInBox2D
File:line: ComputationalGeometry.cs:369
geompp replacement: BBox2D.Contains(Point2D)
Why it's better, not just equivalent: Also removes the manual min/max/round dance
────────────────────────────────────────
Current code: ConvexHull (MIConvexHull wrapper) — comment says "works in python, fails in
C#. big WTF"
File:line: ComputationalGeometry.cs:780-795
geompp replacement: geompp::convex_hull_indices / Polygon2D.ConvexHull() /
GeomUtil.ConvexHull()
Why it's better, not just equivalent: Removes the cross-platform MIConvexHull bug outright
— same algorithm, tested identically in C++/C#/Python
────────────────────────────────────────
Current code: ApproxPolygonOrientation, IsPolygonCounterClockwise, IsPolygonClockwise,
GetPolyOrientation (four separate reimplementations of the same idea)
File:line: ComputationalGeometry.cs:420-495, 2437-2448
geompp replacement: geompp::is_ccw(points) / are_cw(points)
Why it's better, not just equivalent: Collapses four hand-rolled, partially-inconsistent
functions (one is "majority vote over triangles", another is "any CW triangle fails it")
 into one tested primitive
────────────────────────────────────────
Current code: Determinant3Points2D, TriangleOrientation2D, TriangleOrientation
File:line: ComputationalGeometry.cs:92-139,358
geompp replacement: Triangle2D.SignedArea() / Triangle2D.IsCCW() / are_ccw(3 points)
Why it's better, not just equivalent: —
────────────────────────────────────────
Current code: IsPointInTriangle2D (barycentric, no guard against degenerate/collinear
triangle → NaN)
File:line: ComputationalGeometry.cs:150
geompp replacement: Triangle2D.Contains(Point2D)
Why it's better, not just equivalent: Fixes the unguarded division-by-zero-denominator
case
────────────────────────────────────────
Current code: GetSignedPolygonArea, GetPolygonArea, GetPolygonsArea,
GetPolygonsFlattenedArea, CalculateCurveLoopArea (five variants of "compute area",
including a plane-fitting step reimplemented from scratch)
File:line: ComputationalGeometry.cs:1880-2036
geompp replacement: signed_area(points) (3D overload auto-detects the plane) /
Polygon2D.Area() / Polygon3D.Area()
Why it's better, not just equivalent: The 3D free function already does the "find the
plane, then measure" logic that GetPolygonsArea hand-rolls with PointToLineLocation +
Plane.CreateByThreePoints
────────────────────────────────────────
Current code: Extensions.cs Centroid(List<UV>), Centroid(List<XYZ>) — both explicitly
TODO'd for GeomSharp/geompp already
File:line: Extensions.cs:2213,2315
geompp replacement: centroid(points) / Polygon2D.Centroid() / Polygon3D.Centroid()
Why it's better, not just equivalent: You already wrote the TODO — this is literally
waiting to be done
────────────────────────────────────────
Current code: VerticalProjectIntoXY, ProjectInto (manual dot-product-onto-plane-axes
projection)
File:line: ComputationalGeometry.cs:1898-1968
geompp replacement: View2D::XY() / .x(p)/.y(p) , or Plane.ProjectInto/ProjectOnto
Why it's better, not just equivalent: View2D is explicitly built for this — "particularly
efficient when streaming large containers of 3D points into 2D"
────────────────────────────────────────
Current code: SegmentPointDistance (3 overloads, hand-rolled via MathNet vectors)
File:line: ComputationalGeometry.cs:866-928
geompp replacement: LineSegment2D.DistanceTo(Point2D) / LineSegment3D.DistanceTo(Point3D)
Why it's better, not just equivalent: Drops the MathNet.Numerics dependency for this one
thing
────────────────────────────────────────
Current code: FillImaginaryPoints + SmoothCurve (custom Laplacian-relaxation corner
smoother, "Punnu's algorithm from Coursera I think")
File:line: ComputationalGeometry.cs:1298-1510
geompp replacement: bezier_smoothing_2(p0, p1, p2, smoothness, minDistance|numSegments)
Why it's better, not just equivalent: Same purpose (round one polyline corner), tested
algorithm instead of an iterative heuristic with a hardcoded max_iters=100 bailout
────────────────────────────────────────
Current code: FitPlane (manual least-squares z = ax+by+c via MathNet matrix inverse)
File:line: ComputationalGeometry.cs:2691
geompp replacement: principal_normal(points) / GeomUtil.PrincipalAxes()
Why it's better, not just equivalent: PCA-based, handles near-vertical planes that a
z=f(x,y) regression can't
────────────────────────────────────────
Current code: MergeFacets (custom shared-edge-chain stitcher for two adjacent polygons)
File:line: ComputationalGeometry.cs:635-687
geompp replacement: Polygon2D.Union(Polygon2D) (or Polygon3D if coplanar)
Why it's better, not just equivalent: This is exactly what Union does for two polygons
sharing a boundary — and it already handles holes, which MergeFacets says it merely
"replicates"
────────────────────────────────────────
Current code: The whole #region PolygonContainment — GetPolygonRelationship,
PolygonContained (×2 overloads), AreTouching, GetEdges (~200 lines, built on Clipper2's
PolyTree)
File:line: ComputationalGeometry.cs:3006-3195
geompp replacement: Polygon2D.Intersects/Touches/Contains,
Polygon2D.Intersection(Polygon2D), Polygon2D.HasHoles()/Holes()
Why it's better, not just equivalent: geompp's Polygon2D natively supports holes and has
Touch/Overlap/Contains/boolean-ops as first-class operations — this region is
reimplementing exactly that surface on top of Clipper primitives
────────────────────────────────────────
Current code: AssociationStrategy.cs bbox-overlap helpers ("TODO: this function should
exist in GeomSharp and Bbox should be a class" ×2)
File:line: AssociationStrategy.cs:315,642
geompp replacement: BBox2D/BBox3D class + .Contains()
Why it's better, not just equivalent: Answers the TODO directly — geompp's BBox is a
first-class type
────────────────────────────────────────
Current code: AssociationStrategy.cs CG.ConvexHull(...) → CP.PathD for signal hulls
File:line: AssociationStrategy.cs:386
geompp replacement: Polygon2D.ConvexHull() / convex_hull(points)
Why it's better, not just equivalent: Same convex-hull swap as above, applied at the call
site that currently round-trips through Clipper
────────────────────────────────────────
Current code: Extensions.cs:2395,2444 — TODO referencing an unmerged GeomSharp bugfix
branch for Contains
File:line: Extensions.cs:2395,2444
geompp replacement: Polygon2D.Contains / LineSegment2D.Contains
Why it's better, not just equivalent: Removes the dependency on someone else's unlanded
fix entirely
────────────────────────────────────────
Current code: FloorShape.cs:572 — .ToSegments() "in GeomSharp v1.0.9+... compiler doesn't
see it" workaround
File:line: FloorShape.cs:561-614
geompp replacement: Polyline2D.ToSegments() / Polygon2D.ToSegments()
Why it's better, not just equivalent: Working version of the exact method they tried to
call

Other geompp functions worth adopting even though nothing calls their equivalent today

- Polyline.Reduce() / dist_decimation / rdp_decimation / vw_decimation — road/lane polylines coming out of Revit or OSM import are frequently over-tessellated; nothing in this repo currently decimates a polyline. Good fit for Modelling/Roads where curve point counts matter for export size.
- polyline_expansion(points, settings) — rounds every inner corner of a polyline in one pass. BeizierCurves/GetBeizierCurveConstructionInfo in ComputationalGeometry.cs hand-build per-corner Bezier control points for junction left/right/center curves; this is the general-purpose version of that same problem (junction fillets, curb returns).
- find_extreme_points(polygon, line) and distance_to(polygon, line) — "the two polygon vertices least/greatest projected along a line's direction" is close to what OrderedPointsToLine (ComputationalGeometry.cs:2059, ~370 lines of matrix-rotation bookkeeping to order points relative to a reference line for PACC/parking-spot construction) is trying to do by hand. Worth checking if find_extreme_points + Location() per point can replace the custom rotate/sort/wrap logic — that function is the single largest, most fragile piece of hand-rolled math in the file.
- tangents_to(polygon, point) / tangents_to(polygon, otherPolygon) — common outer tangent lines between two polygons, or from a point to a polygon. Nothing today computes tangent lines, but this is a natural fit for junction/curb-return geometry and for AssociationStrategy.cs's signal-to-lane visibility checks.
- has_intersections(segments) (Shamos–Hoey) / find_intersections(segments) (Bentley–Ottmann) — validating that an exported road/lane polygon is simple (non-self-intersecting) before writing it out. Polygon.IsSimple() would be a good pre-export sanity check in Validation/ or PostExportChecks.cs, which currently has no systematic self-intersection check.
- Polygon2D.Triangulate(strategy) — if any downstream export or visualization needs a triangulated mesh from a road/parking polygon instead of relying on Revit's own tessellation.
- WktParser / ToWkt()/FromWkt() — already informally present (Extensions.cs has ToWkt() helpers on DB.Line/points used purely for debug logging). Swapping those debug dumps to real WKT via geompp gets you round-trippable output for free — could paste straight into QGIS/PostGIS for debugging exports, which the ad hoc ToStringEx/ToWkt string-builders in Extensions.cs don't give you.
- BBall (bounding ball / minimum enclosing circle) and BRect2D (minimum-area oriented rectangle via rotating calipers) — no current code computes either; likely useful for parking-spot/PACC placement and for tighter Revit BoundingBoxXYZ alternatives in GetIntersectingElements's bbox pre-filter (ComputationalGeometry.cs:2861), where an axis-aligned box is a loose filter for angled roads.

Practical migration note

LineSegment2D.Location()/.IsLeft() and Ray2D.Intersection() map almost 1:1 onto GeomSharp calls already in the codebase (G.Line2D.Location(...) in ButtonJunctionFromModelLines.cs, G.Ray2D implied by the Checkup/CurveCreator ray usage), so those files are the lowest-risk starting point — you're swapping one library's API for a structurally similar one, not introducing a new pattern. The highest-value targets are ComputationalGeometry.cs's PolygonContainment region and OrderedPointsToLine, since both are large, untested-by-comparison, hand-rolled reimplementations of things Polygon2D does natively — but they're also the riskiest to touch first since nothing else depends on their exact (possibly buggy) behavior today, so regression risk is highest there.







// new task polygonize

bro I want to implement the polygonization.
1. I already added my parameters in constants.hpp (PolygonizationParams, with 3 strategies of algorithms)
2. The entry point of these algorithms are 
    vector<Polygon> polygonize(vector<Triangle> const&, PolygonizationParams const&); // 2D and 3D 
                                                                                      // should this go in polygon*.hpp? or calc_utls/*? 
    PolyMesh Mesh::Polygonize(PolygonizationParams const&); // 2D and 3D inside mesh*.hpp/.cpp 
    PolyMesh ConnectedMesh::Polygonize(PolygonizationParams const&); // 2D and 3D inside connected_mesh*.hpp/.cpp 
    vector<Polygon> merge(vector<Polygon> const&); // 2D and 3D
                                                   // will use something somilar to PolygonizationParams::PlanarBoundaryExtraction
                                                   // 1. Group polygons by plane (Normal + Distance)
                                                   // 2. Identify and cancel internal shared edges (A -> B vs B -> A)
                                                   // 3. Chain un-cancelled directed edges into outer boundary loops
3. the algorithm should be implemeted in a way that is usable by the first 2 entry point, at least
   and it will be called, as usual 
   vector<Polygon> polygonize_impl(..., PolygonizationParams const&);
   inside of which there will be some input cleansing, and the switch on the strategy to pick either
    \_ hertel_mehlhorn_polygonization()
    \_ boundary_extraction_polygonization()
    \_ quad_only_polygonization()
  to achieve that it probably needs to implement templated algos
  - inputs are guaranteed CCW per polygon or per triangle 
    (because all our triangles and polygons are, with respect to their normal)
  - however, are_coplanar() needs to be used with the template arguments (because we merge only triangles/polygons on the same plane)
  - the function, for efficiency, probably needs to access the Mesh, ConnectedMesh private members
    so maybe some "friend" function needs to be declared in their hpp files? 
  - probably a function to build_neighbors() given input of "vector<Triangle>" or "mesh data"

4. all the entry points in (2) have to be validated and expanded
5. all the innerworkings (3) have to be cpp tested


M1 — utils build neighbor refs
First checkpoint done and verified: detail::build_neighbor_refs is extracted into utils.hpp/utils.cpp, both GridCellMapForConnectedMesh2D::Make and 3D::Make now call it instead of duplicating the edge-hashmap logic, geompp + geompp_tests build clean, and all 100 ConnectedMesh/GridCell/Mesh/Transformations tests pass unchanged.

M2 — 2D scaffolding, proven on the simplest strategy
MeshFaceView concept, MeshFaceView2D, partition_into_coplanar_clusters (2D path is trivially-coplanar, so this is cheap to get right first), promote trace_directed_boundary/package_result_rings out of anonymous-namespace, and boundary_extraction_polygonization — the one strategy that's just those promoted primitives, no pairwise merge logic. polygonize_impl skeleton dispatching to it. Tests in test_calc_utils2d.cpp against hand-built triangle sets, exercised directly (not yet wired into any public entry point). This is the milestone that validates the whole architecture before I duplicate it anywhere.

M3 — the other two 2D strategies
hertel_mehlhorn_polygonization (union-find + convexity test) and quad_only_polygonization (greedy pairing + odd-triangle passthrough). Each strategy is genuinely separate logic, so separable review points if you want them split further.

M4 — wire the 2D entry points
ConnectedMesh2D::Polygonize(), Mesh2D::Polygonize(), the two narrowly-friended PolyMesh2D assembly paths, free polygonize(vector<Triangle2D>, params). Tests in test_mesh2d.cpp/test_connected_mesh2d.cpp/test_polygon2d.cpp. geompp/CMakeLists.txt gets the new file added as soon as it's created here (needed just to keep building/ctest-ing locally — the other 3 duplicated source lists wait for M7).

M5 — merge(vector<Polygon2D>)
Trivial plane-grouping in 2D (one implicit group), so this isolates the real new logic: outer-ring cancellation, hole touch-detection, and the reverse→Union→reverse hole-merge. Tests including the touching-holes case.

M6 — mirror everything to 3D
MeshFaceView3D, the if constexpr/Plane::AlmostEquals coplanarity gate, all 3 strategies, ConnectedMesh3D/Mesh3D::Polygonize, free polygonize(vector<Triangle3D>), and merge(vector<Polygon3D>) — this is where the real plane hash-bucket-then-verify grouping and the View2D::OnPlane/xyz projection round-trip actually get exercised (2D never needed them). Biggest milestone, since 3D is where most of the earlier design discussion concentrated.

M7 — the other 3 build-system lists geompp_python/CMakeLists.txt' .vcxproj inline core lists.One consolidated pass, right before M8 needs Python/C# to actually build.             
M8 — validation_and_expansion, per entry point polygonize(), Mesh::Polygonize(), merge() — four runs, eachtaking that entry point from "C++-tested" to bound/tested in Python+C#, changelogged, exemplified.

I'd pause for your look after(first working entry points),and M6 (full 2D+3D feature complete) — M3/M5/M7/M8 are more mechanical continuations of what came before. Want me to





> "If each piece ran remove_collinear() independently, the rectangle would drop that
  midpoint vertex from its own ring, and the two pieces would no longer share a common
  vertex there — leaving the square's corner touching the middle of the rectangle's edge
  instead of an actual vertex." 
-> they do not have to share this common vertex because
  (1) it's colinear for the bottom rectangle, 
  (2) it is a vertex of a face stuck midway through the edge of another face (it shouldn't exist). 
  
> what to do:
    - create a FromCCWPoints() which calls remove_collinear() before passing the output back to FromUniqueCCWPoints().
    - let's think about the (2) problem ... somewhere our algo is wrong. 
        - maybe the neighboring function
            - two triangles (top and bottom) are neighbors
              .. but once they get merged into a square (top) or rectnagle (bottom), they aren't 
                .. maybe they should remain triangles (and neighbors) because removing that diagonal would cause the square to casually lie on the rectangle


ANSWER

FromCCWPoints() is added (polygon2d.hpp:213-224, polygon2d.cpp) and builds clean — but I deliberately did not wire it into polygons_from_pieces() yet. Here's why, and what I found digging into point (2):

I looked at the actual rendered images, not just the text. images/polygonize_before.png / polygonize_hertel_mehlhorn.png confirm the concrete case: L-shape = 2 bottom unit-squares merged into a 2×1 rectangle, 1 top unit-square left standalone. The vertex at (1,1) (rectangle's top-edge midpoint / square's bottom-left corner) is the reflex corner of the original L-shape — a real mesh vertex, not injected garbage.

Why remove_collinear() alone can't fix this, even via FromCCWPoints(): on the rectangle's own ring, the three points (0,1), (1,1), (2,1) are exactly collinear — remove_collinear() has no way to see that (1,1) is needed by the square, because that fact lives in a completely different ring it never looks at. So calling FromCCWPoints() on polygons_from_pieces()'s output would strip (1,1) right back out and reintroduce the T-junction — which is exactly why the current code avoids it. Your point (1) (the useless seam vertex at (1,0), needed by nobody) is the only case FromCCWPoints() safely fixes on its own.

The actual mechanism (traced through polygonization2d.cpp): cancel_reverse_pairs() (line 217) already works at raw triangle-edge granularity — every triangle contributes its 3 individual edges, internal ones cancel, survivors pass straight to trace_directed_boundary(). Nothing ever merges collinear survivors into longer runs — there's no step doing this wrong, it's simply absent. So every vertex that shows up in the output is a real triangle-mesh vertex on the group's boundary; the "junk" ones (like (1,0)) are junk only because the code never asks "does the identity of what's on the other side of this edge actually change here."

That question is answerable cheaply, and is the real fix for (2): for every surviving boundary edge, we already know its source triangle face, and FaceViewT::Neighbor() tells us the adjacent face (or nullopt for a true mesh boundary). Combined with HertelMehlhorn's union-find parent array (uf_find), each surviving edge can be tagged with "outer boundary" or "root of neighbor group G". Two consecutive, collinear edges in the traced ring should only ever collapse into one straight edge when their tags match. Under that rule:
- (1,0): both adjoining edges tag as "outer boundary" → collapses away. ✅.
- (1,1): one edge tags "neighbor = square-group", the other "outer boundary" → tags differ → kept, correctly, without needing the current blunt "preserve everything" policy. ✅.

This is a real, targeted fix — not the "preserve every vertex forever" hack currently in place — but it's a moderate change: it touches cancel_reverse_pairs/trace_face_group_boundary_2d to carry the tag through, plus a new selective-collapse pass before package_result_rings.

On your "keep them as triangles/neighbors instead" idea: I'd push back on that one — the partial-edge situation isn't itself wrong (two convex pieces of different sizes sharing part of an edge is completely valid tiling), and forcing HertelMehlhorn to avoid it would mean refusing merges purely to dodge a bookkeeping gap, defeating the "fewest pieces" point of the algorithm. I think the tag-based fix above is the right layer for this.

Want me to implement the neighbor-tagging fix now?



> look not at the commit, it's old, but at the uncommitted changes. talk me through them.
  btw it didn't work. visual doc still shows a bad picture


ANSWER

Good — that prompt-geompp.md file (untracked, your working notes) is the exact design log for this. It confirms and matches what's in the diff. Here's the walkthrough.

The actual bug, per your own notes: polygonize()'s HertelMehlhorn strategy on the L-shape kept a load-bearing T-junction vertex (1,1) (needed by the neighboring square piece) correctly, but also kept a useless one (1,0) (the leftover seam between two merged triangle-squares that nothing else touches) — both were preserved unconditionally because the code had no way to tell "does the far side actually change here?" You correctly rejected your own first idea (a blind FromCCWPoints() → remove_collinear() pass) because it can't distinguish the two: (1,1) is just as collinear on the rectangle's own ring as (1,0) is — the fact that it's load-bearing lives entirely in the neighboring ring, invisible to a per-piece collinear pass.

The real fix in the uncommitted diff (polygonization2d.cpp):
1. far_side_group_of_boundary() — for every raw triangle edge, tags what output-group id lies on the other side (or a kNoNeighborGroup sentinel for a true mesh boundary), using each face's final group id.
2. collapse_redundant_seams() — walks a traced ring and drops a vertex only when it's collinear and the far-side tag matches on both flanking edges. (1,0): both sides tag "outer boundary" → collapses. (1,1): one side tags "square's group," the other "outer boundary" → survives.
3. hertel_mehlhorn_polygonization()/quad_only_polygonization() were each split into two phases (settle every group's final id first, then trace) — needed because collapsing requires knowing a neighbor's final group, not a still-in-progress one.
4. trace_face_group_boundary() now takes an optional face_group_id pointer and runs the collapse before unprojecting back to 3D.
5. FromCCWPoints() (your rejected-but-harmless first idea) is still there in polygon2d.hpp/cpp/polygon3d.hpp but is dead code now — polygons_from_pieces() never calls it, only FromUniqueCCWPoints() directly. Worth deleting or wiring in as a general-purpose API, your call.

I verified the fix actually works by building geompp_tests and running a scratch check on your doc's own 2×1-rectangle example: current output is POLYGON ((0 0, 2 0, 2 1, 0 1, 0 0)) — 4 vertices, fully collapsed, correct. The two regenerated images (polygonize_hertel_mehlhorn.png, polygonize_boundary_extraction.png) also correctly show it: the rectangle's bottom seam-dot is gone, the T-junction dot at the shared corner remains. The code fix is not broken.

What's actually still bad: visual_doc_and_sample_code.md's prose was rewritten to describe the new behavior, but the printed sample output blocks for the 2×1-rectangle example — all three of C++/Python/C# — were never regenerated. They still show the stale pre-fix line:
hertel-mehlhorn:  POLYGON ((0 0, 1 0, 2 0, 2 1, 1 1, 0 1, 0 0))
which contradicts both the code and the prose right above it. That's almost certainly the "bad picture" you're seeing — it's a stale doc artifact, not a regression. Want me to fix those three sample-output blocks (and delete or wire up the dead FromCCWPoints)?



