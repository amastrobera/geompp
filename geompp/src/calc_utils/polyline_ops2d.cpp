#include "calc_utils/polyline_ops2d.hpp"

#include "point3d.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <stack>
#include <utility>

namespace geompp {

inline namespace geometry {

namespace {

// made in order to avoid the use of std::sqrt()
template <typename P>
double distance2(P const& a, P const& b) {
  auto c = b - a;
  return c.Dot(c);
}

}  // namespace

template <PointContainer Points>
Points dist_decimation(Points const& points, double threshold) {
  if (points.size() < 3) {
    return points;
  }
  Points result;
  result.push_back(points.front());

  double threshold2 = threshold * threshold;

  for (std::size_t i = 1; i < points.size() - 1; ++i) {
    if (compare(distance2(points[i], result.back()), threshold2) > 0) {
      result.push_back(points[i]);
    }
  }
  result.push_back(points.back());
  return result;
}

template std::vector<Point2D> dist_decimation(std::vector<Point2D> const&, double threshold);
template std::vector<Point3D> dist_decimation(std::vector<Point3D> const&, double threshold);

namespace {

// function created in order to avoid the ctor of LineSegment to use DistanceTo(Point)
template <typename P>
double line_distance_2(P const& s_p0, P const& s_p1, P const& p) {
  auto cathet = s_p1 - s_p0;
  auto hypotenuse = p - s_p0;

  double cathet_len2 = cathet.Dot(cathet);
  if (compare(cathet_len2, 0) == 0) {
    return (p - s_p0).Dot(p - s_p0);  // Fallback to point-point distance
  }

  // t belongs to [0,1] if on the segment, otherwise belongs to the infinite line
  // we don't particularly care of the segment distance here
  double t = hypotenuse.Dot(cathet) / cathet_len2;

  return distance2(p, s_p0 + cathet * t);
}

}  // namespace

template <PointContainer Points>
Points rdp_decimation(Points const& points, double threshold) {
  if (points.size() < 3) {
    return points;
  }

  double threshold2 = threshold * threshold;  // comparable to the quick and easy square_area2

  std::size_t n = points.size();

  // maskof booleans, very light in memory
  std::vector<bool> keep(n, false);
  keep.front() = true;
  keep.back() = true;

  // avoiding recursion when splitting the polyline in two, using iteration on a stack
  std::vector<std::pair<std::size_t, std::size_t>> i_vec;
  i_vec.reserve(n);  // Zero allocation overhead during the loop
  std::stack<std::pair<std::size_t, std::size_t>, std::vector<std::pair<std::size_t, std::size_t>>> i_stack(
      std::move(i_vec));
  i_stack.push({0, n - 1});

  while (!i_stack.empty()) {
    auto [i_start, i_end] = i_stack.top();
    i_stack.pop();

    // dynamic finding of the max distance
    std::size_t i_max = i_start;
    double dist2_max = 0;
    for (std::size_t i = i_start + 1; i < i_end; ++i) {
      double dist2 = line_distance_2(points[i_start], points[i_end], points[i]);
      if (compare(dist2, threshold2) > 0 && compare(dist2_max, dist2) < 0) {
        dist2_max = dist2;
        i_max = i;
      }
    }

    if (i_max != i_start) {
      // keep that index
      keep[i_max] = true;
      // split the points in two portions and guarantee an end condition for the stack
      if (i_start + 1 < i_max) {  // below
        i_stack.push({i_start, i_max});
      }
      if (i_max + 1 != i_end) {  // above
        i_stack.push({i_max, i_end});
      }
    }
  }

  // return decimated polyline
  // use reserve to optimize the vector's space
  std::size_t m = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (keep[i]) {
      ++m;
    }
  }

  Points result;
  result.reserve(m);
  for (std::size_t i = 0; i < n; ++i) {
    if (keep[i]) {
      result.emplace_back(points[i]);
    }
  }
  return result;
}

template std::vector<Point2D> rdp_decimation(std::vector<Point2D> const&, double threshold);
template std::vector<Point3D> rdp_decimation(std::vector<Point3D> const&, double threshold);

namespace {
// A lighter tracker: Just stores the area and the vertex index for the heap
struct HeapEntry {
  double area;
  std::size_t index;

  // std::priority_queue is a max-heap by default;
  // greater-than operator turns it into a min-heap
  bool operator>(HeapEntry const& other) const {
    return area > other.area;  // preferrable to compare(are, other.area) > 0
                               // (1) cost: DynamicEpsilon::operator double() is a transcendental call, recomputed on
                               //           every single invocation (it's not cached — DECIMAL_PRECISION is a
                               //           thread_local runtime value, so the compiler can't fold it).
                               // (2) correctness: epsilon-equivalence isn't transitive, and std::priority_queue
                               //                  requires it to be.
  }
};

// To handle topology, we STILL need to track current neighbors
struct Topology {
  std::size_t prev;
  std::size_t next;
  double current_area;
};

template <typename P>
double square_area2(P const& s_p0, P const& s_p1, P const& p) {
  auto c = (p - s_p0).Cross(s_p1 - s_p0);
  return c * c;  // valid for both 3D (this is a .Dot() prod) and 2D (it's a scalar prod of doubles)
}

}  // namespace

template <PointContainer Points>
Points vw_decimation(Points const& points, double threshold) {
  if (points.size() < 3) {
    return points;
  }

  double square_threshold2 = threshold * threshold * 4;  // comparable to the quick and easy square_area2

  std::size_t n = points.size();
  std::vector<Topology> line(n);

  // The real C++ heap structure!
  std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> min_heap;

  // 1. Initialize topology and heap
  for (std::size_t i = 0; i < n; ++i) {
    line[i].prev = (i == 0) ? std::numeric_limits<std::size_t>::max() : i - 1;
    line[i].next = (i == n - 1) ? std::numeric_limits<std::size_t>::max() : i + 1;

    if (i == 0 || i == n - 1) {
      line[i].current_area = std::numeric_limits<double>::infinity();
    } else {
      line[i].current_area = square_area2(points[i - 1], points[i], points[i + 1]);
      min_heap.push({line[i].current_area, i});
    }
  }

  // 2. Main Loop
  while (!min_heap.empty()) {
    auto [area, idx] = min_heap.top();
    min_heap.pop();

    // LAZY DELETION GUARD: If this area doesn't match the updated truth,
    // it's a stale duplicate. Toss it out.
    if (area != line[idx].current_area) {
      continue;
    }
    if (compare(area, square_threshold2) > 0) {
      break;
    }

    std::size_t p = line[idx].prev;
    std::size_t nxt = line[idx].next;

    // Bypass the current vertex in our topology chain
    if (p != std::numeric_limits<std::size_t>::max()) {
      line[p].next = nxt;
    }
    if (nxt != std::numeric_limits<std::size_t>::max()) {
      line[nxt].prev = p;
    }

    // Update neighbor 'p' and push a fresh copy to the heap
    if (p != std::numeric_limits<std::size_t>::max() && line[p].prev != std::numeric_limits<std::size_t>::max()) {
      line[p].current_area = square_area2(points[line[p].prev], points[p], points[line[p].next]);
      min_heap.push({line[p].current_area, p});
    }

    // Update neighbor 'nxt' and push a fresh copy to the heap
    if (nxt != std::numeric_limits<std::size_t>::max() && line[nxt].next != std::numeric_limits<std::size_t>::max()) {
      line[nxt].current_area = square_area2(points[line[nxt].prev], points[nxt], points[line[nxt].next]);
      min_heap.push({line[nxt].current_area, nxt});
    }
  }

  // 3. Build output path
  Points result;
  std::size_t curr = 0;
  while (curr != std::numeric_limits<std::size_t>::max()) {
    result.push_back(points[curr]);
    curr = line[curr].next;
  }
  return result;
}

template std::vector<Point2D> vw_decimation(std::vector<Point2D> const&, double threshold);
template std::vector<Point3D> vw_decimation(std::vector<Point3D> const&, double threshold);

namespace {

// Steps 1-2 shared by both bezier_smoothing_2 overloads: clamp smoothness into a trim fraction and compute
// the trimmed tangent points T0/T1. Kept as a single source of truth so the degeneracy guard below only has
// to be gotten right once.
template <typename PointT>
std::pair<PointT, PointT> bezier_trimmed_tangents(PointT const& p0, PointT const& p1, PointT const& p2,
                                                  double smoothness, double min_segment_length) {
  // clamp the input values: transforming user input [0,1] into maths input [0,0.5]
  double internal_k = std::clamp(smoothness * 0.5, 0.0, 1.0);
  double k = std::clamp(internal_k, 0.0, 0.5);

  double len1 = p0.DistanceTo(p1);
  double len2 = p2.DistanceTo(p1);
  double max_trim = std::min(len1, len2) * k;

  // A zero-length incoming/outgoing edge (p0 == p1 or p2 == p1) has no direction to trim along:
  // Normalize() on a zero vector returns NaN, and NaN * 0 is still NaN (not 0), so falling through
  // to the general formula would silently poison T0/T1 (and therefore the whole sampled curve) with
  // NaN. Fall back to "no trim on that side" instead — T0/T1 degenerate to the corner point itself,
  // which is exactly what max_trim = 0 already means geometrically. min_segment_length generalizes
  // this from "exactly zero" to "at or below a caller-chosen length", so callers can also skip
  // trimming (or, if both sides are short, skip the whole corner) on edges that are merely tiny
  // rather than perfectly degenerate.
  PointT T0 = compare(len1, min_segment_length) <= 0 ? p1 : p1 + (p0 - p1).Normalize() * max_trim;
  PointT T1 = compare(len2, min_segment_length) <= 0 ? p1 : p1 + (p2 - p1).Normalize() * max_trim;
  return {T0, T1};
}

// Step 5 shared by both bezier_smoothing_2 overloads: sample the quadratic Bezier curve
// B(t) = (1-t)^2 * T0 + 2(1-t)t * p1 + t^2 * T1 at num_segments + 1 evenly spaced parameter values.
template <typename PointT>
std::vector<PointT> sample_quadratic_bezier(PointT const& T0, PointT const& p1, PointT const& T1, int num_segments) {
  std::vector<PointT> result;
  result.reserve(num_segments + 1);

  for (int i = 0; i <= num_segments; ++i) {
    double t = static_cast<double>(i) / num_segments;
    // we can't sum up points (geometrically non sense) so we rewrite the Bernstein-form blend as nested lerps
    // (De Casteljau's algorithm for a quadratic Bezier — mathematically identical to the desired:
    //      PointT pt = T0 * (u * u) + p1 * (2.0 * u * t) + T1 * (t * t);      with u = 1 - t
    PointT a = lerp(T0, p1, t);
    PointT b = lerp(p1, T1, t);
    PointT pt = lerp(a, b, t);

    result.push_back(pt);
  }

  return result;
}

}  // namespace

template <typename PointT>
std::vector<PointT> bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, double min_distance,
                                       double min_segment_length) {
  // min_distance is a sampling interval we later divide by — unlike a threshold that's only ever
  // squared/compared (see dist_decimation), 0 or negative here means dividing by zero/negative and
  // casting an out-of-range double to int, which is undefined behavior. There is no sane fallback
  // value, so reject it outright.
  if (compare(min_distance, 0.0) <= 0) {
    throw std::invalid_argument("bezier_smoothing_2: min_distance must be > 0");
  }

  auto [T0, T1] = bezier_trimmed_tangents(p0, p1, p2, smoothness, min_segment_length);

  // Estimate curve arc length, then pick enough samples to honor min_distance.
  double approx_length = (T0.DistanceTo(p1) + p1.DistanceTo(T1) + T0.DistanceTo(T1)) / 2.0;
  int num_segments = std::max(1, static_cast<int>(std::floor(approx_length / min_distance)));

  return sample_quadratic_bezier(T0, p1, T1, num_segments);
}

template std::vector<Point2D> bezier_smoothing_2(Point2D p0, Point2D p1, Point2D p2, double smoothness,
                                                 double min_distance, double min_segment_length);
template std::vector<Point3D> bezier_smoothing_2(Point3D p0, Point3D p1, Point3D p2, double smoothness,
                                                 double min_distance, double min_segment_length);

template <typename PointT>
std::vector<PointT> bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, int num_segments,
                                       double min_segment_length) {
  // Exact-count overload: num_segments is the caller's direct choice, not a value derived from curve
  // length, so it gets its own precondition instead of being reconciled against min_distance.
  if (num_segments < 1) {
    throw std::invalid_argument("bezier_smoothing_2: num_segments must be >= 1");
  }

  auto [T0, T1] = bezier_trimmed_tangents(p0, p1, p2, smoothness, min_segment_length);

  return sample_quadratic_bezier(T0, p1, T1, num_segments);
}

template std::vector<Point2D> bezier_smoothing_2(Point2D p0, Point2D p1, Point2D p2, double smoothness,
                                                 int num_segments, double min_segment_length);
template std::vector<Point3D> bezier_smoothing_2(Point3D p0, Point3D p1, Point3D p2, double smoothness,
                                                 int num_segments, double min_segment_length);

template <typename PointT>
std::vector<PointT> polyline_expansion(std::vector<PointT> const& input, PolylineExpansionParams const& settings) {
  std::size_t n = input.size();

  std::vector<PointT> output;
  // Upper bound on the final size: each of the (n-2) inner corners contributes at most
  // segments_per_corner + 1 points in FixedSegments mode. MinDistance mode can't be sized exactly
  // without redoing bezier_smoothing_2's own arc-length math per corner, so this is a rough
  // (usually-undershooting) guess there — reserve() only needs to be in the right ballpark to avoid
  // repeated reallocation as output grows, not exact.
  if (n >= 2) {
    std::size_t points_per_corner = settings.mode == PolylineExpansionParams::Mode::FixedSegments
                                        ? static_cast<std::size_t>(std::max(settings.segments_per_corner, 1)) + 1
                                        : 4;
    output.reserve(2 + (n - 2) * points_per_corner);
  }

  // Skips a point that would be a zero-length segment from the last one already in output — both a
  // corner fully skipped via min_segment_length (T0 == T1 == p1, so every one of its samples is the
  // same point) and, in principle, two adjacent corners' arcs meeting exactly at a shared edge's
  // midpoint (bezier_smoothing_2's trim is capped at half of each adjacent edge, so adjacent arcs can
  // touch but never cross) would otherwise hand Polyline2D::Make() consecutive duplicate points, which
  // it rejects as a degenerate (zero-length) segment.
  auto append_unique = [&output](PointT const& p) {
    if (output.empty() || !output.back().AlmostEquals(p)) {
      output.push_back(p);
    }
  };

  // 1. Start with the very first point
  append_unique(input[0]);

  // 2. Loop over every INNER corner (index 1 through N-2). p0/p1/p2 always come from the ORIGINAL
  // input, never from the growing output buffer — see the doc comment in calc_utils/polyline_ops2d.hpp
  // for why that matters (it's what keeps adjacent corners' trims from ever exceeding their shared edge).
  for (std::size_t i = 1; i < n - 1; ++i) {
    PointT const& p0 = input[i - 1];
    PointT const& p1 = input[i];  // The corner point
    PointT const& p2 = input[i + 1];

    std::vector<PointT> curve;
    // Generate the Bézier curve points between T_entry and T_exit
    if (settings.mode == PolylineExpansionParams::Mode::FixedSegments) {
      curve = bezier_smoothing_2(p0, p1, p2, settings.smoothness, settings.segments_per_corner,
                                 settings.min_segment_length);

    } else if (settings.mode == PolylineExpansionParams::Mode::MinDistance) {
      curve = bezier_smoothing_2(p0, p1, p2, settings.smoothness, settings.min_distance, settings.min_segment_length);
    }

    // Append all curve points into the output buffer
    for (auto const& pt : curve) {
      append_unique(pt);
    }
  }

  // 3. Finish with the very last point
  append_unique(input.back());

  return output;
}

template std::vector<Point2D> polyline_expansion(std::vector<Point2D> const& input,
                                                 PolylineExpansionParams const& settings);
template std::vector<Point3D> polyline_expansion(std::vector<Point3D> const& input,
                                                 PolylineExpansionParams const& settings);

}  // namespace geometry

}  // namespace geompp
