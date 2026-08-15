# `GridCellMapForPolyMesh3D` (Python)

Welds a set of polygons' vertices via GridCell3D bucketing into a unique-vertex list plus a flat variable-length face-index buffer (one run of indices per input polygon).

Backs [PolyMesh3D](PolyMesh3D.md)::FromPolygons .

## `make`

**static** `make(polygons: list[`[`Polygon3D`](Polygon3D.md)`]) -> GridCellMapForPolyMesh3D`


**Parameters**

- `polygons` (list[`[Polygon3D`](Polygon3D.md)])

## `get_uniques`

`get_uniques() -> std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > >`

The vector of unique points registered (shared, not copied).

## `get_face_indices`

`get_face_indices() -> std::shared_ptr< std::vector< std::size_t > >`

The flat index buffer: every registered polygon's vertex indices, concatenated (shared, not copied).

## `get_face_idx_begins`

`get_face_idx_begins() -> std::shared_ptr< std::vector< std::size_t > >`

Each polygon's starting offset into GetFaceIndices (shared, not copied).

## `get_face_idx_offsets`

`get_face_idx_offsets() -> std::shared_ptr< std::vector< std::size_t > >`

Each polygon's vertex count, i.e. run length into GetFaceIndices (shared, not copied).


---

**See also:** [Point3D](Point3D.md), [Polygon3D](Polygon3D.md)
