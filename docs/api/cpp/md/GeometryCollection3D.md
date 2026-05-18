# `GeometryCollection3D` (C++)

## `AlmostEquals`

`bool AlmostEquals(GeometryCollection3D const & other, double epsilon) const`


**Parameters**

- `other` (`GeometryCollection3D const &`)
- `epsilon` (`double`)

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Size`

`std::size_t Size() const`


## `Add`

`void Add(`[`Point3D`](Point3D.md)` const & point)`


**Parameters**

- `point` ([`Point3D`](Point3D.md) const &)

`void Add(`[`Line3D`](Line3D.md)` const & line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`void Add(`[`LineSegment3D`](LineSegment3D.md)` const & line_segment)`


**Parameters**

- `line_segment` ([`LineSegment3D`](LineSegment3D.md) const &)

`void Add(`[`Ray3D`](Ray3D.md)` const & ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`void Add(`[`Polyline3D`](Polyline3D.md)` const & polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

`void Add(`[`Triangle3D`](Triangle3D.md)` const & triangle)`


**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md) const &)

`void Add(`[`Polygon3D`](Polygon3D.md)` const & polygon)`


**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md) const &)

`void Add(GeometryCollection3D const & polygon)`


**Parameters**

- `polygon` (`GeometryCollection3D const &`)

## `Get`

`Shape3D Get(std::size_t index) const`


**Parameters**

- `index` (`std::size_t`)

## `FromWkt`

**static** `GeometryCollection3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `GeometryCollection3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md)
