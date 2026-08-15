# `GridCellMapForConnectedMesh2D` (C# / .NET)

## `Make`

**static** `GridCellMapForConnectedMesh2D^ Make(std::vector< `[`Triangle2D`](Triangle2D.md)` > triangles)`


**Parameters**

- `triangles` (std::vector< [`Triangle2D`](Triangle2D.md) >)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point2D`](Point2D.md)` > > GetUniques()`

Moves out the vector of unique points registered.

## `GetTriangles`

`std::shared_ptr< std::vector< std::size_t > > GetTriangles()`


## `GetNeighborRefs`

`std::shared_ptr< std::vector< std::array< detail::TriangleCompactNeighborRef , 3 > > > GetNeighborRefs()`



---

**See also:** [Point2D](Point2D.md), [Triangle2D](Triangle2D.md)
