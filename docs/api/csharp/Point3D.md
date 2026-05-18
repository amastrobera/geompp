# `Point3D` (C# / .NET)

## `x`

`double x()`


## `y`

`double y()`


## `z`

`double z()`


## `ToVector`

[`Vector3D`](Vector3D.md)`^ ToVector()`


## `AlmostEquals`

`bool AlmostEquals(Point3D^ other, double epsilon)`


**Parameters**

- `other` (`Point3D^`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(Point3D^ other)`


**Parameters**

- `other` (`Point3D^`)

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `FromWkt`

**static** `Point3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Point3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Zero`

**static** `Point3D^ Zero()`



---

**See also:** [Vector3D](Vector3D.md)
