# `GridCellMapForMesh3D` (C# / .NET)

Welds a set of triangles' vertices via GridCell3D bucketing into a unique-vertex list plus one index triple per input triangle.

Backs [Mesh3D](Mesh3D.md)::FromTriangles .

## `Make`

**static** `GridCellMapForMesh3D^ Make(std::vector< `[`Triangle3D`](Triangle3D.md)` > triangles)`


**Parameters**

- `triangles` (std::vector< [`Triangle3D`](Triangle3D.md) >)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > > GetUniques()`

The vector of unique points registered (shared, not copied).

## `GetFaceIndices`

`std::shared_ptr< std::vector< std::array< std::size_t, 3 > > > GetFaceIndices()`

One vertex-index triple per input triangle, indexing into GetUniques (shared, not copied).


---

**See also:** [Point3D](Point3D.md), [Triangle3D](Triangle3D.md)
