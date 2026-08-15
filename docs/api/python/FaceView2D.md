# `FaceView2D` (Python)

## `id`

`id() -> int`

ID of the facet, corresponding to the location in the array TRIANGLES of the ConnectedMesh.

## `geometry`

`geometry() -> `[`Triangle2D`](Triangle2D.md)

facet geometry, rebuilt as a [Triangle2D](Triangle2D.md) from the mesh's welded vertices.

## `neighbor`

`neighbor(edge_id: detail::TriangleCompactNeighborRef::TriangleEdge) -> FaceView2D | None`

Topology navigation across one of this facet's 3 edges, returning another FaceView2D so calls chain as face.Neighbor(edge)-> Geometry() .

**Parameters**

- `edge_id` (`detail::TriangleCompactNeighborRef::TriangleEdge`) — FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.

**Returns** — The adjacent facet, or std::nullopt if edge_id is a boundary edge (no twin).

## `neighbor_entry_edge`

`neighbor_entry_edge(edge_id: detail::TriangleCompactNeighborRef::TriangleEdge) -> detail::TriangleCompactNeighborRef::TriangleEdge`

Which edge (FIRST/SECOND/THIRD) of the Neighbor() facet we entered into, i.e.

the local edge id of the twin edge on the other side of edge_id .

**Parameters**

- `edge_id` (`detail::TriangleCompactNeighborRef::TriangleEdge`) — FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.

**Returns** — TriangleEdge::INVALID if edge_id is a boundary edge (no twin).


---

**See also:** [Triangle2D](Triangle2D.md)
