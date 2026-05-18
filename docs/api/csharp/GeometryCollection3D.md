# `GeometryCollection3D` (C# / .NET)

## `AlmostEquals`

`bool AlmostEquals(GeometryCollection3D^ other, double epsilon)`


**Parameters**

- `other` (`GeometryCollection3D^`)
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

`void Add(`[`Point3D`](Point3D.md)`^ point)`


**Parameters**

- `point` ([`Point3D`](Point3D.md)^)

`void Add(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`void Add(`[`LineSegment3D`](LineSegment3D.md)`^ line_segment)`


**Parameters**

- `line_segment` ([`LineSegment3D`](LineSegment3D.md)^)

`void Add(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`void Add(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

`void Add(`[`Triangle3D`](Triangle3D.md)`^ triangle)`


**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md)^)

`void Add(`[`Polygon3D`](Polygon3D.md)`^ polygon)`


**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md)^)

`void Add(GeometryCollection3D^ polygon)`


**Parameters**

- `polygon` (`GeometryCollection3D^`)

## `Get`

`Shape3D^ Get(size_t index)`


**Parameters**

- `index` (`size_t`)

## `FromWkt`

**static** `GeometryCollection3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `GeometryCollection3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md)
