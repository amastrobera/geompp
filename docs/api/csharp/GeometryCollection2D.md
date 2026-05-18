# `GeometryCollection2D` (C# / .NET)

## `AlmostEquals`

`bool AlmostEquals(GeometryCollection2D^ other, double epsilon)`


**Parameters**

- `other` (`GeometryCollection2D^`)
- `epsilon` (`double`)

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`size_t Size()`


## `Add`

`void Add(`[`Point2D`](Point2D.md)`^ point)`


**Parameters**

- `point` ([`Point2D`](Point2D.md)^)

`void Add(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`void Add(`[`LineSegment2D`](LineSegment2D.md)`^ line_segment)`


**Parameters**

- `line_segment` ([`LineSegment2D`](LineSegment2D.md)^)

`void Add(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`void Add(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

`void Add(`[`Triangle2D`](Triangle2D.md)`^ triangle)`


**Parameters**

- `triangle` ([`Triangle2D`](Triangle2D.md)^)

`void Add(`[`Polygon2D`](Polygon2D.md)`^ polygon)`


**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md)^)

`void Add(GeometryCollection2D^ polygon)`


**Parameters**

- `polygon` (`GeometryCollection2D^`)

## `Get`

`Shape2D^ Get(size_t index)`


**Parameters**

- `index` (`size_t`)

## `FromWkt`

**static** `GeometryCollection2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `GeometryCollection2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md)
