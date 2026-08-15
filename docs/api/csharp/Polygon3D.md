# `Polygon3D` (C# / .NET)

## `Make`

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points, std::vector< std::vector< `[`Point3D`](Point3D.md)` > > holes)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `holes` (std::vector< std::vector< [`Point3D`](Point3D.md) > >)

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points)`

Same as the const& overload, but consumes points instead of copying it — every point-cleanup step (collinear removal, etc.) reuses points' own storage instead of allocating a fresh vector.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points, std::vector< std::vector< `[`Point3D`](Point3D.md)` > > holes)`

Same as the const& overload, but consumes both points and holes instead of copying them.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `holes` (std::vector< std::vector< [`Point3D`](Point3D.md) > >)

## `FromWkt`

**static** `Polygon3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polygon3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`size_t Size()`


## `GetPlane`

[`Plane`](Plane.md)`^ GetPlane()`


## `AlmostEquals`

`bool AlmostEquals(Polygon3D^ other, double epsilon)`


**Parameters**

- `other` (`Polygon3D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)`^ ToSegments()`


## `Centroid`

[`Point3D`](Point3D.md)`^ Centroid()`


## `Area`

`double Area()`

Same convention as Polygon3D::Area() : holes are always simple (Make() rejects a self-intersecting hole outright) so their contribution is a direct O(1)-per-hole shoelace sum; the outer ring, if also simple, makes the whole thing O(n), otherwise it's decomposed at O(n log n) into its real bounded faces and their (plane-aware, not merely 2D-projected) areas summed.

## `PerimeterSize`

`double PerimeterSize()`


## `IsSimple`

`bool IsSimple()`


## `IsConvex`

`bool IsConvex()`


## `ConvexHull`

`Polygon3D^ ConvexHull()`


## `Perimeter`

`std::vector< `[`Point3D`](Point3D.md)` > Perimeter()`


## `HasHoles`

`bool HasHoles()`

Whether this polygon has one or more holes.

## `Holes`

`std::vector< std::vector< `[`Point3D`](Point3D.md)` > > Holes()`

The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.

## `Simplify`

`std::vector< Polygon3D > Simplify()`

Decomposes a self-intersecting polygon into one or more simple polygons.

**Returns** — {*this} if already simple; otherwise the set of simple polygons covering the same area.

## `Triangulate`

`std::vector< `[`Triangle3D`](Triangle3D.md)` > Triangulate(`[`TriangulationParams`](TriangulationParams.md)`::Strategy^ strategy)`

Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles, in the polygon's own plane (GetPlane().normal() supplies the projection — no PCA re-fit needed since it's already known).

Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate points, so this always calls the free triangulate() with every [TriangulationParams](TriangulationParams.md) check set to Guaranteed — no re-validation cost.

**Parameters**

- `strategy` ([`TriangulationParams`](TriangulationParams.md)::Strategy^) — which triangulation algorithm to run (see [TriangulationParams](TriangulationParams.md)::Strategy).

**Returns** — one [Triangle3D](Triangle3D.md) per triangle; Size() - 2 triangles.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test. Must lie in the polygon's plane.

**Returns** — true if point is in the polygon's closed region (interior or boundary).

## `IsOnPerimeter`

`bool IsOnPerimeter(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies on the polygon's boundary (any of its edges).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`

Tests whether this polygon intersects a line.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`

Tests whether this polygon intersects a ray.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Tests whether this polygon intersects a segment.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

`bool Intersects(Polygon3D^ other)`

Tests whether this polygon shares any point with another — area overlap when coplanar, or a genuine strike-through (a shared segment on the two planes' common line) when not.

**Parameters**

- `other` (`Polygon3D^`)

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`

Intersection of this polygon with a line.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the polygon.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`

Intersection of this polygon with a ray.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Intersection of this polygon with a segment.

This is a pure strike-through. Co-planar lines intersecting on a 2D planes are called Overlap, so only one Point is returned.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`std::variant< std::vector< Polygon3D >, std::vector< `[`LineSegment3D`](LineSegment3D.md)` > >^  (nullable) Intersection(Polygon3D^ other)`

Intersection of this polygon with another.

Coplanar (same plane): the set intersection of the two areas, same semantics as Polygon3D::Intersection(Polygon3D) — zero or more result polygons. Not coplanar, planes crossing: the two flat regions can only share points along the planes' common line, so the result is the chain of segments where both polygons' bounded regions cover that line (empty chain omitted — reported as std::nullopt, not an empty vector). Not coplanar, planes parallel (and distinct, since coplanar was already ruled out): std::nullopt — parallel distinct planes never share a point.

**Parameters**

- `other` (`Polygon3D^`)

**Returns** — std::nullopt if the two polygons share no point; otherwise whichever variant alternative matches the coplanar/non-coplanar case above.

## `Union`

`std::vector< Polygon3D > Union(Polygon3D^ other)`

Set union of this polygon and other.

**Parameters**

- `other` (`Polygon3D^`)

## `Difference`

`std::vector< Polygon3D > Difference(Polygon3D^ other)`

Set difference (this minus other).

**Parameters**

- `other` (`Polygon3D^`)

## `Xor`

`std::vector< Polygon3D > Xor(Polygon3D^ other)`

Symmetric difference (the area covered by exactly one of the two polygons).

**Parameters**

- `other` (`Polygon3D^`)

## `begin`

`const_iterator begin()`


## `end`

`const_iterator end()`


## `cbegin`

`const_iterator cbegin()`


## `cend`

`const_iterator cend()`



---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md), [Triangle3D](Triangle3D.md), [TriangulationParams](TriangulationParams.md)
