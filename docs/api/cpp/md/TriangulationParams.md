# `TriangulationParams` (C++)

## Fields

- `strategy` (`Strategy`) — Triangulation algorithm.
- `simplicity` (`Simplicity`) — How to handle non-simple input (self-intersecting polygons).
- `ccw_winding` (`Winding`) — Vertices should be sorted in CCW order.
- `collinearity` (`Collinearity`) — The list of vertices should have no collinear points.
- `conformity` (`AdjacencyConformity`) — How to handle a batch of facets that violate the mesh-conformity rule see the standalone AdjacencyConformity's own docs.
