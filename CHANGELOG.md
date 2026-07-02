# Changelog

All notable changes to geompp are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Each release covers all three packages at the same version:
- **C++ library** — tagged `v{version}`
- **C# / NuGet** — tagged `csharp-v{version}`
- **Python / PyPI** — tagged `python-v{version}`

---

## [0.12.0] - 2026-07-02

> C++ library — tagged `v0.12.0` · C# / NuGet — tagged `csharp-v0.12.0` · Python / PyPI — tagged `python-v0.12.0`

> Adds four bounding-shape classes (`BRect2D`, `BPrism3D`, `BBall2D`, `BBall3D`) across all three language targets. Enables Link-Time Optimization in Release builds, moves all bounding-shape template constructors to `.cpp` files (extern template pattern), and ships the NuGet package as four separate DLLs covering .NET 8/9/10 and .NET Framework 4.8. Python wheels now cover 3.8–3.14.

### Added

**C++ core**
- `BRect2D` (`brect2d.hpp`) — minimum oriented bounding rectangle. Uses Andrew's monotone-chain convex hull followed by the rotating-calipers algorithm (Freeman & Shapira 1975 / Toussaint 1983) to find the minimum-area enclosing rectangle. Stores `center`, `axis_u`, `axis_v` (both unit vectors), `half_len_u`, `half_len_v`. Methods: `Corners()` (4 `Point2D`), `Contains(Point2D)`, `area()`, `width()`, `height()`, `AlmostEquals()`. Throws `std::invalid_argument` for fewer than 3 points or for a collinear / coincident point cloud (degenerate convex hull).
- `BPrism3D` (`bprism3d.hpp`) — oriented bounding prism. Runs `principal_axes(points)` (PCA) to determine the dominant-variance plane, then applies rotating calipers in that plane to find the minimum-area rectangle; the third axis is the PCA normal, extruded symmetrically to cover all points. Stores `center`, `axis_u`, `axis_v`, `axis_w` (all unit vectors), `half_len_u`, `half_len_v`, `half_len_w`. Methods: `Corners()` (8 `Point3D`), `Contains(Point3D)`, `volume()`, `width()`, `height()`, `depth()`, `AlmostEquals()`. Coplanar inputs produce a valid prism with `half_len_w == DOUBLE_EPSILON`. Throws `std::invalid_argument` for fewer than 3 points or for a collinear / coincident cloud.
- `BBall2D` (`bball2d.hpp`) — minimum bounding ball in 2D; Ritter's two-pass O(N) algorithm. Finds the widest-spread axis-aligned diameter pair in the first pass, then expands the sphere to cover any outliers in the second pass. Stores `center` (`Point2D`) and `radius`. Methods: `Contains(Point2D)`, `AlmostEquals()`. Degenerate inputs: 1 point → zero-radius ball; 2 points → ball with their midpoint as center and half-distance as radius. Throws `std::runtime_error` for empty input.
- `BBall3D` (`bball3d.hpp`) — minimum bounding ball in 3D; same Ritter algorithm extended to three axes (`x_min/max`, `y_min/max`, `z_min/max`). Throws `std::runtime_error` for empty input.

**Python / PyPI**
- `BBall2D(center: Point2D, radius: float)` / `BBall2D(points: list[Point2D])` — mirrors C++ API; `center`, `radius`, `contains(p)`, `almost_equals(other)`.
- `BBall3D(center: Point3D, radius: float)` / `BBall3D(points: list[Point3D])` — same in 3D.
- `BRect2D(points: list[Point2D])` — `center`, `axis_u`, `axis_v`, `half_len_u`, `half_len_v`, `width()`, `height()`, `area()`, `corners()`, `contains(p)`, `almost_equals(other)`.
- `BPrism3D(points: list[Point3D])` — `center`, `axis_u`, `axis_v`, `axis_w`, `half_len_u`, `half_len_v`, `half_len_w`, `width()`, `height()`, `depth()`, `volume()`, `corners()`, `contains(p)`, `almost_equals(other)`.

**C# / NuGet**
- `BBall2D(Point2D^ center, double radius)` / `BBall2D(array<Point2D^>^ points)` — `Center`, `Radius`, `Contains(Point2D^)`, `AlmostEquals(BBall2D^)`.
- `BBall3D(Point3D^ center, double radius)` / `BBall3D(array<Point3D^>^ points)` — same in 3D.
- `BRect2D(array<Point2D^>^ points)` — `Center`, `AxisU`, `AxisV`, `HalfLenU`, `HalfLenV`, `Width()`, `Height()`, `Area()`, `Corners()`, `Contains(Point2D^)`, `AlmostEquals(BRect2D^)`.
- `BPrism3D(array<Point3D^>^ points)` — `Center`, `AxisU`, `AxisV`, `AxisW`, `HalfLenU`, `HalfLenV`, `HalfLenW`, `Width()`, `Height()`, `Depth()`, `Volume()`, `Corners()`, `Contains(Point3D^)`, `AlmostEquals(BPrism3D^)`.
- Added `.NET 8` build target (`GeomPP_Net8.vcxproj`, output `x64\Release_Net8\GeomPP.dll`) and `.NET 9` build target (`GeomPP_Net9.vcxproj`, output `x64\Release_Net9\GeomPP.dll`). The NuGet package now ships four separate C++/CLI DLLs compiled for `net8.0-windows7.0`, `net9.0-windows7.0`, `net10.0-windows7.0`, and `net48`.

### Performance

**C++ core**
- **Link-Time Optimization (LTO)** enabled for Release builds on the `geompp` static library and `_geompp` Python extension. Uses CMake `CheckIPOSupported` with a graceful `STATUS` fallback on platforms where LTO is unavailable (no build failure). On MSVC this activates `/GL` (whole-program compilation) + `/LTCG` (link-time code generation); on GCC/Clang it activates `-flto`.
- **Extern template for all bounding-shape constructors**: the `vector<PointN>` template constructors of `BBox2D`, `BBox3D`, `BBall2D`, `BBall3D`, `BRect2D`, and `BPrism3D` have been moved from header to `.cpp` using the `extern template` / explicit-instantiation pattern (same as `convex_hull_monotone_chain` and `min_bounding_rect`). Every translation unit that includes these headers now suppresses implicit instantiation and links against one shared copy, reducing compilation time and binary size.
- **`View2D` 3D-to-2D projection**: `View2D::x(Point3D)` and `View2D::y(Point3D)` project 3D points into 2D scalars without allocating an intermediate `Point2D` container. Axis-aligned views (`XY`, `YZ`, `ZX`) read a single coordinate component at zero arithmetic cost. The `Custom` projection computes `(p − ORIGIN).Dot(AXIS_U/V)` in-place. These getters are used by the internal `convex_hull_monotone_chain` and `min_bounding_rect` algorithms when operating on 3D point clouds projected onto an arbitrary plane, avoiding any heap allocation per point.

**Python / PyPI**
- Python wheel targets extended to **3.8–3.14** (was 3.8–3.12). Wheels for CPython 3.13 and 3.14 are now published on PyPI for both Linux x86_64 and Windows AMD64.

### Tests

**C++ (`geompp_tests`)**
- `test_bball2d.cpp`: `ConstructorCenterRadius`, `ConstructorFromSinglePoint`, `ConstructorFromTwoPoints`, `ConstructorFromPointsAllContained`, `ConstructorEmptyThrows`, `CopyConstructor`, `Assignment`, `AlmostEquals`, `Contains`.
- `test_bball3d.cpp`: same suite plus `ConstructorFromTwoPointsAlongZ`.
- `test_brect2d.cpp`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_ZeroExtent`, `ConstructorTwoPoints_DegenerateLine`, `ConstructorAxisAlignedSquare`, `ConstructorAxisAlignedRectangle`, `ConstructorNonConvex_SmallArea`, `ConstructorAllPointsContained`, `Accessors_AxisesAreUnitVectors`, `Accessors_AxesOrthogonal`, `Accessors_WidthHeightArea`, `Corners_FourDistinctPoints`, `Contains_Center_True`, `Contains_Interior_True`, `Contains_Boundary_True`, `Contains_Outside_False`, `AlmostEquals_SameRect`, `AlmostEquals_DifferentRect`, `CopyConstructor`, `Assignment`.
- `test_bprism3d.cpp`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_Throws`, `ConstructorTwoPoints_Throws`, `ConstructorAxisAlignedBox`, `ConstructorFlatCloud_WIsEpsilon`, `ConstructorNonConvex_AllPointsContained`, `Accessors_AxesAreUnitVectors`, `Accessors_AxesOrthogonal`, `Accessors_WidthHeightDepthVolume`, `Corners_EightDistinctPoints`, `Contains_Center_True`, `Contains_Interior_True`, `Contains_Outside_False`, `Contains_Boundary_True`, `AlmostEquals_Same`, `AlmostEquals_Different`, `CopyConstructor`, `Assignment`.

**Python (`geompp_python/tests`)**
- `TestBBall2D`: `test_constructor_center_radius`, `test_constructor_from_single_point`, `test_constructor_from_two_points`, `test_constructor_from_points_all_contained`, `test_contains_center`, `test_contains_boundary`, `test_contains_outside`, `test_almost_equals`.
- `TestBBall3D`: same suite.
- `TestBRect2D`: `test_empty_throws`, `test_single_point_zero_extent`, `test_two_points_degenerate_line`, `test_axis_aligned_rectangle_center`, `test_all_points_contained`, `test_axes_are_unit_vectors`, `test_axes_orthogonal`, `test_corners_returns_four_points`, `test_contains_center`, `test_contains_boundary`, `test_contains_outside`, `test_almost_equals`, `test_not_almost_equals_different`.
- `TestBPrism3D`: `test_empty_throws`, `test_single_point_zero_extent`, `test_two_points_degenerate_line`, `test_axis_aligned_box`, `test_flat_cloud_w_is_epsilon`, `test_nonconvex_all_points_contained`, `test_axes_are_unit_vectors`, `test_axes_orthogonal`, `test_width_height_depth_volume`, `test_corners_eight_distinct_all_contained`, `test_contains_center`, `test_contains_outside_false`, `test_almost_equals`, `test_not_almost_equals_different`.

**C# (`geompp_csharp/tests`)**
- `BBall2D` / `BBall3D`: `ConstructorCenterRadius`, `ConstructorFromSinglePoint`, `ConstructorFromTwoPoints`, `ConstructorFromPointsAllContained`, `Contains_Inside_True`, `Contains_Outside_False`, `AlmostEquals_Same`, `AlmostEquals_Different`.
- `BRect2D`: `ConstructorEmpty_Throws`, `ConstructorFromPoints_AllContained`, `Accessors_AxesUnitAndOrthogonal`, `Contains_Center_True`, `Contains_Outside_False`, `AlmostEquals_Same`.
- `BPrism3D`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_Throws`, `ConstructorTwoPoints_Throws`, `ConstructorAxisAlignedBox`, `Accessors_AxesUnitAndOrthogonal`, `Contains_Center_True`, `Contains_Outside_False`, `AlmostEquals_Same`.

---

## [0.11.0] - 2026-06-24

> C++ library — tagged `v0.11.0` · C# / NuGet — tagged `csharp-v0.11.0` · Python / PyPI — tagged `python-v0.11.0`

> Convexity predicates for Polygon and Polyline, planar Polyline3D operations (IsPlanar, IsSimple, ConvexHull, ToPolygon), and PCA-based principal axes for 3D point clouds

### Added

**C++ core**
- `Polygon2D::IsConvex()` (`polygon2d.hpp`) — returns false if the polygon has holes or any concave turn; true otherwise
- `Polygon3D::IsConvex()` (`polygon3d.hpp`) — same, using the stored plane normal for the 3D left-turn test
- `Polyline3D::IsPlanar()` (`polyline3d.hpp`) — true if all knots are coplanar (degenerate cases: <3 points or all collinear also return true)
- `Polyline3D::IsSimple()` (`polyline3d.hpp`) — no self-intersections; uses Shamos–Hoey for planar polylines, Bentley–Ottmann + 3D verification for non-planar
- `Polyline3D::IsConvex()` (`polyline3d.hpp`) — throws `std::logic_error` if not planar; checks all consecutive triples make a left turn relative to the plane normal
- `Polyline3D::ConvexHull()` (`polyline3d.hpp`) — throws if not planar; Melkman's deque algorithm; returns `Polyline3D` (open hull path, not a closed polygon)
- `Polyline3D::ToPolygon()` (`polyline3d.hpp`) — throws if not planar; closes the open path into a `Polygon3D`
- `CoordinateFrame` struct (`calc_utils3d.hpp`) — `Vector3D X` (primary/largest variance), `Y` (secondary), `Z` (normal/least variance)
- `principal_axes(vector<Point3D>)` (`calc_utils3d.hpp`) — PCA via Jacobi eigendecomposition on the 3×3 covariance matrix; returns `CoordinateFrame`; stable for any point distribution including non-planar clouds and helices
- `principal_normal(vector<Point3D>)` (`calc_utils3d.hpp`) — best-fit plane normal; delegates to `principal_axes().Z`
- `principal_direction(vector<Point3D>)` (`calc_utils3d.hpp`) — dominant spread direction; delegates to `principal_axes().X`

**Python / PyPI**
- `Polygon2D.is_convex()`, `Polygon3D.is_convex()`
- `Polyline3D.is_planar()`, `is_simple()`, `is_convex()`, `convex_hull()` → `Polyline3D`, `to_polygon()` → `Polygon3D`
- `CoordinateFrame` class with `x`, `y`, `z` attributes (all `Vector3D`)
- `principal_axes(points)` → `CoordinateFrame`, `principal_normal(points)` → `Vector3D`, `principal_direction(points)` → `Vector3D`

**C# / NuGet**
- `Polygon2D.IsConvex()`, `Polygon3D.IsConvex()`
- `Polyline3D.IsPlanar()`, `IsSimple()`, `IsConvex()`, `ConvexHull()` → `Polyline3D^`, `ToPolygon()` → `Polygon3D^`
- `CoordinateFrame` ref class with `X`, `Y`, `Z` properties (`Vector3D^`)
- `GeomUtil.PrincipalAxes()` → `CoordinateFrame^`, `PrincipalNormal()` → `Vector3D^`, `PrincipalDirection()` → `Vector3D^`

### Fixed

**C++ core**
- `Plane(origin, normal)` private constructor now normalizes the normal (`normal.Normalize()`). Previously the raw (non-unit) vector was stored, causing `SignedDistanceTo` and other distance operations to return scaled results when the input normal was not already a unit vector. `From3Points` and `FromOriginAndAxes` were not affected (they normalized via cross product already).

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: `IsConvex_Square_True`, `IsConvex_ConcavePolygon_False`, `IsConvex_WithHole_False`, `IsConvex_Triangle_True`
- `test_polygon3d.cpp`: `IsConvex_Square_XYPlane_True`, `IsConvex_ConcavePolygon_False`, `IsConvex_WithHole_False`, `IsConvex_YZPlane_True`
- `test_polyline3d.cpp`: `IsPlanar_XYPlane_True`, `IsPlanar_NonPlanar_False`, `IsPlanar_Collinear_True`, `IsSimple_PlanarNoSelfIntersect_True`, `IsSimple_PlanarSelfIntersecting_False`, `IsConvex_PlanarConvex_True`, `IsConvex_PlanarConcave_False`, `IsConvex_NotPlanar_Throws`, `ConvexHull_PlanarPolyline_ReturnsPolyline`, `ConvexHull_NotPlanar_Throws`, `ConvexHull_ThenToPolygon_ValidPolygon`, `ToPolygon_PlanarPolyline_Valid`, `ToPolygon_NotPlanar_Throws`
- `test_calc_utils3d.cpp`: `PrincipalAxes_PlanarXYCloud_ZIsNormal`, `PrincipalAxes_ElongatedAlongX_XIsLongest`, `PrincipalAxes_AxesAreOrthogonal`, `PrincipalAxes_AxesAreUnitVectors`, `PrincipalNormal_PlanarCloud_MatchesBasisZ`, `PrincipalDirection_ElongatedAlongX_MatchesBasisX`, `PrincipalAxes_TooFewPoints_Throws`

**Python (`geompp_python/tests`)**
- `TestPolygon2DIsConvex`: `test_square_is_convex`, `test_concave_not_convex`, `test_with_hole_not_convex`
- `TestPolygon3DIsConvex`: `test_square_xy_plane_is_convex`, `test_concave_not_convex`, `test_with_hole_not_convex`
- `TestPolyline3DPlanarConvex`: `test_is_planar_xy`, `test_is_planar_nonplanar`, `test_is_simple_planar`, `test_is_convex_planar`, `test_is_convex_not_planar_throws`, `test_convex_hull_returns_polyline`, `test_convex_hull_to_polygon`, `test_to_polygon_not_planar_throws`
- `TestPrincipalAxes`: `test_coordinate_frame_attributes`, `test_z_is_normal_for_flat_xy_cloud`, `test_axes_are_orthogonal`, `test_axes_are_unit_vectors`, `test_principal_normal_matches_z`, `test_principal_direction_matches_x`, `test_too_few_points_throws`

**C# (`geompp_csharp/tests`)**
- `IsConvex_Square_True`, `IsConvex_Concave_False`, `IsConvex_WithHole_False` (Polygon2D)
- `IsConvex_Square_True`, `IsConvex_Concave_False` (Polygon3D)
- `IsPlanar_XY_True`, `IsPlanar_NonPlanar_False`, `IsSimple_True`, `IsConvex_Planar_True`, `IsConvex_NotPlanar_Throws`, `ConvexHull_ReturnsPolyline`, `ConvexHull_ThenToPolygon`, `ToPolygon_Valid`, `ToPolygon_NotPlanar_Throws` (Polyline3D)
- `PrincipalAxes_NotNull`, `PrincipalAxes_Z_IsNormal`, `PrincipalNormal_NotNull`, `PrincipalDirection_NotNull`, `PrincipalDirection_AlongX` (GeomUtil)

---

## [0.10.0] - 2026-06-20

> C++ library — tagged `v0.10.0` · C# / NuGet — tagged `csharp-v0.10.0` · Python / PyPI — tagged `python-v0.10.0`

> Adds `convex_hull` (Andrew's monotone chain) for 2D and 3D point clouds and as a method on `Polygon2D` / `Polygon3D`. Moves `has_intersections` / `find_intersections` to the `line_segment2d` public API and simplifies `find_intersections` to return plain crossing points — segment indices are no longer part of any public API.

### Added

**C++ core**
- `convex_hull(std::vector<Point2D> const& points)` (`point2d.hpp`) — Andrew's monotone chain algorithm; returns the convex hull of a 2D point cloud as a `std::vector<Point2D>` in counter-clockwise order. Sets of ≤ 3 points are returned as-is.
- `convex_hull(std::vector<Point3D> const& points, std::optional<Vector3D> normal = std::nullopt)` (`point3d.hpp`) — projects coplanar points onto their dominant-axis plane, runs the monotone-chain core, and lifts back to 3D. Auto-detects the plane normal if omitted; throws on non-coplanar input.
- `Polygon2D::ConvexHull()` — instance method returning a new `Polygon2D` whose vertices are the hull in CCW order.
- `Polygon3D::ConvexHull()` — same for 3D polygons.
- `Polygon3D::IsSimple()` — returns `true` if the polygon boundary has no self-intersections; uses dominant-axis projection to `LineSegment2D` then delegates to `has_intersections` (Shamos–Hoey).
- `Polyline2D::ConvexHull()` — Melkman's O(n) convex hull for simple polylines; returns a `Polygon2D`. Precondition: the polyline must be simple (call `IsSimple()` first); behaviour is undefined on non-simple input.

**Python / PyPI**
- `convex_hull(points)` — module-level free function accepting a `list[Point2D]`; returns `list[Point2D]` in CCW order.
- `convex_hull(points, normal=None)` — accepts `list[Point3D]`; optional `Vector3D` normal for the projection plane.
- `Polygon2D.convex_hull()` / `Polygon3D.convex_hull()` — instance methods mirroring the C++ API.
- `Polygon3D.is_simple()` — returns `True` if the polygon boundary has no self-intersections.
- `Polyline2D.convex_hull()` — Melkman's O(n) hull; returns a `Polygon2D`. Call `is_simple()` first.

**C# / NuGet**
- `GeomUtil.ConvexHull(List<Point2D^>^)` → `IEnumerable<Point2D^>` — convex hull in CCW order.
- `GeomUtil.ConvexHull(List<Point3D^>^)` → `IEnumerable<Point3D^>` — 3D point cloud convex hull.
- `Polygon2D::ConvexHull()` / `Polygon3D::ConvexHull()` — instance methods.
- `Polygon3D::IsSimple()` — returns `true` if the polygon boundary has no self-intersections.
- `Polyline2D::ConvexHull()` — Melkman's O(n) hull; returns a `Polygon2D^`. Call `IsSimple()` first.

### Changed

**C++ core**
- `has_intersections(std::vector<LineSegment2D> const&)` and `find_intersections(std::vector<LineSegment2D> const&)` are now declared in `line_segment2d.hpp` (previously only in `calc_utils2d.hpp`). The underlying sweep-line templates are renamed to `has_intersections_impl` / `find_intersections_impl` and remain internal to `calc_utils2d`.
- `find_intersections` return type changed from `std::vector<IntersectionEvent2D>` to `std::vector<Point2D>` — only the crossing coordinates are returned; per-segment indices are no longer exposed in the public API.

**Python / PyPI**
- `find_intersections(segments)` now returns `list[Point2D]` (was `list[IntersectionEvent2D]`). Access coordinates directly via `.x` / `.y` instead of `.point.x` / `.point.y`.
- `IntersectionEvent2D` class removed from the public API.

**C# / NuGet**
- `GeomUtil.FindIntersections(List<LineSegment2D^>^)` now returns `IEnumerable<Point2D^>` (was `IEnumerable<IntersectionEvent2D^>`).
- `IntersectionEvent2D` class removed from the public API.

### Fixed

**C++ core**
- `convex_hull`: `std::vector<Point2D> cv(n)` attempted to default-construct `n` `Point2D` objects (no default constructor exists); replaced with `cv.reserve(n)` + `emplace_back`. The previous code produced a vector of size `2n` on compilers that accepted it, or a build error on strict MSVC.
- `point2d.hpp`: duplicate `convex_hull` declaration (appeared in both the first and second `Collections Operations` regions) removed.

### Tests

**C++ (`geompp_tests`)**
- `test_point2d.cpp`: `ConvexHull_TooFewPoints_ReturnsAsIs`, `ConvexHull_ConvexSquare_ReturnsSamePoints`, `ConvexHull_AsymmetricStar_HullIsPentagon` (five outer tips at unequal radii; five inner concave vertices excluded from the hull).
- `test_point3d.cpp`: `ConvexHull_TooFewPoints_ReturnsAsIs`, `ConvexHull_CoplanarSquare_XYPlane_ReturnsFourCorners`, `ConvexHull_CoplanarSquare_YZPlane_ReturnsFourCorners`, `ConvexHull_AsymmetricStar_HullIsPentagon` (3D coplanar point cloud).
- `test_polygon2d.cpp`: `ConvexHull_StarPolygon_IsAPentagon`, `ConvexHull_ConvexPolygon_Unchanged`.
- `test_polygon3d.cpp`: `ConvexHull_StarPolygon_IsAPentagon`, `ConvexHull_ConvexPolygon_Unchanged`; `IsSimple_ConvexSquareIsSimple`, `IsSimple_ConvexSquare_YZPlane_IsSimple`, `IsSimple_SelfIntersectingIsNotSimple`.
- `test_polyline2d.cpp`: `ConvexHull_TooFewPoints_Throws`, `ConvexHull_ThreePoints_ReturnsTriangle`, `ConvexHull_ConcavePath_InnerPointExcluded`.
- `test_calc_utils2d.cpp`: updated 7 tests that previously accessed `IntersectionEvent2D::Point` / `::SegmentIds`; now access `Point2D` coordinates directly. Two tests using `SegmentRange2D` updated to call `has_intersections_impl` / `find_intersections_impl` directly (those are the only callers that still need the internal templates).

**Python (`geompp_python/tests`)**
- `TestConvexHull`: `test_convex_hull_few_points`, `test_convex_hull_square`, `test_convex_hull_asymmetric_star_pentagon`.
- `TestConvexHull3D`: `test_few_points_returns_as_is`, `test_coplanar_square_xy_plane`, `test_coplanar_square_yz_plane`, `test_asymmetric_star_hull_is_pentagon`, `test_with_explicit_normal`.
- `TestPolygon2DConvexHull`: `test_convex_hull_star_is_pentagon`, `test_convex_hull_convex_polygon_unchanged`.
- `TestPolygon3DConvexHull`: `test_convex_hull_star_is_pentagon`, `test_convex_hull_convex_polygon_unchanged`.
- `TestPolygon3DIsSimple`: `test_is_simple_convex_square_xy_plane_true`, `test_is_simple_convex_square_yz_plane_true`, `test_is_simple_self_intersecting_false`.
- `TestPolyline2DConvexHull`: `test_too_few_points_throws`, `test_three_points_returns_triangle`, `test_concave_path_inner_point_excluded`.
- Existing `test_find_intersections_reports_crossing`: updated to use `.x` / `.y` directly (was `.point.x` / `.point.y`).

**C# (`geompp_csharp/tests`)**
- `ConvexHull_AsymmetricStar_IsAPentagon`, `ConvexHull_StarOuterTipsAllOnHull`, `ConvexHull_FewPoints_ReturnsAsIs`.
- `ConvexHull3D_XYPlaneSquare_ReturnsFourCorners`, `ConvexHull3D_AsymmetricStar_IsAPentagon`, `ConvexHull3D_StarOuterTipsAllOnHull`.
- `ConvexHull_StarPolygon_IsAPentagon` (Polygon2D method), `ConvexHull3D_StarPolygon_IsAPentagon` (Polygon3D method).
- `IsSimple3D_ConvexSquare_XYPlane_True`, `IsSimple3D_ConvexSquare_YZPlane_True`, `IsSimple3D_SelfIntersecting_False`.
- `ConvexHull_TooFewPoints_Throws`, `ConvexHull_ThreePoints_ReturnsTriangle`, `ConvexHull_ConcavePath_InnerPointExcluded` (Polyline2D method).
- Existing `FindIntersections_ReportsCrossing`: updated to iterate `IEnumerable<Point2D^>` and access `.X` / `.Y` directly.

---

## [0.9.0] - 2026-06-15

> C++ library — tagged `v0.9.0` · C# / NuGet — tagged `csharp-v0.9.0` · Python / PyPI — tagged `python-v0.9.0`

> Adds polygon simplicity testing (`Polygon2D::IsSimple`) backed by a new `calc_utils2d` sweep-line module implementing the Shamos–Hoey and Bentley–Ottmann algorithms for segment-set intersection, plus the supporting 2D orientation/intersection primitives `is_left` / `is_right` / `intersect`.

### Added

**C++ core**
- `Polygon2D::IsSimple()` — returns `true` if the polygon's boundary (outer ring and all holes) is free of self-intersections; delegates to `has_intersections(ToSegments())`.
- `calc_utils2d.hpp` / `.cpp` — new sweep-line module:
  - `EventType2D` enum — `LEFT(0)` / `INTERSECTION(1)` / `RIGHT(2)`; enum value order is load-bearing (ties are broken LEFT < INTERSECTION < RIGHT).
  - `Event2D` — sweep event carrying a `Point2D`, a `SegmentId`, an optional `InterSegmentId` (INTERSECTION events only), and `operator<` / `operator>` / `operator==`; `operator>` is required by `std::greater<Event2D>` inside `EventMinHeap`.
  - `EventQueue2D` — min-heap priority queue (`EventMinHeap`, built on `std::priority_queue` with `std::greater<Event2D>`) constructed from a `std::vector<LineSegment2D>`, a `SegmentRange2D`, or a `Polygon2D`; exposes `Top()`, `Pop()`, `Empty()`, `Push()`, `Contains()`. `Contains()` recognises that `INTERSECTION(A,B)` and `INTERSECTION(B,A)` are the same event (commutative check).
  - `SweepLineComparator<Segments>` — functor used as the `std::set` comparator inside `SweepLine2D`; orders segments by their y-value at the current sweep x (`GetYAtX` with a midpoint fallback for vertical segments) and breaks ties by segment id to prevent `std::set` treating geometrically equal-y segments as identical.
  - `SweepLine2D<SegmentList>` — templated status structure over the `SegmentList` concept (any type offering `size()` + indexed `LineSegment2D` access); members `Add`, `Get`, `Remove`, `SetX`, `GetX`. `Add` and `Get` return a `SweepLineElement2D` triplet `{Segment, Above, Below}` giving the inserted/queried segment together with its immediate neighbours. Member definitions live in `.cpp` and are emitted via explicit instantiation for `std::vector<LineSegment2D>` and `SegmentRange2D`.
  - `IntersectionEvent2D` — output struct holding a `Point2D` and `std::vector<std::size_t> SegmentIds`; the vector length is ≥ 2 and grows when three or more segments cross at the same point (star case).
  - Free `has_intersections(segments)` — Shamos–Hoey algorithm; returns `true` as soon as any crossing is found.
  - Free `find_intersections(segments)` → `std::vector<IntersectionEvent2D>` — Bentley–Ottmann algorithm; reports every crossing point together with all segment ids that pass through it, sorted left-to-right.
- `is_left(v1, v2, p)` / `is_right(v1, v2, p)` (`point2d.hpp`) — orientation of point `p` relative to directed edge `v1→v2` (strict; a point on the line returns neither).
- `intersect(seg1, seg2)` (`line_segment2d.hpp`) — boolean segment-segment crossing test via orientation signs; returns `true` for proper crossings, shared endpoints, and T-intersections.
- `shares_endpoint(seg1, seg2)` (internal free function in `calc_utils2d.cpp`) — true if any endpoint of `seg1` equals any endpoint of `seg2`; used as a guard so that the algorithms do not report adjacent segments as intersecting.

**Python / PyPI**
- `Polygon2D.is_simple()`.
- `has_intersections(segments)` / `find_intersections(segments)` — module-level free functions accepting a `list[LineSegment2D]`; `find_intersections` returns a `list[IntersectionEvent2D]`.
- `IntersectionEvent2D` — new type with properties `point` (`Point2D`), `segment_id1` (int), `segment_id2` (int), and `segment_ids` (list[int], all segment indices through that point).

**C# / NuGet**
- `Polygon2D.IsSimple()`.
- `GeomUtil.HasIntersections(List<LineSegment2D^>)` and `GeomUtil.FindIntersections(List<LineSegment2D^>)` → `IEnumerable<IntersectionEvent2D^>`.
- `IntersectionEvent2D` — managed wrapper with properties `Point` (`Point2D^`), `SegmentId1` (int), `SegmentId2` (int), and `SegmentIds` (`List<int>^`, all segment indices through that point).

**Tooling**
- `BUILD_TESTING OFF` set before `FetchContent_MakeAvailable(glog)` — prevents glog's own unit tests from registering with CTest; previously they appeared as spurious failures in Docker.

### Fixed

**C++ core (`calc_utils2d`)**
- Event queue was a **max-heap** (popped RIGHT events before LEFT): switched to `EventMinHeap` (`std::greater<Event2D>`) so the sweep proceeds left-to-right.
- `SweepLineComparator::GetYAtX` divided by zero for vertical segments: now returns the midpoint y and skips the linear-interpolation branch.
- `SweepLineComparator::operator()` lacked a tiebreaker: when two segments share the same y at the sweep x, `id1 < id2` prevents `std::set` from treating them as equal and silently dropping one.
- `has_intersections` and `find_intersections` called `intersect()` on adjacent (shared-endpoint) segments, producing false positives: added `shares_endpoint()` guard at every intersection check site.
- `EventQueue2D::Contains()` did not recognise `INTERSECTION(A,B)` and `INTERSECTION(B,A)` as the same event: added commutative reversed-pair check.
- INTERSECTION handler in `find_intersections` pushed new-neighbour events without a `Contains()` guard: fixed, preventing duplicate events.
- Stale INTERSECTION events (queued before a third segment was inserted between the pair) were processed unconditionally, corrupting sweep-line order: adjacency check added — the event is skipped if `seg1` and `seg2` are no longer immediate neighbours.
- Star case (three or more segments crossing at the same point): each co-incident INTERSECTION event reset `sweep_x` to the intersection x and re-advanced by `+DOUBLE_EPSILON`, accumulating drift; the outer `SetX` call is now conditional (`< 0` guard), and the inner advance is anchored to `inter_event.Point.x() + DOUBLE_EPSILON` so sweep_x stays at exactly `P.x + ε` for all events at the same point.
- `std::get<Point2D>` called unconditionally on `Intersection()` results: guarded with `std::holds_alternative<Point2D>` at all five call sites — collinear overlapping segments are now silently skipped instead of throwing `std::bad_variant_access`.

**C# / NuGet**
- `IntersectionEvent2D` bindings referenced non-existent fields `SegmentId1` / `SegmentId2` on the native struct (which uses `SegmentIds`): fixed to `_native->SegmentIds[0]` / `[1]`.

**Python / PyPI**
- `bind_free_functions.cpp` used `.def_readonly("segment_id1", &IntersectionEvent2D::SegmentId1)` referencing non-existent fields: replaced with `.def_property_readonly` lambdas over `SegmentIds[0]` / `[1]`.

---

## [0.8.0] - 2026-05-15

> C++ library — tagged `v0.8.0` · C# / NuGet — tagged `csharp-v0.8.0` · Python / PyPI — tagged `python-v0.8.0`

> Touches `Plane`, `Triangle3D`, `Point2D`, `Point3D`, `Line3D`, `Ray3D`, `LineSegment3D`. Adds the `Distance` / `DistanceTo` family across every pair of 3D linear primitives, factors the closest-points-of-two-lines math into a new `calc_utils3d` header, and introduces a Doxygen-driven API documentation pipeline. Closes 6 stub methods (19 → 13).

### Added

**C++ core**
- `Plane::Intersects(Ray3D)` / `Plane::Intersection(Ray3D)` — delegates to the line case, then keeps the hit only if it is ahead of the ray's origin.
- `Plane::Intersects(LineSegment3D)` / `Plane::Intersection(LineSegment3D)` — delegates to the line case, then keeps the hit only if it lies within the segment.
- `Plane::Intersects(Plane)` / `Plane::Intersection(Plane)` — closed-form line of intersection from the cross product of the two normals and a point in the span of those normals; returns a `Line3D` (direction = `N1 × N2`).
- `Plane::Intersects(Triangle3D)` / `Plane::Intersection(Triangle3D)` — delegates to the new `Triangle3D::Intersection(Plane)` (unwraps/rewraps because the two `ReturnSet` variants don't share alternatives).
- `Plane::IsParallel(Line3D / Ray3D / LineSegment3D)` — tests `direction · normal == 0`. Returns `true` for lines/rays/segments lying *in* the plane (coplanar ⊂ parallel by this definition).
- `Plane::IsCoplanar(Line3D / Ray3D / LineSegment3D)` — strict-subset of parallel: requires the first point to also lie on the plane.
- `Triangle3D::Intersection(Line3D)` — intersects the line with the triangle's plane, then runs a 3D barycentric inside-test (`U·U`, `V·V`, `U·V`, `W·U`, `W·V`) using `within_axis_boundary(sc, tc)`. No 2D projection.
- `Triangle3D::Intersection(Ray3D)` — line-intersection result, kept only if `ray.IsAhead(hit)`.
- `Triangle3D::Intersection(LineSegment3D)` — line-intersection result, kept only if `segment.Contains(hit)`.
- `Triangle3D::Intersection(Plane)` — plane-plane intersection line, then projects both the line and the triangle vertices into the triangle's 2D frame and delegates to `Triangle2D::Intersection(Line2D)`; lifts the resulting `Point2D`/`LineSegment2D` back to 3D via `Plane::Evaluate`.
- `Triangle3D::Intersection(Triangle3D)` — computes each triangle's intersection with the other's plane (both yield collinear `LineSegment3D`s on the planes' common line) and returns the segment overlap.
- `Triangle3D::Intersects(Plane)` — `bool` overload.
- `Point2D(Vector2D const&)` — implicit construction from a vector. Mirrors the new `Point3D(Vector3D const&)` constructor.
- `Point3D(Vector3D const&)` — implicit construction from a vector. Lets `Vector3D` arithmetic results flow directly into `Point3D`-typed APIs (e.g., `Line3D::Make(Vector3D-result, …)`).
- `calc_utils3d.hpp` / `.cpp` — new header exposing two free functions:
  - `distance_line_to_line(L1_P0, L1_P1, L2_P0, L2_P1, sc, tc)` — solves the 2×2 perpendicular-distance system between two parameterized lines using Cramer's rule; `sc=0` / `tc=largest-denominator-projection` in the parallel branch (gives the "magic zero" when lines are collinear).
  - `intersection_line_to_line(L1_P0, L1_P1, L2_P0, L2_P1, sc, tc) -> std::optional<Point3D>` — same setup, but returns the unique intersection point when the closest-approach distance is zero and the lines aren't parallel.
- `Line3D::Distance(Line3D|Ray3D|LineSegment3D)` — directed `LineSegment3D` from this line's closest point to the other primitive's closest point; `nullopt` when they intersect or overlap.
- `Line3D::DistanceTo(Line3D|Ray3D|LineSegment3D)` — scalar distance; 0 when they intersect or overlap.
- `Ray3D::Distance(Line3D|Ray3D|LineSegment3D)` / `Ray3D::DistanceTo(...)` — same shape for rays, with the ray-parameter clamp `sc >= 0`.
- `LineSegment3D::Distance(Line3D|Ray3D|LineSegment3D)` / `LineSegment3D::DistanceTo(...)` — same shape for segments, with the segment-parameter clamp `sc ∈ [0, 1]`.
- `LineSegment3D::Flip()` — returns a segment with endpoints swapped.

**Python / PyPI**
- `Plane.intersects(ray|segment|plane|triangle)` / `Plane.intersection(...)` overloads added.
- `Plane.is_parallel(line|ray|segment)` and `Plane.is_coplanar(line|ray|segment)` added.
- `Triangle3D.intersects(plane)` and `Triangle3D.intersection(plane)` overloads added.
- `Triangle3D.intersection(line|ray|segment|triangle)` now return real results (the bindings existed before but routed to stubs that threw).
- `Point2D(Vector2D)` and `Point3D(Vector3D)` constructors exposed via `py::init<const Vector2D&>()` / `py::init<const Vector3D&>()`.
- `Line3D.distance(other|ray|segment)` / `Line3D.distance_to(other|ray|segment)`.
- `Ray3D.distance(line|other|segment)` / `Ray3D.distance_to(line|other|segment)`.
- `LineSegment3D.distance(line|ray|other)` / `LineSegment3D.distance_to(line|ray|other)`.
- `LineSegment3D.flip()`.

**C# / NuGet**
- `Plane.Intersects(Ray3D^|LineSegment3D^|Plane^|Triangle3D^)` and matching `Plane.Intersection(...)` overloads. Plane∩Plane returns a `Line3D^` (or `null`); Plane∩Triangle returns a `Point3D^` / `LineSegment3D^` (or `null`).
- `Plane.IsParallel(Line3D^|Ray3D^|LineSegment3D^)` and `Plane.IsCoplanar(Line3D^|Ray3D^|LineSegment3D^)`.
- `Triangle3D.Intersects(Plane^)` and `Triangle3D.Intersection(Plane^)` overloads.
- `Triangle3D.Intersection(Line3D^|Ray3D^|LineSegment3D^|Triangle3D^)` now return real results (the bindings existed but the native side threw).
- `Point2D(Vector2D^)` and `Point3D(Vector3D^)` constructors.
- `Line3D.Distance(Line3D^|Ray3D^|LineSegment3D^)` / `Line3D.DistanceTo(...)`.
- `Ray3D.Distance(Line3D^|Ray3D^|LineSegment3D^)` / `Ray3D.DistanceTo(...)`.
- `LineSegment3D.Distance(Line3D^|Ray3D^|LineSegment3D^)` / `LineSegment3D.DistanceTo(...)`.

**Tooling / Documentation**
- `docs/Doxyfile` — Doxygen config: reads `geompp/include/*.hpp`, emits HTML reference under `docs/api/cpp/html/` and machine-readable XML under `docs/api/cpp/xml/`.
- `docs/gen_bindings_md.py` — transforms the Doxygen XML into per-class Markdown for three languages: `docs/api/cpp/md/<Class>.md`, `docs/api/python/<Class>.md`, `docs/api/csharp/<Class>.md`. Maps types and names per language (e.g., `LineSegment3D const&` → `LineSegment3D` in Python, `LineSegment3D^` in C#; `std::optional<X>` → `X | None` in Python, `X^ (nullable)` in C#; PascalCase → `snake_case` for Python only). Cross-links every class-name occurrence in signatures, parameter columns, and prose, and appends a `**See also:** …` footer per file.
- Doxygen `@brief` / `@param` / `@return` comments added to ~155 public methods across `line3d`, `line_segment3d`, `ray3d` (3D linear primitives), their 2D counterparts (`line2d`, `line_segment2d`, `ray2d`), `triangle2d/3d`, `polyline2d/3d`, `polygon2d/3d`, `bbox2d/3d`, `plane`, and `wkt_parser`. Coverage targets methods in `#pragma region line operations` and `#pragma region Geometrical Operations` (plus Plane's `Geometrial Operations` / `Collection Operations`), plus the typical ray helpers (`IsAhead`, `IsBehind`, `ToLine`, `ProjectOnto`) and `WktParser::Open` / `Next` / `HasNext`.

### Changed

**C++ core**
- `Plane::SignedDistanceTo(Point3D)` — removed the `round()` call. The default precision was 0 decimal places, so the function used to snap the signed distance to the nearest integer. This silently broke `Plane::ProjectOnto` (which multiplies the signed distance by the normal) for points within ±0.5 of the plane.
- `Triangle3D::Intersects(Triangle3D)` — was a throwing stub; now `return Intersection(other).has_value()` like every other `Intersects` overload.
- `Plane::Intersection(Triangle3D)` — was a throwing stub; now delegates to `Triangle3D::Intersection(Plane)`, unwrapping its variant into `Plane::ReturnSet`'s alternatives (`Point3D` or `LineSegment3D`).
- `Line3D::Intersection(other, sc, tc)` — the internal three-argument overload (used as a shared kernel by `Ray3D` / `LineSegment3D` intersection) was removed from `Line3D`'s public surface. The math now lives in the free function `intersection_line_to_line` in `calc_utils3d`. The single-argument `Line3D::Intersection(Line3D)` is unchanged.

### Fixed

**C++ core**
- `Plane` header had a duplicate `bool Intersects(Line3D const&) const;` declaration; removed.
- `Ray3D::DistanceTo(Line3D)` — when a ray hit the line at a non-origin point (`Distance` returned `nullopt`), the function fell through to `other.DistanceTo(ORIGIN)` (perpendicular distance from the ray's origin to the line) instead of returning 0. Now returns 0 whenever `Distance` is `nullopt` (intersect or overlap), matching the docstring.
- `Ray3D::Distance(Ray3D)`, `LineSegment3D::Distance(Ray3D)`, `LineSegment3D::Distance(LineSegment3D)` — collinear-overlap cases (two primitives sharing a region of the same infinite line) returned a non-zero segment because the ray / segment parameter clamping ran *before* the closest-points equality check, corrupting `distance_line_to_line`'s "magic zero" in the parallel branch. Each function now detects collinearity (unclamped points coincide) and short-circuits to `nullopt` when the two primitives genuinely share a region (verified via `Contains` on endpoints/origins).

### Notes / known limitations

- `Triangle2D::Intersection(Line2D)` returns `nullopt` when *all* intersection points coincide with triangle vertices (the deliberate "touch along an edge ≠ intersection" rule at `triangle2d.cpp:144`). This propagates through `Triangle3D::Intersection(Plane)` and `Triangle3D::Intersection(Triangle3D)`: a plane that cuts the triangle exactly along an edge will report no intersection. Tests document this behavior rather than work around it.
- `Plane::Intersection(Plane)`, `Plane::Intersection(Line3D/Ray3D/LineSegment3D)`, and the propagated triangle variants still collapse the coplanar case to `nullopt` — the `ReturnSet` variant can't represent "infinite intersections." `IsCoplanar` is the workaround.
- `Triangle3D::Intersection(Triangle3D)`: when the two triangles' plane-intersection segments are collinear but disjoint, the segment-overlap branch falls through and throws `"unexpected type of intersection result"` instead of returning `nullopt`. The test `Triangle3DTest.IntersectionWTriangle` asserts this with `EXPECT_ANY_THROW` so it's documented.

### Tests

**C++ (`geompp_tests`)**
- `test_plane.cpp`: `IntersectionWRay`, `IntersectionWLineSegment`, `IntersectionWPlane`, `IntersectionWTriangle`, `IsParallelWLine/Ray/LineSegment`, `IsCoplanarWLine/Ray/LineSegment` added.
- `test_triangle3d.cpp`: `IntersectionWLine` replaced with a real test (interior hit, vertex hit, edge-midpoint, miss, parallel-above, coplanar). `IntersectionWRay`, `IntersectionWLineSegment`, `IntersectionWPlane`, `IntersectionWPlane_Symmetric`, `IntersectionWTriangle` added (including the documented disjoint-segments throw).
- `test_point2d.cpp` / `test_point3d.cpp`: `FromVector` — explicit construction, implicit conversion, and round-trip via `ToVector`.
- `test_calc_utils3d.cpp` (new file): `DistanceLineToLine_{Intersecting,Skew,ParallelDistinct,Overlap,ZeroLengthInputs}` and `IntersectionLineToLine_{Intersecting,SkewLinesNoIntersection,ParallelDistinctNoIntersection,OverlapNoIntersection,IntersectAtEndpointParams}`.
- `test_line3d.cpp`: `DistanceToLine3D`, `DistanceToRay3D`, `DistanceToLineSegment3D` — each covers crossing → 0, parallel-distinct → perp dist, collinear overlap → 0, and skew where applicable.
- `test_ray3d.cpp`: `DistanceToLine3D`, `DistanceToRay3D`, `DistanceToLineSegment3D` — same shape (crossing, parallel, skew, collinear overlap, plus back-to-back rays).
- `test_line_segment3d.cpp`: `Flip`, `DistanceToLine3D`, `DistanceToRay3D`, `DistanceToLineSegment3D` — including the 2D-top-cross / 3D-separated skew case (`LineSegment3D ↔ Ray3D` where the ray crosses the segment in XY but sits at z=5).

**Python (`geompp_python/tests`)**
- `TestPoint2D.test_construction_from_vector` and `TestPoint3D.test_construction_from_vector`.
- `TestPlane`: `test_intersection_with_ray`, `test_intersection_with_line_segment`, `test_intersection_with_plane`, `test_is_parallel`, `test_is_coplanar`.
- `TestTriangle3D`: `test_intersection_with_line`, `test_intersection_with_ray`, `test_intersection_with_line_segment`, `test_intersection_with_plane`, `test_intersection_with_triangle`.
- `TestLine3D` / `TestRay3D` / `TestLineSegment3D`: `test_distance_to_line3d`, `test_distance_to_ray3d`, `test_distance_to_segment3d` — covering crossing → 0, parallel → perp dist, and collinear-overlap → 0 (and `Distance(...)` returning `None`). `TestLineSegment3D.test_flip` added.

**C# (`geompp_csharp/tests`)**
- `Point2D` / `Point3D` — `CreateFromVector_CopiesComponents`, `CreateFromVector_RoundtripViaToVector`.
- `Plane` — `Intersects_*`/`Intersection_*` for `Ray3D`, `LineSegment3D`, `Plane`, `Triangle3D`; `IsParallel_*` and `IsCoplanar_*` for line/ray/segment.
- `Triangle3D` — `Intersects_Line3D_*`/`Intersection_Line3D_*` and the same for `Ray3D`, `LineSegment3D`, `Plane`, `Triangle3D` (12+ new tests, including parallel-above, coplanar, and the documented disjoint-segments throw).
- `Line3D` / `Ray3D` / `LineSegment3D` — `*_DistanceTo_*_{Crossing_IsZero,ParallelDistinct,Skew,Overlap_IsZero}` for every pair of 3D linear primitives (~29 new tests). `LineSegment3D_DistanceTo_Ray3D_SkewTopCross` covers the 2D-top-cross / 3D-separated case.

---

## [0.7.0] - 2026-05-06

> C++ library — tagged `v0.7.0` · C# / NuGet — tagged `csharp-v0.7.0` · Python / PyPI — tagged `python-v0.7.0`

> Touches `Triangle2D`, `Triangle3D`, `Polygon2D`, `Polygon3D`.

### Added

**C++ core**
- `Triangle2D::Location(Point2D const&)` → `std::optional<std::tuple<double, double>>` — returns barycentric coordinates `(s, t)` where `P = P0 + s·(P1−P0) + t·(P2−P0)`, if the point is inside or on the boundary; `nullopt` if outside. Inverse of `Interpolate`. Implemented via 2D perpendicular dot products (`u.Perp()` / `v.Perp()`).
- `Triangle3D::Location(Point3D const&)` → `std::optional<std::tuple<double, double>>` — same semantics; returns `nullopt` also when the point is off the triangle's plane. Implemented via 3D cross-product isolating each barycentric coordinate without any 2D projection.
- `Polygon2D::IsOnBoundary(Point2D const&) const` — returns `true` if the point lies exactly on an edge (outer ring or any hole boundary); uses `LineSegment2D::Contains` per edge, which tolerates floating-point rounding up to `DECIMAL_PRECISION` digits.
- `Polygon3D::IsOnBoundary(Point3D const&) const` — same semantics; rejects off-plane points immediately, then projects to 2D and delegates to `Polygon2D::IsOnBoundary`.

**Python / PyPI**
- `Triangle2D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Triangle3D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Polygon2D.is_on_boundary(point)` → `bool` — Python binding for the new method.
- `Polygon3D.is_on_boundary(point)` → `bool` — Python binding for the new method.

**C# / NuGet**
- `Triangle2D.Location(Point2D^ point)` → `Tuple<double, double>^` (or `null` if outside) — C# binding for the new `Location` method.
- `Triangle3D.Location(Point3D^ point)` → `Tuple<double, double>^` (or `null` if off-plane or outside) — C# binding for the new `Location` method.
- `Polygon2D.IsOnBoundary(Point2D^ point)` → `bool` — C# binding for the new method.
- `Polygon3D.IsOnBoundary(Point3D^ point)` → `bool` — C# binding for the new method.

### Changed

**C++ core**
- `Triangle2D::Contains(Point2D const&)` — rewritten to delegate entirely to `Location(point).has_value()`. Behavior is unchanged; implementation is now consistent and symmetric with `Interpolate`.
- `Triangle3D::Contains(Point3D const&)` — was an unimplemented stub (`throw std::runtime_error("not implemented")`); now fully implemented. Rejects off-plane points via `BBox3D` and plane check, then delegates to `Location(point).has_value()`. No 2D projection is performed.
- `Polygon2D::Contains(Point2D const&)` — was an unimplemented stub; now implemented using a winding-number algorithm with boundary-inclusive semantics: calls `IsOnBoundary` first, then falls back to winding number for strictly interior points.
- `Polygon3D::Contains(Point3D const&)` — was an unimplemented stub; now implemented. Returns `false` immediately for off-plane points; projects to 2D and applies the winding-number algorithm for in-plane points.

### Fixed

**C++ core**
- `Polygon2D::Contains(Point2D const&)` — fixed inverted boundary logic: `IsOnBoundary` is called first and short-circuits to `true`; previously the order was reversed, causing interior points to return `false`.
- `Polygon2D::FromWkt` / `Polygon3D::FromWkt` — were throwing for valid WKT strings in C++ tests (leftover `EXPECT_ANY_THROW` from when the function was a stub); tests updated to expect successful parse and verify vertex count and first point.
- `Triangle3DTest::Contains_OnBoundary` — off-plane assertion changed from `z=0.001` to `z=0.01`; with `DECIMAL_PRECISION=3` the epsilon is exactly `0.001`, so the old value was within tolerance and the point was classified as on-plane.

### Tests

**C++ (`geompp_tests`)**
- `test_triangle2d.cpp`: `Location` test rewritten — `check_inside` / `check_outside` lambdas that assert `Location` value AND `Contains` status together; round-trip A (`Interpolate(Location(p)) == p`) and round-trip B (`Location(Interpolate(s,t)) == (s,t)`).
- `test_triangle3d.cpp`: `Location` test added — same `check_inside` / `check_outside` / round-trip structure; off-plane point asserts both `Location == nullopt` and `Contains == false`.
- `test_triangle3d.cpp`: `Contains_OnBoundary` — off-plane assertion fixed to `z=0.01`.
- `test_polygon2d.cpp`: `Contains` test completed — interior, near-corner, exterior, and polygon-with-hole cases. `Contains_OnBoundary` — vertices, edge midpoints, and hole boundary. `IsOnBoundary_True` / `IsOnBoundary_False` — explicit standalone tests. `Wkt` and `FromFile` updated to verify successful round-trip.
- `test_polygon3d.cpp`: same coverage as 2D plus off-plane and YZ-plane cases.

**Python (`geompp_python/tests`)**
- `TestTriangle2D.test_location`: rewritten with `check_inside`/`check_outside` helpers and both round-trips.
- `TestTriangle3D.test_location`: added — same structure, including off-plane case.
- `TestPolygon2D.test_contains`: completed — interior, near-corner, exterior, polygon-with-hole, and boundary cases.
- `TestPolygon3D.test_contains`: completed — same plus off-plane assertion.
- `TestPolygon2D.test_is_on_boundary`: vertices, edge midpoints, interior/exterior false cases, hole boundary true and false cases.
- `TestPolygon3D.test_is_on_boundary`: same in 3D plus off-plane false case.

**C# (`geompp_csharp/tests`)**
- `Triangle2D` — `Location_Vertices_ReturnExpectedCoords_2D`, `Location_Centroid_OneThirdEach_2D`, `Location_NullImpliesNotContained_2D`, `Location_RoundTrip_A_And_B_2D`: each asserts `Location` value AND paired `Contains` call; round-trips A and B included.
- `Triangle3D` — `Location_Vertices_ReturnExpectedCoords`, `Location_Centroid_OneThirdEach`, `Location_NullImpliesNotContained`, `Location_RoundTrip_A_And_B`: same relationship-focused structure.
- `Polygon2D` — `Contains_Interior_True`, `Contains_Exterior_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnBoundary_OnEdge_True`, `IsOnBoundary_Interior_False` added.
- `Polygon3D` — `Contains_Interior_True`, `Contains_OffPlane_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnBoundary_OnEdge_True`, `IsOnBoundary_Interior_False` added.
- `Triangle3D` — `Contains_Interior_True`, `Contains_OffPlane_False` added (delegating to `Location`).

---

## [0.6.0] - 2026-05-04

> C++ library — tagged `v0.6.0` · C# / NuGet — tagged `csharp-v0.6.0` · Python / PyPI — tagged `python-v0.6.0`

> Touches `Polyline2D`, `Polyline3D`, `Polygon2D`, `Polygon3D`, `Triangle2D`, `Triangle3D`.

### Changed

**C++ core / Python bindings**
- `Polyline2D::Make` and `Polyline3D::Make` — fixed: now call `remove_collinear(points)` instead of `remove_collinear(remove_duplicates(points))`. `remove_duplicates` removed all duplicate points regardless of position, silently corrupting self-intersecting or backtracking paths; `remove_collinear` already handles consecutive duplicates as a degenerate collinear triplet, so the extra pass was both wrong and redundant.
- `Polygon2D::Make` and `Polygon3D::Make` (no-holes overload) — fixed: now call `remove_collinear(points)`, consistent with the with-holes overload. Previously the no-holes path only called `remove_duplicates`, skipping collinear simplification.
- `remove_duplicates_from_sorted_list()` renamed to `remove_consecutive_duplicates()` (Point2D and Point3D overloads). The old name was misleading — the function removes consecutive equal elements, not all duplicates from a sorted container. Affects `geompp::remove_consecutive_duplicates`, `geompp.remove_consecutive_duplicates` (Python), and all internal callers (`Polygon2D::Make`, `Polygon3D::Make`, `Triangle2D::Intersection`).

**C++ core**
- `Polyline2D::Length()` and `Polyline3D::Length()` — now inline cached getters; value is pre-computed in `Make()` and stored as a private member. No change to public API.
- `Polygon2D::Perimeter()` and `Polygon3D::Perimeter()` — same inline-cached pattern. No change to public API.
- `Polyline2D::operator=` and `Polyline3D::operator=` — now copy the cached `LENGTH` member.
- `Polygon2D::operator=` and `Polygon3D::operator=` — now copy the cached `PERIMETER` member (and `PLANE` for 3D).
- `Polyline2D::AlmostEquals()` and `Polyline3D::AlmostEquals()` — added fast-rejection on `LENGTH` difference before comparing vertices.
- `Polyline2D::Contains()` and `Polyline3D::Contains()` — rewritten with `std::ranges::any_of` over `ToSegments()`.
- `Polyline2D::DistanceTo()` and `Polyline3D::DistanceTo()` — rewritten with `std::ranges::min` + `views::transform` over `ToSegments()`; no intermediate allocation.
- `Polyline2D::ProjectOnto()` and `Polyline3D::ProjectOnto()` — fixed double segment construction; now carries the best projected point directly through the loop.
- `Polyline2D::Location()` and `Polyline3D::Location()` — fixed double `segs[i]` construction per iteration; each segment is now materialized once with `auto seg = segs[i]`.
- `Polyline2D::Interpolate()` and `Polyline3D::Interpolate()` — now throws `std::invalid_argument` when `pct` is outside `[0, 1]`; previously clamped silently.
- `Triangle2D::Contains()` — inlines the removed `Location()` math directly; behaviour unchanged.
- `Triangle2D::DistanceTo()` — reverted to `throw std::runtime_error("not implemented")`; previous implementation was incorrect.

### Added

**Python / PyPI**
- `LineSegment2D.project_onto()`, `LineSegment3D.project_onto()`, `Ray2D.project_onto()`, `Ray3D.project_onto()`, `Polyline2D.project_onto()`, `Polyline3D.project_onto()` — bound from the existing C++ `ProjectOnto(Point)` method; was callable from C++ and C# but missing from Python bindings.

### Fixed

**C++ core**
- `Polyline2D::Location()` and `Polyline3D::Location()` — corrected unit mismatch: `seg.Location(point)` returns a [0, 1] fraction but was treated as a raw length accumulator; result is now `(tot_len + seg.Location(point) * seg.Length()) / LENGTH`.
- `Polyline2D::Interpolate()` and `Polyline3D::Interpolate()` — corrected unit mismatch: `pct` ∈ [0, 1] was compared directly against cumulative segment lengths; fixed by computing `target = pct * LENGTH` before the selection loop.
- `WktParser::FromWkt()` — `LINESTRING` WKT strings were silently parsed as `Line2D`/`Line3D` because the `LINE` prefix check ran before the `LINESTRING` check; fixed by moving the `LINESTRING` branch first.

**C# / NuGet**
- `Line2D` binding — removed spurious `Location(Point2D^)` declaration and implementation; the backing `geompp::Line2D` has no such method, causing a build failure.

### Removed (breaking)

**C++ core**
- `Polygon2D::Location()` and `Polygon3D::Location()` — removed; function had no meaningful geometric definition for a polygon.
- `Polygon2D::Interpolate()` and `Polygon3D::Interpolate()` — removed; function had no meaningful geometric definition for a polygon.
- `Triangle2D::Location()` — removed; returned parametric `(s,t)` coordinates as a `Point2D`, which is a type misuse and leaked an internal implementation detail.
- `Triangle3D::Location()` — removed (was unimplemented stub).

**C# / NuGet (breaking)**
- `Polygon2D.Location()`, `Polygon3D.Location()`, `Polygon2D.Interpolate()`, `Polygon3D.Interpolate()` — removed.
- `Triangle2D.Location()` and `Triangle3D.Location()` — removed.

**Python / PyPI (breaking)**
- `Polygon2D.location()`, `Polygon3D.location()`, `Polygon2D.interpolate()`, `Polygon3D.interpolate()` — removed.
- `Triangle2D.location()` and `Triangle3D.location()` — removed.

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: added `ToSegments`.
- `test_polygon3d.cpp`: added `ToSegments`.
- `test_polyline2d.cpp`: updated `Interpolate` — replaced clamp-assertions with `EXPECT_ANY_THROW` for out-of-range `pct`.
- `test_polyline3d.cpp`: same as above.
- `test_triangle2d.cpp`: replaced `Location`-based roundtrip assertion in `Interpolate` test with `Contains(Centroid())`; added outside-returns-nullopt cases; replaced `DistanceTo` test body with `EXPECT_ANY_THROW`.
- `test_triangle3d.cpp`: removed `Location` test.

**Python (`geompp_python/tests`)**
- `TestPolygon2D`, `TestPolygon3D`, `TestPolyline2D`, `TestPolyline3D`: added `test_to_segments`.
- `TestPolyline2D`, `TestPolyline3D`: updated `test_interpolate` — added `pytest.raises` for out-of-range `pct`.
- `TestTriangle2D`: removed `test_location`; expanded `test_interpolate` (vertex checks, outside returns `None`).
- `TestTriangle3D`: added `test_interpolate` (vertices, centroid, outside returns `None`).

**C# (`geompp_csharp/tests`)**
- `Vector2D`: added ~12 tests covering `AlmostEquals`, arithmetic operators, `Length`, `Dot`, `Cross`, `Normalize`, `Perp`, `IsParallel`, `IsPerpendicular`.
- `WktParser`: added 10 tests (`Open_ValidFile_NoThrow`, `Next_Returns*` for Point2D/3D, LineSegment2D/3D, Polyline2D/3D, `Next_SkipsComments`, `FromWkt_Point2D`, `ToWkt_Point2D`); all file-using tests wrapped in `using (var parser = ...)` blocks to ensure `Dispose()` closes the `std::ifstream` before `File.Delete`.
- `GeometryCollection2D` and `GeometryCollection3D`: added ~15 tests each covering `Add`, `Get`, `Size`, `AlmostEquals`, `ToWkt`, `FromWkt`, `ToFile`/`FromFile`.
- `Polyline2D` / `Polyline3D`: corrected `DistanceTo_PointAboveSegment` — expected distance changed from `3.0` to `2.0`: for polyline `(0,0)→(4,0)→(4,4)` and point `(2,3)`, the nearest point is on the vertical segment at `(4,3)`, distance 2.

**Python (`geompp_python/tests`)**
- `TestPolyline2D` / `TestPolyline3D`: corrected `test_project_onto` — expected `project_onto(Point(2,3))` updated to `Point(3,3)` (nearest, distance 1) from `Point(2,0)` (distance 3).

---

## [0.5.0] - 2026-05-01

> C++ library — tagged `v0.5.0` · C# / NuGet — tagged `csharp-v0.5.0` · Python / PyPI — tagged `python-v0.5.0`

### Added

**C++ core**
- `Axis` enum (`X`, `Y`, `Z`) in `vector3d.hpp` — axis identifier used by the dominant-axis algorithm.
- `Vector3D::DominantAxis()` — returns the `Axis` whose absolute component is largest; used internally by the 3D shoelace formula and available as a public API.
- `Point2D::operator+=(Vector2D const&)` and `Point3D::operator+=(Vector3D const&)` — in-place point translation.
- `Point2D operator/(Point2D, double)` and `Point3D operator/(Point3D, double)` — scalar division (throws on division by zero).
- `centroid(std::vector<Point2D> const&)` free function (declared in `point2d.hpp`) — shoelace-formula centroid of a 2D polygon ring.
- `centroid(std::vector<Point3D> const&, std::optional<Plane>)` free function (declared in `plane.hpp`) — 3D centroid; auto-detects the plane if not supplied.
- `Polygon2D::Perimeter()` — sum of outer-ring edge lengths (holes excluded).
- `Polygon3D::Perimeter()` — sum of outer-ring edge lengths.
- `Polygon3D::Centroid()` — area-weighted centroid; hole areas contribute with negative weight.
- `Polygon3D::GetPlane()` — returns the `Plane` stored on construction.

**C# / NuGet**
- `Polygon2D.Perimeter()` and `Polygon3D.Perimeter()` exposed.
- `Polygon3D.Centroid()` and `Polygon3D.GetPlane()` exposed.

**Python / PyPI**
- `Axis` enum exposed (`geompp.Axis.X / .Y / .Z`).
- `Vector3D.dominant_axis()` exposed.
- `Point2D.__iadd__(Vector2D)` and `Point3D.__iadd__(Vector3D)` exposed (`p += v` syntax).
- `Polygon2D.perimeter()` and `Polygon3D.perimeter()` exposed.
- `Polygon3D.centroid()` and `Polygon3D.get_plane()` exposed.
- `centroid(points)` (2D list) and `centroid(points, plane)` (3D list) free functions exposed.
- `signed_area(points, plane)` (3D) free function exposed.

### Fixed

**C++ core**
- `Polygon2D::Centroid()` with holes: the weighted sum was computing `hole_area²` instead of `hole_area`, producing wrong centroids whenever holes were present. Also fixed an MSVC `C2672: std::construct_at` build error caused by pre-sizing a `std::vector<std::pair<Point2D, double>>` over a type with no default constructor — rewritten to use `reserve()` + range iteration.
- `signed_area(std::vector<Point3D>, Plane)`: now throws `std::runtime_error` for fewer than 3 unique points (previously returned 0 silently).

### Removed

**C++ core (breaking)**
- `Polygon2D::SignedArea()` — removed. Use the free function `signed_area(points)` (2D) instead.
- `Polygon3D::SignedArea()` — removed. Use the free function `signed_area(points, plane)` (3D) instead.

**C# / NuGet (breaking)**
- `Polygon2D.SignedArea()` and `Polygon3D.SignedArea()` removed to match the C++ API change above.

### Tests

**C++ (`geompp_tests`)**
- `test_point2d.cpp`: added `AddVectorInPlace` (3 cases).
- `test_point3d.cpp`: added `AddVectorInPlace` (3 cases).
- `test_plane.cpp`: added 9 `Centroid_*` tests for the 3D `centroid()` free function.
- `test_polygon2d.cpp`: added `Centroid_SquareWithCenteredHole`, `Centroid_SquareWithOffCenterHole`; added `Perimeter_Square`, `Perimeter_Rectangle`, `Perimeter_Triangle`.
- `test_polygon3d.cpp`: added `Centroid_Square`, `Centroid_ElevatedSquare`, `Centroid_Triangle`, `Centroid_NonXYPlane`, `Centroid_SquareWithCenteredHole`, `Centroid_SquareWithOffCenterHole`; added `Perimeter_Square`, `Perimeter_NonXYPlane`, `Perimeter_Triangle`.

**C# (`geompp_csharp/tests`)**
- `Polygon2D`: added `Perimeter_Square`, `Perimeter_Rectangle`, `Centroid_Square`, `Centroid_Rectangle`.
- `Polygon3D`: added `Perimeter_Square`, `Perimeter_NonXYPlane`, `Centroid_Square`, `Centroid_ElevatedSquare`, `GetPlane_XYPlane_NormalPointsInZ`, `GetPlane_ContainsAllVertices`.

**Python (`geompp_python/tests`)**
- `TestVector3D`: added `test_dominant_axis`.
- `TestPoint2D`: added `test_iadd_vector`, `test_iadd_zero_vector_unchanged`.
- `TestPoint3D`: added `test_iadd_vector`, `test_iadd_zero_vector_unchanged`.
- `TestPolygon2D`: added `test_perimeter_square`, `test_perimeter_rectangle`, `test_perimeter_triangle`.
- `TestPolygon3D`: added `test_perimeter_square`, `test_perimeter_non_xy_plane`, `test_perimeter_triangle`, `test_get_plane_returns_plane`, `test_get_plane_contains_all_vertices`, `test_get_plane_with_holes`.
- `TestCentroid3D` (new class): 6 tests for `centroid(points)` and `centroid(points, plane)`.

---

## [0.4.0] - 2026-04-25

> C++ library — tagged `v0.4.0` · C# / NuGet — tagged `csharp-v0.4.0` · Python / PyPI — tagged `python-v0.4.0`

### Changed

**All packages**
- `LVSParser` renamed to `WktParser` everywhere: C++ class, header (`wkt_parser.hpp`), source (`wkt_parser.cpp`), C# wrapper (`WktParser.hpp/.cpp`), Python binding (`bind_wktparser.cpp`, exposed as `geompp.WktParser`), all CMakeLists, vcxproj files, tests, and documentation. The old name is gone entirely — update any call sites.

### Added

**C++ core**
- `GeometryCollection2D` / `GeometryCollection3D`: heterogeneous container holding any mix of 2D (or 3D) primitives. Supports `Add()`, `Get(index)`, `Size()`, `ToWkt()`, `FromWkt()`, `ToFile()`, `FromFile()`, `AlmostEquals()`, and `operator==`.
- `Polygon2D::Make(points, holes)`: new overload that accepts an outer ring and a list of hole rings. Validates that the outer ring is CCW and each hole is CW; throws on violation.
- `Polygon3D::Make(points, holes)`: same, additionally validates that outer ring and all hole rings are coplanar.
- `are_ccw(points)` / `are_cw(points)` (2D, declared in `point2d.hpp`): free functions returning `true` when the ordered point list has the specified winding.
- `are_coplanar(points)` / `are_ccw(points)` / `are_cw(points)` (3D, declared in `plane.hpp`): 3D winding and coplanarity checks on point lists.
- `WktParser::FromWkt(wkt)`: static method — parse any WKT string into a `ReturnSet` (already existed as `Get()`, renamed to `FromWkt()` for consistency with the serialization API).
- `WktParser::ToWkt(shape)`: static method — serialize a `ReturnSet` back to its WKT string; throws on `nullopt`.

**C# / NuGet**
- `Polygon2D.Make(array<Point2D^>^ points, array<array<Point2D^>^>^ holes)` exposed.
- `Polygon3D.Make(array<Point3D^>^ points, array<array<Point3D^>^>^ holes)` exposed.

**Python / PyPI**
- `GeometryCollection2D` and `GeometryCollection3D` exposed with full `add()`, `get()`, `size()`, WKT, file I/O, and equality support.
- `Polygon2D.make(points)` and `Polygon2D.make(points, holes)` exposed.
- `Polygon3D.make(points)` and `Polygon3D.make(points, holes)` exposed.
- `are_ccw(points)` / `are_cw(points)` exposed for `list[Point2D]`.
- `are_coplanar(points)` / `are_ccw(points)` / `are_cw(points)` exposed for `list[Point3D]`.
- `WktParser.from_wkt(wkt)` — parse a WKT string; returns a geometry object or `None`.
- `WktParser.to_wkt(shape)` — serialize any geometry object to its WKT string; raises on `None` or unsupported type.

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: added `WktWithHoles`, `WithHoles_Valid`, `WithHoles_PerimeterCW_Throws`, `WithHoles_HoleCCW_Throws`, `WithHoles_HoleTooFewPoints_Throws`.
- `test_polygon3d.cpp`: added same suite plus `WithHoles_NonCoplanar_Throws`.
- `test_utils.cpp`: added `AreCCW_2D`, `AreCoplanar_3D`, `AreCCW_3D`.
- `test_wkt_parser.cpp` (renamed from `test_lsv_parser.cpp`): existing `FromFile` test retained.

**Python (`geompp_python/tests`)**
- `TestWktParser` (renamed from `TestLVSParser`): added `test_get_returns_geometry`, `test_get_returns_none_on_unknown`, `test_to_wkt_point2d`, `test_to_wkt_linesegment2d`, `test_to_wkt_point3d`, `test_to_wkt_none_raises`, `test_to_wkt_roundtrip`, `test_to_wkt_unsupported_type_raises`.
- `TestPolygon2D`: added holes construction, CCW-outer throw, CW-hole throw, and too-few-points throw cases.
- `TestPolygon3D`: added same suite plus non-coplanar throw.
- `TestFreeFunctions`: added `are_ccw`/`are_cw` (2D), `are_coplanar`/`are_ccw`/`are_cw` (3D).

---

## [0.3.0] - 2026-04-24

> C++ library — tagged `v0.3.0`

### Changed

**C++ core**
- `Line2D::AlmostEquals` / `operator==`: now geometric equality — two lines are equal if they lie on the same infinite line (parallel directions, collinear origins). The old P0/P1 coordinate comparison is replaced. Anti-parallel lines on the same infinite line are equal; parallel but offset lines are not. **Breaking for callers that relied on the old point-coordinate comparison.**
- `Line3D::AlmostEquals` / `operator==`: same geometric-equality change.
- `LineSegment2D::AlmostEquals` / `operator==`: now order-agnostic — `Make(A, B) == Make(B, A)`. **Breaking for callers that relied on direction-sensitive equality.**
- `LineSegment3D::AlmostEquals` / `operator==`: same order-agnostic change.

### Fixed

**C++ core**
- `Ray2D::Contains`: replaced ad-hoc cross-product collinearity check with `ToLine().Contains(point) && IsAhead(point)` for consistency with `Line2D::Contains`.
- `Triangle2D::Intersects(Triangle2D const&)`: corrected return type from `ReturnSet` to `bool`, matching the pattern of all other `Intersects` overloads.
- `Triangle3D::Intersects(Triangle3D const&)`: same return-type correction.

### Added

**C++ core**
- `Triangle2D::Intersection(Ray2D const&)`: implemented. Returns a `Point2D` if the ray clips one edge, a `LineSegment2D` if it crosses two edges, or `nullopt` if it misses or is entirely behind the triangle.
- `Triangle2D::Intersection(LineSegment2D const&)`: implemented. Returns a `Point2D` if the segment crosses one boundary edge, a `LineSegment2D` if it crosses two, or `nullopt` if entirely outside or entirely inside.
- `Triangle2D::Intersects(Ray2D const&)` and `Triangle2D::Intersects(LineSegment2D const&)`: now delegate to the corresponding `Intersection` overloads (were stubs that threw).

### Tests
- `test_line2d.cpp`: added `Location`, `ProjectOnto`, `AlmostEquals` (geometric-equality and epsilon cases).
- `test_line3d.cpp`: extended `AlmostEquals` with same-infinite-line and reversed-direction cases; added `DistanceTo`.
- `test_line_segment2d.cpp`: added `AlmostEquals` including reversed-segment equality.
- `test_line_segment3d.cpp`: extended `AlmostEquals` with reversed-segment case.
- `test_ray3d.cpp`: added `DistanceTo`, `IntersectionWithRay3D`.
- `test_polyline3d.cpp`: added `Location`.
- `test_triangle2d.cpp`: added `IntersectionWRay` and `IntersectionWSegment`.

---

## [0.2.0] - 2026-04-24

> C++ library — tagged `v0.2.0` · C# / NuGet — tagged `csharp-v0.2.0` · Python / PyPI — tagged `python-v0.2.0`

### Changed

**C++ core**
- `Triangle3D::SignedArea(Vector3D const& ref_normal)`: signature changed — now requires an explicit reference normal. Result is `ref_normal.Dot(AreaVector())`; sign depends on which side of the plane `ref_normal` points to. Existing no-argument callers must be updated.
- `Triangle3D::IsCCW(Vector3D const& ref_normal)`: same — now requires explicit reference normal.

**C# / NuGet**
- `Triangle3D.SignedArea(Vector3D^ refNormal)` and `Triangle3D.IsCCW(Vector3D^ refNormal)`: updated to match new C++ signatures.

**Python / PyPI**
- `Triangle3D.signed_area(ref_normal)` and `Triangle3D.is_ccw(ref_normal)`: updated to match new C++ signatures.

### Added

**C++ core**
- `Triangle2D::IsCCW()`: returns `true` if winding is counter-clockwise (`SignedArea() > 0`).
- `Triangle3D::Normal()`: returns the unit normal vector (`AreaVector().Normalize()`).
- `Triangle3D::ToPolygon()`: converts the triangle to a `Polygon3D` with the same three vertices.

**C# / NuGet**
- `Triangle2D.IsCCW()`, `Triangle3D.Normal()`, `Triangle3D.ToPolygon()` newly exposed.

**Python / PyPI**
- `Triangle2D.is_ccw()`, `Triangle3D.normal()`, `Triangle3D.to_polygon()` newly exposed.

### Fixed

**C++ core**
- `triangle3d.hpp`: typo `book IsCCW()` corrected to `bool IsCCW()` — function was syntactically invalid before this fix.
- `test_plane.cpp`: `Plane::AlmostEquals` was already implemented but the test was a stale placeholder expecting a throw; replaced with real assertions.

### Tests
- `test_plane.cpp`: added `AlmostEquals` cases — equal planes, distinct planes, anti-parallel normals at same offset, different offsets.
- `test_triangle3d.cpp`: added `ToPolygon`, `Normal`, `IsCCW`; updated `SignedArea` to pass `ref_normal`.
- `test_triangle2d.cpp`: added `IsCCW`.

---

## [0.1.3] - 2026-04-13

> C++ library — tagged `v0.1.3`

### Fixed

- `Polygon2D::AlmostEquals` and `Polygon2D::ToWkt`: bit-shift (`<<`) typo in loop
  conditions corrected to less-than (`<`); loops now iterate over all vertices.
- `Polygon2D::ToWkt`: ring now closes correctly — first vertex repeated at the end,
  producing valid WKT (`POLYGON ((x0 y0, …, x0 y0))`).
- `Polygon3D`: same loop and ring-closure fixes; `ToWkt` now also includes the z
  coordinate (`"{} {} {}"` format instead of `"{} {}"`).
- `BBox3D` copy constructor: was initialising `MIN` from `b.MAX` instead of `b.MIN`;
  all `BBox3D` copies were silently corrupt.
- `BBox3D::Contains`: z-axis bounds check was missing; added.
- `BBox2D::operator=` and `BBox3D::operator=`: declared but never defined; linker
  error on any code using assignment; implementations added.
- `Line3D::Contains` and `Polyline3D::Contains`: used `Vector3D::Perp()` for the
  collinearity check, which is incorrect in 3D — a single perpendicular dot product
  being zero does not guarantee the point lies on the geometry. Both functions now
  use the cross-product magnitude check. `Contains` and `Location` are public API
  exposed in the C# and Python bindings, so callers may have received wrong results.

### Tests
- Added full test suites for `Plane`, `BBox2D`, `BBox3D`, `Polygon2D`, `Polygon3D`.
- Extended coverage for `Triangle3D`, `Polyline2D`, `LineSegment3D`.

---

## [0.1.2] - 2026-04-13

> C# / NuGet — tagged `csharp-v0.1.2` · Python / PyPI — tagged `python-v0.1.2`

### Fixed

**C# / NuGet**
- Rebuilt against geompp core 0.1.3. Inherits all C++ bugfixes: `Polygon2D`/`Polygon3D`
  loop and ring-closure corrections, `BBox3D` copy-constructor MIN/MAX fix,
  `BBox3D::Contains` z-check, `BBox2D`/`BBox3D` assignment operator.
- `Line3D.Contains`, `Line3D.Location`, `Polyline3D.Contains`, `Polyline3D.Location`:
  fixed incorrect 3D collinearity check (see C++ entry above).

**Python / PyPI**
- Rebuilt against geompp core 0.1.3. Same bugfixes as C# above.
- `line3d.contains()`, `line3d.location()`, `polyline3d.contains()`,
  `polyline3d.location()`: fixed incorrect 3D collinearity check (see C++ entry above).
- `pypi-publish.yml` now stamps `pyproject.toml` version from the git tag at build
  time — no manual file edits needed before releasing.

---

## [0.1.1] - 2026-04-13

### Fixed

**C++ core**
- `Line3D::Intersection(sc, tc)`: parameter `tc` was passed by value instead of
  by reference, leaving the caller's variable uninitialized in Debug builds
  (MSVC Run-Time Check Failure #3).
- `Line2D::Intersection(Line2D const&, double& sc, double& tc)` and
  `Line3D::Intersection(Line3D const&, double& sc, double& tc)` now wrap their
  computation in a `try/catch(...)` block; unexpected exceptions log a warning
  and return `std::nullopt` with `sc = tc = NaN` instead of propagating.

**C# (GeomPP NuGet)**
- Updated to geompp core 0.1.1. Exposes the corrected `Line3D.IntersectionWithParams`
  which now correctly returns the `tc` out-parameter.
- Fixed `0xC0000005` crash when loading `GeomPP.dll` under .NET 5+: geompp core
  is now recompiled inside the C++/CLI project with `GEOMPP_NO_THREAD_LOCAL`,
  replacing `thread_local` globals with plain globals (the .NET 5+ CLR does not
  call `DllMain` for C++/CLI assemblies, preventing TLS slot allocation).
- Added C# smoke tests (`geompp_csharp/tests/`) covering `Precision`, `Point2D`,
  `Point3D`, `Vector3D`, `Line2D`, `Line3D`, and `LineSegment2D`; run via
  `dotnet run --project geompp_csharp\tests\GeomPPTests.csproj`.

**Python (geompp PyPI)**
- Updated to geompp core 0.1.1. `line3d.intersection_with_params()` now correctly
  returns the `tc` value.

### Build / CI
- `enable_testing()` added to root `CMakeLists.txt` so `ctest` finds tests from
  the root build directory on all generators.
- CI `ctest` steps now point at `build/geompp_tests` to avoid running glog's own
  internal test suite alongside geompp's tests.

---

## [0.1.0] - 2026-04-08

### Added

**C++ core**
- Core 2D primitives: `Point2D`, `Vector2D`, `Line2D`, `Ray2D`, `LineSegment2D`,
  `Polyline2D`, `Triangle2D`, `Polygon2D`, `BBox2D`.
- Core 3D primitives: `Point3D`, `Vector3D`, `Line3D`, `Ray3D`, `LineSegment3D`,
  `Polyline3D`, `Triangle3D`, `Polygon3D`, `BBox3D`, `Plane`.
- Intersection queries among `Line`, `Ray`, and `LineSegment` in 2D and 3D.
- `ToWkt()` / `FromWkt()` / `ToFile()` / `FromFile()` serialization for all types.
- Static library release archives for Linux x86_64 and Windows x64.

**C# (GeomPP NuGet)**
- C++/CLI bindings targeting .NET 10 (`net10.0-windows7.0`) and .NET Framework 4.8.
- Full wrapper for all 2D/3D primitives with operator support and WKT serialization.

**Python (geompp PyPI)**
- pybind11 bindings for all 2D/3D primitives.
- Available on PyPI: `pip install geompp`.
- Wheels for Linux x86_64 and Windows AMD64, Python 3.8–3.12.
