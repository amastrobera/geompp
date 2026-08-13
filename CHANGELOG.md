# Changelog

All notable changes to geompp are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Each release covers all three packages at the same version:
- **C++ library** — tagged `v{version}`
- **C# / NuGet** — tagged `csharp-v{version}`
- **Python / PyPI** — tagged `python-v{version}`

---

## [0.17.0] - 2026-08-13

> All existing C++ core code now lives in an `inline namespace geometry` inside `geompp` (`geompp::geometry::Point2D`, transparently still reachable as `geompp::Point2D` — no downstream source changes needed in tests or bindings). New `geompp::maths` module: fixed-size linear algebra (`Vector2/3/4`, `Matrix2/3/4`) with Gauss/Cramer solvers and homogeneous-transform factories, bound in Python (`geompp.maths` submodule) and C# (`GeomPP.Maths` namespace). New `geompp::transformations` module built on top of it: affine transforms (translate/rotate/scale/shear/reflect, and the general `transform(primitive, matrix)`) for every `geompp::geometry` primitive from `Point2D/3D` to `PolyMesh2D/3D`, plus fluent composers `TransformBuilder2D`/`TransformBuilder3D`, bound in Python (`geompp.transformations`) and C# (`GeomPP.Transformations`).

### Added

**C++ core**
- `namespace geompp { inline namespace geometry { ... } }` (every header/source in `geompp/include`, `geompp/src`) — wraps all existing geometry classes/free functions in an *inline* namespace. Purely organizational: an inline namespace's members are still visible unqualified through the parent (`geompp::Point2D` keeps compiling exactly as before), so this needed zero changes anywhere outside `geompp/` itself. Groundwork for `geompp::maths`/`geompp::transformations` to sit alongside `geompp::geometry` as siblings without a naming collision (e.g. `maths::Vector3` vs. `geometry::Vector3D` — deliberately distinct types, see `maths/vector.hpp`'s class docs).
- `geompp::maths` (`geompp/include/maths/`, header-only) — a from-scratch, compile-time-dimensioned linear algebra module, independent of `geompp::geometry`:
  - `Numeric<T>` concept (`maths_concepts.hpp`) — any `std::is_arithmetic_v` type.
  - `Vector<T, N>` (`vector.hpp`) + `Vector2`/`Vector3`/`Vector4` (`double` aliases) — component constructor/accessors (`x()`/`y()`/`z()`/`w()`, gated by `N` via `requires` so e.g. `Vector2::z()` is a compile error, not a runtime one), `+`/`-`/scalar `*`//`, `Dot()`, `Cross()` (`N == 3` only), `Length()`/`LengthSquared()`, `Normalized()` (throws `std::invalid_argument` on a zero-length vector), `Transpose()` (returns a `Matrix<T,1,N>` row vector).
  - `Matrix<T, Rows, Cols>` (`matrix.hpp`) + `Matrix2`/`Matrix3`/`Matrix4` (`double`, square aliases) — row-major `operator()`/`At()`, `+`/`-`/scalar `*`//`, matrix\*matrix and matrix\*vector products (size compatibility enforced at compile time via the operand's own template parameters — a mismatched pairing is a compile error, a stronger guarantee than a runtime throw), `Transpose()`, `Determinant()`/`Inverse()` (square only; `Inverse()` throws `std::invalid_argument` on a singular matrix), `Identity()`/`Zero()`, and homogeneous affine-transform factories: `Translation(Vector3)`/`Rotation(angle_rad, axis)` (Rodrigues' formula; throws on a zero-length axis)/`Scale(factor)`/`Scale(sx,sy,sz)`/`Shear(xy,xz,yx,yz,zx,zy)`/`Reflection(Vector3 normal)` (Householder reflection `I - 2nn^T`; throws on a zero-length normal) (square 4x4 only), and their `Translation(Vector2)`/`Rotation(angle_rad)`/`Scale(factor)`/`Scale(sx,sy)`/`Shear(shx,shy)`/`Reflection(Vector2 normal)` 2D (square 3x3) counterparts — the primitives `geompp::transformations` builds on (`TransformBuilder3D` for the 4x4 ones, `TransformBuilder2D`/`transform(primitive, Matrix3)` for the 3x3 ones).
  - `detail::determinant_generic()`/`detail::gauss_jordan_eliminate()` (`detail/linear_algebra_detail.hpp`) — raw-buffer (not `Matrix`-typed) recursive cofactor-expansion determinant and partial-pivoting Gauss-Jordan elimination, shared by `Matrix::Determinant()`/`Inverse()` and the solvers below without a `matrix.hpp` <-> `solvers.hpp` include cycle.
  - `solve_gauss(Matrix<T,N,N>, Vector<T,N>)`/`solve_cramer(Matrix<T,N,N>, Vector<T,N>)` (`solvers.hpp`) — solve a square linear system `a*x = b` via Gauss-Jordan elimination or Cramer's rule respectively; both throw `std::invalid_argument` if `a` is singular.
  - `maths.hpp` — umbrella header including the whole module.
- `geompp::transformations` (`geompp/include/transformations/`) — affine transforms for `geompp::geometry` primitives, built on `geompp::maths`:
  - `translate()`/`rotate()`/`scale()`/`shear()`/`reflect()` (`transformations2d.hpp`/`transformations3d.hpp`) — fast direct-arithmetic transforms on a single `Point2D`/`Point3D`, no matrix built (2D `rotate()` takes a scalar angle about the origin; 3D `rotate()` takes an axis-angle pair via `Matrix4::Rotation()`, throwing on a zero-length axis; 2D `shear()` takes 2 terms, 3D takes 6; `reflect()` takes a normal vector (`Vector2`/`Vector3`) and throws on a zero-length one).
  - `transform(primitive, Matrix3)` / `transform(primitive, Matrix4)` — the general path, one overload per primitive from `Point2D/3D`/`Vector2D/3D` through `LineSegment`/`Polyline`/`Triangle`/`Polygon` (outer + holes) to `Mesh`/`PolyMesh` (2D and 3D, 16 overloads total). Composite primitives are rebuilt by transforming each constituent point and re-validating through the type's own `Make()`/`FromTriangles()`/`FromPolygons()`. A `Vector2D`/`Vector3D` transform ignores any translation component of the matrix (homogeneous w = 0) — a displacement has no position to translate.
  - `TransformBuilder3D` (`transform_builder3d.hpp`) — fluent composer for a single `Matrix4`: `Translate()`/`Rotate()`/`Scale()`/`Shear()`/`Reflect()`/`Combine()` each pre-multiply the new operation onto the accumulated matrix, so chained calls apply in the order they're written (`builder.Translate(t).Rotate(r)` moves a point by `t` first, then rotates the result by `r`); `Get()`/`Build()` return the composed matrix. `TransformBuilder2D` (`transform_builder2d.hpp`) is the `Matrix3` 2D counterpart — same composition rule, `Rotate(angle_rad)` with no axis, `Shear(shx, shy)` with 2 terms.
  - `transformations.hpp` — umbrella header including the whole module.

**Python bindings**
- `geompp.maths` submodule (`bind_maths.cpp`, registered as a real pybind11 submodule and mirrored into `sys.modules["geompp.maths"]` so both `from geompp import maths` and `import geompp.maths` work) — `Vector2`/`Vector3`/`Vector4`, `Matrix2`/`Matrix3`/`Matrix4` (indexing, arithmetic operators, `@` for `__matmul__` matrix/vector products, `transpose()`/`determinant()`/`inverse()`, `Matrix4.translation()`/`.rotation()`/`.scale()`/`.shear()`/`.reflection()`, `Matrix3.translation()`/`.rotation()`/`.scale()`/`.shear()`/`.reflection()`), and `solve_gauss()`/`solve_cramer()` overloaded per size. `ValueError` on the same native `std::invalid_argument` cases as the C++ core (pybind11's default exception translation).
- `geompp.transformations` submodule (`bind_transformations.cpp`, same real-submodule/`sys.modules` mirroring as `geompp.maths`) — `translate()`/`rotate()`/`scale()`/`shear()`/`reflect()`, `transform()` overloaded per primitive (both dimensions), and `TransformBuilder3D`/`TransformBuilder2D` classes whose chained methods (`.translate().rotate().shear().reflect()...`) return `self` via `reference_internal`.

**C# bindings**
- `GeomPP.Maths` namespace (`MathsVector.hpp`/`.cpp`, `MathsMatrix.hpp`/`.cpp`) — `Vector2`/`Vector3`/`Vector4` and `Matrix2`/`Matrix3`/`Matrix4` sealed ref classes (concrete per-size wrappers, since the native templates can't cross the CLR boundary), an indexer property for element access, the same operator surface as the C++ core via C++/CLI `operator` overloads, `Matrix3/4.Shear()`/`.Reflection()` static factories, and a static `Solvers` class (`SolveGauss`/`SolveCramer`, both overloaded per size).
- `GeomPP.Transformations` namespace (`Transform.hpp`/`.cpp`, `TransformBuilder2D.hpp`/`.cpp`, `TransformBuilder3D.hpp`/`.cpp`) — a static `Transform` class (`Translate`/`Rotate`/`Scale`/`Shear`/`Reflect` for the fast path, `Apply` for the general `transform(obj, matrix)` path — named `Apply` rather than `Transform` to avoid a method sharing its containing class's name) and `TransformBuilder3D`/`TransformBuilder2D` sealed ref classes (`Translate`/`Rotate`/`Scale`/`Shear`/`Reflect`/`Combine`, each returning `this` for chaining; `Get`/`Build`).

### Fixed

- **`Polygon3D` winding validation was rotation-dependent** — a plain rotation (no reflection, determinant +1) of a valid `Polygon3D` could spuriously throw "points in anti clock-wise order", because `Make()`'s winding check fell back to `signed_area()`'s no-plane heuristic (`closest_world_plane_to()`), which snaps to the nearest world axis but always with a *positive* sign regardless of which way the polygon's own local normal pointed along it — so a rotation that carried the dominant axis from positive to negative (e.g. a `+Z`-normal square rotated 90° about `X`, landing `-Y`-dominant) silently flipped the winding verdict. Fixed by having `Polygon3D::FromUniquePoints()` compute one shared reference plane via `newell_normal()` (robust to any single reflex/near-collinear vertex, and exact per-point-position rather than a coarse axis snap) and reusing it for both `are_coplanar()` and `are_ccw()`, rather than each silently re-deriving its own. This also exposed a deeper point: **a lone 3D ring's winding has no externally meaningful CCW/CW without a reference plane** (unlike 2D, which has one fixed, unambiguous convention) — so `Polygon3D::Make()`'s outer-ring winding check is no longer a "reject wrong order" gate (removed, since it was provably tautological once the reference stopped being external) but now auto-canonicalizes to whichever plane makes the *given* order read as positive; the check for holes, which genuinely is a meaningful comparison (hole vs. the already-fixed outer plane, not a property either ring has standing alone), was fixed in the same pass to actually compare against that outer plane instead of a self-referential default that never verified anything relative to it.
- **`transform(Polygon2D/3D, matrix)` couldn't apply a reflection** — a negative-determinant matrix flips a polygon ring's winding, and `transform()` handed the raw flipped ring straight to `Make()`, which (correctly, per its own contract) rejected it — so mirroring a `Polygon2D`/`Polygon3D` via `transform()` was simply impossible before this release, independent of the 3D-specific bug above. Fixed by having `transform()` read the matrix's `Determinant()` sign (a homogeneous matrix's full determinant already equals its linear submatrix's, so no extra work) and reverse the outer ring and every hole ring before construction when it's negative, restoring correct CCW-outer/CW-hole winding. Also faster for the common (non-degenerate) case: new private `FromUniqueCCWPoints()`/`FromUniqueCoplanarCCWPoints()` constructors (friended to `transform()` specifically, not public) skip the winding/coplanarity re-validation and the `IsConvex()` recomputation `transform()` already knows the answer to (both properties are affine-invariant), since the ring was just freshly transformed and, for 3D, correctly re-planed via the same `newell_normal()` fix above. A genuinely singular transform (`Determinant() == 0`, e.g. `Scale(0, 1)`) still falls back to the fully-validated `Make()` path.
- **`Mesh2D/3D`, `PolyMesh2D/3D`, and `ConnectedMesh2D/3D` couldn't be reassigned** — each class declared both a copy and a move constructor but no `operator=` at all; declaring a move constructor implicitly deletes the compiler-generated copy-assignment operator too (not just move-assignment), so with no `operator=` ever declared, none existed. `mesh = rotate(mesh, ...)` — the exact idiom every other value type in this library supports (`Polygon2D`/`Polygon3D` explicitly declare both `operator=` overloads) — was a hard compile error for the entire mesh family. Fixed by adding explicit `operator=(T const&) = default;`/`operator=(T&&) = default;` to all six classes; correct for free since every member is a `shared_ptr` (copy-assignment is a refcount bump, not a deep copy) or a trivial scalar.

## [0.16.0] - 2026-07-26

> `Mesh2D/3D` (triangle-faced) and `PolyMesh2D/3D` (arbitrary-polygon-faced) meshes with spatial-hash vertex welding, bound in Python and C#. Also closes several test-coverage gaps flagged in `test_coverage_report.md`: C++ `Vector2D` core methods, `Polygon2D/3D::ConvexHull()`, Python `Vector3D` operators, and C#'s `ToString()`/`operator==` across all bound classes.

### Added

**C++ core**
- `Mesh2D`/`Mesh3D` (`mesh2d.hpp`/`mesh3d.hpp`) — a mesh of adjacent triangles stored as unique welded vertices plus a per-face `array<size_t, 3>` index. `FromTriangles(vector<Triangle2D/3D>)` welds each triangle's vertices via `GridCell2D/3D`-bucketed spatial hashing (`detail::GridCellMapForMesh2D/3D`) instead of an O(n) `AlmostEquals` scan; `Size()`, `Area()` (sum of each input triangle's own area, independent of welding), `operator[]` (bounds-checked), and a lazy `Faces()` range view (borrows `this` — must not outlive the mesh).
- `PolyMesh2D`/`PolyMesh3D` (`polymesh2d.hpp`/`polymesh3d.hpp`) — same idea for arbitrary-sided polygon faces, stored as a flat variable-length index buffer (`FACE_INDICES`/`FACE_IDX_BEGINS`/`FACE_IDX_OFFSETS`) rather than a fixed-3 array. `FromPolygons(vector<Polygon2D/3D>)` throws `std::invalid_argument` if any input polygon has holes, since a mesh facet can't represent one.
- `GridCell2D`/`GridCell3D` (`grid_cell2d.hpp`/`grid_cell3d.hpp`) — axis-aligned spatial hash bucket (`floor(coord / epsilon)` per axis) backing the vertex-welding above. `FromPoint()` defaults `epsilon` to `DOUBLE_EPSILON` (tracks the same thread-local `DECIMAL_PRECISION` as `AlmostEquals`, but compares via grid-cell bucketing rather than direct distance — two points up to `sqrt(2)`/`sqrt(3)` × epsilon apart can land in the same cell, and two points closer than epsilon can land in different cells if they straddle a cell boundary).
- `detail::GridCellMapForPolyMesh2D`/`GridCellMapForPolyMesh3D` (`grid_cell2d.hpp`/`grid_cell3d.hpp`) — extracted from what was previously ~30 lines inlined directly in `PolyMesh2D/3D::FromPolygons` (hole validation, vertex welding, and variable-length index-buffer construction all mixed into one function), mirroring the `GridCellMapForMesh2D/3D` shape already used by `Mesh2D/3D`. `FromPolygons` is now a 3-line delegate, matching `FromTriangles`.
- `Mesh2D`/`Mesh3D`/`PolyMesh2D`/`PolyMesh3D` now store every buffer (`VERTICES`, `FACE_INDICES`, and for `PolyMesh2D/3D` also `FACE_IDX_BEGINS`/`FACE_IDX_OFFSETS`) as `std::shared_ptr<std::vector<T>>` instead of a plain `std::vector<T>`, and each class collapsed to a single private constructor taking those `shared_ptr`s by `const&` and copying them in (an atomic refcount bump, not a vector copy). Copying a `Mesh2D/3D`/`PolyMesh2D/3D` — or, in the future, handing its vertex buffer to a `Polygonize()`/`Triangulate()` conversion between the two representations — is now O(1) instead of an O(n) deep copy of every point/index; safe without copy-on-write since none of these classes ever exposes a mutable reference to a buffer after construction. The `detail::GridCellMapFor*` builder classes backing all four factories were updated to match: their own buffers are also `shared_ptr<vector<T>>` now, and their accessors were renamed from `MoveUniques()`/`MoveFaceIndices()`/etc. (which moved their result out, callable only once) to `GetUniques()`/`GetFaceIndices()`/etc. (`const`, return a copy of the `shared_ptr`, callable any number of times).
- `Polygon2D`/`Polygon3D`'s private data constructor is now overloaded on value category (`const&` and `&&`) instead of `const&`-only; `Make()` now moves its locally-built `unique_points`/`unique_holes_points` into the `&&` overload, saving one vector copy per call versus the previous copy-in-body. All ctors stay private, so this is purely an internal optimization — no public signature changed.
- `Polygon2D`/`Polygon3D::Make()` gained public `Make(vector<Point>&&)`/`Make(vector<Point>&&, vector<vector<Point>>&&)` overloads (`polygon2d.hpp`/`polygon3d.hpp`) — new public API surface, unlike the private-constructor change above. To make these actually save a copy rather than just add an overload, `remove_collinear`/`remove_consecutive_duplicates` (`point2d.hpp/cpp`, `point3d.hpp/cpp`) gained matching `&&` overloads that compact surviving points in place (reusing the caller's own vector, moving/erasing rather than allocating a fresh one and copying survivors into it) instead of the `const&` overloads' copy-into-new-vector. `Polygon2D/3D::Make()`'s validation/wrap logic was factored into a shared private static `FromUniquePoints()` per class (two overloads: with/without holes) so all four public `Make()` overloads (const&/&&, with/without holes) delegate to one validated implementation instead of duplicating it.
- `ConnectedMesh2D` (`connected_mesh2d.hpp`/`.cpp`) — a triangle-faced mesh like `Mesh2D`, additionally precomputing per-facet edge adjacency (`detail::TriangleCompactNeighborRef`, a 4-byte packed `{triangle_id, local_edge_id}` twin reference per edge, built by `detail::GridCellMapForConnectedMesh2D::Make()` via an edge-key hashmap over the welded triangle indices) alongside the same `GridCell2D`-bucketed vertex welding used by `Mesh2D`. `Size()`, `Area()`, `operator[]`, and a lazy `Faces()` range view match `Mesh2D`'s surface; the adjacency data itself is exposed via `FaceView2D` (see below).
- `ConnectedMesh2D::FaceView2D` — a lightweight, chainable handle onto one facet returned by `operator[]`/`Faces()`, exposing both its geometry and the precomputed adjacency: `ID()`, `Geometry()` (rebuilds a `Triangle2D`, same as `Mesh2D::operator[]`), `Neighbor(edge)` (crosses one of the facet's 3 edges — `TriangleEdge::FIRST`/`SECOND`/`THIRD`, numbered `v0`-`v1`/`v1`-`v2`/`v2`-`v0` — returning `std::optional<FaceView2D>`, `std::nullopt` at a boundary edge with no twin; chains as `face.Neighbor(edge)->Geometry()`), and `NeighborEntryEdge(edge)` (which edge of `Neighbor(edge)` was entered through, `TriangleEdge::INVALID` at a boundary, letting a caller cross straight back via `Neighbor(entry_edge)`).
- `ConnectedMesh3D`/`ConnectedMesh3D::FaceView3D` (`connected_mesh3d.hpp`/`.cpp`) — the 3D counterpart of `ConnectedMesh2D`/`FaceView2D`: same `detail::GridCellMapForConnectedMesh3D`-computed per-facet edge adjacency (backed by the same `detail::TriangleCompactNeighborRef`/`TriangleEdge`, shared between both dimensions), same `Size()`/`Area()`/`operator[]`/`Faces()`/`FaceView3D::ID()`/`Geometry()`/`Neighbor(edge)`/`NeighborEntryEdge(edge)` surface, operating on `Triangle3D`/`Point3D` instead of `Triangle2D`/`Point2D`.
- `TriangulationParams` (`constants.hpp`) — bundles a `Strategy` (`EarClipping` implemented; `MonotonePolygon`/`Delaunay` throw `"not yet implemented"`) with three independent input-quality checks, each `Guaranteed`/`Assert`/`Enforce`: `Simplicity` (self-intersection), `Winding` (CCW-ness), `Collinearity` (collinear/duplicate consecutive points — a duplicate is just the degenerate case of three collinear points, so one check covers both). `Guaranteed` skips the check entirely; `Assert` throws `std::invalid_argument` on violation; `Enforce` fixes it in place (`simplify_rings`/reverse/`remove_collinear` respectively) before triangulating.
- `triangulate(points, settings)` (`calc_utils2d.hpp`/`calc_utils3d.hpp`) — O(n²)-worst-case ear-clipping triangulation of a simple polygon's outer loop (no holes) into `Triangle2D`/`Triangle3D`. 2D is native; the 3D overloads assume flat/coplanar input, either projected via a caller-supplied plane normal or one fitted via PCA (`principal_normal`) when omitted. Runs the three `TriangulationParams` checks in Collinearity → Winding → Simplicity order (cheapest/most-likely-to-shrink-the-ring first) before dispatching to the chosen `Strategy`.
- `Polygon2D::Triangulate(strategy)`/`Polygon3D::Triangulate(strategy)` — triangulates the outer ring only (holes ignored); since `Make()` already guarantees the ring is simple/CCW/collinear-free, always calls `triangulate()` with every `TriangulationParams` check set to `Guaranteed`.
- `PolyMesh2D::Triangulate(strategy)`/`PolyMesh3D::Triangulate(strategy)` — triangulates every facet independently (same `Guaranteed` reasoning as `Polygon2D/3D::Triangulate()`) and combines the results into one `Mesh2D`/`Mesh3D`.
- `detail::view::is_ccw`/`has_collinears` (`calc_utils2d.hpp`/`.cpp`) — the `View2D`-projected building blocks behind the `Winding`/`Collinearity` checks: shoelace-sign and consecutive-triplet-cross-product tests, the same pattern `is_simple`/`is_convex` already use, so one implementation covers both 2D and 3D input.
- `TriangulationParams::Strategy::EarClippingBestFit` (`constants.hpp`) — a second ear-clipping strategy, now the **default** (was `EarClipping`). Backed by `detail::view::ear_clipping_best_fit_triangulation` (`calc_utils2d.hpp`/`.cpp`): every step does a full lap over the *current* ring and clips the best-scoring valid ear (by a cheap scale-invariant shape-quality score, `|cross(prev,cur,next)| / (a² + b² + c²)`) instead of the first one found, avoiding the visually thin slivers plain scan-order clipping can produce even on ordinary input. Never rejects a geometrically valid ear outright — only reorders which one is preferred — so it keeps the same Two Ears Theorem termination guarantee as `EarClipping`, at the cost of being unconditionally ~O(n²) (a full rescan on every single clip, not just worst-case, versus `EarClipping`'s O(n²) worst-case/O(n)-ish typical case). Every call site that previously hardcoded a default of `EarClipping` (`PolyMesh2D/3D::Triangulate()`'s C++ default parameter, the Python/C# binding defaults, the C# zero-arg `Triangulate()` overloads) now defaults to `EarClippingBestFit` instead.
- `AdjacencyConformity` (`constants.hpp`), `AdjacencyViolation<PointT>`, `validate_adjacency()`/`fix_adjacency()` (`calc_utils2d.hpp`/`.cpp`, templated for `Point2D`/`Point3D` but not `View2D`-projected — a shared 2D projection would be wrong for a general 3D mesh whose facets aren't all coplanar) — mesh-conformity checking: "every edge has at most 1 neighbor," i.e. no facet vertex may lie in the interior of another facet's edge except at that edge's own endpoints (no "hanging nodes"/FEM, no "T-junctions"/graphics, a valid PSLG/mesh generation). Two violation kinds: a T-junction (fixable) and a non-manifold edge (a full edge shared by 3+ facets — never fixable, so `fix_adjacency()` throws even under `Enforce`).
- `fix_adjacency(vector<Polygon2D> const&)`/`fix_adjacency(vector<Polygon3D> const&)` — repairs a T-junction by splicing the foreign vertex into the coarse facet's ring, then cutting a diagonal from it to its nearest ring vertex that forms a valid non-crossing diagonal (checked via proper-segment-intersection against every other edge, plus a winding-number interior test on the diagonal's midpoint) — the standard "diagonal-to-nearest-vertex" polygon-splitting technique; a valid diagonal is guaranteed to exist from any vertex of a simple polygon with ≥4 vertices, the same guarantee ear-clipping itself relies on. The coarse facet becomes two facets sharing the new edge; an unaffected facet passes through unchanged. Since the cut always goes to the *nearest* valid vertex rather than straight across, the resulting pieces aren't necessarily tidy (e.g. splitting a rectangle can yield a mix of triangles and quads rather than clean strips) — a deliberate trade-off for a technique that's provably valid on any simple polygon, convex or not. Returns raw point rings, not `Polygon2D/3D` objects, since `Polygon2D/3D::Make()` unconditionally strips collinear points and would undo the splice step.
- `Mesh2D/3D::FromTriangles`, `PolyMesh2D/3D::FromPolygons`, `ConnectedMesh2D/3D::FromTriangles` (no signature change) now call `validate_adjacency` unconditionally and throw `std::invalid_argument` naming the offending edge and facet indices on any violation — non-conforming input is always rejected at construction time, never silently repaired.
- `triangulate(vector<Polygon2D> const&, AdjacencyConformity conformity = Enforce, TriangulationParams const& settings = {})` — new free-function overload batch-triangulating a set of polygon facets, the free-function equivalent of `PolyMesh2D::FromPolygons(polygons).Triangulate()` for callers who don't want to construct a full `PolyMesh2D`. Unlike the mesh constructors above, defaults to repairing what it can (`Enforce`) rather than always rejecting; still throws on a non-manifold edge regardless of mode.
- `fix_adjacency(vector<Triangle2D> const&)`/`fix_adjacency(vector<Triangle3D> const&)` — Triangle overloads of `fix_adjacency()`: unlike a `Polygon2D/3D` facet, a triangle can't just absorb a spliced-in T-junction vertex and stay a triangle, so a repaired facet is re-triangulated (via `triangulate()` with `Collinearity::Guaranteed`, needed here since this path still splices before re-triangulating) into 2+ triangles covering the same area as the original one; an unaffected facet passes through unchanged. Throws `std::invalid_argument` on a non-manifold edge, same as the Polygon overload.

**Python bindings**
- `Mesh2D`/`Mesh3D`: `from_triangles(triangles)`, `size()`, `area()`, `__len__`, `__getitem__`, `__iter__`.
- `PolyMesh2D`/`PolyMesh3D`: `from_polygons(polygons)`, `size()`, `area()`, `__len__`, `__getitem__`, `__iter__`.
- `GridCell2D`/`GridCell3D`: `from_point(point[, epsilon])`, `x`/`y`(/`z`), `__eq__`.
- `ConnectedMesh2D`: `from_triangles(triangles)`, `size()`, `area()`, `__len__`, `__getitem__`, `__iter__` (indexing/iteration return `FaceView2D`, not a bare geometry object).
- `FaceView2D`: `id()`, `geometry()`, `neighbor(edge)` (returns `FaceView2D` or `None`), `neighbor_entry_edge(edge)`. Every `FaceView2D` returned to Python is kept alive together with the `ConnectedMesh2D`/`FaceView2D` it came from via `keep_alive`, since the native type holds a raw pointer back to its owning mesh.
- `TriangleEdge` enum: `INVALID`, `FIRST`, `SECOND`, `THIRD` — registered once (by `bind_connected_mesh2d.cpp`) and shared by both `ConnectedMesh2D`/`FaceView2D` and `ConnectedMesh3D`/`FaceView3D`.
- `ConnectedMesh3D`/`FaceView3D`: same surface as `ConnectedMesh2D`/`FaceView2D` (`from_triangles`/`size`/`area`/`__len__`/`__getitem__`/`__iter__`, `id`/`geometry`/`neighbor`/`neighbor_entry_edge`), operating on `Triangle3D`; same `keep_alive` lifetime handling.
- `TriangulationStrategy`/`TriangulationSimplicity`/`TriangulationWinding`/`TriangulationCollinearity`/`TriangulationParams`, and `triangulate(points, settings)` (2D, and 3D with/without an explicit normal) — registered in a new `bind_triangulation_params.cpp`, called *before* `bind_polygon2d`/`bind_polymesh2d`/etc. in `bindings.cpp`: pybind11 resolves a method's default argument value at bind time, not call time, so `Polygon2D.triangulate`'s `strategy=EarClipping` default needs `TriangulationStrategy` already registered by the time that binding runs.
- `Polygon2D.triangulate(strategy=EarClipping)`/`Polygon3D.triangulate(strategy=EarClipping)`, `PolyMesh2D.triangulate(strategy=EarClipping)`/`PolyMesh3D.triangulate(strategy=EarClipping)`.
- `Mesh2D.connect()`/`Mesh3D.connect()` — returns a `ConnectedMesh2D`/`ConnectedMesh3D` with the same facets/vertices plus precomputed edge adjacency; existed in C++ since the `meshes` commit but was never bound.
- `Triangle2D.intersects`/`.intersection` with `Ray2D`, `LineSegment2D`, and `Triangle2D` (the last via `intersects_triangle`, matching the existing self-type-overload naming convention) — only the `Line2D` overload was previously bound, despite all four existing and being C++-tested since `Triangle2D` was first added; `Triangle3D` already had its full 5-overload set bound.
- `TriangulationStrategy.EarClippingBestFit` — the new default strategy value; `TriangulationParams`' `strategy` default argument, and `Polygon2D/3D.triangulate`'s/`PolyMesh2D/3D.triangulate`'s `strategy` default argument, all changed from `EarClipping` to `EarClippingBestFit` to match.
- `AdjacencyConformity` enum (`bind_triangulation_params.cpp`, registered early for the same default-argument-resolution reason as `TriangulationStrategy`), `AdjacencyViolation2D`/`AdjacencyViolation3D`, `validate_adjacency()`/`fix_adjacency()`, and a batch `triangulate(polygons, conformity=Enforce, settings=TriangulationParams())` overload (`bind_free_functions.cpp`) — mesh-conformity checking, mirroring the C++ API.
- `fix_adjacency()` gains `Triangle2D`/`Triangle3D` overloads (`bind_free_functions.cpp`) — re-triangulates a T-junction-affected triangle facet instead of splicing, mirroring the new C++ overloads.

**C# bindings**
- `Mesh2D`/`Mesh3D`: `FromTriangles(triangles)`, `Size()`, `Area()`, indexer, `ToString()`.
- `PolyMesh2D`/`PolyMesh3D`: `FromPolygons(polygons)`, `Size()`, `Area()`, indexer, `ToString()`.
- `GridCell2D`/`GridCell3D`: `FromPoint(point[, epsilon])`, `X`/`Y`(/`Z`), `operator==`, `ToString()`.
- `ToString()` added for all 26 previously-bound classes; `operator==` added for the 20 of them that lacked it (everything except `Vector2D`, `BBall2D/3D`, `BRect2D`, `BPrism3D`, which already had it). `WktParser` has no `operator==` binding — not applicable, no C++ operator to wrap.
- `ConnectedMesh2D`: `FromTriangles(triangles)`, `Size()`, `Area()`, indexer, `ToString()` (indexer returns `FaceView2D^`, not a bare `Triangle2D^`).
- `FaceView2D`: `Id()`, `Geometry()`, `Neighbor(edge)` (returns `FaceView2D^` or `nullptr`), `NeighborEntryEdge(edge)`, `ToString()`. Holds a managed reference to its parent `ConnectedMesh2D`, keeping the native mesh alive for as long as any `FaceView2D` from it is.
- `TriangleEdge` enum class: `Invalid`, `First`, `Second`, `Third` — declared once in `FaceView2D.hpp` and shared by `FaceView3D.hpp`, since both dimensions' native `TriangleEdge` is the same C++ type.
- `ConnectedMesh3D`/`FaceView3D` (new `ConnectedMesh3D.hpp/.cpp`, `FaceView3D.hpp/.cpp`): same surface as `ConnectedMesh2D`/`FaceView2D`, operating on `Triangle3D`; `FaceView3D` likewise holds a managed reference to its parent `ConnectedMesh3D`.
- `TriangulationStrategy`/`TriangulationSimplicity`/`TriangulationWinding`/`TriangulationCollinearity` enums and a `TriangulationParams` ref class (`GeomUtil.hpp/.cpp`, following the existing `PolylineExpansionParams` pattern), plus `GeomUtil.Triangulate(points, settings)` (2D, and 3D with/without an explicit `Vector3D` normal).
- `Polygon2D.Triangulate()`/`Polygon3D.Triangulate()`, `PolyMesh2D.Triangulate()`/`PolyMesh3D.Triangulate()` — each with a parameterless overload (defaults to `EarClipping`) and one taking an explicit `TriangulationStrategy`.
- `Mesh2D.Connect()`/`Mesh3D.Connect()` — returns a `ConnectedMesh2D^`/`ConnectedMesh3D^`; same C++-existed-but-unbound gap as the Python binding above.
- `Triangle2D.Intersects`/`.Intersection` with `Ray2D`, `LineSegment2D`, and `Triangle2D` — same missing-overload gap as the Python binding above; this area had no C# tests at all before now (7 new tests added).
- `TriangulationStrategy.EarClippingBestFit` — the new default strategy value. Inserted at the matching ordinal position relative to the C++ enum (`EarClipping=0, EarClippingBestFit=1, MonotonePolygon=2, Delaunay=3`), not appended at the end: `TriangulationParams.ToNative()` converts via a raw `static_cast` by ordinal, not by name, so appending would have silently remapped `MonotonePolygon`'s existing ordinal `1` onto `EarClippingBestFit`'s C++ behavior instead of throwing. `TriangulationParams`' default constructor and the four parameterless `Triangulate()` convenience overloads (`Polygon2D/3D`, `PolyMesh2D/3D`) changed from `EarClipping` to `EarClippingBestFit` to match.
- `AdjacencyConformity` enum (`GeomUtil.hpp`, ordinals kept in sync with the native `geompp::AdjacencyConformity` — same raw-`static_cast`-by-ordinal reasoning as `TriangulationStrategy`), `AdjacencyViolation2D`/`AdjacencyViolation3D` classes, `GeomUtil.ValidateAdjacency`/`.FixAdjacency` (4 and 2 overloads respectively), and a batch `GeomUtil.Triangulate(array<Polygon2D^>^, AdjacencyConformity, TriangulationParams^)` overload — mesh-conformity checking, mirroring the C++ API.
- `GeomUtil.FixAdjacency` gains `array<Triangle2D^>^`/`array<Triangle3D^>^` overloads, returning `array<Triangle2D^>^`/`array<Triangle3D^>^` — mirrors the new C++/Python Triangle overloads.

### Fixed

- **Critical off-by-one in mesh vertex welding**: `add_point` (`grid_cell2d.cpp`/`grid_cell3d.cpp`) and the equivalent inline lambda in `polymesh2d.cpp`/`polymesh3d.cpp` recorded a newly-welded vertex's index as the container's size *after* `push_back` (a 1-based count) instead of its 0-based position, shifting every face's vertex indices by one and making the *last* unique vertex in any mesh one-past-the-end — an out-of-bounds read on `operator[]`/`Faces()` for any mesh built from ≥2 faces sharing a vertex. Affected all four factories: `Mesh2D/3D::FromTriangles`, `PolyMesh2D/3D::FromPolygons`. Fixed by using `size() - 1`; regression-tested in all three languages via a two-face shared-edge case asserting each face's original vertices survive welding unshifted.
- `polymesh2d.cpp`/`polymesh3d.cpp` referenced `GridCell2DHash`/`GridCell3DHash` unqualified after the type moved into `namespace detail` — compile error, fixed by qualifying as `detail::GridCell2DHash`/`detail::GridCell3DHash`.
- `mesh2d.hpp`/`mesh3d.hpp`/`polymesh2d.hpp`/`polymesh3d.hpp` each had a missing closing brace on the inline `Faces()` definition, leaving `namespace geompp` unclosed for the rest of the header — silently masked by a stale cached build until a forced rebuild surfaced it as a `<deque>`/`<unordered_map>` template-instantiation cascade in unrelated translation units. `polymesh3d.hpp`'s `Faces()` was additionally defined as `PolyMesh2D::Faces()` (copy-paste from the 2D header) instead of `PolyMesh3D::Faces()`.
- `add_triangle` (`grid_cell2d.cpp`/`grid_cell3d.cpp`) was declared to return `std::tuple<size_t, size_t, size_t>` but never returned a value on any path — undefined behavior in principle, though harmless in practice since both call sites discarded the result. Simplified to `void`.
- `Triangle2D`/`Triangle3D`/`Polygon2D`/`Polygon3D` gained a duplicate copy-ctor/move-ctor/copy-assignment declaration (edited in alongside the mesh work) that conflicted with each class's pre-existing custom `operator=(T const&)`. The actually-needed piece — an explicit `operator=(T&&) = default` move-assignment, required for `std::movable` so `Mesh2D/3D::Faces()`'s range-view pipeline compiles — was kept; the accidental duplicates were removed.
- `PolyMesh2D/3D::FromPolygons` now rejects polygons with holes (`std::invalid_argument`) instead of silently discarding the hole and using only the outer ring as the face, matching the class's own "each facet has no holes" documented invariant.
- C++ `Vector2D`: `ToPoint`, `Length`, `AlmostEquals`, `Normalize`, `BasisY` had no direct test in `test_vector2d.cpp` despite being implemented and correct — added tests, no behavior change.
- C++ `Polygon2D/3D::ConvexHull()` (the member method, not the free `convex_hull()` function) had no dedicated test — added tests, no behavior change.
- Python `Vector3D`: `__add__`/`__sub__`/`__mul__`/`__rmul__`/`__truediv__`/`__neg__`/`__eq__` had no test (unlike `Vector2D`, which has `test_arithmetic`) — added tests, no behavior change.
- C++ `sort_ccw` (`point2d.cpp`) was dead, broken code predating this release: not declared in `point2d.hpp` (so unreachable outside its own translation unit), and its body computed a centroid into an unused local without ever sorting or returning anything (a `void`-return-from-non-void-function bug masked because nothing called it). Declared in `point2d.hpp` and implemented properly: sorts an arbitrary (not necessarily ring-ordered) point set into CCW angular order around `average(points)` — deliberately not `centroid()`, whose shoelace formula assumes `points` is already ring-ordered and throws on zero signed area, neither of which holds for an arbitrary input. C++-only for now (no Python/C# binding), matching several other untested/unbound `Point2D` free functions; +2 tests.
- **Critical out-of-bounds indexing in `GridCellMapForConnectedMesh2D::Make()`** (`grid_cell2d.cpp`, uncommitted WIP `connected_mesh2d` work): the adjacency-wiring loop used `tri_id` — a flat offset into `triangle_indices` counting `0, 3, 6, …` — directly as the compact per-triangle index into `neighbour_refs` (sized `n_triangles`) and encoded it into `TriangleCompactNeighborRef`, instead of dividing by 3 first. For any mesh with more than one triangle this read and wrote past the end of `neighbour_refs` and produced garbage-encoded adjacency for every triangle after the first. Fixed by introducing `compact_tri_id = tri_id / 3` and using it everywhere a per-triangle (rather than a flat vertex-triple) index was needed; regression-tested directly against `GridCellMapForConnectedMesh2D::Make()`'s output (`test_grid_cell2d.cpp`) with a shared-edge pair and a 4-triangle fan, since the adjacency data isn't yet exposed through `ConnectedMesh2D`'s own public API.
- `ConnectedMesh2D`'s private constructor declared its `NEIGHBORS` member as a flat `shared_ptr<vector<TriangleCompactNeighborRef>>`, but `GridCellMapForConnectedMesh2D::GetNeighborRefs()` returns `shared_ptr<vector<array<TriangleCompactNeighborRef, 3>>>` (one 3-edge array per facet) — a compile error in `FromTriangles()`. Corrected the member/constructor type to match.
- `ConnectedMesh2D::Faces()` eagerly built and returned an owning `std::vector<Triangle2D>`, contradicting both its own doc comment ("A lazy view … built just before returning") and the `std::views::transform` pattern every sibling mesh class (`Mesh2D`, `PolyMesh2D`) uses for the same method. Rewritten as a `std::views::iota | std::views::transform` lazy range to match.
- `geompp_python/CMakeLists.txt`'s standalone-build-mode `GEOMPP_SOURCES` list (used by `pip install -e ./geompp_python`, as opposed to the parent-CMake `-DBUILD_PYTHON=ON` mode) was missing `grid_cell2d.cpp`/`grid_cell3d.cpp`/`mesh2d.cpp`/`mesh3d.cpp`/`polymesh2d.cpp`/`polymesh3d.cpp` — a pre-existing gap dating to when those classes were added, not something introduced by this pass, but one that made every `GridCell2D/3D`/`Mesh2D/3D`/`PolyMesh2D/3D` Python binding fail to link (`LNK2001` unresolved externals) on a standalone build. Added the missing six alongside the new `connected_mesh2d.cpp`.
- `geompp_python/geompp/__init__.py` didn't re-export `ConnectedMesh2D` from the compiled `_geompp` extension in its explicit `from ._geompp import (...)` list or `__all__`, so `geompp.ConnectedMesh2D` raised `AttributeError` even though the compiled binding worked. Added.
- **Critical off-by-one between `TriangleEdge` and `NEIGHBORS` indexing**: `TriangleEdge` is 1-indexed (`FIRST=1`/`SECOND=2`/`THIRD=3`, `INVALID=0` reserved as a non-edge sentinel) but `NEIGHBORS` is a 0-indexed `std::array<TriangleCompactNeighborRef, 3>`. Both `FaceView2D::Neighbor()`/`NeighborEntryEdge()` and the adjacency-wiring loop in `GridCellMapForConnectedMesh2D::Make()` used the raw `TriangleEdge` value as the array index with no conversion: `Neighbor(THIRD)` read one element past the end of the array (undefined behavior), and `Neighbor(FIRST)`/`Neighbor(SECOND)` silently returned the neighbor of the *next* edge instead of the one requested. Fixed by subtracting 1 at the `FaceView2D` boundary and adding 1 where the builder derives a `TriangleEdge` from a 0-based loop index; regression-tested via a known-adjacency-pattern neighbor walk across a 4-triangle fan.
- `ConnectedMesh2D::FaceView2D::Neighbor()` was declared to return the bare `FaceView2D` but its body returned `std::nullopt` on the boundary-edge case — a compile error. Changed the declared return type to `std::optional<FaceView2D>`.
- `ConnectedMesh2D::operator[]` returned `std::optional<FaceView2D>` and logged a warning + returned `std::nullopt` on an out-of-range index, contradicting its own `@throws std::out_of_range` doc comment, the pre-existing `OperatorBracket_OutOfRange_Throws` test, and every sibling mesh class's `operator[]` (`Mesh2D`, `PolyMesh2D`, `PolyMesh3D` all throw). Changed to throw `std::out_of_range`, matching the rest of the mesh family; `operator[]` now returns a plain `FaceView2D`.
- `grid_cell3d.hpp`'s still-unused `GridCellMapForConnectedMesh3D::Make()` scaffold (no public `ConnectedMesh3D` class consumes it yet) declared its parameter as `std::vector<Triangle2D> const&` instead of `Triangle3D`, and the header was missing the `utils.hpp` include needed for `detail::TriangleCompactNeighborRef` — both compile errors as soon as any translation unit actually built this header. Fixed the type and added the include.
- `grid_cell3d.cpp`'s `GridCellMapForConnectedMesh3D::Make()` built its unique-vertex output as `std::vector<Point2D>`/`shared_ptr<vector<Point2D>>` instead of `Point3D`, and was missing `<cassert>` for the `assert()` calls in its edge-key helper — both compile errors. Fixed the point type and added the include.
- `geompp_python/src/bind_helpers.hpp` didn't include `connected_mesh3d.hpp`, so `bind_connected_mesh3d.cpp` couldn't see `geompp::ConnectedMesh3D`/`FaceView3D` — a compile error as soon as the new binding was added. Added the include, alongside the pre-existing `connected_mesh2d.hpp` one.
- **`ear_clipping_triangulation`'s final leftover triangle used stale loop-local indices**: once the main `while (n > 3)` clipping loop drops to exactly 3 remaining vertices, the loop exits without ever emitting that last triangle, so it has to be pushed separately afterward — but `i_prev`/`i_next` were only ever refreshed at the *top* of each loop iteration (before that iteration's `i = i_next` advance), so by the time the loop exited they described the neighbors of the *previous* `i`, not the current one. Reproduced with a real convex pentagon (`i`/`i_next` both landing on the same index), which threw `"points ... are too close"` from the resulting degenerate `(prev, cur, cur)` triangle. Fixed by refreshing `i_prev = prev_id[i]; i_next = next_id[i];` at the *bottom* of the loop, immediately after `i` advances, so they're always current at loop exit.
- **`PolyMesh2D::Triangulate()`/`PolyMesh3D::Triangulate()` triangulated the whole mesh's shared vertex pool as if it were one polygon ring**: `VERTICES` is the mesh's deduplicated point pool referenced by every facet via `FACE_INDICES`, not a ring at all once a mesh has more than one facet. This "worked" only by coincidence for two facets sharing a welded edge (the welded insertion order happened to trace a plausible-looking path); a two-*disjoint*-facet regression test exposed it directly — 6 triangles / area 11 instead of the correct 4 triangles / area 2. Fixed by triangulating each facet independently (rebuilding its own vertex list from `FACE_IDX_BEGINS`/`FACE_IDX_OFFSETS`/`FACE_INDICES`, the same way `operator[]` does) and combining the results into one mesh.
- `Mesh2D::Connect()`/`Mesh3D::Connect()` had no Python/C# binding and no test in any language despite being implemented and correct since the `meshes` commit — found while drafting a visual-doc conversion demo. Bound and tested in both languages, plus new direct C++ tests.
- **Critical: `ear_clipping_triangulation` could accept a diagonal that exits the polygon.** Its ear-validity check used a strict point-in-triangle test, so a reflex vertex sitting *exactly on* (not just strictly inside) a candidate ear's diagonal went undetected — e.g. an L-shaped hexagon `(0,0),(4,0),(4,2),(2,2),(2,4),(0,4)`, whose reflex vertex `(2,2)` is exactly collinear with the non-adjacent vertices `(0,4)`/`(4,0)`. The resulting triangulation's *total* area still matched the polygon's (one triangle bulged outside while another shrank to compensate), which is why the existing area-sum-only tests never caught it. Fixed by making the check inclusive (`>= 0`, not `> 0`). Single shared function behind `triangulate()`, `Polygon2D/3D::Triangulate()`, and `PolyMesh2D/3D::Triangulate()`, so one fix covers all of them; new tests assert per-triangle containment in the source polygon, not just total area.
- **Critical: `ear_clipping_triangulation` could hang forever on a collinear vertex at the ring's start.** A new skip-branch for collinear ("straight angle") vertices reassigned `i` but never re-derived `i_prev`/`i_next` for it, unlike the loop's normal per-iteration advance. If the ring starts on a collinear vertex, `i_prev`/`i_next` go stale and can end up pointing `i` at itself, spinning forever on a degenerate self-referencing triangle. Fixed by restructuring the loop so the collinear check gates the ear-check-and-clip logic instead of early-`continue`ing past the update, so every iteration reaches the single `i`/`i_prev`/`i_next` update point exactly once.
- `are_collinear()`'s use of `View2D::xy()` used an invalid structured-binding declaration (`double[x0, y0] = view.xy(p0);` — a structured binding may only be declared `auto`, not a concrete type), a compile error caught by the standalone Python build. Fixed to `auto [x0, y0] = view.xy(p0);`.
- **`ear_clipping_triangulation`/`ear_clipping_best_fit_triangulation` could silently swallow an exactly-flat (180°) vertex.** Both only scanned *reflex* vertices as potential ear-blockers — correct for a proper simple polygon, but a flat vertex is neither reflex nor a valid ear itself, and unlike a strictly-interior point it can sit exactly on a candidate ear's boundary edge, so a candidate ear that should have been rejected (its inclusive `is_point_in_triangle` boundary check would have caught the flat vertex, if it had ever been scanned) was wrongly accepted, dropping the flat vertex's two edges from the triangulation entirely. Only reachable under `Collinearity::Guaranteed` fed a genuinely collinear ring — exactly the new `fix_adjacency(vector<Triangle2D/3D>)` re-triangulation path above. Fixed by also treating flat (collinear-with-neighbors) vertices as ear-blockers in both functions.

---

## [0.15.0] - 2026-07-21

> C++ library — tagged `v0.15.0` · C# / NuGet — tagged `csharp-v0.15.0` · Python / PyPI — tagged `python-v0.15.0`

> Polygon boolean operations (`Union`/`Intersection`/`Difference`/`Xor`) for `Polygon2D` and `Polygon3D`, a `clip()` free function templated over `Point2D`/`Point3D` for raw point loops, `View2D` unprojection (`xyz()`), and a `lerp()` free function now backing every hand-rolled two-point interpolation in the codebase — all bound in Python and C# (Python bindings added but unverified in the authoring environment; see note below).

### Added

**C++ core**
- `ScopedPrecision` (`constants.hpp`) — RAII guard that overrides the thread-local `DECIMAL_PRECISION` for a scope and restores it on destruction (exception-safe, nesting-safe). Lets a caller loosen/tighten the comparison tolerance for one operation (e.g. a noisy coplanarity check before a `Polygon3D` boolean op) without the brittle "mutate the global, remember to restore it" pattern.
- `Polygon2D::Union/Intersection/Difference/Xor(Polygon2D const&)` — general map-overlay boolean ops: each operand is first decomposed into simple, correctly-oriented pieces (`Polygon2D::Simplify()` if not already `IsSimple()`, itself otherwise — `detail::RingPieces`), both operands' pieces are merged into one segment pool tagged with their source operand, split at every crossing (`detail::split_segments_at_crossings_tagged`, Bentley-Ottmann via `find_intersections`), and classified (`detail::classify_and_orient_source_tagged`): a split segment's own-operand side is known outright from the CCW-outer/CW-hole convention every piece satisfies (no probe needed), so only the OTHER operand needs a sample point, left and right — half the `polygon_contains` calls of a naive both-sides-probed classifier, and no dependency on the nudge epsilon for the segment's own side. Same-operand internal seams (two pieces of one operand touching along a whole shared edge) are cancelled before classification (`detail::cancel_coincident_same_operand_pairs`) so that "own side is always interior" holds regardless of how many pieces an operand decomposes into. Survivors are traced into closed rings and grouped into `{outer, holes}` via a containment forest. Handles holes and self-intersecting operands (`Simplify()` resolves the self-crossing into pieces with interior consistently on one side, which a single self-intersecting ring's own traversal doesn't guarantee), and correctly produces a hole when one operand is fully nested inside the other with no shared boundary. The lower-level `detail::boolean_op` (single possibly-self-intersecting ring per side, probe-both-sides classification) is retained for the `clip()` free function's raw-loop use case.
- `Polygon2D::Intersects(Polygon2D const&)` — `!Intersection(other).empty()`.
- `clip<Points>(clipper_loop, subject_loop)` (`calc_utils2d.hpp`) — templated over `PointContainer`, so it works on raw `vector<Point2D>` and `vector<Point3D>` loops alike (no holes), for callers who don't have a `Polygon2D`/`Polygon3D`. The `Point3D` instantiation fits a plane from `subject_loop`'s first three points (`Plane::From3Points`), requires `clipper_loop` to be coplanar with it (`std::invalid_argument` otherwise), projects both via `View2D::OnPlane`, and unprojects the result via `xyz()`. Returns a flat list of rings since even hole-less inputs can produce a holed intersection.
- **`Triangle2D::Intersection(Triangle2D const&)` / `Intersects(Triangle2D const&)` implemented** (previously `throw std::runtime_error("not implemented")`), via `clip()`: both triangles are always convex, so `clip()`'s general planar-arrangement engine needs no convex special-casing, and the intersection of two convex regions is itself convex and simply connected — `clip()` can only ever return a single outer ring, never a hole. Maps that ring's vertex count to `ReturnSet`'s existing alternatives: 3 vertices → `Triangle2D::Make(...)`, more → `Polygon2D::Make(...)`. `clip()` already drops zero-area slivers internally (`package_result_rings`), so a mere touch (shared vertex, or edges meeting with no overlapping area, e.g. two triangles sharing a full edge) comes back as an empty result → `std::nullopt`, consistent with how `Intersection(Line2D)` already treats an all-vertices touch as "no intersection" rather than a degenerate point/segment result.
- **`Triangle3D::Overlap(Triangle3D const&)` / `Overlaps(Triangle3D const&)` added**, filling a real gap in the pre-existing `Triangle3D::Intersection(Triangle3D const&)`: that method only ever handles the case where the two triangles' planes actually CROSS (returns the shared chord as a `LineSegment3D`) — for two triangles lying on the SAME plane, its own two internal plane-intersection calls each fail to produce a `LineSegment3D` (a coplanar triangle can't be cut into a chord by its own plane), so it silently returned `std::nullopt` for exactly the coplanar-overlap case, with no test ever exercising it. `Overlap()` fills that gap the same way `Triangle2D::Intersection(Triangle2D)` above does — `clip()` on both triangles' raw point loops after confirming `ToPlane().AlmostEquals(other.ToPlane())` (returns `std::nullopt` outright if the planes differ; use `Intersection()` for that case instead) — mapping the resulting ring to `Triangle3D`/`Polygon3D` by vertex count, `std::nullopt` for a zero-area touch or genuine disjointness. `Intersection(Triangle3D const&)`'s doc comment corrected to no longer claim it handles the coplanar case (it never did) and points callers at `Overlap()` instead.
- **`Polygon2D::DistanceTo(Point2D const&)` / `Polygon3D::DistanceTo(Point3D const&)` implemented** (previously stubs). Mirrors the existing `Polyline2D/3D::DistanceTo(Point)` pattern rather than reusing `distance_to(polygon, line)` (`calc_utils2d.cpp`) — that function computes distance to an *infinite line*, and its O(log n) convex fast path fundamentally depends on the line having a fixed perpendicular direction (making per-vertex signed distance an affine, binary-searchable function); a point target has no such direction, so the algorithm doesn't generalize. Implementation: `0.0` if `Contains(point)` (already covers interior and on-perimeter, including hole boundaries), else the minimum `LineSegment2D/3D::DistanceTo(point)` over every edge of the outer ring *and* every hole — plain O(n), no convex fast path (a real one would need a different algorithm, e.g. angular binary search, not attempted here). For `Polygon3D`, an off-plane point is never `Contains()`-true regardless of its in-plane position, so distance is always measured as true 3D distance to the nearest boundary edge — there is no "perpendicular distance to the flat interior" shortcut, matching `Triangle3D::DistanceTo`'s own existing contract ("0 if inside; otherwise distance to the closest edge or vertex").
- **`Triangle2D::DistanceTo(Point2D const&)` / `Triangle3D::DistanceTo(Point3D const&)` implemented** (previously stubs), same approach as `Polygon2D/3D::DistanceTo` above: `0.0` if `Contains(point)`, else the minimum `LineSegment2D/3D::DistanceTo(point)` over the triangle's three edges (no convex fast path needed at just 3 edges). `Triangle3D`'s off-plane case follows the identical contract `Polygon3D::DistanceTo` does — `Contains()` requires `ToPlane().Contains(point)` first (via `Location()`), so an off-plane point is never `Contains()`-true regardless of its in-plane position, and distance is always true 3D distance to the nearest edge, not a projected-then-perpendicular shortcut. Verified against hand-derived values for a (0,0)/(4,0)/(0,4) right triangle: interior/edge/vertex/hypotenuse points at 0, points outside with the perpendicular foot landing on a leg or the hypotenuse, points outside where the foot falls off every edge (nearest point is a vertex instead), and — for `Triangle3D` — two off-plane cases (nearest point on a vertex, nearest point on the hypotenuse) confirming the true-3D-distance contract rather than a naive `sqrt(in-plane distance² + z²)`.
- `detail::split_segments_at_crossings` now also splits collinear, partially-overlapping segment pairs at their shared sub-segment's endpoints via `LineSegment2D::Overlap` — `find_intersections` alone only reports single-point crossings, since `LineSegment2D::Intersection` returns `nullopt` for parallel input. Shared by `simplify_rings()`, so `Polygon2D::Simplify()` picks up the same fix for self-intersecting polygons with collinear overlaps.
- `View2D::XY(double z=0)` / `YZ(double x=0)` / `ZX(double y=0)` — optional offset parameter (stored in a new `PLANE_OFFSET` member) so these canned axis views can represent any axis-aligned-but-offset plane, not just the ones through the origin.
- `View2D::xyz(Point2D const&)` / `xyz(double, double)` — inverse of `x()`/`y()`/`xy()`: reconstructs the 3D point a view-space `(x, y)` corresponds to. Exact for `OnPlane()`/`Custom` and the offset-taking `XY(z)`/`YZ(x)`/`ZX(y)`; not meant for the bare dominant-axis-approximation views some 3D algorithms use internally.
- `Polygon3D::Union/Difference/Xor(Polygon3D const&)` — same `boolean_op_multi` engine as `Polygon2D` (including the `Simplify()`-decomposition step for self-intersecting operands), projected via `View2D::OnPlane`/unprojected via `xyz()`. Requires `GetPlane().AlmostEquals(other.GetPlane())`; throws `std::logic_error` otherwise, since a non-planar union/difference isn't representable as a single `Polygon3D`. `to_ring_pieces_3d` (`polygon3d.cpp`) re-validates each piece's orientation against the shared projection view rather than trusting its own, independently-computed `Polygon3D::Make()` plane — the two need not agree in normal direction for the same geometric plane, since that's a separate check per piece with no cross-piece consistency guarantee.
- `Polygon3D::Intersection(Polygon3D const&)` → `optional<variant<vector<Polygon3D>, vector<LineSegment3D>>>` — area intersection when coplanar; when the planes cross instead, the shared chord(s) where both polygons' bounded regions cover the planes' common line (via the newly-added `compute_intersection_intervals_3d`, mirroring the existing `compute_intersection_intervals_2d`); `nullopt` for parallel-distinct planes or no overlap.
- `Polygon3D::Intersects(Polygon3D const&)` — `Intersection()` plus a `std::visit` emptiness check.
- `lerp(Point2D const&, Point2D const&, double)` / `lerp(Point3D const&, Point3D const&, double)` (`point2d.hpp`/`point3d.hpp`) — `P0 + t * (P1 - P0)`, not clamped. Replaces the equivalent hand-rolled formula in `LineSegment2D/3D::Interpolate`, `Polyline2D/3D::Interpolate`, `Polygon2D::Intersection(Line2D/Ray2D/LineSegment2D)`'s `eval` lambda (three call sites), and `sample_quadratic_bezier`'s De Casteljau step.

**Python bindings**
- `lerp(p0, p1, t)` — bound for both `Point2D` and `Point3D`.
- `clip(clipper_loop, subject_loop)` — bound for both `Point2D` and `Point3D` loops.
- `Polygon2D.union/intersection/difference/xor(other)`, `Polygon2D.intersects(other)`.
- `Polygon3D.union/difference/xor(other)`, `Polygon3D.intersects(other)`, `Polygon3D.intersection(other)` (via the existing generic `opt_variant_to_py` helper — no new binding infrastructure needed).

**C# bindings**
- `GeomUtil.Lerp(Point2D^, Point2D^, double)` / `(Point3D^, Point3D^, double)`.
- `GeomUtil.Clip(List<Point2D^>^, List<Point2D^>^)` / `(List<Point3D^>^, List<Point3D^>^)`.
- `Polygon2D.Union/Intersection/Difference/Xor(Polygon2D^)`, `Polygon2D.Intersects(Polygon2D^)`.
- `Polygon3D.Union/Difference/Xor(Polygon3D^)`, `Polygon3D.Intersects(Polygon3D^)`, `Polygon3D.Intersection(Polygon3D^)` (returns `System::Object^`: `array<Polygon3D^>^`, `array<LineSegment3D^>^`, or `null`, matching the existing `Plane.Intersection(Triangle3D)` variant-unpacking pattern).

### Fixed

- `Plane::AlmostEquals` — the coplanarity check compared the origins' dot product against `epsilon` itself (using `compare()`'s own default tolerance) instead of comparing it against zero using `epsilon` as the tolerance, so a custom (non-default) `epsilon` could reject two exactly-coincident planes, and even default-epsilon calls rejected coplanar origins with a small dot product of the "wrong" sign. Previously masked in the common case by a since-removed redundant `Origin.AlmostEquals(...)` branch that short-circuited true whenever the two origins happened to be the same point.
- `sample_quadratic_bezier`'s De Casteljau step had a typo (`lerp(a, b t)`, missing a comma) that failed to compile. Never released; caught immediately since nothing downstream could build. Fixed; the construction itself (`a = lerp(T0,p1,t)`, `b = lerp(p1,T1,t)`, `pt = lerp(a,b,t)`) was already mathematically correct.
- `boolean_op`'s per-edge classification nudge briefly lost, then regained, its coupling to `DOUBLE_EPSILON`: `polygon_contains()`'s own internal comparisons use the current `DOUBLE_EPSILON` as their tolerance, so a nudge smaller than it gets read as "on the boundary" instead of definitely inside/outside (confirmed: a point clearly outside a unit square was reported as inside once `DECIMAL_PRECISION` was loosened past the nudge distance). The nudge must stay larger than `DOUBLE_EPSILON` — same implicit assumption every `AlmostEquals`-based comparison in this codebase already makes about `DOUBLE_EPSILON` being small relative to the geometry's own scale.
- `boolean_op`'s `package_result_rings` now filters out zero-area/degenerate result rings (via a raw shoelace that never calls `remove_collinear`, so it can't throw), instead of letting a sliver ring reach `signed_area()`/`Make()` and throw. A boolean op can legitimately produce a grazing-contact sliver; it contributes no area and is dropped rather than crashing the operation.
- **`detail::trace_directed_boundary` merged two logically-separate result faces into one self-touching ring whenever they shared exactly one vertex** (a "pinch point") — e.g. a self-intersecting bowtie's two `Simplify()`-decomposed lobes, which touch only at the original self-crossing. The half-edge walk's "next edge by angle" rule only stops on an already-*used edge*, not an already-*visited vertex*, so on reaching the shared vertex a second time it kept threading through into the other lobe instead of closing the first one — producing one 6-vertex figure-eight ring instead of two separate triangles. Caught by asserting result piece **count** (not just total area) in the new `Polygon3D` self-intersecting-bowtie tests below: `Polygon2D.Intersection` had the identical bug already, silently — its area-only bowtie tests happened to pass because the merged ring's net signed area coincidentally matched the correct total in magnitude *and* sign for that specific geometry, while the `Polygon3D` case came out with the same magnitude but the *opposite* sign and failed loudly. Fixed by tracking each walk's already-visited vertices and peeling off the closed portion as its own ring the moment a vertex repeats, then continuing the walk from there (handles 3+-way pinches, one peel at a time). Scoped to `trace_directed_boundary` specifically (the directed, single-half-edge-per-boundary-edge tracer used by `boolean_op`/`boolean_op_multi`) — `simplify_rings`'s own tracer keeps both directions of every edge (a genuine DCEL twin to rotate from at every vertex) and was not affected.
- **`SweepLine2D`'s crossing handler could corrupt the sweep-line order and cause `find_intersections()` to silently miss a later, genuine crossing** — the bug behind the `DISABLED_BooleanOp_MissedCrossing` repro and most of the `DISABLED_Randomized_*` property-test failures documented below (Known limitations, now resolved). The old handler reacted to a confirmed crossing by `Remove()`-ing both segments, nudging the sweep position by `DOUBLE_EPSILON`, then `Add()`-ing them back — i.e. re-deriving their new sorted position via `std::lower_bound` and the comparator at exactly the x where the two just-crossed segments' y-values are equal, the one point a non-transitive tie-break could hand it a plausible-but-wrong slot (confirmed in the repro: two same-operand edges sharing an endpoint were simultaneously active while one was mid-reinsertion from an earlier crossing, and the tie-break placed it wrong relative to a third edge it should have been tested against). Fixed by `SweepLine2D::ReverseRun` (`calc_utils2d.hpp`/`.cpp`) — a direct `O(1)` positional swap (`std::reverse` over the already-known-adjacent contiguous run, extended to handle 3+ segments meeting at exactly one point in a single reversal instead of a pairwise cascade) that never consults the comparator at the crossing x at all. `run_bentley_ottmann` now groups same-point `INTERSECTION` events (they sort contiguously by construction) before reacting to them, and a `SetX`-tracked "already past this x" guard rejects a pair rediscovered by a later reversal in the same run (`TestPair` is purely geometric and has no notion of "already resolved," so two already-crossed segments pushed back into direct adjacency by a subsequent reversal would otherwise re-trigger the same crossing test and loop forever). `Get`/`Remove` were refactored onto a shared `FindIndex` helper (`lower_bound` with an exact-id linear-scan fallback) used identically by the new `ReverseRun`.
- **`LineSegment2D::intersect()`'s same-side rejection silently never fired when both of the other segment's endpoints were strictly on the right** — the straddle test multiplied two calls to `is_left()`, which returns `bool` (strictly-left vs. not), not a signed magnitude. `false * false == 0`, indistinguishable from "one endpoint exactly on the line," so the `compare(lsign * rsign, 0) > 0` rejection (meant to fire whenever both endpoints land on the same side) only ever fired for "both strictly left," never for "both strictly right" — an asymmetric, silently-wrong same-side test. Confirmed via a `Union()` output ring where two genuinely non-crossing edges were flagged as crossing, making `Polygon2D::IsSimple()` report a valid simple ring as self-intersecting. Fixed by computing the actual signed cross-product magnitude (`(r1 - l1).Cross(l2 - l1)`, matching `is_left`'s own internal formula) instead of calling `is_left()`. `intersect()` is used only by `has_intersections()`/`IsSimple()` (the Shamos-Hoey sweep) — `find_intersections()` uses exact parametric `LineSegment2D::Intersection()`/`Overlap()` throughout and was never affected; this was a false-**positive**-only bug (reported simple rings as self-intersecting), never a false negative.
- **`Randomized_DifferencePartitionsSubject`'s invariant was checking the wrong quantity for self-intersecting operands**, not exercising a library bug: `Polygon2D::Area()` is a raw shoelace sum over the (possibly self-intersecting) outer ring, which is a NET/signed quantity — two lobes of a bowtie wound in opposite directions partially cancel there (see `Simplify_BowtieAreasMatch`, which asserts the opposite: that the two lobes' `Simplify()`'d areas SUM rather than cancel). `Intersection()`/`Difference()` classify by winding-number membership, which counts both lobes as "inside A" regardless of their relative winding sign, i.e. they operate against the `Simplify()`'d/absolute total, not the raw net `Area()`. For a self-intersecting operand those two notions of "area of A" legitimately differ, so the invariant has to be stated against the one `Intersection`/`Difference` actually agree with. Fixed by restating the invariant against `total_area(a.Simplify())`; now passes all 300 trials and is promoted out of `DISABLED_`.
- **`Polygon2D::Make()` / `Polygon3D::Make()` silently accepted a self-intersecting hole ring.** A hole's only validation was point count (≥ 3) and CW winding (`are_cw`) — nothing checked that the ring didn't cross itself, even though `IsSimple()` already treats a self-intersecting hole as making the whole polygon non-simple. A self-intersecting ring's winding is a degenerate notion in the first place (its net signed area can land on either sign depending on the specific crossing), so a coincidentally-CW self-intersecting ring could slip straight through `are_cw()`. Found via `boolean_op_multi`'s tracer occasionally attaching exactly such a malformed hole to a result piece (the self-intersecting-operand-orientation gap noted above), which then confused `IsSimple()`/`Simplify()` downstream in ways that were hard to diagnose after the fact. Fixed by checking each hole for self-intersections (the same `has_intersections` check `IsSimple()` uses per-hole) before the CW check, so a malformed hole is rejected at construction with a clear `std::runtime_error` instead of silently producing a Polygon2D/Polygon3D whose invariants don't hold. `run_boolean_op` / `run_boolean_op_3d` (`polygon2d.cpp` / `polygon3d.cpp`) now catch that rejection and retry assembling the result piece without holes rather than losing the whole piece or letting the exception escape to the caller — a hole-less piece with too much area is a smaller error than crashing or silently shipping a self-intersecting "simple" polygon. Outer rings are unaffected — they're allowed to self-intersect by design (that's what `IsSimple()`/`Simplify()` exist for); this only tightens the hole-specific validation. Two DIFFERENT holes crossing each other went unchecked the same way (no test covered it, and nothing rejected it) — two overlapping holes don't correspond to any coherent "region removed from the polygon" and would corrupt every area/winding-number/boolean-op computation downstream just as badly as a self-intersecting one. Fixed with the same tool: for each pair of holes, concatenate their edges into one flat segment list (a `SegmentRange2D` would wrongly treat the two rings as a single closed path and fabricate a bogus edge joining them, so this builds an explicit `std::vector<LineSegment2D>` instead) and reuse `has_intersections()`. `shares_endpoint()` (inside `has_intersections`) means two holes that merely touch at a single shared vertex are correctly still allowed — only a genuine crossing or edge overlap is rejected, with `GEOMPP_LOG(ERROR)` naming which two hole indices collided.
- **A hole striking through the outer boundary went unchecked the same way.** Neither `has_intersections()` on the combined outer+hole segment set (the outer ring is explicitly allowed to self-intersect by design — that would flag its own legitimate self-crossing as a false positive) nor the hole-vs-hole check's `intersect()`+`shares_endpoint()` combo (too coarse: it flags a hole vertex landing mid-way along an outer edge — a T-junction — or a hole edge running flush along part of the outer boundary as "intersecting," both of which are ordinary, legitimate polygon-with-hole shapes, e.g. a hole notched into a corner — confirmed by a regression: `Centroid_SquareWithOffCenterHole`, an existing valid test, started throwing) are precise enough for this. Fixed with a new `strictly_crosses()` helper using `detail::line_intersection`'s parametric `sc`/`tc` (position along each *infinite* line): only a hole edge striking an outer edge strictly between both segments' own endpoints (`0 < sc < 1` and `0 < tc < 1`) counts as "striking through" — a shared vertex, T-junction, or collinear overlap all land at `sc`/`tc == 0` or `1` and are correctly left alone. `O(H · n_outer · n_hole)` pairwise; fine at construction-time scale.
- **A hole positioned entirely outside the outer loop went unchecked by any of the above.** None of the checks above rule out a hole that never touches or crosses the outer boundary at all but simply floats outside it — every one of them tests edges against edges, and two disjoint rings that don't happen to intersect trivially satisfy all of them. Fixed by checking that each hole's first vertex is contained in (or on the perimeter of) the outer loop, via the same `detail::view::polygon_contains`/`is_on_perimeter` free functions `Polygon2D::Contains()`/`Polygon3D::Contains()` themselves call — checking only one point per hole is sufficient precisely because the strikes-through check above already guarantees a hole can't cross the outer boundary, so every one of its points is on the same side of it. `Polygon3D::Make()` reuses the same dominant-axis 2D projection (`hole_view`) the other hole checks already compute, since both outer and hole are already confirmed coplanar by that point. `GEOMPP_LOG(ERROR)` names the offending hole index; throws `std::runtime_error`.
- **`Polygon2D::Centroid()` / `Polygon3D::Centroid()` had the same self-intersecting-outer-ring bug `Area()` used to have — worse, in fact, since the raw net signed_area is used as a division DENOMINATOR.** The weighted-average formula `c = Σ(aᵢ·cᵢ) / Σ(aᵢ)` used the whole outer ring's own raw (possibly net/cancelling) `signed_area` as its weight `a` — for a bowtie where the two lobes' areas happen to nearly cancel, that denominator lands near zero, making the centroid numerically unstable or outright nonsensical, not just imprecise the way the old `Area()` was. Fixed the same way: when the outer ring is simple, unchanged; when it self-intersects, decompose it via `simplify_rings()` and weight each real interior face's own centroid by its own (always-positive) area, exactly as if each face were a separate positive contribution — same principle `Area()`'s slow path already uses, and now trivial to apply consistently since `simplify_rings()` itself returns pre-oriented interior faces (see Changed, above). Cross-validated against an independently-computed area-weighted average over `Simplify()`'s pieces (2D and a tilted-plane 3D case) rather than a hand-derived expected centroid.
- **`Polygon3D::Make()`'s stored plane could disagree in sign with the CCW check that had just validated the same points.** `outer_plane` was built from `Plane::From3Points(unique_points[0], unique_points[1], unique_points[2])` — a LOCAL quantity (the turn pivoting at `vertex[0]` toward `vertex[1]`/`vertex[2]`) — while the CCW validation that had just accepted those points uses `are_ccw(unique_points)`, which auto-fits a completely different, GLOBAL reference plane (`closest_world_plane_to()`, picking a world axis by magnitude alone and always using its *positive* direction, discarding the input's own sign). The two agree for a convex polygon but can disagree whenever `vertex[0]` happens to be a reflex (concave) corner of an otherwise-valid CCW polygon — confirmed with a concrete repro: a valid CCW L-shaped hexagon rotated to start at its one reflex vertex reported `Area() == -12` instead of `+12`, contradicting `Area()`'s own "guaranteed to be positive by construction" comment (also contradicting `Polygon3D`'s general invariant that the outer ring, containing every hole, is always the CCW/largest one). Two out-of-band attempts at fixing this — reconstructing the plane via `Plane::FromOriginAndNormal(origin, -normal)` (silently discards `axis_u`/`axis_v`), then correcting it lazily at each downstream call site that happened to need a correct sign (`Area()` via `abs()`, a dedicated view-canonicalization helper for `run_boolean_op_3d`) — both missed the direct fix: check the very plane `Make()` is about to store against the very points it already validated, and swap which two of the first three points go into `From3Points` if it disagrees (keeps `axis_u`/`axis_v` genuinely polygon-derived, unlike the `FromOriginAndNormal` reconstruction). That check (`plane_matches_canonical_winding`, new) is O(1), not a second `are_ccw(unique_points, outer_plane)` call (O(n), a full `signed_area` recomputation) as an intermediate version did: every point is already known coplanar by the time `outer_plane` is built, so `outer_plane`'s normal is necessarily exactly parallel or anti-parallel to the one true plane normal `are_ccw(unique_points)` validated against — never some other direction — meaning only `outer_plane`'s own dominant-axis component sign needs checking, not a full re-derivation of the polygon's winding. With the invariant now truly held at construction, `Area()`, `Centroid()`, and `run_boolean_op_3d`'s shared-view projection all go back to trusting `GetPlane()`'s sign outright — no `abs()`, no defensive re-checks, no per-caller workarounds.

### Known limitations

- ~~Polygon boolean ops (2D and 3D) can drop a whole result component on rare configurations~~ — **resolved, see Fixed above** (`SweepLine2D::ReverseRun`). `BooleanOp_MissedCrossing` now passes and is promoted out of `DISABLED_`.
- **`Randomized_ResultsAreSimple` has one remaining failure (of 200 trials) after the two fixes above**, down from 28/200: a self-intersecting *operand* (not a sweep-ordering issue) produces a non-simple `Difference` piece. Matches the self-intersecting-operand-orientation gap noted below (`trace_directed_boundary`/`cancel_coincident_same_operand_pairs`), not the sweep bug just fixed — stays `DISABLED_` pending that separate fix.
- **Self-intersecting operands previously regressed when boolean-op classification was tried as a pure orientation lookup** (assume interior is always on a traversed edge's left) instead of a winding-number probe: a self-intersecting ring's own traversal doesn't keep interior consistently on one side, so that shortcut misclassified the bowtie case. Resolved by decomposing each operand into `Polygon2D::Simplify()`'s simple, correctly-oriented pieces *before* classification (`detail::RingPieces`) rather than trying to make the shortcut work on a raw self-intersecting ring — see `boolean_op_multi` in Added, above. That decomposition is what surfaced the pinch-vertex tracer bug above in the first place (two touching-at-a-point pieces are exactly its trigger condition) — the two fixes are complementary, not redundant.

### Changed

- **`are_coplanar()` now uses Newell's method instead of trusting `points[0..2]` as the reference plane.** The old approach — build a normal from `(P1-P0) × (P2-P0)` and test every other point's `Dot(normal, Pi-P0)` against zero — is mathematically orientation-sign-invariant (a CW vs. CCW first triple can't flip the test's outcome; empirically confirmed against reflex-vertex and near-collinear-first-triple stress cases on a tilted plane, none of which reproduced a false result), so it wasn't the bug it looked like it could be. It's still less robust than necessary, though: a single triple's normal is only as well-conditioned as that one triple, and `remove_collinear()` only guarantees the triple isn't collinear *within epsilon*, not that it's well-separated. Newell's method (M. Newell, Utah — the standard industry technique for exactly this problem) sums a per-edge contribution across every edge of the ring instead of trusting one triple, so no single vertex can dominate the result; for an exactly-planar input the two methods agree exactly, Newell's is just never worse-conditioned. Correct for any cyclic point ordering (not just a proper simple polygon) as long as the points are genuinely coplanar — verified this covers `Polyline3D::IsPlanar()`'s open (non-ring) knot sequences too, since the implicit wraparound edge Newell's method needs is still a chord between two coplanar points either way. Full suite reruns unchanged (including the `Clip_Point3D_*` tests exercising the one call site — `calc_utils2d.cpp`'s 3D `clip()` coplanarity pre-check — that concatenates two unrelated loops rather than passing a real ring, the case most exposed to Newell's-method magnitude cancellation in principle).
- **`detail::view::simplify_rings()` now internally discards the arrangement's single unbounded "outside" face and returns only the real interior faces, each already CCW.** Previously it returned every traced face — interior AND the one outside face — leaving each caller to rediscover the same fact (exactly one of the returned rings is the outside face, topologically guaranteed by Euler's formula for a planar graph) and calibrate which absolute orientation sign meant "interior" for its own view (a Y-dominant-axis `View2D` mirrors chirality, flipping the answer). Four call sites (`Polygon2D::Simplify()`, `Polygon3D::Simplify()`, and the new `Polygon2D::Area()`/`Polygon3D::Area()` above) each independently reimplemented this exact filter/flip/discard logic. Moved inside `simplify_rings()` itself, calibrated once against `outer`'s own projected orientation (the same reference each caller used to compute externally) — every caller now gets back exactly the faces it actually wants, pre-oriented, with no filtering of its own. Also picked up the per-ring `try/catch` around `signed_area()` (which throws on a degenerate near-collinear trace) that each caller used to do individually, so this doesn't regress that robustness. Purely an internal contract change — full suite reruns byte-identical.
- **`Polygon2D::Area()` / `Polygon3D::Area()` reworked around the invariant the new hole validation above now guarantees: holes are always individually simple.** Previously self-intersecting-outer-ring support meant `Area()` always had to consider the possibility that decomposition was needed; now that `Make()` rejects a self-intersecting hole outright, a hole's contribution to `Area()` is always exactly `signed_area(hole)` (or `(hole, PLANE)` in 3D) — no decomposition, no recursion, ever — computed once up front in O(N_holes). Only the OUTER ring can still be self-intersecting (by design — that's the whole bowtie feature), so the fast/slow branch now checks *only* `VERTICES`' simplicity, not the whole polygon's: simple outer → pure O(n) shoelace as before; self-intersecting outer → decompose *only* the outer ring via `detail::view::simplify_rings` directly (not the public `Simplify()`/`Make()` pipeline, which would also re-run hole assignment and reconstruct `Polygon2D`/`Polygon3D` objects for no reason here) and sum its real bounded faces' areas (discarding the single unbounded "outside" face the decomposition also produces), then combine with the hole total. In `Polygon3D`, each decomposed 2D face is unprojected back to 3D and measured with the existing plane-aware `signed_area(..., PLANE)` rather than a naive 2D-projected shoelace, which would be wrong on a tilted plane (foreshortening) — cross-validated against `Simplify()` + per-piece `Area()` on a deliberately tilted bowtie rather than hand-deriving the expected value. `SelfIntersectingBowtie_IsNotSimple`'s bowtie now reports area 1.7 (the `Simplify()`'d-lobe sum, 1.6 + 0.1) rather than the net/cancelling shoelace value (1.6 − 0.1 = 1.5).
- `detail::run_bentley_ottmann`'s segment-pair predicate is now supplied by the visitor (`TestPair(a, b) -> vector<Point2D>`, 0/1/2 points) instead of being hardcoded to `LineSegment2D::Intersection` at five call sites in the algorithm. `find_intersections()`'s visitor implements `TestPair` as `Intersection() || Overlap()`, so **collinear, partially-overlapping segments are now detected during the sweep itself**, at the same O((n+k) log n) the sweep already guarantees for transversal crossings (two overlapping collinear segments are necessarily adjacent in the sweep-line ordering throughout their shared range, same argument that makes ordinary crossing detection correct). This replaces the separate O(n²) all-pairs `Overlap()` pass `split_segments_at_crossings` grew earlier in this same version, which is now deleted — `boolean_op` and `simplify_rings` both get the fix for free, at a better complexity, with no call-site changes.
- **Both sweeps now let the visitor own the full definition of "these two segments intersect," including `shares_endpoint()`.** Previously `run_bentley_ottmann` hardcoded `!shares_endpoint(a, b)` as a gate before ever calling the visitor's `TestPair`, and `run_shamos_hoey` hardcoded `!shares_endpoint(a, b) && intersect(a, b)` directly in the algorithm body (three call sites, no visitor hook for it at all) — an inconsistency, since `TestPair` already let the visitor pick its own crossing predicate but not whether adjacent-in-the-source-ring pairs were even worth asking. `ShamosHoeyVisitor2D` gained a new required `IsIntersecting(a, b) -> bool` hook, mirroring `TestPair`'s role; both existing visitors (`FirstIntersectionVisitor2D`, `CollectIntersectionsVisitor2D`) now implement the exclusion themselves by calling `shares_endpoint()` (still a shared free function, not reimplemented per visitor) from inside their own predicate, so the sweep algorithms no longer need to know about it at all. Purely a reusability/API-consistency change — full suite reruns byte-identical (same 827 active passing, same 3 `DISABLED_` failing). Used to test a hypothesis about the `DISABLED_BooleanOp_MissedCrossing` bug (see Known limitations) — running the repro through a visitor with NO `shares_endpoint` exclusion at all still misses the same crossing, ruling that out as the cause.

### Tests

- `test_polygon2d.cpp` / `test_polygon3d.cpp`: `WithHoles_SelfIntersectingHole_Throws`, `WithHoles_TwoHolesOverlap_Throws`, `WithHoles_TwoHolesTouchAtVertex_DoesNotThrow`, `WithHoles_HoleStrikesThroughOuter_Throws`, `WithHoles_HoleFlushAgainstOuterEdge_DoesNotThrow`, `WithHoles_HoleEntirelyOutsideOuter_Throws`, and (2D only) `WithHoles_TwoHolesDisjoint_DoesNotThrow` — coverage for the four hole-validation fixes above; the touch-at-vertex and flush-against-edge cases specifically confirm the checks don't over-reject legitimate touching/flush configurations, only genuine crossings, and `HoleEntirelyOutsideOuter_Throws` covers the one configuration (a hole that never touches or crosses the outer boundary) none of the edge-based checks alone can catch.
- `test_polygon2d.cpp`: `Area_SelfIntersectingOuterWithHole` — a self-intersecting bowtie outer ring combined with a hole, exercising `Area()`'s slow path (outer decomposed via `simplify_rings`) together with its direct O(1)-per-hole subtraction in the same call. `test_polygon3d.cpp`: `Area_SelfIntersectingOuterInXYPlane` (same bowtie, direct value) and `Area_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifySum` (same bowtie lifted onto a non-axis-aligned plane, cross-validated against `Simplify()` + per-piece `Area()` rather than a hand-derived expected value — the case that would have caught a naive 2D-projected-shoelace mistake in the new slow path).
- `test_polygon2d.cpp`: `Centroid_SelfIntersectingOuter_MatchesSimplifyWeightedAverage`. `test_polygon3d.cpp`: `Centroid_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifyWeightedAverage` — same cross-validation-against-`Simplify()` strategy as the `Area()` tests above, for the `Centroid()` fix.
- **All of the above C++ hole-validation/`Area()`/`Centroid()` tests ported to Python (`geompp_python/tests/test_geompp.py`) and C# (`geompp_csharp/tests/Program.cs`)**, same geometry and expected values as their C++ originals in each case (`test_with_holes_*`/`WithHoles_*`, `test_area_self_intersecting_*`/`Area_SelfIntersectingOuter*`, `test_centroid_self_intersecting_*`/`Centroid_SelfIntersectingOuter*`). 715 Python tests and 753 C# tests pass alongside the 846 C++ tests.
- `test_polygon2d.cpp` / `test_polygon3d.cpp` / `test_calc_utils2d.cpp`: `Union_*`/`Intersection_*`/`Difference_*`/`Xor_*` (overlapping, nested-non-touching, disjoint, with holes, collinear-overlapping-edges), `Intersects_Polygon_*`, `Clip_*` (2D and 3D: coplanar, non-axis-aligned plane, disjoint, non-coplanar throw, too-few-points throw), `Intersection_Polygon_PlanesCrossing_*` (returns segment / bounds miss / parallel-distinct).
- `test_triangle2d.cpp`: `IntersectionWTriangle` — partial overlap (shrunk-to-a-triangle result, cross-validated against an independently-computed `Polygon2D::Intersection(Polygon2D)` run on the same two triangles), full containment (equals the smaller triangle), disjoint, edge-only touch (`nullopt`), and a rotated triangle producing a `Polygon2D` (non-triangle) result. `test_triangle3d.cpp`: `OverlapWTriangle` — the same case set lifted onto the XY plane, plus a same-shape-but-different-plane case confirming `Overlap()` returns `nullopt` even where `Intersection()` would find a crossing chord (each requires the OTHER's specific plane relationship). `test_polygon2d.cpp` / `test_polygon3d.cpp`: `DistanceTo` expanded from a placeholder `EXPECT_ANY_THROW` into real correctness coverage (interior, on-edge, on-vertex, nearest-edge, nearest-corner, and — 3D only — off-plane cases where the answer is true 3D edge distance, not a plane-projected shortcut), plus `DistanceTo_WithHole` confirming a point inside a hole measures to the hole's boundary, not the outer ring.
- `test_polygon2d.cpp`: `SelfIntersectingBowtie_*` — an asymmetric self-crossing operand (nonzero net area, unlike a symmetric bowtie which `Polygon2D::Make` rejects), cross-validated against `Polygon2D::Simplify()`'s already-trusted decomposition for `Intersects`/`Union`/`Intersection`/`Difference`; `_AsClip_*` variants with the bowtie as the second operand (nothing previously exercised that side of `boolean_op_multi`'s two-operand symmetry); both operands self-intersecting at once, cross-validated against a piecewise sum over both sides' `Simplify()`'d pieces. The `Intersection`-with-fully-containing-square cases assert result piece **count** (matches `Simplify()`'s piece count) and `IsSimple()` on every piece, not just total area — the check that would have caught the pinch-vertex tracer bug above from the start.
- `test_polygon3d.cpp`: `SelfIntersectingBowtie_*` — same coverage as `Polygon2D`'s (`Union`/`Intersection`/`Difference`, piece-count + `IsSimple()` assertions), lifted into the XY plane; this is what surfaced the `trace_directed_boundary` pinch-vertex bug above (the `Polygon2D` equivalent had the same bug already, undetected).
- `test_polygon2d.cpp`: added a repeated-vertex check (`bowtie_test_has_repeated_vertex`, in both `test_polygon2d.cpp` and `test_polygon3d.cpp`) to every self-intersecting-operand boolean-op test, alongside the existing area/piece-count/`IsSimple()` assertions — `IsSimple()` alone is not a reliable signal for the pinch-merge bug above, since two non-adjacent edges that merely share an endpoint are explicitly excluded from the Shamos-Hoey crossing check and can pass `IsSimple() == true` even when the ring is the product of the bug. Also added `SelfIntersectingPentagram_*`: a 5-pointed star (`Simplify()` → central pentagon + 5 point-triangles, each triangle sharing a FULL edge with the pentagon, unlike the bowtie's lobes which only ever touch at one point) — the scenario `detail::cancel_coincident_same_operand_pairs` exists for. `Intersection` with a fully-containing square must recover the star as one 10-vertex ring, not 6 separate pieces; verified this actually exercises the cancellation logic (not passing for an unrelated reason) by temporarily disabling it and confirming the test fails with a spurious hole and the pentagon's own area silently missing from the total.
- `test_polygon2d.cpp` / `test_polygon3d.cpp`: `*LoosePrecision*` — demonstrates `DECIMAL_PRECISION` set before a call already loosens/tightens `Union`/`Intersection`'s effective tolerance (no dedicated parameter needed), on properly-scaled (100-unit) geometry so the loosened epsilon stays small relative to the polygons' own size.
- `test_view2d.cpp`: offset-constructor projection/unprojection round-trips.
- `test_plane.cpp`: `AlmostEquals` with a custom epsilon against exactly- and nearly-coplanar origins.
- `test_point2d.cpp` / `test_point3d.cpp`: `Lerp` — endpoints, midpoint, extrapolation past `t=0`/`t=1`.
- `test_geompp.py`, `Program.cs`: `Lerp`/`Clip`/polygon boolean-op coverage matching the C++ suite. **C# verified (735/735 passing)**; Python **now also verified (697/697 passing)** — the earlier failures traced to `lerp`/`clip` not being re-exported from `geompp_python/geompp/__init__.py` (bound correctly on the C++ side, just missing from the package's Python-level import list) and `View2D.xy/yz/zx` losing their default-argument value across the pybind11 boundary (a raw function-pointer `def_static` doesn't carry a C++ default; needs an explicit `py::arg(...) = value`), plus a stale installed `_geompp` extension module predating this branch's boolean-op work. Fixed in `__init__.py` / `bind_view2d.cpp`; unrelated to the CMake/glog FetchContent issue noted previously, which still blocks a from-scratch `pip install --no-build-isolation -e ./geompp_python` in this sandbox (worked around by copying the `build_win`-produced `.pyd` directly).
- `test_utils.cpp`: `ScopedPrecision_*` — restore on scope exit, restore on exception (unwinding past the guard), and correct nesting (each level restores what it saved).
- `test_polygon2d.cpp`: `Randomized_InclusionExclusion` and `Randomized_XorMatchesSymmetricDifference` — promoted out of `DISABLED_`: the `boolean_op_multi` / source-tagged classification rewrite (see Added, above) happened to make the engine robust enough that these two now pass consistently, though the rewrite targeted a different bug (self-intersecting-operand orientation) and doesn't touch `find_intersections`. `BooleanOp_MissedCrossing` and `Randomized_DifferencePartitionsSubject` are now also promoted out of `DISABLED_` (see Fixed, above). `DISABLED_Randomized_ResultsAreSimple` remains `DISABLED_`, down to a single residual failure unrelated to the sweep fix (see Known limitations); run with `--gtest_also_run_disabled_tests`.
- `test_polygon3d.cpp`: `SelfIntersectingPentagram_*` — 3D counterpart of the `Polygon2D` pentagram tests, same coordinates lifted into the XY plane, same coverage of `cancel_coincident_same_operand_pairs`.
- `test_polygon3d.cpp`: `Area_ReflexFirstVertex_SignIsStableAcrossRotation` — regression test for the `Make()` fix above: a valid CCW L-shaped hexagon, rotated to start at each of its vertices in turn (including its one reflex corner), must report the same positive `Area()` *and* the same `GetPlane()` normal regardless — the exact `Area() == -12` repro that found the bug. `GetPlane_NormalIsConsistentAcrossIndependentlyConstructedCoplanarPolygons` — two coplanar polygons built from the same ring starting at different vertices must report the identical `GetPlane()` normal, the property `to_ring_pieces_3d` relies on instead of a defensive re-check.
- `test_polygon2d.cpp`: `random_polygon()` (new, alongside `random_convex_polygon()`) — with 35% probability, "twists" the convex hull into a self-intersecting polygon by swapping two non-adjacent vertices before `Make()`, giving the randomized invariant tests coverage of self-intersecting operands. Used by the three still-`DISABLED_` tests (free coverage for whoever eventually fixes the sweep) but deliberately **not** by the two promoted, active tests: an experiment substituting it into them pushed `Randomized_InclusionExclusion`'s failure rate from effectively zero to ~23% (70/300 trials) — self-intersecting operands are far more exposed to the missed-crossing bug than the "~1 in a few hundred" documented for convex-only pairs, since `Simplify()`'s decomposition adds more edges and therefore more crossing candidates for the sweep to miss one of.
- `test_calc_utils2d.cpp`: `Benchmark_BooleanOpVsBooleanOpMulti` — informational (prints timing, no wall-clock assertions), head-to-head `detail::boolean_op` vs `detail::boolean_op_multi` on equivalent single-piece operands, isolating the classification difference the source-tagged rewrite claims (4 vs 2 `polygon_contains` calls per split segment). Measured: a wash at small sizes (~8-10 vertices/operand — the new engine's extra tagging/cancellation bookkeeping roughly offsets the halved probe count there), turning into a real but noisy 3-27% speedup at larger sizes (~40-60 vertices/operand) where `polygon_contains`'s O(vertices) cost per probe starts to dominate. The halving is structurally real but its payoff is size-dependent, not a blanket win. Informative about the bug's severity; reverted out of the active tests to keep them reliably green.

---

## [0.14.0] - 2026-07-20

> C++ library — tagged `v0.14.0` · C# / NuGet — tagged `csharp-v0.14.0` · Python / PyPI — tagged `python-v0.14.0`

> Quadratic Bezier corner smoothing: `bezier_smoothing_2()` rounds a polyline corner (p0, p1, p2) with a quadratic Bezier arc (density-based or exact-count, with a tunable tiny-edge skip threshold), and `Polyline2D::Expand()` / `Polyline3D::Expand()` — the inverse of `Reduce()` — apply it to every inner corner of a polyline via a new `polyline_expansion()` free function.

### Added

**C++ core**
- `bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, double min_distance, double min_segment_length = DOUBLE_EPSILON)` (`calc_utils2d.hpp`) — trims tangent points into p1 by up to `smoothness` (∈ [0,1]) fraction of the shorter adjacent edge, then samples the resulting quadratic Bezier arc via De Casteljau's algorithm at roughly `min_distance` apart. An adjacent edge at or below `min_segment_length` isn't trimmed into (that tangent point falls back to `p1`); if both are, the whole corner collapses to `p1` (no curve). Templated over `PointT`; explicit-instantiated for `Point2D`/`Point3D`. Throws `std::invalid_argument` if `min_distance <= 0`.
- `bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, int num_segments, double min_segment_length = DOUBLE_EPSILON)` — same tangent trimming and sampling, but takes an exact segment count instead of a distance-derived one; a distinct overload rather than a parameter that reinterprets `min_distance`, since the two controls don't share a unit. Throws `std::invalid_argument` if `num_segments < 1`.
- `PolylineExpansionParams` (`constants.hpp`) — bundles `smoothness`, a `Mode` (`FixedSegments` / `MinDistance`) mirroring `bezier_smoothing_2`'s two overloads, `segments_per_corner`, `min_distance`, and `min_segment_length`.
- `polyline_expansion(std::vector<PointT> const& input, PolylineExpansionParams const& settings)` (`calc_utils2d.hpp`) — rounds every inner corner of a raw point list via `bezier_smoothing_2`, deduplicating consecutive points (a corner fully skipped by `min_segment_length`, or two adjacent corners' arcs meeting exactly at a shared edge's midpoint — each corner's trim is independently capped at half its shared edge, so adjacent arcs can touch but never cross) rather than emitting zero-length segments. Templated over `PointT`; explicit-instantiated for `Point2D`/`Point3D`.
- `Polyline2D::Expand(PolylineExpansionParams const& settings = PolylineExpansionParams{})` / `Polyline3D::Expand(...)` — the inverse of `Reduce()`: adds vertices rather than removing them. Thin wrappers delegating to `polyline_expansion()`.

**Python bindings**
- `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance, min_segment_length=DOUBLE_EPSILON)` and `bezier_smoothing_2(p0, p1, p2, smoothness, num_segments, min_segment_length=DOUBLE_EPSILON)` — bound for both `Point2D` and `Point3D`; overload resolution follows pybind11's no-conversion-first pass, matching the C++ int-vs-double disambiguation.
- `PolylineExpansionMode` (`FixedSegments`, `MinDistance`), `PolylineExpansionParams(smoothness=0.5, mode=FixedSegments, segments_per_corner=4, min_distance=0.1, min_segment_length=DOUBLE_EPSILON)`, `polyline_expansion(points, settings)`.
- `Polyline2D.expand(settings=PolylineExpansionParams())` / `Polyline3D.expand(...)`.

**C# bindings**
- `GeomUtil.BezierSmoothing2(Point2D^, Point2D^, Point2D^, double, double)` / `(..., int)` — and the `Point3D^` overloads — plus `minSegmentLength` overloads of each.
- `PolylineExpansionMode`, `PolylineExpansionParams` (explicit-backing-field properties, matching this codebase's convention — no C++/CLI auto-properties elsewhere), `GeomUtil.PolylineExpansion(points, settings)`.
- `Polyline2D.Expand()` / `Polyline2D.Expand(PolylineExpansionParams^)` — and the `Polyline3D` equivalents.

### Fixed

- A pre-release iteration of `polyline_expansion()`/`Expand()` read a corner's `p0`/`p1` back out of the *already-built* output buffer instead of the original input knots — this broke the invariant that each corner's trim is bounded by its true adjacent-edge length (adjacent corners' arcs could no longer be guaranteed to meet without crossing), and on the very first corner processed could even set `p1 == p2`. Never released; caught during validation before landing. All `p0`/`p1`/`p2` are read directly from the original knot list, unconditionally.
- `size_t` → `std::size_t` normalized throughout `calc_utils2d.cpp`.

### Tests

- `test_calc_utils2d.cpp`: `BezierSmoothing2_*` — both overloads' throw paths, trimmed-tangent endpoint values (hand-verified), `smoothness=0` sharp-corner collapse, coincident-endpoint degeneracy (no NaN), exact point-count for `num_segments`, and `MinSegmentLength_*` (single-side skip, both-side skip, `DOUBLE_EPSILON` default, explicit-zero opt-out). `PolylineExpansion_*` — two-point no-op, single-corner trim, a dedicated multi-corner regression test proving each corner uses its own original knots, `min_segment_length` full-corner skip.
- `test_polyline2d.cpp` / `test_polyline3d.cpp`: `Expand_*` — unchanged below 3 knots, trimmed-tangent endpoints, `min_segment_length` full-skip matches the original polyline exactly, `MinDistance` mode density scaling, thrown-precondition propagation from `bezier_smoothing_2`.
- `test_geompp.py`: `TestBezierSmoothing2FreeFunction`, `TestPolylineExpansionParams`, `TestPolylineExpansionFreeFunction`, `TestPolyline2DExpand`, `TestPolyline3DExpand` — same coverage as the C++ suite, plus `Point3D` cases.
- `Program.cs`: equivalent `BezierSmoothing2_*`, `PolylineExpansionParams_*`, `PolylineExpansion_*`, `Polyline2D_Expand_*`, `Polyline3D_Expand_*` coverage.

---

## [0.13.0] - 2026-07-10

> C++ library — tagged `v0.13.0` · C# / NuGet — tagged `csharp-v0.13.0` · Python / PyPI — tagged `python-v0.13.0`

> Polyline vertex-count reduction: `Polyline2D/3D::Reduce()` with three selectable strategies (RadialDistance, RamerDouglasPeucker, VisvalingamWhyatt), backed by three new dimension-agnostic free functions in `calc_utils2d.hpp`. `geompp_sample/` removed (superseded by the test suite and README code examples).

### Added

**C++ core**
- `PolylineDecimationStrategy` enum (`constants.hpp`) — `RadialDistance`, `RamerDouglasPeucker`, `VisvalingamWhyatt`.
- `dist_decimation(Points const&, double threshold)` (`calc_utils2d.hpp`) — O(n) radial-distance decimation: drops a vertex when it's closer than `threshold` to the last kept vertex. Templated over `PointContainer`; explicit-instantiated for `std::vector<Point2D>` / `std::vector<Point3D>`.
- `rdp_decimation(Points const&, double threshold)` — Ramer-Douglas-Peucker, O(n log n) to O(n²): iterative (explicit-stack, no recursion) search for the vertex with maximum perpendicular distance from the chord spanning its segment; keeps it and recurses when that distance exceeds `threshold`, else discards the whole span. Perpendicular distance is computed via vector projection (`line_distance_2`), avoiding the `LineSegment` constructor and working uniformly across `Vector2D`/`Vector3D`.
- `vw_decimation(Points const&, double threshold)` — Visvalingam-Whyatt, O(n log n) to O(n²): doubly-linked-list topology + lazy-deletion min-heap keyed on (squared) triangle area; repeatedly removes the lowest-area vertex while its neighbors' areas are recomputed and re-pushed, until the smallest remaining area exceeds `threshold`.
- `Polyline2D::Reduce(PolylineDecimationStrategy strategy = RamerDouglasPeucker, double threshold = 0.5) const` and `Polyline3D::Reduce(...)` — returns a copy of the polyline with fewer vertices. The `VisvalingamWhyatt` and `RamerDouglasPeucker` cases run a cheap `dist_decimation` noise pre-pass (`threshold * 0.1`) before the main algorithm.

**Python bindings**
- `PolylineDecimationStrategy` enum (`RadialDistance`, `RamerDouglasPeucker`, `VisvalingamWhyatt`).
- `dist_decimation(points, threshold)`, `rdp_decimation(points, threshold)`, `vw_decimation(points, threshold)` — bound for both `Point2D` and `Point3D` point lists.
- `Polyline2D.reduce(strategy=RamerDouglasPeucker, threshold=0.5)` and `Polyline3D.reduce(...)`.

**C# bindings**
- `PolylineDecimationStrategy` enum (declared alongside `Polyline2D`, shared by `Polyline3D`).
- `GeomUtil.DistDecimation(List<Point2D^>^, double)` / `RdpDecimation(...)` / `VwDecimation(...)` — and the `Point3D^` overloads.
- `Polyline2D.Reduce()` (defaults) and `Polyline2D.Reduce(PolylineDecimationStrategy, double)`; same pair on `Polyline3D`.

### Removed

- `geompp_sample/` (the standalone `sample.cpp` demo app and its `CMakeLists.txt`) — dropped from the top-level `CMakeLists.txt` and the README's "How to use it" pointer. The test suite and README code examples already cover the same ground.

### Tests

- `test_calc_utils2d.cpp` / `test_calc_utils3d.cpp`: `DistDecimation_*`, `RdpDecimation_*`, `VwDecimation_*` — clustered-point removal, collinear collapse, a hand-verified triangular-spike case (peak kept, shoulders discarded) cross-checked by hand for both the RDP chord-distance and VW triangle-area math, and epsilon-boundary behavior at `threshold=0`. 3D cases isometrically embed the same numeric scenarios in a non-axis-aligned plane to exercise the `Vector3D` code path.
- `test_polyline2d.cpp` / `test_polyline3d.cpp`: `Reduce_*` — all three strategies, default-parameter equivalence to explicit `RamerDouglasPeucker`/`0.5`, and the two-point pass-through case.
- Python `TestDecimationFreeFunctions`, `TestPolyline2DReduce`, `TestPolyline3DReduce`; C# `DistDecimation_*`/`RdpDecimation_*`/`VwDecimation_*`, `Polyline2D_Reduce_*`, `Polyline3D_Reduce_*` mirror the same cases.

---

## [0.12.0] - 2026-07-07

> C++ library — tagged `v0.12.0` · C# / NuGet — tagged `csharp-v0.12.0` · Python / PyPI — tagged `python-v0.12.0`

> Overlap and Touch detection for all 1D primitives and Polyline (Line, Ray, LineSegment, Polyline2D/3D in 2D and 3D); polygon extreme-point search along a line (Daniel Sunday's O(log n) for convex, O(n) otherwise); polygon-to-point and polygon-to-polygon tangent segments (2D and 3D, coplanar); polygon hole accessors; `Polygon2D/3D::ToPoints()` renamed to `Perimeter()` and the old `Perimeter()` renamed to `PerimeterSize()`; `Polygon2D/3D::operator[]` now takes `std::size_t`; FromWkt off-by-one fix.

### Added

**C++ core**
- `Vector2D::IsParallel(Vector2D const& other)` — returns `true` when the 2D cross product (perp-product) is within precision of zero; consistent with `Vector3D::IsParallel`.
- `Overlaps(X)` (bool) and `Overlap(X)` (returns the shared geometry) for all pairs among `Line2D`, `Ray2D`, `LineSegment2D` — and the exact mirror for `Line3D`, `Ray3D`, `LineSegment3D`.
  - `Line::Overlap(Line)` → `std::optional<Line>` — entire line if collinear, nullopt otherwise.
  - `Line::Overlap(Ray)` / `Ray::Overlap(Line)` → `std::optional<Ray>` — the full ray when collinear.
  - `Line::Overlap(Segment)` / `Segment::Overlap(Line)` → `std::optional<Segment>` — the full segment when collinear.
  - `Ray::Overlap(Ray)` → `std::optional<std::variant<Ray, Segment>>` — same-direction: the ray starting later; opposite-direction: the finite segment between origins; single-point touch: nullopt.
  - `Ray::Overlap(Segment)` / `Segment::Overlap(Ray)` → `std::optional<Segment>` — clipped segment; nullopt when disjoint or only touching at origin.
  - `Segment::Overlap(Segment)` → `std::optional<Segment>` — intersection of the two ranges; nullopt when disjoint or single-point touch.
- All 18 `Overlaps()` boolean wrappers delegate to `Overlap().has_value()` (no circular delegation).
- `Touches(X)` (bool) and `Touch(X)` (returns `std::optional<Point>`) for all pairs among `Line2D`, `Ray2D`, `LineSegment2D` — and the exact mirror for `Line3D`, `Ray3D`, `LineSegment3D`.
  - Touch is defined as: two geometries share exactly one endpoint-contact point (not an interior crossing, not a shared segment).
  - `Ray::Touch(Line)` — nullopt if collinear (Overlap), else origin if line contains origin.
  - `Ray::Touch(Ray)` — collinear same-direction: nullopt; collinear anti-parallel same origin: that origin; anti-parallel overlapping: nullopt; non-parallel: origin of whichever ray the other contains.
  - `Segment::Touch(Line)` — nullopt if collinear; P0 or P1 if line contains exactly one endpoint.
  - `Segment::Touch(Ray)` — XOR: return the endpoint the ray contains exclusively; or ray origin if it falls on the interior of the segment (non-collinear).
  - `Segment::Touch(Segment)` — non-parallel: XOR endpoint containment; parallel collinear: endpoint coincidence without overlap.
  - `Line::Touches/Touch(Ray)` and `Line::Touches/Touch(Segment)` delegate to the Ray/Segment counterparts.
- All 18 `Touches()` boolean wrappers delegate to `Touch().has_value()`.
- `Polyline2D::Overlaps/Overlap(Line2D|Ray2D|LineSegment2D|Polyline2D)` — iterates segments, collects all overlapping sub-segments. Returns `std::optional<std::vector<LineSegment2D>>`.
- `Polyline2D::Touches/Touch(Line2D|Ray2D|LineSegment2D|Polyline2D)` — iterates segments, collects all touch points. Returns `std::optional<std::vector<Point2D>>`.
- `Line2D/Ray2D/LineSegment2D::Overlaps/Overlap/Touches/Touch(Polyline2D)` — each delegates to `polyline.method(*this)`.
- Exact 3D mirrors: `Polyline3D`, `Line3D`, `Ray3D`, `LineSegment3D`.
- `find_extreme_points(Polygon2D const&, Line2D const&)` (`calc_utils2d.hpp`) and `find_extreme_points(Polygon3D const&, Line3D const&)` (`calc_utils3d.hpp`) — return `ExtremePoints<PointN>{min_point, max_point}`: the outer-ring vertices with least / greatest projection along the line's direction. Convex polygons use Daniel Sunday's O(log n) binary search (`detail::extreme_points_impl`); non-convex polygons fall back to an O(n) linear scan. Holes are ignored.
- `ExtremePoints<PointT>` struct (`calc_utils2d.hpp`) — `min_point`, `max_point`; shared by the 2D and 3D overloads.
- Concepts `VectorType`, `ProjectablePointWith<P, V>`, and `ProjectablePointContainerWith<R, V>` (`concepts.hpp`) — constrain the generic extreme-point search to ranges of points projectable onto a vector type; the search compares only scalar projections, so one template body drives both 2D and 3D.
- `Polygon2D/3D::HasHoles()` — `true` when the polygon has one or more holes.
- `Polygon2D/3D::Holes()` — returns `std::vector<std::vector<PointN>> const&`, the (CW) hole rings; empty when the polygon has no holes.
- `PolygonTangents<LineSegmentT>` struct (`calc_utils2d.hpp`) — `left`, `right`; shared by the 2D and 3D overloads below.
- `tangents_to(Polygon2D const&, Point2D const&)` and `tangents_to(Polygon3D const&, Point3D const&)` — left/right tangent `LineSegment` from an external point to a polygon. Convex polygons use Daniel Sunday's O(log n) binary search; non-convex polygons are reduced to their convex hull first (a tangent point can only ever be a hull vertex), then mapped back to the original vertex index. The 3D overload requires the point to be coplanar with the polygon (a tangent is inherently planar) and throws `std::logic_error` otherwise.
- `tangents_to(Polygon2D const&, Polygon2D const&)` and `tangents_to(Polygon3D const&, Polygon3D const&)` — the two common outer tangent `LineSegment`s between two polygons. Neither polygon needs to be convex — each is independently reduced to its convex hull when needed. The 3D overload requires both polygons to share the same plane and throws `std::logic_error` otherwise.
- Internally, the convex tangent search (`detail::view::point_poly_tangent_lr_to` / `detail::view::poly_poly_RL_tangent_to`) is templated on `PointContainer` + `View2D`, following the existing `detail::view::*` projection pattern (`is_convex`, `distance_to`, `polygon_contains`, …) so the same O(log n) algorithm drives both `Point2D` and `Point3D` inputs — the 3D entry points just pick the polygon's own plane as the projection view.

**Python bindings**
- `Vector2D.is_parallel(other)` — bound from `Vector2D::IsParallel`.
- `find_extreme_points(polygon, line)` — returns `ExtremePoints2D` / `ExtremePoints3D` (with `.min_point` / `.max_point`) for `Polygon2D`/`Line2D` and `Polygon3D`/`Line3D` respectively.
- `Polygon2D/3D.has_holes()` and `Polygon2D/3D.holes()` — `holes()` returns a list of hole rings (each a list of points), empty when none.
- `PolygonTangents2D` / `PolygonTangents3D` (`.left` / `.right`) and `tangents_to(polygon, point_or_polygon)` — bound for `Polygon2D`×`Point2D`/`Polygon2D`, `Polygon3D`×`Point3D`/`Polygon3D`. The 3D overloads raise on non-coplanar input.
- `overlaps(other)` and `overlap(other)` exposed on `Line2D`, `Ray2D`, `LineSegment2D`, `Line3D`, `Ray3D`, `LineSegment3D`.
- `overlap(Ray, Ray)` uses `opt_variant_to_py` — returns `Ray2D` or `LineSegment2D` (resp. 3D) depending on geometry.
- `touches(other)` and `touch(other)` exposed on all six classes; `touch` always returns a `Point` or `None`.
- `overlaps/overlap/touches/touch(polyline)` added to all six 2D and 3D primitive classes (delegates).
- `overlaps/overlap/touches/touch` added to `Polyline2D` and `Polyline3D`; `overlap` returns a Python list of segments or `None`; `touch` returns a Python list of points or `None`.

**C# bindings**
- `Vector2D::IsParallel(Vector2D^ other)` — managed wrapper delegating to the native method.
- `Overlaps(X^)` (bool) and `Overlap(X^)` (managed ref, null on miss) added to all six managed classes.
- `Ray2D::Overlap(Ray2D^)` / `Ray3D::Overlap(Ray3D^)` return `System::Object^` (null, `Ray^`, or `LineSegment^`); use `is` pattern matching.
- `Touches(X^)` (bool) and `Touch(X^)` (`Point^`, null on miss) added to all six managed classes.
- `Overlaps/Overlap/Touches/Touch(Polyline2D^)` added to `Line2D`, `Ray2D`, `LineSegment2D` managed classes; `Overlap` returns `array<LineSegment2D^>^` (null on miss); `Touch` returns `array<Point2D^>^` (null on miss).
- Same for the 3D managed classes with `Polyline3D^`.
- `Overlaps/Overlap/Touches/Touch` (all 4 overloads each) added to `Polyline2D` and `Polyline3D` managed classes.
- `GeomUtil.FindExtremePoints(Polygon2D^, Line2D^)` and `GeomUtil.FindExtremePoints(Polygon3D^, Line3D^)` — return `ExtremePoints2D^` / `ExtremePoints3D^`, each exposing `MinPoint` / `MaxPoint`.
- `Polygon2D/3D.HasHoles()` and `Polygon2D/3D.Holes()` — `Holes()` returns `array<array<PointN^>^>^` (empty when none).
- `PolygonTangents2D^` / `PolygonTangents3D^` (`Left` / `Right`) and `GeomUtil.TangentsTo(...)` — overloads for `Polygon2D^`×`Point2D^`/`Polygon2D^`, `Polygon3D^`×`Point3D^`/`Polygon3D^`. The 3D overloads throw on non-coplanar input.

### Changed

**C++ core**
- `Polygon2D/3D::ToPoints()` renamed to `Perimeter()`, coherent with `IsOnPerimeter()`; still returns `std::vector<PointN> const&` and is `const` (was a by-value copy in an earlier pass). Avoids copying the vertex vector.
- `Polygon2D/3D::Perimeter()` (the boundary length) renamed to `PerimeterSize()`, freeing the `Perimeter()` name for the point-returning method above.
- `Polygon2D/3D::operator[]` now takes `std::size_t` instead of `int`, matching `Polyline2D/3D` and the segment iterators. Removes the `static_cast<int>` previously needed at the two `find_extreme_points()` call sites in `calc_utils2d.cpp` / `calc_utils3d.cpp`.

**Bindings**
- Python: `Polygon2D`/`Polygon3D` `.to_points()` → `.perimeter()`, `.perimeter()` → `.perimeter_size()`.
- C#: `Polygon2D`/`Polygon3D` `ToPoints()` → `Perimeter()`, `Perimeter()` → `PerimeterSize()`.

### Tests

- `test_calc_utils2d.cpp` / `test_calc_utils3d.cpp`: `ExtremePoints_*` — convex (Sunday O(log n)) and concave (brute-force) paths, diagonal/oblique directions, tilted-plane 3D polygons, hole-ignoring, and a convex-vs-brute-force agreement check. Convex binary search independently cross-checked against brute force over 200k randomized convex polygons.
- `test_polygon2d.cpp` / `test_polygon3d.cpp`: `HasHoles_*` — presence flag and hole-ring contents with and without holes.
- Python `TestExtremePoints`, `TestPolygon2DHoles`, `TestPolygon3DHoles`; C# `FindExtremePoints2D/3D_*`, `HasHoles*`.
- `test_calc_utils2d.cpp` / `test_calc_utils3d.cpp`: `TangentsTo_*` — convex point/polygon (O(log n) path, including a larger convex loop that forces the binary search to actually iterate), non-convex point/polygon (convex-hull reduction, including hull-index → original-index mapping), coplanar 3D on both a world-axis plane and a custom tilted plane, and the non-coplanar `std::logic_error` cases. Every expected vertex was independently cross-checked via the supporting-line cross-product test (all other vertices on one consistent side of the tangent ray) before being hard-coded as an assertion. Python `TestTangentsTo`, C# `TangentsTo2D/3D_*` mirror the same cases.

### Fixed

- `FromWkt` off-by-one substring error in all six 2D/3D line, ray, and segment source files: `substr(end_gtype+1+end_p1+1, end_p2-1)` → `substr(end_gtype+1+end_p1, end_p2)`. Previously caused WKT tokens to be parsed with the first character clipped and the last character included from the surrounding delimiter.

---

## [0.11.0] - 2026-06-24

> C++ library — tagged `v0.11.0` · C# / NuGet — tagged `csharp-v0.11.0` · Python / PyPI — tagged `python-v0.11.0`

> Polygon2D line/ray/segment intersection and simplification; four bounding-shape classes (BRect2D, BPrism3D, BBall2D, BBall3D); convexity predicates for Polygon2D/3D and Polyline3D; planar Polyline3D operations (IsPlanar, IsSimple, ConvexHull, ToPolygon); PCA-based principal axes; Link-Time Optimization in Release builds; Python wheels extended to 3.8–3.14.

### Added

**C++ core**
- `Polygon2D::Intersection(Line2D)` / `Intersection(Ray2D)` / `Intersection(LineSegment2D)` — computes the chord(s) where a line, ray, or segment crosses a 2D polygon. Returns `std::optional<std::vector<LineSegment2D>>`: `std::nullopt` on miss, or one or more chord segments. Convex polygons use the fast Cyrus-Beck parametric clip (outward-normal convention, D < 0 entering); non-convex polygons use the Jordan-curve parity approach. Results are clipped to the ray's or segment's domain. Implemented via `detail::compute_intersection_intervals_2d`.
- `Polygon2D::Intersects(Line2D)` / `Intersects(Ray2D)` / `Intersects(LineSegment2D)` — boolean wrappers delegating to `Intersection`.
- `Polygon2D::Simplify()` / `Polygon3D::Simplify()` — decomposes a self-intersecting polygon into a `vector` of simple polygons via Bentley–Ottmann intersection detection followed by planar-graph half-edge face tracing. Returns `{*this}` when already simple. Handles all three dominant-axis projections (X, Y, Z) including the Y-axis chirality flip case.
- `Polygon2D::IsConvex()` — returns false if the polygon has holes or any concave turn; true otherwise.
- `Polygon3D::IsConvex()` — same, using the stored plane normal for the 3D left-turn test.
- `Polyline3D::IsPlanar()` — true if all knots are coplanar (degenerate cases: <3 points or all collinear also return true).
- `Polyline3D::IsSimple()` — no self-intersections; uses Shamos–Hoey for planar polylines, Bentley–Ottmann + 3D verification for non-planar.
- `Polyline3D::IsConvex()` — throws `std::logic_error` if not planar; checks all consecutive triples make a left turn relative to the plane normal.
- `Polyline3D::ConvexHull()` — throws if not planar; Melkman's deque algorithm; returns `Polyline3D` (open hull path, not a closed polygon).
- `Polyline3D::ToPolygon()` — throws if not planar; closes the open path into a `Polygon3D`.
- `CoordinateFrame` struct (`calc_utils3d.hpp`) — `Vector3D X` (primary/largest variance), `Y` (secondary), `Z` (normal/least variance).
- `principal_axes(vector<Point3D>)` — PCA via Jacobi eigendecomposition on the 3×3 covariance matrix; returns `CoordinateFrame`; stable for any point distribution including non-planar clouds and helices.
- `principal_normal(vector<Point3D>)` — best-fit plane normal; delegates to `principal_axes().Z`.
- `principal_direction(vector<Point3D>)` — dominant spread direction; delegates to `principal_axes().X`.
- `BRect2D` (`brect2d.hpp`) — minimum oriented bounding rectangle via Andrew's monotone-chain convex hull followed by rotating calipers (Freeman & Shapira 1975 / Toussaint 1983). Stores `center`, `axis_u`, `axis_v` (unit vectors), `half_len_u`, `half_len_v`. Methods: `Corners()` (4 `Point2D`), `Contains(Point2D)`, `area()`, `width()`, `height()`, `AlmostEquals()`. Throws `std::invalid_argument` for fewer than 3 points or a collinear/coincident cloud.
- `BPrism3D` (`bprism3d.hpp`) — oriented bounding prism via PCA + rotating calipers. Stores `center`, `axis_u`, `axis_v`, `axis_w` (unit vectors), `half_len_u`, `half_len_v`, `half_len_w`. Methods: `Corners()` (8 `Point3D`), `Contains(Point3D)`, `volume()`, `width()`, `height()`, `depth()`, `AlmostEquals()`. Coplanar inputs produce `half_len_w == DOUBLE_EPSILON`. Throws for fewer than 3 points or a collinear/coincident cloud.
- `BBall2D` (`bball2d.hpp`) — minimum bounding ball in 2D; Ritter's two-pass O(N) algorithm. Stores `center` (`Point2D`) and `radius`. Degenerate inputs: 1 point → zero-radius ball; 2 points → midpoint center, half-distance radius. Throws for empty input.
- `BBall3D` (`bball3d.hpp`) — same Ritter algorithm in 3D. Throws for empty input.

**Python / PyPI**
- `Polygon2D.intersection(line|ray|segment)` — returns `None` on miss or `list[LineSegment2D]` for chord(s).
- `Polygon2D.intersects(line|ray|segment)` — boolean check.
- `Polygon2D.simplify()` / `Polygon3D.simplify()` — returns `list[Polygon2D]` or `list[Polygon3D]`.
- `Polygon2D.is_convex()`, `Polygon3D.is_convex()`.
- `Polyline3D.is_planar()`, `is_simple()`, `is_convex()`, `convex_hull()` → `Polyline3D`, `to_polygon()` → `Polygon3D`.
- `CoordinateFrame` class with `x`, `y`, `z` attributes (all `Vector3D`).
- `principal_axes(points)` → `CoordinateFrame`, `principal_normal(points)` → `Vector3D`, `principal_direction(points)` → `Vector3D`.
- `BBall2D(center, radius)` / `BBall2D(points)` — `center`, `radius`, `contains(p)`, `almost_equals(other)`.
- `BBall3D(center, radius)` / `BBall3D(points)` — same in 3D.
- `BRect2D(points)` — `center`, `axis_u`, `axis_v`, `half_len_u`, `half_len_v`, `width()`, `height()`, `area()`, `corners()`, `contains(p)`, `almost_equals(other)`.
- `BPrism3D(points)` — `center`, `axis_u`, `axis_v`, `axis_w`, `half_len_u`, `half_len_v`, `half_len_w`, `width()`, `height()`, `depth()`, `volume()`, `corners()`, `contains(p)`, `almost_equals(other)`.

**C# / NuGet**
- `Polygon2D.Intersection(Line2D|Ray2D|LineSegment2D)` — returns `null` on miss or `LineSegment2D[]` for chord(s). Use `is LineSegment2D[] segs` pattern matching.
- `Polygon2D.Intersects(Line2D|Ray2D|LineSegment2D)` — boolean wrappers.
- `Polygon2D.Simplify()` / `Polygon3D.Simplify()` — returns `Polygon2D[]` or `Polygon3D[]`.
- `Polygon2D.IsConvex()`, `Polygon3D.IsConvex()`.
- `Polyline3D.IsPlanar()`, `IsSimple()`, `IsConvex()`, `ConvexHull()` → `Polyline3D^`, `ToPolygon()` → `Polygon3D^`.
- `CoordinateFrame` ref class with `X`, `Y`, `Z` properties (`Vector3D^`).
- `GeomUtil.PrincipalAxes()` → `CoordinateFrame^`, `PrincipalNormal()` → `Vector3D^`, `PrincipalDirection()` → `Vector3D^`.
- `BBall2D(Point2D^, double)` / `BBall2D(array<Point2D^>^)` — `Center`, `Radius`, `Contains(Point2D^)`, `AlmostEquals(BBall2D^)`.
- `BBall3D(Point3D^, double)` / `BBall3D(array<Point3D^>^)` — same in 3D.
- `BRect2D(array<Point2D^>^)` — `Center`, `AxisU`, `AxisV`, `HalfLenU`, `HalfLenV`, `Width()`, `Height()`, `Area()`, `Corners()`, `Contains(Point2D^)`, `AlmostEquals(BRect2D^)`.
- `BPrism3D(array<Point3D^>^)` — `Center`, `AxisU`, `AxisV`, `AxisW`, `HalfLenU`, `HalfLenV`, `HalfLenW`, `Width()`, `Height()`, `Depth()`, `Volume()`, `Corners()`, `Contains(Point3D^)`, `AlmostEquals(BPrism3D^)`.
- `.NET 8` build target (`GeomPP_Net8.vcxproj`) and `.NET 9` build target (`GeomPP_Net9.vcxproj`). The NuGet package ships four C++/CLI DLLs: `net8.0-windows7.0`, `net9.0-windows7.0`, `net10.0-windows7.0`, and `net48`.

### Performance

**C++ core**
- **Link-Time Optimization (LTO)** enabled for Release builds on the `geompp` static library and `_geompp` Python extension (`/GL` + `/LTCG` on MSVC; `-flto` on GCC/Clang). Uses `CheckIPOSupported` with a graceful `STATUS` fallback when LTO is unavailable.
- **Extern template for all bounding-shape constructors**: `vector<PointN>` template constructors of `BBox2D`, `BBox3D`, `BBall2D`, `BBall3D`, `BRect2D`, and `BPrism3D` moved from header to `.cpp` (same pattern as `convex_hull_monotone_chain` and `min_bounding_rect`), reducing per-TU instantiation cost and binary size.
- **`View2D` 3D-to-2D projection**: `View2D::x(Point3D)` / `y(Point3D)` project without allocating an intermediate `Point2D`. Axis-aligned views (`XY`, `YZ`, `ZX`) read a single coordinate at zero arithmetic cost; `Custom` computes `(p − ORIGIN).Dot(AXIS_U/V)` in-place.
- `simplify_rings_impl`: adjacency-list duplicate check changed from O(degree) `std::find` per edge to a single `std::sort` + `std::unique` pass after insertion.
- `simplify_rings_impl`: half-edge walk neighbor lookup changed from O(degree) linear scan to O(log degree) `std::upper_bound` on a precomputed angle array.
- `Polygon2D/3D::Simplify()`: hole-assignment polygon construction reduced from O(nc²) repeated `Polygon2D::Make` calls to O(nc) pre-built polygons with reuse.

**Python / PyPI**
- Wheel targets extended to **3.8–3.14** (was 3.8–3.12). Wheels for CPython 3.13 and 3.14 published on PyPI for Linux x86_64 and Windows AMD64.

### Fixed

**C++ core**
- `Plane(origin, normal)` private constructor now normalizes the normal. Previously the raw non-unit vector was stored, causing `SignedDistanceTo` and other distance operations to return scaled results. `From3Points` and `FromOriginAndAxes` were not affected.
- `Polygon2D/3D::Simplify()`: hole-assignment test point changed from midpoint of first edge (can land on a boundary) to centroid of the ring (always interior for convex decomposition faces).
- `Polygon2D/3D::Simplify()`: silent `catch(...)` blocks replaced with `catch(std::runtime_error const&)` and `GEOMPP_LOG(WARNING)` so degenerate-ring failures are visible.
- `simplify_rings_impl`: half-edge walk `delta <= 0.0` comparison replaced with `compare(delta, 0.0, 1e-9) <= 0` to prevent floating-point noise from selecting the reverse edge.

### Tests

**C++ (`geompp_tests`)**
- `test_polygon2d.cpp`: `Intersection_Line_*`, `Intersection_Ray_*`, `Intersection_Segment_*`, `Intersects_*`; `IsConvex_Square_True`, `IsConvex_ConcavePolygon_False`, `IsConvex_WithHole_False`, `IsConvex_Triangle_True`; Simplify suite.
- `test_polygon3d.cpp`: `IsConvex_Square_XYPlane_True`, `IsConvex_ConcavePolygon_False`, `IsConvex_WithHole_False`, `IsConvex_YZPlane_True`; Simplify suite.
- `test_polyline3d.cpp`: `IsPlanar_XYPlane_True`, `IsPlanar_NonPlanar_False`, `IsPlanar_Collinear_True`, `IsSimple_PlanarNoSelfIntersect_True`, `IsSimple_PlanarSelfIntersecting_False`, `IsConvex_PlanarConvex_True`, `IsConvex_PlanarConcave_False`, `IsConvex_NotPlanar_Throws`, `ConvexHull_PlanarPolyline_ReturnsPolyline`, `ConvexHull_NotPlanar_Throws`, `ConvexHull_ThenToPolygon_ValidPolygon`, `ToPolygon_PlanarPolyline_Valid`, `ToPolygon_NotPlanar_Throws`.
- `test_calc_utils3d.cpp`: `PrincipalAxes_PlanarXYCloud_ZIsNormal`, `PrincipalAxes_ElongatedAlongX_XIsLongest`, `PrincipalAxes_AxesAreOrthogonal`, `PrincipalAxes_AxesAreUnitVectors`, `PrincipalNormal_PlanarCloud_MatchesBasisZ`, `PrincipalDirection_ElongatedAlongX_MatchesBasisX`, `PrincipalAxes_TooFewPoints_Throws`.
- `test_bball2d.cpp`: `ConstructorCenterRadius`, `ConstructorFromSinglePoint`, `ConstructorFromTwoPoints`, `ConstructorFromPointsAllContained`, `ConstructorEmptyThrows`, `CopyConstructor`, `Assignment`, `AlmostEquals`, `Contains`.
- `test_bball3d.cpp`: same suite plus `ConstructorFromTwoPointsAlongZ`.
- `test_brect2d.cpp`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_ZeroExtent`, `ConstructorTwoPoints_DegenerateLine`, `ConstructorAxisAlignedSquare`, `ConstructorAxisAlignedRectangle`, `ConstructorNonConvex_SmallArea`, `ConstructorAllPointsContained`, `Accessors_AxisesAreUnitVectors`, `Accessors_AxesOrthogonal`, `Accessors_WidthHeightArea`, `Corners_FourDistinctPoints`, `Contains_Center_True`, `Contains_Interior_True`, `Contains_Boundary_True`, `Contains_Outside_False`, `AlmostEquals_SameRect`, `AlmostEquals_DifferentRect`, `CopyConstructor`, `Assignment`.
- `test_bprism3d.cpp`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_Throws`, `ConstructorTwoPoints_Throws`, `ConstructorAxisAlignedBox`, `ConstructorFlatCloud_WIsEpsilon`, `ConstructorNonConvex_AllPointsContained`, `Accessors_AxesAreUnitVectors`, `Accessors_AxesOrthogonal`, `Accessors_WidthHeightDepthVolume`, `Corners_EightDistinctPoints`, `Contains_Center_True`, `Contains_Interior_True`, `Contains_Outside_False`, `Contains_Boundary_True`, `AlmostEquals_Same`, `AlmostEquals_Different`, `CopyConstructor`, `Assignment`.

**Python (`geompp_python/tests`)**
- `TestPolygon2DIntersection`: intersection and intersects suites for line, ray, and segment.
- `TestPolygon2DSimplify`, `TestPolygon3DSimplify`: Simplify suites.
- `TestPolygon2DIsConvex`: `test_square_is_convex`, `test_concave_not_convex`, `test_with_hole_not_convex`.
- `TestPolygon3DIsConvex`: `test_square_xy_plane_is_convex`, `test_concave_not_convex`, `test_with_hole_not_convex`.
- `TestPolyline3DPlanarConvex`: `test_is_planar_xy`, `test_is_planar_nonplanar`, `test_is_simple_planar`, `test_is_convex_planar`, `test_is_convex_not_planar_throws`, `test_convex_hull_returns_polyline`, `test_convex_hull_to_polygon`, `test_to_polygon_not_planar_throws`.
- `TestPrincipalAxes`: `test_coordinate_frame_attributes`, `test_z_is_normal_for_flat_xy_cloud`, `test_axes_are_orthogonal`, `test_axes_are_unit_vectors`, `test_principal_normal_matches_z`, `test_principal_direction_matches_x`, `test_too_few_points_throws`.
- `TestBBall2D` / `TestBBall3D`: constructor, contains, almost_equals suites.
- `TestBRect2D` / `TestBPrism3D`: constructor, accessors, corners, contains, almost_equals suites.

**C# (`geompp_csharp/tests`)**
- `Polygon2D`: Intersection/Intersects suites for Line2D, Ray2D, LineSegment2D; Simplify suite; `IsConvex_Square_True`, `IsConvex_Concave_False`, `IsConvex_WithHole_False`.
- `Polygon3D`: `IsConvex_Square_True`, `IsConvex_Concave_False`; Simplify suite.
- `Polyline3D`: `IsPlanar_XY_True`, `IsPlanar_NonPlanar_False`, `IsSimple_True`, `IsConvex_Planar_True`, `IsConvex_NotPlanar_Throws`, `ConvexHull_ReturnsPolyline`, `ConvexHull_ThenToPolygon`, `ToPolygon_Valid`, `ToPolygon_NotPlanar_Throws`.
- `GeomUtil`: `PrincipalAxes_NotNull`, `PrincipalAxes_Z_IsNormal`, `PrincipalNormal_NotNull`, `PrincipalDirection_NotNull`, `PrincipalDirection_AlongX`.
- `BBall2D` / `BBall3D`: `ConstructorCenterRadius`, `ConstructorFromSinglePoint`, `ConstructorFromTwoPoints`, `ConstructorFromPointsAllContained`, `Contains_Inside_True`, `Contains_Outside_False`, `AlmostEquals_Same`, `AlmostEquals_Different`.
- `BRect2D`: `ConstructorEmpty_Throws`, `ConstructorFromPoints_AllContained`, `Accessors_AxesUnitAndOrthogonal`, `Contains_Center_True`, `Contains_Outside_False`, `AlmostEquals_Same`.
- `BPrism3D`: `ConstructorEmpty_Throws`, `ConstructorSinglePoint_Throws`, `ConstructorTwoPoints_Throws`, `ConstructorAxisAlignedBox`, `Accessors_AxesUnitAndOrthogonal`, `Contains_Center_True`, `Contains_Outside_False`, `AlmostEquals_Same`.

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
- `Polygon2D::IsOnPerimeter(Point2D const&) const` — returns `true` if the point lies exactly on an edge (outer ring or any hole boundary); uses `LineSegment2D::Contains` per edge, which tolerates floating-point rounding up to `DECIMAL_PRECISION` digits.
- `Polygon3D::IsOnPerimeter(Point3D const&) const` — same semantics; rejects off-plane points immediately, then projects to 2D and delegates to `Polygon2D::IsOnPerimeter`.

**Python / PyPI**
- `Triangle2D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Triangle3D.location(point)` → `tuple[float, float] | None` — Python binding for the new `Location` method.
- `Polygon2D.is_on_perimeter(point)` → `bool` — Python binding for the new method.
- `Polygon3D.is_on_perimeter(point)` → `bool` — Python binding for the new method.

**C# / NuGet**
- `Triangle2D.Location(Point2D^ point)` → `Tuple<double, double>^` (or `null` if outside) — C# binding for the new `Location` method.
- `Triangle3D.Location(Point3D^ point)` → `Tuple<double, double>^` (or `null` if off-plane or outside) — C# binding for the new `Location` method.
- `Polygon2D.IsOnPerimeter(Point2D^ point)` → `bool` — C# binding for the new method.
- `Polygon3D.IsOnPerimeter(Point3D^ point)` → `bool` — C# binding for the new method.

### Changed

**C++ core**
- `Triangle2D::Contains(Point2D const&)` — rewritten to delegate entirely to `Location(point).has_value()`. Behavior is unchanged; implementation is now consistent and symmetric with `Interpolate`.
- `Triangle3D::Contains(Point3D const&)` — was an unimplemented stub (`throw std::runtime_error("not implemented")`); now fully implemented. Rejects off-plane points via `BBox3D` and plane check, then delegates to `Location(point).has_value()`. No 2D projection is performed.
- `Polygon2D::Contains(Point2D const&)` — was an unimplemented stub; now implemented using a winding-number algorithm with boundary-inclusive semantics: calls `IsOnPerimeter` first, then falls back to winding number for strictly interior points.
- `Polygon3D::Contains(Point3D const&)` — was an unimplemented stub; now implemented. Returns `false` immediately for off-plane points; projects to 2D and applies the winding-number algorithm for in-plane points.

### Fixed

**C++ core**
- `Polygon2D::Contains(Point2D const&)` — fixed inverted boundary logic: `IsOnPerimeter` is called first and short-circuits to `true`; previously the order was reversed, causing interior points to return `false`.
- `Polygon2D::FromWkt` / `Polygon3D::FromWkt` — were throwing for valid WKT strings in C++ tests (leftover `EXPECT_ANY_THROW` from when the function was a stub); tests updated to expect successful parse and verify vertex count and first point.
- `Triangle3DTest::Contains_OnBoundary` — off-plane assertion changed from `z=0.001` to `z=0.01`; with `DECIMAL_PRECISION=3` the epsilon is exactly `0.001`, so the old value was within tolerance and the point was classified as on-plane.

### Tests

**C++ (`geompp_tests`)**
- `test_triangle2d.cpp`: `Location` test rewritten — `check_inside` / `check_outside` lambdas that assert `Location` value AND `Contains` status together; round-trip A (`Interpolate(Location(p)) == p`) and round-trip B (`Location(Interpolate(s,t)) == (s,t)`).
- `test_triangle3d.cpp`: `Location` test added — same `check_inside` / `check_outside` / round-trip structure; off-plane point asserts both `Location == nullopt` and `Contains == false`.
- `test_triangle3d.cpp`: `Contains_OnBoundary` — off-plane assertion fixed to `z=0.01`.
- `test_polygon2d.cpp`: `Contains` test completed — interior, near-corner, exterior, and polygon-with-hole cases. `Contains_OnBoundary` — vertices, edge midpoints, and hole boundary. `IsOnPerimeter_True` / `IsOnPerimeter_False` — explicit standalone tests. `Wkt` and `FromFile` updated to verify successful round-trip.
- `test_polygon3d.cpp`: same coverage as 2D plus off-plane and YZ-plane cases.

**Python (`geompp_python/tests`)**
- `TestTriangle2D.test_location`: rewritten with `check_inside`/`check_outside` helpers and both round-trips.
- `TestTriangle3D.test_location`: added — same structure, including off-plane case.
- `TestPolygon2D.test_contains`: completed — interior, near-corner, exterior, polygon-with-hole, and boundary cases.
- `TestPolygon3D.test_contains`: completed — same plus off-plane assertion.
- `TestPolygon2D.test_is_on_perimeter`: vertices, edge midpoints, interior/exterior false cases, hole boundary true and false cases.
- `TestPolygon3D.test_is_on_perimeter`: same in 3D plus off-plane false case.

**C# (`geompp_csharp/tests`)**
- `Triangle2D` — `Location_Vertices_ReturnExpectedCoords_2D`, `Location_Centroid_OneThirdEach_2D`, `Location_NullImpliesNotContained_2D`, `Location_RoundTrip_A_And_B_2D`: each asserts `Location` value AND paired `Contains` call; round-trips A and B included.
- `Triangle3D` — `Location_Vertices_ReturnExpectedCoords`, `Location_Centroid_OneThirdEach`, `Location_NullImpliesNotContained`, `Location_RoundTrip_A_And_B`: same relationship-focused structure.
- `Polygon2D` — `Contains_Interior_True`, `Contains_Exterior_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnPerimeter_OnEdge_True`, `IsOnPerimeter_Interior_False` added.
- `Polygon3D` — `Contains_Interior_True`, `Contains_OffPlane_False`, `Contains_WithHole`, `Contains_OnBoundary_True`, `IsOnPerimeter_OnEdge_True`, `IsOnPerimeter_Interior_False` added.
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
