# `FaceView2D` (C# / .NET)

## `ID`

`size_t ID()`

ID of the facet, corresponding to the location in the array TRIANGLES of the ConnectedMesh.

## `Geometry`

[`Triangle2D`](Triangle2D.md)`^ Geometry()`

facet geometry, rebuilt as a [Triangle2D](Triangle2D.md) from the mesh's welded vertices.

## `Neighbor`

`FaceView2D^  (nullable) Neighbor(TriangleCompactNeighborRef::TriangleEdge^ edge_id)`

Topology navigation across one of this facet's 3 edges, returning another FaceView2D so calls chain as face.Neighbor(edge)-> Geometry() .

**Parameters**

- `edge_id` (`TriangleCompactNeighborRef::TriangleEdge^`) — FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.

**Returns** — The adjacent facet, or std::nullopt if edge_id is a boundary edge (no twin).

## `NeighborEntryEdge`

`TriangleCompactNeighborRef::TriangleEdge^ NeighborEntryEdge(TriangleCompactNeighborRef::TriangleEdge^ edge_id)`

Which edge (FIRST/SECOND/THIRD) of the Neighbor() facet we entered into, i.e.

the local edge id of the twin edge on the other side of edge_id .

**Parameters**

- `edge_id` (`TriangleCompactNeighborRef::TriangleEdge^`) — FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.

**Returns** — TriangleEdge::INVALID if edge_id is a boundary edge (no twin).


---

**See also:** [Triangle2D](Triangle2D.md)
