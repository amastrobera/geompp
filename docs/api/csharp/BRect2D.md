# `BRect2D` (C# / .NET)

## `center`

[`Point2D`](Point2D.md)`^ center()`


## `axis_u`

[`Vector2D`](Vector2D.md)`^ axis_u()`


## `axis_v`

[`Vector2D`](Vector2D.md)`^ axis_v()`


## `half_len_u`

`double half_len_u()`


## `half_len_v`

`double half_len_v()`


## `width`

`double width()`


## `height`

`double height()`


## `area`

`double area()`


## `Corners`

`std::array< `[`Point2D`](Point2D.md)` , 4 > Corners()`


## `AlmostEquals`

`bool AlmostEquals(BRect2D^ other, double epsilon)`


**Parameters**

- `other` (`BRect2D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ p)`

Tests whether a point lies inside this oriented bounding rectangle (inclusive of the boundary).

Projects the point onto the rectangle's local axes; returns true when both projections are within [-half_len_u, half_len_u] x [-half_len_v, half_len_v].

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)


---

**See also:** [Point2D](Point2D.md), [Vector2D](Vector2D.md)
