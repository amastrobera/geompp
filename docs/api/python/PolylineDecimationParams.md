# `PolylineDecimationParams` (Python)

Bundles the vertex-decimation strategy and its threshold for [Polyline2D](Polyline2D.md)::Reduce() / [Polyline3D](Polyline3D.md)::Reduce().

Defaults to {RamerDouglasPeucker, 0.5} , matching Reduce()'s own defaults, so Reduce() with no arguments and Reduce( PolylineDecimationParams {}) behave identically.

## Fields

- `strategy` (`Strategy`)
- `threshold` (`float`) — Meaning depends on strategy: a distance for RadialDistance, a perpendicular chord-distance for RamerDouglasPeucker, or a triangle area for VisvalingamWhyatt.
