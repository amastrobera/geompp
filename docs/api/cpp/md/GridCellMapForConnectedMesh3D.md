# `GridCellMapForConnectedMesh3D` (C++)

## `Make`

**static** `GridCellMapForConnectedMesh3D Make(std::vector< `[`Triangle3D`](Triangle3D.md)` > const & triangles)`


**Parameters**

- `triangles` (std::vector< [`Triangle3D`](Triangle3D.md) > const &)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point3D`](Point3D.md)` > > GetUniques()`

Moves out the vector of unique points registered.

## `GetTriangles`

`std::shared_ptr< std::vector< std::size_t > > GetTriangles()`


## `GetNeighborRefs`

`std::shared_ptr< std::vector< std::array< detail::TriangleCompactNeighborRef , 3 > > > GetNeighborRefs()`



---

**See also:** [Point3D](Point3D.md), [Triangle3D](Triangle3D.md)
