# `Triangle3D` (C++)

## `Make`

**static** `Triangle3D Make(`[`Point3D`](Point3D.md)` const & p0, `[`Point3D`](Point3D.md)` const & p1, `[`Point3D`](Point3D.md)` const & p2)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md) const &)
- `p1` ([`Point3D`](Point3D.md) const &)
- `p2` ([`Point3D`](Point3D.md) const &)

## `FromWkt`

**static** `Triangle3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Triangle3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Vertices`

`std::tuple< `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` > const Vertices() const`


## `AlmostEquals`

`bool AlmostEquals(Triangle3D const & other, double epsilon) const`


**Parameters**

- `other` (`Triangle3D const &`)
- `epsilon` (`double`)

## `Centroid`

[`Point3D`](Point3D.md)` Centroid() const`


## `ToPolygon`

[`Polygon3D`](Polygon3D.md)` ToPolygon() const`


## `ToPlane`

[`Plane`](Plane.md)` ToPlane() const`


## `AreaVector`

[`Vector3D`](Vector3D.md)` AreaVector() const`


## `Normal`

[`Vector3D`](Vector3D.md)` Normal() const`


## `SignedArea`

`double SignedArea(`[`Vector3D`](Vector3D.md)` const & ref_normal) const`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md) const &)

## `Area`

`double Area() const`


## `Perimeter`

`double Perimeter() const`


## `IsCCW`

`bool IsCCW(`[`Vector3D`](Vector3D.md)` const & ref_normal) const`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md) const &)

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle's closed region; otherwise the distance to the closest edge or vertex.

## `ToAxis`

`std::tuple< `[`Vector3D`](Vector3D.md)` , `[`Vector3D`](Vector3D.md)` > ToAxis() const`

Local 2D basis spanning the triangle's plane.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `Interpolate`

`std::optional< `[`Point3D`](Point3D.md)` > Interpolate(double s, double t) const`

Interpolates a point in the triangle's plane from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`double`) — Scalar along the U axis (= P1 - P0).
- `t` (`double`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t) describes a point inside the triangle (s, t, s+t in [0, 1]); otherwise std::nullopt.

## `Location`

`std::optional< std::tuple< double, double > > Location(`[`Point3D`](Point3D.md)` const & point) const`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to locate. Must lie in the triangle's plane.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test. Must lie in the triangle's plane (within decimal precision).

**Returns** — true if point is in the triangle's closed region.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the ray hits the triangle within its own domain (sc on the ray >= 0).

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`bool Intersects(`[`Plane`](Plane.md)` const & plane) const`

Tests whether this triangle intersects a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md) const &) — The plane.

**Returns** — true if the plane cuts through the triangle or touches it.

`bool Intersects(Triangle3D const & other) const`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle3D const &`) — The other triangle.

**Returns** — true if the two triangles share any point.

## `Overlaps`

`bool Overlaps(Triangle3D const & other) const`

Tests whether this triangle coplanar-overlaps another triangle.

See Overlap for what "overlap" means here (same-plane shared AREA), as opposed to Intersects (crossing planes).

**Parameters**

- `other` (`Triangle3D const &`) — The other triangle.

**Returns** — true if the two lie on the same plane and share a positive-area region (a mere touching point or shared edge, with no area in common, does not count — see Overlap ).

## `Intersection`

`ReturnSet Intersection(`[`Line3D`](Line3D.md)` const & line) const`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the triangle.

`ReturnSet Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — The crossing point if it lies within the ray's domain, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`ReturnSet Intersection(`[`Plane`](Plane.md)` const & plane) const`

Intersection of this triangle with a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md) const &) — The plane.

**Returns** — A [Point3D](Point3D.md) (touches at a vertex), a [LineSegment3D](LineSegment3D.md) (cuts through interior), or std::nullopt if disjoint.

`ReturnSet Intersection(Triangle3D const & other) const`

Intersection of two triangles whose planes cross (are neither coincident nor parallel).

**Parameters**

- `other` (`Triangle3D const &`) — The other triangle.

**Returns** — The shared chord as a [LineSegment3D](LineSegment3D.md) where both triangles' bounded regions cover the two planes' common line, or std::nullopt if the chord falls outside one of the triangles (or the planes don't cross at all). For two triangles on the SAME plane, use Overlap instead — this method returns std::nullopt for coincident planes, since no such chord exists on a single plane.

## `Overlap`

`ReturnSet Overlap(Triangle3D const & other) const`

Coplanar overlap of two triangles — the shared region when both lie on the same plane.

Complements Intersection(Triangle3D const&), which only handles crossing (non-coincident) planes and returns std::nullopt for coplanar input.

**Parameters**

- `other` (`Triangle3D const &`) — The other triangle. If it does not lie on the same plane as this one, returns std::nullopt (use Intersection instead for that case).

**Returns** — The shared area as a Triangle3D or [Polygon3D](Polygon3D.md) (whichever shape the overlap region takes), or std::nullopt if the two lie on the same plane but share no area — a mere touching vertex or a shared edge with no interior overlap does not count as an overlap here.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Ray3D](Ray3D.md), [Vector3D](Vector3D.md)
