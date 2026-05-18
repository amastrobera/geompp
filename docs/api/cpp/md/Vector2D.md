# `Vector2D` (C++)

## `x`

`double const x() const`


## `y`

`double const y() const`


## `ToPoint`

[`Point2D`](Point2D.md)` ToPoint() const`


## `Length`

`double Length() const`


## `AlmostEquals`

`bool AlmostEquals(Vector2D const & other, double epsilon) const`


**Parameters**

- `other` (`Vector2D const &`)
- `epsilon` (`double`)

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Dot`

`double Dot(Vector2D const & v) const`


**Parameters**

- `v` (`Vector2D const &`)

## `Cross`

`double Cross(Vector2D const & v) const`


**Parameters**

- `v` (`Vector2D const &`)

## `Perp`

`Vector2D Perp() const`


## `Normalize`

`Vector2D Normalize() const`


## `FromWkt`

**static** `Vector2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Vector2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `BasisX`

**static** `Vector2D BasisX()`


## `BasisY`

**static** `Vector2D BasisY()`



---

**See also:** [Point2D](Point2D.md)
