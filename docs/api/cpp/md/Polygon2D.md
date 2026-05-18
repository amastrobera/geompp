# `Polygon2D` (C++)

## `Make`

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > const & points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > const &)

**static** `Polygon2D Make(std::vector< `[`Point2D`](Point2D.md)` > const & points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > const & holes)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > const &)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > > const &)

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


## `Perimeter`

`double Perimeter() const`


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

## `IsOnBoundary`

`bool IsOnBoundary(`[`Point2D`](Point2D.md)` const & point) const`

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

## `Intersection`

`ReturnSet Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — The crossing point, or std::nullopt if the line misses the polygon.

`ReturnSet Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & other) const`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
