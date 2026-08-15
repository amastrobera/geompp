# `GridCellMapForConnectedMesh3D` (Python)

## `make`

**static** `make(triangles: list[`[`Triangle3D`](Triangle3D.md)`]) -> GridCellMapForConnectedMesh3D`


**Parameters**

- `triangles` (list[`[Triangle3D`](Triangle3D.md)])

## `get_uniques`

`get_uniques() -> std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > >`

Moves out the vector of unique points registered.

## `get_triangles`

`get_triangles() -> std::shared_ptr< std::vector< std::size_t > >`


## `get_neighbor_refs`

`get_neighbor_refs() -> std::shared_ptr< std::vector< std::array< detail::TriangleCompactNeighborRef , 3 > > >`



---

**See also:** [Point3D](Point3D.md), [Triangle3D](Triangle3D.md)
