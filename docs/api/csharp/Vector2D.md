# `Vector2D` (C# / .NET)

## `x`

`double x()`


## `y`

`double y()`


## `ToPoint`

[`Point2D`](Point2D.md)`^ ToPoint()`


## `Length`

`double Length()`


## `AlmostEquals`

`bool AlmostEquals(Vector2D^ other, double epsilon)`


**Parameters**

- `other` (`Vector2D^`)
- `epsilon` (`double`)

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Dot`

`double Dot(Vector2D^ v)`


**Parameters**

- `v` (`Vector2D^`)

## `Cross`

`double Cross(Vector2D^ v)`


**Parameters**

- `v` (`Vector2D^`)

## `Perp`

`Vector2D^ Perp()`


## `Normalize`

`Vector2D^ Normalize()`


## `IsParallel`

`bool IsParallel(Vector2D^ other)`


**Parameters**

- `other` (`Vector2D^`)

## `FromWkt`

**static** `Vector2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Vector2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `BasisX`

**static** `Vector2D^ BasisX()`


## `BasisY`

**static** `Vector2D^ BasisY()`



---

**See also:** [Point2D](Point2D.md)
