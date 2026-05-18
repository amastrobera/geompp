# `GeometryCollection2D` (C++)

## `AlmostEquals`

`bool AlmostEquals(GeometryCollection2D const & other, double epsilon) const`


**Parameters**

- `other` (`GeometryCollection2D const &`)
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

`void Add(`[`Point2D`](Point2D.md)` const & point)`


**Parameters**

- `point` ([`Point2D`](Point2D.md) const &)

`void Add(`[`Line2D`](Line2D.md)` const & line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`void Add(`[`LineSegment2D`](LineSegment2D.md)` const & line_segment)`


**Parameters**

- `line_segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`void Add(`[`Ray2D`](Ray2D.md)` const & ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`void Add(`[`Polyline2D`](Polyline2D.md)` const & polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

`void Add(`[`Triangle2D`](Triangle2D.md)` const & triangle)`


**Parameters**

- `triangle` ([`Triangle2D`](Triangle2D.md) const &)

`void Add(`[`Polygon2D`](Polygon2D.md)` const & polygon)`


**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md) const &)

`void Add(GeometryCollection2D const & polygon)`


**Parameters**

- `polygon` (`GeometryCollection2D const &`)

## `Get`

`Shape2D Get(std::size_t index) const`


**Parameters**

- `index` (`std::size_t`)

## `FromWkt`

**static** `GeometryCollection2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `GeometryCollection2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md)
