# `GeometryCollection2D` (Python)

## `almost_equals`

`almost_equals(other: GeometryCollection2D, epsilon: float) -> bool`


**Parameters**

- `other` (`GeometryCollection2D`)
- `epsilon` (`float`)

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `add`

`add(point: `[`Point2D`](Point2D.md)`) -> None`


**Parameters**

- `point` ([`Point2D`](Point2D.md))

`add(line: `[`Line2D`](Line2D.md)`) -> None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`add(line_segment: `[`LineSegment2D`](LineSegment2D.md)`) -> None`


**Parameters**

- `line_segment` ([`LineSegment2D`](LineSegment2D.md))

`add(ray: `[`Ray2D`](Ray2D.md)`) -> None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`add(polyline: `[`Polyline2D`](Polyline2D.md)`) -> None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

`add(triangle: `[`Triangle2D`](Triangle2D.md)`) -> None`


**Parameters**

- `triangle` ([`Triangle2D`](Triangle2D.md))

`add(polygon: `[`Polygon2D`](Polygon2D.md)`) -> None`


**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md))

`add(polygon: GeometryCollection2D) -> None`


**Parameters**

- `polygon` (`GeometryCollection2D`)

## `get`

`get(index: int) -> Shape2D`


**Parameters**

- `index` (`int`)

## `from_wkt`

**static** `from_wkt(wkt: str) -> GeometryCollection2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> GeometryCollection2D`


**Parameters**

- `path` (`str`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md)
