# `PolylineExpansionParams` (C++)

Controls [Polyline2D](Polyline2D.md)::Expand() / [Polyline3D](Polyline3D.md)::Expand() — rounds every inner corner of a polyline with a quadratic Bezier arc (see bezier_smoothing_2, which each corner is delegated to).

## Fields

- `smoothness` (`double`) — In [0, 1]: fraction of a corner's shorter adjacent edge to trim into the arc's tangent points.
- `mode` (`Mode`)
- `segments_per_corner` (`int`) — Exact number of samples per corner arc. Used only when mode is FixedSegments .
- `min_distance` (`double`) — Target spacing between consecutive samples along a corner arc.
- `min_segment_length` (`double`) — Forwarded to bezier_smoothing_2's own min_segment_length : a corner's adjacent edge at or below this length isn't trimmed into (falls back to the sharp corner point on that side); if both adjacent edges are that short, the whole corner is skipped (stays sharp).
