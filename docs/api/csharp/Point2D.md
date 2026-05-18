# `Point2D` (C# / .NET)

## `x`

`double x()`


## `y`

`double y()`


## `ToVector`

[`Vector2D`](Vector2D.md)`^ ToVector()`


## `AlmostEquals`

`bool AlmostEquals(Point2D^ other, double epsilon)`


**Parameters**

- `other` (`Point2D^`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(Point2D^ other)`


**Parameters**

- `other` (`Point2D^`)

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `FromWkt`

**static** `Point2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Point2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Zero`

**static** `Point2D^ Zero()`



---

**See also:** [Vector2D](Vector2D.md)
