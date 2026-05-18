# `Point2D` (C++)

## `x`

`double x() const`


## `y`

`double y() const`


## `ToVector`

[`Vector2D`](Vector2D.md)` ToVector() const`


## `AlmostEquals`

`bool AlmostEquals(Point2D const & other, double epsilon) const`


**Parameters**

- `other` (`Point2D const &`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(Point2D const & other) const`


**Parameters**

- `other` (`Point2D const &`)

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `FromWkt`

**static** `Point2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Point2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Zero`

**static** `Point2D Zero()`



---

**See also:** [Vector2D](Vector2D.md)
