# `Point3D` (C++)

## `x`

`double x() const`


## `y`

`double y() const`


## `z`

`double z() const`


## `ToVector`

[`Vector3D`](Vector3D.md)` ToVector() const`


## `AlmostEquals`

`bool AlmostEquals(Point3D const & other, double epsilon) const`


**Parameters**

- `other` (`Point3D const &`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(Point3D const & other) const`


**Parameters**

- `other` (`Point3D const &`)

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `FromWkt`

**static** `Point3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Point3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Zero`

**static** `Point3D Zero()`



---

**See also:** [Vector3D](Vector3D.md)
