# `Triangle2D` (C++)

## `Make`

**static** `Triangle2D Make(`[`Point2D`](Point2D.md)` const & p0, `[`Point2D`](Point2D.md)` const & p1, `[`Point2D`](Point2D.md)` const & p2)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md) const &)
- `p1` ([`Point2D`](Point2D.md) const &)
- `p2` ([`Point2D`](Point2D.md) const &)

## `FromWkt`

**static** `Triangle2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Triangle2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Vertices`

`std::tuple< `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` > const Vertices() const`


## `AlmostEquals`

`bool AlmostEquals(Triangle2D const & other, double epsilon) const`


**Parameters**

- `other` (`Triangle2D const &`)
- `epsilon` (`double`)

## `Centroid`

[`Point2D`](Point2D.md)` Centroid() const`


## `ToPolygon`

[`Polygon2D`](Polygon2D.md)` ToPolygon() const`


## `SignedArea`

`double SignedArea() const`


## `Area`

`double Area() const`


## `Perimeter`

`double Perimeter() const`


## `IsCCW`

`bool IsCCW() const`


## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle; otherwise the distance to the closest edge or vertex.

## `ToAxis`

`std::tuple< `[`Vector2D`](Vector2D.md)` , `[`Vector2D`](Vector2D.md)` > ToAxis() const`

Local 2D basis spanning the triangle.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `Interpolate`

`std::optional< `[`Point2D`](Point2D.md)` > Interpolate(double s, double t) const`

Interpolates a point in the triangle from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`double`) — Scalar along the U axis (= P1 - P0).
- `t` (`double`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t, s+t) all lie in [0, 1]; otherwise std::nullopt.

## `Location`

`std::optional< std::tuple< double, double > > Location(`[`Point2D`](Point2D.md)` const & point) const`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to locate.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is in the triangle's closed region.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)` const & line) const`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — true if the ray hits the triangle within its own domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`bool Intersects(Triangle2D const & other) const`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle2D const &`) — The other triangle.

**Returns** — true if the two share any point.

## `Intersection`

`ReturnSet Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — A [Point2D](Point2D.md) (line touches a vertex), a [LineSegment2D](LineSegment2D.md) (line cuts through interior), or std::nullopt if disjoint.

`ReturnSet Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

`ReturnSet Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

`ReturnSet Intersection(Triangle2D const & other) const`

Intersection of two triangles.

**Parameters**

- `other` (`Triangle2D const &`) — The other triangle.

**Returns** — A [Point2D](Point2D.md) , [LineSegment2D](LineSegment2D.md) , Triangle2D , or [Polygon2D](Polygon2D.md) depending on overlap, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Ray2D](Ray2D.md), [Vector2D](Vector2D.md)
