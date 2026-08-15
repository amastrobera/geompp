# `Polygon2D` (Python)

## `make`

**static** `make(points: list[`[`Point2D`](Point2D.md)`]) -> Polygon2D`


**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])

**static** `make(points: list[`[`Point2D`](Point2D.md)`], holes: list[list[`[`Point2D`](Point2D.md)`]]) -> Polygon2D`


**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])
- `holes` (list[`list[[Point2D`](Point2D.md)]])

**static** `make(points: list[`[`Point2D`](Point2D.md)`]) -> Polygon2D`

Same as the const& overload, but consumes points instead of copying it — every point-cleanup step (collinear removal, etc.) reuses points' own storage instead of allocating a fresh vector.

**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])

**static** `make(points: list[`[`Point2D`](Point2D.md)`], holes: list[list[`[`Point2D`](Point2D.md)`]]) -> Polygon2D`

Same as the const& overload, but consumes both points and holes instead of copying them.

**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])
- `holes` (list[`list[[Point2D`](Point2D.md)]])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polygon2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polygon2D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `almost_equals`

`almost_equals(other: Polygon2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polygon2D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange2D`](SegmentRange2D.md)


## `centroid`

`centroid() -> `[`Point2D`](Point2D.md)


## `area`

`area() -> float`

Outer ring area minus holes.

Holes are always simple (Make() rejects a self-intersecting hole outright), so their contribution is always a direct O(1)-per-hole shoelace sum. If the outer ring is also simple, the whole thing is O(n). If the outer ring self-intersects (e.g. a bowtie), it's decomposed at O(n log n) into its real bounded faces (discarding the unbounded "outside" face the decomposition also produces) and their areas summed — the total COVERED area, matching what Intersection() /Difference()/etc. operate against (winding-number membership counts every lobe as "inside" regardless of local winding sign), not a net/signed sum where opposite-winding lobes would otherwise partially cancel.

## `perimeter_size`

`perimeter_size() -> float`


## `is_simple`

`is_simple() -> bool`


## `is_convex`

`is_convex() -> bool`


## `convex_hull`

`convex_hull() -> Polygon2D`


## `simplify`

`simplify() -> list[Polygon2D]`

Decomposes a self-intersecting polygon into one or more simple polygons.

**Returns** — {*this} if already simple; otherwise the set of simple polygons covering the same area.

## `perimeter`

`perimeter() -> list[`[`Point2D`](Point2D.md)`]`


## `has_holes`

`has_holes() -> bool`

Whether this polygon has one or more holes.

## `holes`

`holes() -> list[list[`[`Point2D`](Point2D.md)`]]`

The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.

## `triangulate`

`triangulate(strategy: TriangulationParams::Strategy) -> list[`[`Triangle2D`](Triangle2D.md)`]`

Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles.

Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate points, so this always calls the free triangulate() with every TriangulationParams check set to Guaranteed — no re-validation cost.

**Parameters**

- `strategy` (`TriangulationParams::Strategy`) — which triangulation algorithm to run (see TriangulationParams::Strategy).

**Returns** — one [Triangle2D](Triangle2D.md) per triangle; Size() - 2 triangles.

## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is in the polygon's closed region.

## `is_on_perimeter`

`is_on_perimeter(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on the polygon's boundary.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

`intersects(other: Polygon2D) -> bool`

Tests whether this polygon shares any area (or boundary) with another.

**Parameters**

- `other` (`Polygon2D`) — The other polygon.

**Returns** — true if the two polygons overlap, touch, or either fully contains the other.

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — The crossing point, or std::nullopt if the line misses the polygon.

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`intersection(other: `[`LineSegment2D`](LineSegment2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`intersection(other: Polygon2D) -> list[Polygon2D]`

Set intersection of this polygon and other (overloads Intersection() by argument type).

**Parameters**

- `other` (`Polygon2D`)

**Returns** — Zero or more result polygons — empty if the two polygons don't overlap.

## `union`

`union(other: Polygon2D) -> list[Polygon2D]`

Set union of this polygon and other.

Handles holes and self-intersecting operands; a disjoint pair of polygons yields more than one result polygon.

**Parameters**

- `other` (`Polygon2D`)

**Returns** — Zero or more result polygons (zero is impossible for Union unless both operands are empty).

## `difference`

`difference(other: Polygon2D) -> list[Polygon2D]`

Set difference (this minus other).

Handles the case where other lies entirely inside this polygon with no shared boundary, correctly producing a hole.

**Parameters**

- `other` (`Polygon2D`)

**Returns** — Zero or more result polygons — empty if other fully covers this polygon.

## `xor`

`xor(other: Polygon2D) -> list[Polygon2D]`

Symmetric difference (the area covered by exactly one of the two polygons).

**Parameters**

- `other` (`Polygon2D`)

**Returns** — Zero or more result polygons.

## `begin`

`begin() -> const_iterator`


## `end`

`end() -> const_iterator`


## `cbegin`

`cbegin() -> const_iterator`


## `cend`

`cend() -> const_iterator`



---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md), [Triangle2D](Triangle2D.md)
