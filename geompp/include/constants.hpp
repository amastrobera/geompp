#pragma once

#include <cmath>
#include <thread>

namespace geompp {

#pragma region Precision

const int DP_THREE = 3;
const int DP_SIX = 6;
const int DP_NINE = 9;

#ifdef GEOMPP_NO_THREAD_LOCAL
extern int DECIMAL_PRECISION;
#else
extern thread_local int DECIMAL_PRECISION;
#endif

struct DynamicEpsilon {
  operator double() const { return pow(10.0, -DECIMAL_PRECISION); }
};

#ifdef GEOMPP_NO_THREAD_LOCAL
extern DynamicEpsilon DOUBLE_EPSILON;
#else
extern thread_local DynamicEpsilon DOUBLE_EPSILON;
#endif

/// @brief Scoped, exception-safe override of the thread-local @ref DECIMAL_PRECISION.
///
/// `DECIMAL_PRECISION` is a thread-global comparison tolerance every `AlmostEquals`-based operation
/// reads. Changing it for a single call (e.g. to loosen a coplanarity/overlap check for one noisy
/// dataset) otherwise means "mutate the global, remember to restore it" — brittle, and leaks the change
/// to unrelated comparisons if an exception unwinds past the restore. This RAII guard saves the current
/// value on construction and restores it on destruction, so a loosened precision is confined to a block:
///
/// @code
///   {
///     geompp::ScopedPrecision sp(1);   // epsilon = 0.1 within this scope only
///     auto merged = a.Union(b);        // coplanarity/classification use the looser tolerance
///   }                                  // previous precision restored here, even if Union threw
/// @endcode
///
/// Only affects the calling thread (matching `DECIMAL_PRECISION`'s thread-local storage). Nesting is
/// fine — each guard restores the value that was current when it was constructed.
struct ScopedPrecision {
  int saved;
  explicit ScopedPrecision(int precision) : saved(DECIMAL_PRECISION) { DECIMAL_PRECISION = precision; }
  ~ScopedPrecision() { DECIMAL_PRECISION = saved; }

  ScopedPrecision(ScopedPrecision const&) = delete;
  ScopedPrecision& operator=(ScopedPrecision const&) = delete;
};

#pragma endregion

enum class GeometryType {
  Point,
  Line,
  Ray,
  LineSegment,
  Polyline,
  Triangle,
  Polygon,
  GeometryCollection
};  // update as needed

/// @brief Bundles the vertex-decimation strategy and its threshold for Polyline2D::Reduce() / Polyline3D::Reduce().
/// Defaults to `{RamerDouglasPeucker, 0.5}`, matching Reduce()'s own defaults, so `Reduce()` with no
/// arguments and `Reduce(PolylineDecimationParams{})` behave identically.
struct PolylineDecimationParams {
  /// @brief Vertex-reduction algorithm. All three take the same @ref threshold meaning "how far/large is
  /// still considered noise", but interpret it differently (a plain distance for RadialDistance, a
  /// perpendicular chord-distance for RamerDouglasPeucker, a triangle area for VisvalingamWhyatt) — so the
  /// same threshold value produces different results across strategies.
  enum class Strategy {
    /// O(N) brute-force pass: drops a vertex if it's closer than `threshold` to the last *kept* vertex.
    /// Cheapest and least accurate — good as a fast noise-clustering pre-pass (see @ref threshold), not as
    /// the sole strategy when shape fidelity matters.
    RadialDistance,
    /// O(N log N) to O(N²): recursively drops vertices closer than `threshold` to the chord spanning
    /// their segment. Given points P1, P2, P3, drops P2 when its perpendicular distance from the P1-P3
    /// chord is below `threshold`; otherwise keeps P2 and recurses on both halves. Best general-purpose
    /// choice — preserves the vertices that most define the polyline's shape.
    RamerDouglasPeucker,
    /// O(N log N) to O(N²): repeatedly drops the vertex forming the smallest-area triangle with its two
    /// neighbors, while that area stays below `threshold`, then re-evaluates the neighbors. Tends to
    /// preserve visually significant features (sharp spikes) better than RadialDistance while being
    /// similarly simple to reason about.
    VisvalingamWhyatt
  };
  Strategy strategy = Strategy::RamerDouglasPeucker;

  /// @brief Meaning depends on @ref strategy: a distance for RadialDistance, a perpendicular chord-distance
  /// for RamerDouglasPeucker, or a triangle area for VisvalingamWhyatt. Larger values remove more vertices.
  double threshold = 0.5;
};

/// @brief Controls Polyline2D::Expand() / Polyline3D::Expand() — rounds every inner corner of a polyline
/// with a quadratic Bezier arc (see bezier_smoothing_2, which each corner is delegated to).
struct PolylineExpansionParams {
  /// @brief In [0, 1]: fraction of a corner's shorter adjacent edge to trim into the arc's tangent
  /// points. 0 leaves every corner sharp (no rounding); 1 trims half of the shorter edge.
  double smoothness = 0.5;  // 0.0 (sharp) to 1.0 (max round)

  /// @brief How densely each corner's arc is sampled — see @ref segments_per_corner / @ref min_distance.
  enum class Mode { FixedSegments, MinDistance };
  Mode mode = Mode::FixedSegments;

  /// @brief Exact number of samples per corner arc. Used only when @ref mode is `FixedSegments`.
  int segments_per_corner = 4;
  /// @brief Target spacing between consecutive samples along a corner arc. Used only when @ref mode is
  /// `MinDistance`.
  double min_distance = 0.1;

  /// @brief Forwarded to bezier_smoothing_2's own `min_segment_length`: a corner's adjacent edge at or
  /// below this length isn't trimmed into (falls back to the sharp corner point on that side); if
  /// *both* adjacent edges are that short, the whole corner is skipped (stays sharp). Note
  /// `Polyline::Make()` already rejects any edge shorter than `DECIMAL_PRECISION`'s epsilon at
  /// construction time — this is a separate, caller-chosen threshold for "don't bother rounding a
  /// technically-valid but visually tiny/noisy corner", not a degeneracy guard.
  double min_segment_length = DOUBLE_EPSILON;
};

struct TriangulationParams {
  enum class Strategy { EarClipping, MonotonePolygon, Delaunay };
  /// @brief Triangulation algorithm.
  ///        - EarClipping is the most robust and general-purpose, but slower than the others. O(n²) worst-case.
  ///        - MonotonePolygon (which requires a monotone polygon). O(n log n) to O(n²) worst-case
  ///        - Delaunay (which requires a point set and produces a triangulation of the convex hull, not a polygon).
  ///          O(n log n) to O(n²) worst-case.
  Strategy strategy = Strategy::EarClipping;

  enum class Simplicity { Guaranteed, Assert, Enforce };
  /// @brief How to handle non-simple input (self-intersecting polygons).
  ///        - Guaranteed no check is carried out (the algo runs at your own risk);
  ///        - Assert throws if the input isn't simple;
  ///        - Enforce attempts to fix it (via simplify_rings) before triangulating.
  Simplicity simplicity = Simplicity::Enforce;

  enum class Winding { Guaranteed, Assert, Enforce };
  /// @brief Vertices should be sorted in CCW order
  ///        - Guaranteed no check is carried out (the algo runs at your own risk);
  ///        - Assert throws if the input isn't CCW;
  ///        - Enforce attempts to fix it (reversing the vertex order) before triangulating.
  Winding ccw_winding = Winding::Enforce;

  enum class Collinearity { Guaranteed, Assert, Enforce };
  /// @brief The list of vertices should have no collinear points.
  ///        A duplicate vertex is just the degenerate case of three collinear points (two of them
  ///        coinciding), so a single collinearity check covers both — no separate duplicates check needed.
  ///        - Guaranteed no check is carried out (the algo runs at your own risk);
  ///        - Assert throws if the input has collinear (or duplicate) points;
  ///        - Enforce attempts to fix it (using remove_collinear) before triangulating.
  Collinearity collinearity = Collinearity::Enforce;
};

}  // namespace geompp
