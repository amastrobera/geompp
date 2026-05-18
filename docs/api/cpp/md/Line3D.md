# `Line3D` (C++)

Infinite 3D line, internally stored as an origin point and a unit direction vector.

Construct with one of the Make factories. Lines compare by collinearity, not by the specific endpoint pair used to construct them.

## `Make`

**static** `Line3D Make(`[`Point3D`](Point3D.md)` const & p0, `[`Point3D`](Point3D.md)` const & p1)`

Construct a line through two distinct points.

**Parameters**

- `p0` ([`Point3D`](Point3D.md) const &) — First point on the line; becomes First and Origin .
- `p1` ([`Point3D`](Point3D.md) const &) — Second point on the line; becomes Last .

**Returns** — A new Line3D whose direction is the normalized vector from p0 to p1.

**static** `Line3D Make(`[`Point3D`](Point3D.md)` const & orig, `[`Vector3D`](Vector3D.md)` const & dir)`

Construct a line through a point along a direction vector.

**Parameters**

- `orig` ([`Point3D`](Point3D.md) const &) — Origin point on the line.
- `dir` ([`Vector3D`](Vector3D.md) const &) — Non-zero direction vector. Need not be unit; will be normalized.

**Returns** — A new Line3D rooted at orig with normalized direction dir .

## `FromWkt`

**static** `Line3D FromWkt(std::string const & wkt)`

Parse a line from a WKT string.

**Parameters**

- `wkt` (`std::string const &`) — Well-Known Text string of the form "LINE (x0 y0 z0, x1 y1 z1)".

**Returns** — The parsed Line3D .

## `FromFile`

**static** `Line3D FromFile(std::string const & path)`

Read a line from a WKT file.

**Parameters**

- `path` (`std::string const &`) — Filesystem path containing a WKT line.

**Returns** — The parsed Line3D .

## `First`

[`Point3D`](Point3D.md)` const & First() const`

First point used to construct the line.

**Returns** — Same point passed as p0 to Make.

## `Last`

[`Point3D`](Point3D.md)` const & Last() const`

Second point used to construct the line.

**Returns** — Same point passed as p1 to Make, or orig + dir for the (origin, direction) form.

## `Origin`

[`Point3D`](Point3D.md)` const & Origin() const`

Origin point of the line.

Equivalent to First .

**Returns** — Origin of the line as a [Point3D](Point3D.md) .

## `Direction`

[`Vector3D`](Vector3D.md)` const & Direction() const`

Unit direction vector of the line.

**Returns** — Normalized vector from First to Last .

## `AlmostEquals`

`bool AlmostEquals(Line3D const & other, double epsilon) const`

Tests whether two lines are the same infinite line (collinear, same/opposite direction).

**Parameters**

- `other` (`Line3D const &`) — Line to compare to.
- `epsilon` (`double`) — Tolerance for parallelism and collinearity checks.

**Returns** — true if other is collinear with this line (heading does not matter).

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Perpendicular distance from a point to this infinite line.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — Length of the perpendicular from point onto this line.

`double DistanceTo(Line3D const & other) const`

Scalar distance between two lines.

**Parameters**

- `other` (`Line3D const &`) — The other line.

**Returns** — 0 if the lines intersect or are collinear; otherwise the perpendicular distance between them.

`double DistanceTo(`[`Ray3D`](Ray3D.md)` const & ray) const`

Scalar distance between this line and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — 0 if the line intersects the ray or contains it; otherwise the minimum distance.

`double DistanceTo(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`

Scalar distance between this line and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — 0 if the line intersects or contains the segment; otherwise the minimum distance.

## `ProjectOnto`

[`Point3D`](Point3D.md)` ProjectOnto(`[`Point3D`](Point3D.md)` const & point) const`

Orthogonal projection of a point onto this infinite line.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — Closest point on this line to point .

## `Distance`

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(Line3D const & other) const`

Directed line-segment whose endpoints are the pair of closest points between the two lines.

**Parameters**

- `other` (`Line3D const &`) — The other line.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to other's closest point, or std::nullopt if the lines intersect or are collinear (in which case the distance is 0).

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(`[`Ray3D`](Ray3D.md)` const & ray) const`

Directed line-segment whose endpoints are the closest pair between this line and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`

Directed line-segment whose endpoints are the closest pair between this line and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to the segment's closest point, or std::nullopt if they intersect or overlap.

## `ToWkt`

`std::string ToWkt() const`

WKT (Well-Known Text) representation of the line, formatted as "LINE (x0 y0 z0, x1 y1 z1)".

**Returns** — WKT string at the current decimal precision.

## `ToFile`

`void ToFile(std::string const & path) const`

Write the line to a file as WKT.

**Parameters**

- `path` (`std::string const &`) — Filesystem path to write to. Existing content is overwritten.

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on this line.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point is collinear with this line within decimal precision.

## `Intersects`

`bool Intersects(Line3D const & other) const`

Tests whether this line intersects another line.

**Parameters**

- `other` (`Line3D const &`) — The other line.

**Returns** — true if they meet at a single point (skew or parallel lines return false; collinear lines return true).

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether this line intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the line meets the ray's domain (sc on the ray must be >= 0).

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether this line intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if the line meets the segment's domain (sc on the segment must be in [0, 1]).

`bool Intersects(`[`Triangle3D`](Triangle3D.md)` const & t) const`

Tests whether this line intersects a triangle (interior or edge).

**Parameters**

- `t` ([`Triangle3D`](Triangle3D.md) const &) — The triangle.

**Returns** — true if the line passes through the triangle's closed region.

## `Intersection`

`ReturnSet Intersection(Line3D const & other) const`

Intersection point of two lines.

**Parameters**

- `other` (`Line3D const &`) — The other line.

**Returns** — The intersection point wrapped in the variant, or std::nullopt for parallel/skew lines.

`ReturnSet Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`

Intersection point of this line with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — The intersection point if it lies on the ray (sc >= 0), or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Intersection point of this line with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — The intersection point if it lies on the segment (sc in [0, 1]), or std::nullopt otherwise.


---

**See also:** [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md), [Vector3D](Vector3D.md)
