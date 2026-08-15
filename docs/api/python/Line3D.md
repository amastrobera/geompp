# `Line3D` (Python)

Infinite 3D line, internally stored as an origin point and a unit direction vector.

Construct with one of the Make factories. Lines compare by collinearity, not by the specific endpoint pair used to construct them.

## `make`

**static** `make(p0: `[`Point3D`](Point3D.md)`, p1: `[`Point3D`](Point3D.md)`) -> Line3D`

Construct a line through two distinct points.

**Parameters**

- `p0` ([`Point3D`](Point3D.md)) — First point on the line; becomes First and Origin .
- `p1` ([`Point3D`](Point3D.md)) — Second point on the line; becomes Last .

**Returns** — A new Line3D whose direction is the normalized vector from p0 to p1.

**static** `make(orig: `[`Point3D`](Point3D.md)`, dir: `[`Vector3D`](Vector3D.md)`) -> Line3D`

Construct a line through a point along a direction vector.

**Parameters**

- `orig` ([`Point3D`](Point3D.md)) — Origin point on the line.
- `dir` ([`Vector3D`](Vector3D.md)) — Non-zero direction vector. Need not be unit; will be normalized.

**Returns** — A new Line3D rooted at orig with normalized direction dir .

## `from_wkt`

**static** `from_wkt(wkt: str) -> Line3D`

Parse a line from a WKT string.

**Parameters**

- `wkt` (`str`) — Well-Known Text string of the form "LINE (x0 y0 z0, x1 y1 z1)".

**Returns** — The parsed Line3D .

## `from_file`

**static** `from_file(path: str) -> Line3D`

Read a line from a WKT file.

**Parameters**

- `path` (`str`) — Filesystem path containing a WKT line.

**Returns** — The parsed Line3D .

## `first`

`first() -> `[`Point3D`](Point3D.md)

First point used to construct the line.

**Returns** — Same point passed as p0 to Make.

## `last`

`last() -> `[`Point3D`](Point3D.md)

Second point used to construct the line.

**Returns** — Same point passed as p1 to Make, or orig + dir for the (origin, direction) form.

## `origin`

`origin() -> `[`Point3D`](Point3D.md)

Origin point of the line.

Equivalent to First .

**Returns** — Origin of the line as a [Point3D](Point3D.md) .

## `direction`

`direction() -> `[`Vector3D`](Vector3D.md)

Unit direction vector of the line.

**Returns** — Normalized vector from First to Last .

## `almost_equals`

`almost_equals(other: Line3D, epsilon: float) -> bool`

Tests whether two lines are the same infinite line (collinear, same/opposite direction).

**Parameters**

- `other` (`Line3D`) — Line to compare to.
- `epsilon` (`float`) — Tolerance for parallelism and collinearity checks.

**Returns** — true if other is collinear with this line (heading does not matter).

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Perpendicular distance from a point to this infinite line.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — Length of the perpendicular from point onto this line.

`distance_to(other: Line3D) -> float`

Scalar distance between two lines.

**Parameters**

- `other` (`Line3D`) — The other line.

**Returns** — 0 if the lines intersect or are collinear; otherwise the perpendicular distance between them.

`distance_to(ray: `[`Ray3D`](Ray3D.md)`) -> float`

Scalar distance between this line and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — 0 if the line intersects the ray or contains it; otherwise the minimum distance.

`distance_to(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> float`

Scalar distance between this line and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — 0 if the line intersects or contains the segment; otherwise the minimum distance.

## `project_onto`

`project_onto(point: `[`Point3D`](Point3D.md)`) -> `[`Point3D`](Point3D.md)

Orthogonal projection of a point onto this infinite line.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — Closest point on this line to point .

## `distance`

`distance(other: Line3D) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment whose endpoints are the pair of closest points between the two lines.

**Parameters**

- `other` (`Line3D`) — The other line.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to other's closest point, or std::nullopt if the lines intersect or are collinear (in which case the distance is 0).

`distance(ray: `[`Ray3D`](Ray3D.md)`) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment whose endpoints are the closest pair between this line and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.

`distance(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment whose endpoints are the closest pair between this line and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — A [LineSegment3D](LineSegment3D.md) from this line's closest point to the segment's closest point, or std::nullopt if they intersect or overlap.

## `to_wkt`

`to_wkt() -> str`

WKT (Well-Known Text) representation of the line, formatted as "LINE (x0 y0 z0, x1 y1 z1)".

**Returns** — WKT string at the current decimal precision.

## `to_file`

`to_file(path: str) -> None`

Write the line to a file as WKT.

**Parameters**

- `path` (`str`) — Filesystem path to write to. Existing content is overwritten.

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on this line.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is collinear with this line within decimal precision.

## `intersects`

`intersects(other: Line3D) -> bool`


**Parameters**

- `other` (`Line3D`)

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md))

`intersects(t: `[`Triangle3D`](Triangle3D.md)`) -> bool`


**Parameters**

- `t` ([`Triangle3D`](Triangle3D.md))

`intersects(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `intersection`

`intersection(other: Line3D) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `other` (`Line3D`)

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md))

`intersection(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::variant< `[`Point3D`](Point3D.md)` , std::vector< `[`Point3D`](Point3D.md)` > > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `overlaps`

`overlaps(line: Line3D) -> bool`


**Parameters**

- `line` (`Line3D`)

`overlaps(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlaps(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`overlaps(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `overlap`

`overlap(line: Line3D) -> Line3D | None`


**Parameters**

- `line` (`Line3D`)

`overlap(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Ray3D`](Ray3D.md)` | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlap(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`LineSegment3D`](LineSegment3D.md)` | None`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`overlap(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::vector< `[`LineSegment3D`](LineSegment3D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `touches`

`touches(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touches(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`touches(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `touch`

`touch(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touch(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`touch(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))


---

**See also:** [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md), [Vector3D](Vector3D.md)
