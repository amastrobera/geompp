# `GridCellMapForPolyMesh3D` (C# / .NET)

Welds a set of polygons' vertices via GridCell3D bucketing into a unique-vertex list plus a flat variable-length face-index buffer (one run of indices per input polygon).

Backs [PolyMesh3D](PolyMesh3D.md)::FromPolygons .

## `Make`

**static** `GridCellMapForPolyMesh3D^ Make(std::vector< `[`Polygon3D`](Polygon3D.md)` > polygons)`


**Parameters**

- `polygons` (std::vector< [`Polygon3D`](Polygon3D.md) >)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > > GetUniques()`

The vector of unique points registered (shared, not copied).

## `GetFaceIndices`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIndices()`

The flat index buffer: every registered polygon's vertex indices, concatenated (shared, not copied).

## `GetFaceIdxBegins`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIdxBegins()`

Each polygon's starting offset into GetFaceIndices (shared, not copied).

## `GetFaceIdxOffsets`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIdxOffsets()`

Each polygon's vertex count, i.e. run length into GetFaceIndices (shared, not copied).


---

**See also:** [Point3D](Point3D.md), [Polygon3D](Polygon3D.md)
