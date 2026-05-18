# `Vector3D` (C++)

## `x`

`double const x() const`


## `y`

`double const y() const`


## `z`

`double const z() const`


## `ToPoint`

[`Point3D`](Point3D.md)` ToPoint() const`


## `Length`

`double Length() const`


## `DominantAxis`

`Axis DominantAxis() const`


## `AlmostEquals`

`bool AlmostEquals(Vector3D const & other, double epsilon) const`


**Parameters**

- `other` (`Vector3D const &`)
- `epsilon` (`double`)

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Dot`

`double Dot(Vector3D const & other) const`


**Parameters**

- `other` (`Vector3D const &`)

## `Cross`

`Vector3D Cross(Vector3D const & other) const`


**Parameters**

- `other` (`Vector3D const &`)

## `Perp`

`Vector3D Perp() const`


## `Normalize`

`Vector3D Normalize() const`


## `IsParallel`

`bool IsParallel(Vector3D const & other) const`


**Parameters**

- `other` (`Vector3D const &`)

## `FromWkt`

**static** `Vector3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Vector3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `BasisX`

**static** `Vector3D BasisX()`


## `BasisY`

**static** `Vector3D BasisY()`


## `BasisZ`

**static** `Vector3D BasisZ()`



---

**See also:** [Point3D](Point3D.md)
