# `GeometryCollection3D` (Python)

## `almost_equals`

`almost_equals(other: GeometryCollection3D, epsilon: float) -> bool`


**Parameters**

- `other` (`GeometryCollection3D`)
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

`add(point: `[`Point3D`](Point3D.md)`) -> None`


**Parameters**

- `point` ([`Point3D`](Point3D.md))

`add(line: `[`Line3D`](Line3D.md)`) -> None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`add(line_segment: `[`LineSegment3D`](LineSegment3D.md)`) -> None`


**Parameters**

- `line_segment` ([`LineSegment3D`](LineSegment3D.md))

`add(ray: `[`Ray3D`](Ray3D.md)`) -> None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`add(polyline: `[`Polyline3D`](Polyline3D.md)`) -> None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

`add(triangle: `[`Triangle3D`](Triangle3D.md)`) -> None`


**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md))

`add(polygon: `[`Polygon3D`](Polygon3D.md)`) -> None`


**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md))

`add(polygon: GeometryCollection3D) -> None`


**Parameters**

- `polygon` (`GeometryCollection3D`)

## `get`

`get(index: int) -> Shape3D`


**Parameters**

- `index` (`int`)

## `from_wkt`

**static** `from_wkt(wkt: str) -> GeometryCollection3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> GeometryCollection3D`


**Parameters**

- `path` (`str`)


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md)
