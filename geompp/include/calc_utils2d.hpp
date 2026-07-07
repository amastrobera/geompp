#pragma once

#include "concepts.hpp"
#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "view2d.hpp"

#include <compare>
#include <optional>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

namespace geompp {

// Forward declarations for types only needed by reference in EventQueue2D constructors.
// Placed here (not inside detail) so name lookup from within detail finds geompp::Polygon2D
// and geompp::SegmentRange2D, not shadow types.
class Polygon2D;
class SegmentRange2D;
class Line2D;

/// @brief The pair of points on a shape that are extreme (least / greatest) along a given direction.
/// @tparam PointT Point2D or Point3D.
template <typename PointT>
struct ExtremePoints {
  PointT min_point;
  PointT max_point;
};

namespace detail {

// Note: Point2D is NOT declared here. Unqualified lookup walks up to namespace geompp
// and finds geompp::Point2D (which is fully defined via "point2d.hpp" above).
// Do NOT add 'class Point2D;' or 'using Point2D = ...' here — MSVC mangles alias
// names differently from the canonical type in explicit template instantiations.

/// @brief Indices of the two vertices extreme (least / greatest projection) along a direction.
/// @tparam V a vector type supporting Dot (Vector2D / Vector3D).
/// @tparam R a forward range of points, each supporting ToVector().Dot(V) (e.g. std::vector<Point2D/3D>).
/// @param vertices ordered polygon vertices (CCW). For the convex fast-path they must form a convex ring.
/// @param is_convex when true, uses Daniel Sunday's O(log n) binary search; otherwise an O(n) linear scan.
/// @param dir direction to measure extremeness along (e.g. a line's direction).
/// @returns {min_index, max_index} — indices into @p vertices of the least- and greatest-projected points.
/// @throws std::invalid_argument if @p vertices is empty.
///
/// The convex binary search only ever compares scalar projections proj(i) = vertices[i].ToVector().Dot(dir),
/// so it is dimension-agnostic: the same code drives Point2D and Point3D rings. Defined in calc_utils2d.cpp;
/// only the two instantiations below are available (extern template suppresses implicit instantiation
/// elsewhere, matching convex_hull_monotone_chain / is_convex_with_view in this same header).
template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points_impl(R const& vertices, bool is_convex, V const& dir);

extern template std::pair<std::size_t, std::size_t> extreme_points_impl(std::vector<Point2D> const&, bool,
                                                                        Vector2D const&);
extern template std::pair<std::size_t, std::size_t> extreme_points_impl(std::vector<Point3D> const&, bool,
                                                                        Vector3D const&);

std::partial_ordering compare_event_point(Point2D a, Point2D b);  // for ordering events in the sweep line algorithm

enum class EventType2D {
  UNKN = -1,
  LEFT,
  INTERSECTION,
  RIGHT
};  // it's critical to respect the LEFT < INTERSECTION < RIGHT order for the event queue processing

struct Event2D {
  EventType2D Type;
  Point2D Point;
  std::size_t SegmentId;  // reference to the segment that generated the event (if EventType::LEFT or EventType::RIGHT)
  std::optional<std::size_t> InterSegmentId;  // if EventType::INTERSECTION this is the second segment involved in the
                                              // intersection, otherwise std::nullopt

  bool operator<(Event2D const& other) const;   // for the priority queue in EventQueue2D
  bool operator>(Event2D const& other) const;   // required by std::greater<Event2D> (EventMinHeap)
  bool operator==(Event2D const& other) const;  // for EventQueue2D::Contains
};

// EventQueue2D never needs the segments themselves: it builds the events once, then only ever manipulates
// integer SegmentIds. So it stays a plain (non-template) class with the three convenience constructors.
class EventQueue2D {
 public:
  EventQueue2D(std::vector<LineSegment2D> const& segments);
  EventQueue2D(SegmentRange2D const& segments);  // lazy view over a point sequence (polyline / polygon boundary)
  EventQueue2D(Polygon2D const& polygon);        // ideal case to check for self-intersections of a polygon
  ~EventQueue2D() = default;

  std::optional<Event2D> Top() const;
  std::optional<Event2D> Pop();
  bool Empty() const;
  bool Contains(Event2D const& event) const;  // true if the event queue contains an event for the given segment index
  void Push(Event2D const& event);            // simply adds the event and does not check if it is present already

 private:
  struct EventMinHeap : std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>> {
    using std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>>::priority_queue;
    using std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>>::c;
  };
  EventMinHeap EVENTS;
};

template <SegmentList Segments>
struct SweepLineComparator {
  const double& sweep_x;     // Direct reference to the SweepLine's master variable
  Segments const* segments;  // bound from `Segments const&`, so the pointee is const

  bool operator()(std::size_t id1, std::size_t id2) const;

 private:
  double GetYAtX(LineSegment2D const& seg, double x) const;
};

// SweepLine2D is the one place that resolves event.SegmentId back to a full LineSegment2D, so it is templated
// on the concrete SegmentList it points at. Member functions are DECLARED here but DEFINED in calc_utils2d.cpp;
// the .cpp then explicitly instantiates SweepLine2D for the concrete SegmentList types (see bottom of the .cpp).
// Only those listed types may be used with SweepLine2D.
template <SegmentList Segments>
class SweepLine2D {
 public:
  // NOTE: stores a non-owning pointer — `segments` must outlive this SweepLine2D (the driver owns it).
  explicit SweepLine2D(Segments const& segments);
  ~SweepLine2D() = default;

  struct IdSegPair {
    std::size_t Id;
    LineSegment2D Seg;  // stored by value: SegmentRange2D::operator[] returns temporaries, pointers would dangle
  };

  struct SweepLineElement2D {
    std::optional<IdSegPair> Segment;
    std::optional<IdSegPair> Above;
    std::optional<IdSegPair> Below;
  };

  /// @brief
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment, Above, Below) = pointers to the segment of index seg_id in the SegmentList,  the above and
  /// below segment pointers. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, warning log if seg_id not found
  SweepLineElement2D Get(std::size_t seg_id) const;

  /// @brief Adds a segment in the tree, sorted
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment = ptr to segment just inserted, Above / Below = ptr to above or below segments in the tree).
  /// Above and Below may be either or both std::nullopt. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, std::logic_error if insertion in tree not possible,
  /// warning log if seg_id not found
  SweepLineElement2D Add(std::size_t seg_id);

  /// @brief Remove the seg_id from the tree, which remains sorted
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment = std::nullopt, Above = ptr to above segment before deletion, Below = ptr to below segment
  /// before deletion). Either or both Above and Below can be std::nullopt. If seg_id not found (std::nullopt,
  /// std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, std::logic_error if insertion in tree not possible,
  /// warning log if seg_id not found
  SweepLineElement2D Remove(std::size_t seg_id);

  /// @brief adjusts the current sweep x coordinate to a desired value, and lets the algorithms continue
  /// @param val usually the X of the next_event in the EventQueue.Pop() or the current X + EPSILON
  void SetX(double val);

  double GetX() const;

 private:
  double SWEEP_X;

  Segments const* PTR_SEGMENTS;  // bound from `Segments const&`, so the pointee is const

  std::vector<std::size_t> ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, kept sorted
                                             // by geometric y at SWEEP_X; lower_bound gives O(log n) search
};

/// @brief Intersection of two infinite lines defined by two points each, returning parametric values.
/// @param p0, p1            Two points on the first line.
/// @param other_p0, other_p1  Two points on the second line.
/// @param sc  Output: parameter along the first line at the intersection.
/// @param tc  Output: parameter along the second line at the intersection.
/// @return The intersection point, or std::nullopt for parallel lines.
std::optional<Point2D> line_intersection(Point2D const& p0, Point2D const& p1, Point2D const& other_p0,
                                         Point2D const& other_p1, double& sc, double& tc);

/// @brief the Shamos-Hoey algorithm for checking polygon simplicity (no self-intersections)
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @returns true - if any intersection exists
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments>
bool has_intersections_impl(Segments const& segments);

struct IntersectionEvent2D {
  Point2D Point;  // point of intersections, returned by the Bentley-Ottmann algorithm in `intersections` below;
  std::vector<std::size_t>
      SegmentIds;  // a point can be the intersection of 2 or more segments
                   // TODO: make a templated (or not) wrapper of std::vector to control it always has 2 elements

  bool operator<(IntersectionEvent2D const& other) const;
  bool operator==(IntersectionEvent2D const& other) const;
};

/// @brief the Bentley-Ottmann algorithm for finding all intersection points among a set of segments
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @returns list of intersection points - in sorted order bottom-left to top-right (the intersecting 2+ segment IDs are
/// also reported)
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections_impl(Segments const& segments);

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull (generic, index-based)
/// @param points random-access range of Point2D or Point3D
/// @param view   projects each point to 2D x/y coordinates
/// @returns list of indices into `points` that form the convex hull in CCW order
template <PointContainer Points>
std::vector<std::size_t> convex_hull_monotone_chain(Points const& points, View2D const& view);

extern template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point2D> const&, View2D const&);

extern template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point3D> const&, View2D const&);

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull
/// @param points cloud of points
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points);

struct MinBoundingRectResult {
  double u_axis_x, u_axis_y;  // unit edge direction (in View2D space)
  double v_axis_x, v_axis_y;  // CCW perpendicular (in View2D space)
  double half_len_u, half_len_v;
  double center_u, center_v;  // center as offset from an arbitrary origin (in u,v local coords)
  // The center in the 2D projection is: origin + center_u * u_axis + center_v * v_axis
  // where origin is the first hull point (p0 of the best edge).
  // For the caller to recover the 2D center:
  //   center_2d_x = origin_x + center_u * u_axis_x + center_v * v_axis_x
  //   center_2d_y = origin_y + center_u * u_axis_y + center_v * v_axis_y
  double origin_x, origin_y;  // origin point (first point of best edge) in View2D x,y space
};

/// @brief Winding-number contribution of a single ring (vertices) around point p.
/// @returns winding number increment/decrement for the ring
int winding_number(std::vector<Point2D> const& vertices, Point2D const& p);

/// @brief Core convexity check: all consecutive cross products have the same sign.
/// Does NOT check holes — callers are responsible for that guard.
template <PointContainer Points>
bool is_convex_with_view(Points const& vertices, View2D const& view);

extern template bool is_convex_with_view(std::vector<Point2D> const&, View2D const&);
extern template bool is_convex_with_view(std::vector<Point3D> const&, View2D const&);

/// @brief Returns true if a 2D polygon (CCW outer ring + optional holes) is convex.
/// A polygon with holes is never convex.
bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes);

/// @brief Decomposes polygon rings into simple closed rings via half-edge face tracing.
/// Projects each point through @p view, builds 2D segments internally, finds all crossings
/// (Bentley-Ottmann), splits at those points, and returns one ring per bounded face.
/// Caller classifies outers vs holes via signed_area.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each point to 2D x/y coordinates.
/// @returns Closed rings in 2D (vertex sequence; closing vertex not repeated). Must be >= 3 total edges.
template <PointContainer Points>
std::vector<std::vector<Point2D>> simplify_rings_impl(Points const& outer, std::vector<Points> const& holes,
                                                      View2D const& view);

extern template std::vector<std::vector<Point2D>> simplify_rings_impl(std::vector<Point2D> const&,
                                                                      std::vector<std::vector<Point2D>> const&,
                                                                      View2D const&);

/// @brief Rotating calipers (Freeman & Shapira 1975 / Toussaint 1983) on a convex hull.
/// Projects points through `view` into 2D, computes the minimum-area bounding rectangle.
/// Requires at least 3 non-degenerate points with a valid convex hull.
/// @param hull_indices indices of convex hull points in CCW order (from convex_hull_monotone_chain)
/// @param points the original point container
/// @param view 2D projection used for x/y extraction
/// @returns MinBoundingRectResult with axes, half-lengths, and center in View2D 2D space
template <PointContainer Points>
MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const& hull_indices, Points const& points,
                                        View2D const& view);

extern template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point2D> const&,
                                                        View2D const&);
extern template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point3D> const&,
                                                        View2D const&);

/// @brief Computes the parametric intervals [t0, t1] on a line where it intersects a polygon.
/// @param outer_coplanar_ccw  Outer ring vertices in CCW winding order, all coplanar. Asserted in debug mode.
/// @param holes_coplanar_cw   Hole ring vertices in CW winding order, all coplanar. Asserted in debug mode.
/// @param is_convex_input     Caller's assertion that the polygon is convex (enables the fast convex path).
///                            Asserted in debug mode against the actual vertex data.
template <PointContainer Points, Point P>
std::vector<std::pair<double, double>> compute_parametric_intersection_intervals(
    Points const& outer_coplanar_ccw, std::vector<Points> const& holes_coplanar_cw, bool is_convex_input,
    P const& line_p0, P const& line_p1, View2D const& view);

/// @brief Concrete Point2D wrapper for compute_parametric_intersection_intervals.
/// The template definition lives in calc_utils2d.cpp only; this non-template
/// declaration lets other TUs call it without triggering implicit instantiation
/// (which would fail because the template body is not in the header).
std::vector<std::pair<double, double>> compute_intersection_intervals_2d(
    std::vector<Point2D> const& outer_coplanar_ccw, std::vector<std::vector<Point2D>> const& holes_coplanar_cw,
    bool is_convex_input, Point2D const& line_p0, Point2D const& line_p1, View2D const& view);

/// @brief Point-on-edge perimeter test projected through a View2D.
/// Works for both 2D (View2D::XY()) and 3D (dominant-axis view) rings.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each vertex to 2D x/y coordinates.
/// @param px     Test point x in view space.
/// @param py     Test point y in view space.
template <PointContainer Points>
bool is_on_perimeter_with_view(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px,
                               double py);

extern template bool is_on_perimeter_with_view(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&,
                                               View2D const&, double, double);
extern template bool is_on_perimeter_with_view(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&,
                                               View2D const&, double, double);

/// @brief Winding-number point-in-polygon test projected through a View2D.
/// Works for both 2D (View2D::XY()) and 3D (dominant-axis view) rings.
/// Does NOT check the perimeter — callers handle that separately.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each vertex to 2D x/y coordinates.
/// @param px     Test point x in view space.
/// @param py     Test point y in view space.
template <PointContainer Points>
bool polygon_contains_with_view(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px,
                                double py);

extern template bool polygon_contains_with_view(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&,
                                                View2D const&, double, double);
extern template bool polygon_contains_with_view(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&,
                                                View2D const&, double, double);

}  // namespace detail

/// @brief Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.
/// Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.
/// @param polygon The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
/// @param line    The line whose Direction() defines the axis of projection.
/// @returns ExtremePoints{min_point, max_point} — the outer-ring vertices with least / greatest projection.
ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line);

}  // namespace geompp
