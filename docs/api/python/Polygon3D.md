# `Polygon3D` (Python)

## `make`

**static** `make(points: list[`[`Point3D`](Point3D.md)`]) -> Polygon3D`


**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])

**static** `make(points: list[`[`Point3D`](Point3D.md)`], holes: list[list[`[`Point3D`](Point3D.md)`]]) -> Polygon3D`


**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])
- `holes` (list[`list[[Point3D`](Point3D.md)]])

**static** `make(points: list[`[`Point3D`](Point3D.md)`]) -> Polygon3D`

Same as the const& overload, but consumes points instead of copying it — every point-cleanup step (collinear removal, etc.) reuses points' own storage instead of allocating a fresh vector.

**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])

**static** `make(points: list[`[`Point3D`](Point3D.md)`], holes: list[list[`[`Point3D`](Point3D.md)`]]) -> Polygon3D`

Same as the const& overload, but consumes both points and holes instead of copying them.

**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])
- `holes` (list[`list[[Point3D`](Point3D.md)]])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polygon3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polygon3D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `get_plane`

`get_plane() -> `[`Plane`](Plane.md)


## `almost_equals`

`almost_equals(other: Polygon3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polygon3D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange3D`](SegmentRange3D.md)


## `centroid`

`centroid() -> `[`Point3D`](Point3D.md)


## `area`

`area() -> float`

Same convention as Polygon3D::Area() : holes are always simple (Make() rejects a self-intersecting hole outright) so their contribution is a direct O(1)-per-hole shoelace sum; the outer ring, if also simple, makes the whole thing O(n), otherwise it's decomposed at O(n log n) into its real bounded faces and their (plane-aware, not merely 2D-projected) areas summed.

## `perimeter_size`

`perimeter_size() -> float`


## `is_simple`

`is_simple() -> bool`


## `is_convex`

`is_convex() -> bool`


## `convex_hull`

`convex_hull() -> Polygon3D`


## `perimeter`

`perimeter() -> list[`[`Point3D`](Point3D.md)`]`


## `has_holes`

`has_holes() -> bool`

Whether this polygon has one or more holes.

## `holes`

`holes() -> list[list[`[`Point3D`](Point3D.md)`]]`

The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.

## `simplify`

`simplify() -> list[Polygon3D]`

Decomposes a self-intersecting polygon into one or more simple polygons.

**Returns** — {*this} if already simple; otherwise the set of simple polygons covering the same area.

## `triangulate`

`triangulate(strategy: TriangulationParams::Strategy) -> list[`[`Triangle3D`](Triangle3D.md)`]`

Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles, in the polygon's own plane (GetPlane().normal() supplies the projection — no PCA re-fit needed since it's already known).

Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate points, so this always calls the free triangulate() with every TriangulationParams check set to Guaranteed — no re-validation cost.

**Parameters**

- `strategy` (`TriangulationParams::Strategy`) — which triangulation algorithm to run (see TriangulationParams::Strategy).

**Returns** — one [Triangle3D](Triangle3D.md) per triangle; Size() - 2 triangles.

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test. Must lie in the polygon's plane.

**Returns** — true if point is in the polygon's closed region (interior or boundary).

## `is_on_perimeter`

`is_on_perimeter(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on the polygon's boundary (any of its edges).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether this polygon intersects a line.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether this polygon intersects a ray.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether this polygon intersects a segment.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

`intersects(other: Polygon3D) -> bool`

Tests whether this polygon shares any point with another — area overlap when coplanar, or a genuine strike-through (a shared segment on the two planes' common line) when not.

**Parameters**

- `other` (`Polygon3D`)

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a line.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the polygon.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a ray.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a segment.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`intersection(other: Polygon3D) -> std::variant< std::vector< Polygon3D >, std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > | None`

Intersection of this polygon with another.

Coplanar (same plane): the set intersection of the two areas, same semantics as Polygon3D::Intersection(Polygon3D) — zero or more result polygons. Not coplanar, planes crossing: the two flat regions can only share points along the planes' common line, so the result is the chain of segments where both polygons' bounded regions cover that line (empty chain omitted — reported as std::nullopt, not an empty vector). Not coplanar, planes parallel (and distinct, since coplanar was already ruled out): std::nullopt — parallel distinct planes never share a point.

**Parameters**

- `other` (`Polygon3D`)

**Returns** — std::nullopt if the two polygons share no point; otherwise whichever variant alternative matches the coplanar/non-coplanar case above.

## `union`

`union(other: Polygon3D) -> list[Polygon3D]`

Set union of this polygon and other.

**Parameters**

- `other` (`Polygon3D`)

## `difference`

`difference(other: Polygon3D) -> list[Polygon3D]`

Set difference (this minus other).

**Parameters**

- `other` (`Polygon3D`)

## `xor`

`xor(other: Polygon3D) -> list[Polygon3D]`

Symmetric difference (the area covered by exactly one of the two polygons).

**Parameters**

- `other` (`Polygon3D`)

## `begin`

`begin() -> const_iterator`


## `end`

`end() -> const_iterator`


## `cbegin`

`cbegin() -> const_iterator`


## `cend`

`cend() -> const_iterator`



---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md), [Triangle3D](Triangle3D.md)
