# `Polygon2D` (C++)

## `Make`

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > const & points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > const &)

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > const & points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > const & holes)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > const &)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > > const &)

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > && points)`

Same as the const& overload, but consumes points instead of copying it — every point-cleanup step (collinear removal, etc.) reuses points' own storage instead of allocating a fresh vector.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > &&)

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > && points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > && holes)`

Same as the const& overload, but consumes both points and holes instead of copying them.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > &&)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > > &&)

## `FromWkt`

**static** `Polygon2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Polygon2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Size`

`std::size_t Size() const`


## `AlmostEquals`

`bool AlmostEquals(Polygon2D const & other, double epsilon) const`


**Parameters**

- `other` (`Polygon2D const &`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)` ToSegments() const`


## `Centroid`

[`Point2D`](Point2D.md)` Centroid() const`


## `Area`

`double Area() const`

Outer ring area minus holes.

Holes are always simple (Make() rejects a self-intersecting hole outright), so their contribution is always a direct O(1)-per-hole shoelace sum. If the outer ring is also simple, the whole thing is O(n). If the outer ring self-intersects (e.g. a bowtie), it's decomposed at O(n log n) into its real bounded faces (discarding the unbounded "outside" face the decomposition also produces) and their areas summed — the total COVERED area, matching what Intersection() /Difference()/etc. operate against (winding-number membership counts every lobe as "inside" regardless of local winding sign), not a net/signed sum where opposite-winding lobes would otherwise partially cancel.

## `PerimeterSize`

`double PerimeterSize() const`


## `IsSimple`

`bool IsSimple() const`


## `IsConvex`

`bool IsConvex() const`


## `ConvexHull`

`Polygon2D ConvexHull()`


## `Simplify`

`std::vector< Polygon2D > Simplify() const`

Decomposes a self-intersecting polygon into one or more simple polygons.

**Returns** — {*this} if already simple; otherwise the set of simple polygons covering the same area.

## `Perimeter`

`std::vector< `[`Point2D`](Point2D.md)` > const & Perimeter() const`


## `HasHoles`

`bool HasHoles() const`

Whether this polygon has one or more holes.

## `Holes`

`std::vector< std::vector< `[`Point2D`](Point2D.md)` > > const & Holes() const`

The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.

## `Triangulate`

`std::vector< `[`Triangle2D`](Triangle2D.md)` > Triangulate(TriangulationParams::Strategy strategy) const`

Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles.

Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate points, so this always calls the free triangulate() with every TriangulationParams check set to Guaranteed — no re-validation cost.

**Parameters**

- `strategy` (`TriangulationParams::Strategy`) — which triangulation algorithm to run (see TriangulationParams::Strategy).

**Returns** — one [Triangle2D](Triangle2D.md) per triangle; Size() - 2 triangles.

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is in the polygon's closed region.

## `IsOnPerimeter`

`bool IsOnPerimeter(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on the polygon's boundary.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)` const & line) const`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

`bool Intersects(Polygon2D const & other) const`

Tests whether this polygon shares any area (or boundary) with another.

**Parameters**

- `other` (`Polygon2D const &`) — The other polygon.

**Returns** — true if the two polygons overlap, touch, or either fully contains the other.

## `Intersection`

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — The crossing point, or std::nullopt if the line misses the polygon.

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & other) const`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`std::vector< Polygon2D > Intersection(Polygon2D const & other) const`

Set intersection of this polygon and other (overloads Intersection() by argument type).

**Parameters**

- `other` (`Polygon2D const &`)

**Returns** — Zero or more result polygons — empty if the two polygons don't overlap.

## `Union`

`std::vector< Polygon2D > Union(Polygon2D const & other) const`

Set union of this polygon and other.

Handles holes and self-intersecting operands; a disjoint pair of polygons yields more than one result polygon.

**Parameters**

- `other` (`Polygon2D const &`)

**Returns** — Zero or more result polygons (zero is impossible for Union unless both operands are empty).

## `Difference`

`std::vector< Polygon2D > Difference(Polygon2D const & other) const`

Set difference (this minus other).

Handles the case where other lies entirely inside this polygon with no shared boundary, correctly producing a hole.

**Parameters**

- `other` (`Polygon2D const &`)

**Returns** — Zero or more result polygons — empty if other fully covers this polygon.

## `Xor`

`std::vector< Polygon2D > Xor(Polygon2D const & other) const`

Symmetric difference (the area covered by exactly one of the two polygons).

**Parameters**

- `other` (`Polygon2D const &`)

**Returns** — Zero or more result polygons.

## `begin`

`const_iterator begin() const`


## `end`

`const_iterator end() const`


## `cbegin`

`const_iterator cbegin() const`


## `cend`

`const_iterator cend() const`



---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md), [Triangle2D](Triangle2D.md)
