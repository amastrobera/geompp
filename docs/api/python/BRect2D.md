# `BRect2D` (Python)

## `center`

`center() -> `[`Point2D`](Point2D.md)


## `axis_u`

`axis_u() -> `[`Vector2D`](Vector2D.md)


## `axis_v`

`axis_v() -> `[`Vector2D`](Vector2D.md)


## `half_len_u`

`half_len_u() -> float`


## `half_len_v`

`half_len_v() -> float`


## `width`

`width() -> float`


## `height`

`height() -> float`


## `area`

`area() -> float`


## `corners`

`corners() -> std::array< `[`Point2D`](Point2D.md)` , 4 >`


## `almost_equals`

`almost_equals(other: BRect2D, epsilon: float) -> bool`


**Parameters**

- `other` (`BRect2D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside this oriented bounding rectangle (inclusive of the boundary).

Projects the point onto the rectangle's local axes; returns true when both projections are within [-half_len_u, half_len_u] x [-half_len_v, half_len_v].

**Parameters**

- `p` ([`Point2D`](Point2D.md))


---

**See also:** [Point2D](Point2D.md), [Vector2D](Vector2D.md)
