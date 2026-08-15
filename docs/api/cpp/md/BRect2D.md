# `BRect2D` (C++)

## `center`

[`Point2D`](Point2D.md)` center() const`


## `axis_u`

[`Vector2D`](Vector2D.md)` axis_u() const`


## `axis_v`

[`Vector2D`](Vector2D.md)` axis_v() const`


## `half_len_u`

`double half_len_u() const`


## `half_len_v`

`double half_len_v() const`


## `width`

`double width() const`


## `height`

`double height() const`


## `area`

`double area() const`


## `Corners`

`std::array< `[`Point2D`](Point2D.md)` , 4 > Corners() const`


## `AlmostEquals`

`bool AlmostEquals(BRect2D const & other, double epsilon) const`


**Parameters**

- `other` (`BRect2D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & p) const`

Tests whether a point lies inside this oriented bounding rectangle (inclusive of the boundary).

Projects the point onto the rectangle's local axes; returns true when both projections are within [-half_len_u, half_len_u] x [-half_len_v, half_len_v].

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)


---

**See also:** [Point2D](Point2D.md), [Vector2D](Vector2D.md)
