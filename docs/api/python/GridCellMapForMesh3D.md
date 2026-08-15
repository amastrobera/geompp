# `GridCellMapForMesh3D` (Python)

Welds a set of triangles' vertices via GridCell3D bucketing into a unique-vertex list plus one index triple per input triangle.

Backs [Mesh3D](Mesh3D.md)::FromTriangles .

## `make`

**static** `make(triangles: list[`[`Triangle3D`](Triangle3D.md)`]) -> GridCellMapForMesh3D`


**Parameters**

- `triangles` (list[`[Triangle3D`](Triangle3D.md)])

## `get_uniques`

`get_uniques() -> std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > >`

The vector of unique points registered (shared, not copied).

## `get_face_indices`

`get_face_indices() -> std::shared_ptr< std::vector< std::array< std::size_t, 3 > > >`

One vertex-index triple per input triangle, indexing into GetUniques (shared, not copied).


---

**See also:** [Point3D](Point3D.md), [Triangle3D](Triangle3D.md)
