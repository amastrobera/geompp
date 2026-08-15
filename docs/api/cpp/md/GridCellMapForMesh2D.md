# `GridCellMapForMesh2D` (C++)

Welds a set of triangles' vertices via GridCell2D bucketing into a unique-vertex list plus one index triple per input triangle.

Backs [Mesh2D](Mesh2D.md)::FromTriangles .

## `Make`

**static** `GridCellMapForMesh2D Make(std::vector< `[`Triangle2D`](Triangle2D.md)` > const & triangles)`


**Parameters**

- `triangles` (std::vector< [`Triangle2D`](Triangle2D.md) > const &)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point2D`](Point2D.md)` > > GetUniques() const`

The vector of unique points registered (shared, not copied).

## `GetFaceIndices`

`std::shared_ptr< std::vector< std::array< std::size_t, 3 > > > GetFaceIndices() const`

One vertex-index triple per input triangle, indexing into GetUniques (shared, not copied).


---

**See also:** [Point2D](Point2D.md), [Triangle2D](Triangle2D.md)
