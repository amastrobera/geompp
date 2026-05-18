# `Vector3D` (C# / .NET)

## `x`

`double x()`


## `y`

`double y()`


## `z`

`double z()`


## `ToPoint`

[`Point3D`](Point3D.md)`^ ToPoint()`


## `Length`

`double Length()`


## `DominantAxis`

`Axis^ DominantAxis()`


## `AlmostEquals`

`bool AlmostEquals(Vector3D^ other, double epsilon)`


**Parameters**

- `other` (`Vector3D^`)
- `epsilon` (`double`)

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Dot`

`double Dot(Vector3D^ other)`


**Parameters**

- `other` (`Vector3D^`)

## `Cross`

`Vector3D^ Cross(Vector3D^ other)`


**Parameters**

- `other` (`Vector3D^`)

## `Perp`

`Vector3D^ Perp()`


## `Normalize`

`Vector3D^ Normalize()`


## `IsParallel`

`bool IsParallel(Vector3D^ other)`


**Parameters**

- `other` (`Vector3D^`)

## `FromWkt`

**static** `Vector3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Vector3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `BasisX`

**static** `Vector3D^ BasisX()`


## `BasisY`

**static** `Vector3D^ BasisY()`


## `BasisZ`

**static** `Vector3D^ BasisZ()`



---

**See also:** [Point3D](Point3D.md)
