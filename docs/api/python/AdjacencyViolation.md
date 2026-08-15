# `AdjacencyViolation` (Python)

One "more than 1 neighbor" violation of the mesh-conformity rule found by validate_adjacency() across a batch of facets — see [TriangulationParams](TriangulationParams.md)::AdjacencyConformity 's own docs for what the rule means and why a T-junction is fixable but a non-manifold edge isn't.

PointT [Point2D](Point2D.md) or [Point3D](Point3D.md) . Not [View2D](View2D.md)-projected: unlike triangulation/convexity/winding, "does this vertex lie on this edge" is a well-defined, exact question in native space for either dimension. For a general 3D mesh (facets in many different planes a building's walls and roof, say), flattening through one shared 2D view would actually be WRONG here: it can manufacture a spurious overlap between two edges that don't actually touch in 3D, or miss a genuine touch, depending on the projection angle.

## Fields

- `edge_p0` (`PointT`)
- `edge_p1` (`PointT`) — the shared/coarse edge the violation is on.
- `facet_indices` (`list[int]`) — every facet (index into the input) touching this edge.
- `is_non_manifold` (`bool`) — true: a full edge shared by 3+ facets, not fixable.
- `on_vertex` (`PointT`) — meaningful only when !is_non_manifold: the foreign vertex lying in the interior of (edge_p0, edge_p1).
