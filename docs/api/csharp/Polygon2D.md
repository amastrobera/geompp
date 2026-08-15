# `Polygon2D` (C# / .NET)

## `Make`

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > holes)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > >)

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points)`

Same as the const& overload, but consumes points instead of copying it — every point-cleanup step (collinear removal, etc.) reuses points' own storage instead of allocating a fresh vector.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > holes)`

Same as the const& overload, but consumes both points and holes instead of copying them.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > >)

## `FromWkt`

**static** `Polygon2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polygon2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`size_t Size()`


## `AlmostEquals`

`bool AlmostEquals(Polygon2D^ other, double epsilon)`


**Parameters**

- `other` (`Polygon2D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)`^ ToSegments()`


## `Centroid`

[`Point2D`](Point2D.md)`^ Centroid()`


## `Area`

`double Area()`

Outer ring area minus holes.

Holes are always simple (Make() rejects a self-intersecting hole outright), so their contribution is always a direct O(1)-per-hole shoelace sum. If the outer ring is also simple, the whole thing is O(n). If the outer ring self-intersects (e.g. a bowtie), it's decomposed at O(n log n) into its real bounded faces (discarding the unbounded "outside" face the decomposition also produces) and their areas summed — the total COVERED area, matching what Intersection() /Difference()/etc. operate against (winding-number membership counts every lobe as "inside" regardless of local winding sign), not a net/signed sum where opposite-winding lobes would otherwise partially cancel.

## `PerimeterSize`

`double PerimeterSize()`


## `IsSimple`

`bool IsSimple()`


## `IsConvex`

`bool IsConvex()`


## `ConvexHull`

`Polygon2D^ ConvexHull()`


## `Simplify`

`std::vector< Polygon2D > Simplify()`

Decomposes a self-intersecting polygon into one or more simple polygons.

**Returns** — {*this} if already simple; otherwise the set of simple polygons covering the same area.

## `Perimeter`

`std::vector< `[`Point2D`](Point2D.md)` > Perimeter()`


## `HasHoles`

`bool HasHoles()`

Whether this polygon has one or more holes.

## `Holes`

`std::vector< std::vector< `[`Point2D`](Point2D.md)` > > Holes()`

The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.

## `Triangulate`

`std::vector< `[`Triangle2D`](Triangle2D.md)` > Triangulate(`[`TriangulationParams`](TriangulationParams.md)`::Strategy^ strategy)`

Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles.

Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate points, so this always calls the free triangulate() with every [TriangulationParams](TriangulationParams.md) check set to Guaranteed — no re-validation cost.

**Parameters**

- `strategy` ([`TriangulationParams`](TriangulationParams.md)::Strategy^) — which triangulation algorithm to run (see [TriangulationParams](TriangulationParams.md)::Strategy).

**Returns** — one [Triangle2D](Triangle2D.md) per triangle; Size() - 2 triangles.

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is in the polygon's closed region.

## `IsOnPerimeter`

`bool IsOnPerimeter(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on the polygon's boundary.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

`bool Intersects(Polygon2D^ other)`

Tests whether this polygon shares any area (or boundary) with another.

**Parameters**

- `other` (`Polygon2D^`) — The other polygon.

**Returns** — true if the two polygons overlap, touch, or either fully contains the other.

## `Intersection`

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — The crossing point, or std::nullopt if the line misses the polygon.

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ other)`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`std::vector< Polygon2D > Intersection(Polygon2D^ other)`

Set intersection of this polygon and other (overloads Intersection() by argument type).

**Parameters**

- `other` (`Polygon2D^`)

**Returns** — Zero or more result polygons — empty if the two polygons don't overlap.

## `Union`

`std::vector< Polygon2D > Union(Polygon2D^ other)`

Set union of this polygon and other.

Handles holes and self-intersecting operands; a disjoint pair of polygons yields more than one result polygon.

**Parameters**

- `other` (`Polygon2D^`)

**Returns** — Zero or more result polygons (zero is impossible for Union unless both operands are empty).

## `Difference`

`std::vector< Polygon2D > Difference(Polygon2D^ other)`

Set difference (this minus other).

Handles the case where other lies entirely inside this polygon with no shared boundary, correctly producing a hole.

**Parameters**

- `other` (`Polygon2D^`)

**Returns** — Zero or more result polygons — empty if other fully covers this polygon.

## `Xor`

`std::vector< Polygon2D > Xor(Polygon2D^ other)`

Symmetric difference (the area covered by exactly one of the two polygons).

**Parameters**

- `other` (`Polygon2D^`)

**Returns** — Zero or more result polygons.

## `begin`

`const_iterator begin()`


## `end`

`const_iterator end()`


## `cbegin`

`const_iterator cbegin()`


## `cend`

`const_iterator cend()`



---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md), [Triangle2D](Triangle2D.md), [TriangulationParams](TriangulationParams.md)
