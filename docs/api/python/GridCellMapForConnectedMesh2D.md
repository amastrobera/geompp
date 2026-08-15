# `GridCellMapForConnectedMesh2D` (Python)

## `make`

**static** `make(triangles: list[`[`Triangle2D`](Triangle2D.md)`]) -> GridCellMapForConnectedMesh2D`


**Parameters**

- `triangles` (list[`[Triangle2D`](Triangle2D.md)])

## `get_uniques`

`get_uniques() -> std::shared_ptr< std::vector< `[`Point2D`](Point2D.md)` > >`

Moves out the vector of unique points registered.

## `get_triangles`

`get_triangles() -> std::shared_ptr< std::vector< std::size_t > >`


## `get_neighbor_refs`

`get_neighbor_refs() -> std::shared_ptr< std::vector< std::array< detail::TriangleCompactNeighborRef , 3 > > >`



---

**See also:** [Point2D](Point2D.md), [Triangle2D](Triangle2D.md)
