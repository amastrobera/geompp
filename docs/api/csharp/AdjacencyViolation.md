# `AdjacencyViolation` (C# / .NET)

One "more than 1 neighbor" violation of the mesh-conformity rule found by validate_adjacency() across a batch of facets — see AdjacencyConformity's own docs for what the rule means. Covers two distinct cases, distinguishable by `FacetIndices`' count (1 vs. 3+, never 2) if a caller needs to: a T-junction (`FacetIndices` has exactly 1 entry, `OnVertex` is the foreign vertex lying on the edge), or a non-manifold edge (`FacetIndices` lists 3+ facets that all share this exact edge; `OnVertex` is not meaningful there — just a reused edge endpoint, not a real foreign vertex).

PointT [Point2D](Point2D.md) or [Point3D](Point3D.md) . Not [View2D](View2D.md)-projected: unlike triangulation/convexity/winding, "does this vertex lie on this edge" is a well-defined, exact question in native space for either dimension. For a general 3D mesh (facets in many different planes a building's walls and roof, say), flattening through one shared 2D view would actually be WRONG here: it can manufacture a spurious overlap between two edges that don't actually touch in 3D, or miss a genuine touch, depending on the projection angle.

## Fields

- `edge_p0` (`PointT^`)
- `edge_p1` (`PointT^`) — the shared/coarse edge the violation is on.
- `facet_indices` (`std::vector< std::size_t >`) — for a T-junction: always exactly 1 entry, `facet_indices[0]`, the facet owning the coarse edge — the one `fix_adjacency()` splices `on_vertex` into. For a non-manifold edge: every facet (3+, always) that shares this exact edge, exhaustively.
- `on_vertex` (`PointT^`) — only meaningful for a T-junction (exactly 1 entry in `facet_indices`): the foreign vertex lying in the interior of (edge_p0, edge_p1).
