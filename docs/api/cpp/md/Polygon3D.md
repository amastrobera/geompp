# `Polygon3D` (C++)

## `Make`

**static** `Polygon3D Make(std::vector< `[`Point3D`](Point3D.md)` > const & points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) > const &)

**static** `Polygon3D Make(std::vector< `[`Point3D`](Point3D.md)` > const & points, std::vector< std::vector< `[`Point3D`](Point3D.md)` > > const & holes)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) > const &)
- `holes` (std::vector< std::vector< [`Point3D`](Point3D.md) > > const &)

## `FromWkt`

**static** `Polygon3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Polygon3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Size`

`std::size_t Size() const`


## `GetPlane`

[`Plane`](Plane.md)` GetPlane() const`


## `AlmostEquals`

`bool AlmostEquals(Polygon3D const & other, double epsilon) const`


**Parameters**

- `other` (`Polygon3D const &`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)` ToSegments() const`


## `Centroid`

[`Point3D`](Point3D.md)` Centroid() const`


## `Area`

`double Area() const`


## `Perimeter`

`double Perimeter() const`


## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test. Must lie in the polygon's plane.

**Returns** — true if point is in the polygon's closed region (interior or boundary).

## `IsOnPerimeter`

`bool IsOnPerimeter(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on the polygon's boundary (any of its edges).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

## `Intersection`

`ReturnSet Intersection(`[`Line3D`](Line3D.md)` const & line) const`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the polygon.

`ReturnSet Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & other) const`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
