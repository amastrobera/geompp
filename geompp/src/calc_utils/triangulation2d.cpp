#include "calc_utils/triangulation2d.hpp"

#include "calc_utils/convex_hull2d.hpp"
#include "calc_utils/convex_hull3d.hpp"
#include "calc_utils/polygon_ops2d.hpp"
#include "calc_utils/polygon_queries2d.hpp"
#include "calc_utils/triangulation3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "triangle2d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cstdint>
#include <deque>
#include <stdexcept>
#include <string>
#include <utility>

namespace geompp {

inline namespace geometry {
namespace detail {

namespace view {

namespace helpers {

// helper lambdas
template <typename PointT>
double area2(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  auto [x0, y0] = view.xy(a);
  auto [x1, y1] = view.xy(b);
  auto [x2, y2] = view.xy(c);
  return (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
}

template <typename PointT>
bool is_reflex(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  return compare(helpers::area2(a, b, c, view), 0) < 0;
}

// Inclusive (>= 0, not > 0): a point exactly ON one of the candidate ear's edges must still
// disqualify it, not just a point strictly inside. Without this, a non-adjacent vertex that happens
// to be collinear with two of the ear's vertices (e.g. a reflex vertex sitting exactly on the
// prev-next diagonal, as in an L-shaped polygon whose notch corner lies on that diagonal) is
// invisible to a strict test, so the algorithm accepts a diagonal that actually exits the polygon.
template <typename PointT>
bool is_left_or_on(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  auto u_x = view.x(b) - view.x(a);
  auto u_y = view.y(b) - view.y(a);

  auto v_x = view.x(c) - view.x(a);
  auto v_y = view.y(c) - view.y(a);

  return compare(u_x * v_y - u_y * v_x, 0) >= 0;
};

template <typename PointT>
bool is_point_in_triangle(PointT const& a, PointT const& b, PointT const& c, PointT const& p, View2D const& view) {
  return helpers::is_left_or_on(a, b, p, view) && helpers::is_left_or_on(b, c, p, view) &&
         helpers::is_left_or_on(c, a, p, view);
};

template <typename PointT>
bool are_collinear(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  return compare(helpers::area2(a, b, c, view), 0.0) == 0;
};

}  // namespace helpers

// triangulation functions

// EarClipping: walks the ring and clips the first valid ear it finds, in traversal order. O(n^2)
// worst case, but very often close to O(n) in practice for well-behaved (mostly-convex) polygons,
// since clipping at i frequently leaves i_next immediately clippable too. Doesn't optimize triangle
// shape -- a legitimate but geometrically thin ear can get clipped just because it was encountered
// first, producing a sliver triangle even on perfectly ordinary input. See EarClippingBestFit below
// for the shape-aware alternative and its own, steeper cost.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<PointT> const& input,
                                                                   View2D const& view) {
  std::size_t n = input.size();

  // temporary container for output (excellent for unknown size list)
  std::deque<std::array<std::size_t, 3>> triangles;

  // temporary containers for calculations
  //  \_ previous and next indices (keep them as they are)
  std::vector<std::size_t> next_id(n), prev_id(n);
  for (std::size_t i = 0; i < n; ++i) {
    next_id[i] = (i + 1) % n;
    prev_id[i] = (i + n - 1) % n;
  }

  //  \_ reflex indices to analyze (reduce with a "swap and pop", the order doesn't matter)
  // NOTE: this set also includes exactly-flat (180 degree, collinear-with-neighbors) vertices, not
  // just strictly-reflex ones. The "only reflex vertices can lie inside a candidate ear" theorem the
  // is_ear scan below relies on assumes a proper simple polygon with no 3 consecutive collinear
  // points; a flat vertex is neither reflex nor a valid ear itself, but it CAN sit exactly on another
  // candidate ear's boundary (is_point_in_triangle is inclusive of the boundary), so it must still be
  // scanned as a blocker or that ear gets wrongly accepted and the flat vertex's own two edges never
  // make it into the triangulation. Under Collinearity::Enforce/Assert this never comes up (no flat
  // vertices reach here); Guaranteed callers (e.g. fix_adjacency()'s re-triangulation of a facet with
  // a spliced-in collinear vertex) are exactly the case this guards.
  std::deque<std::size_t> reflex_indices;
  std::vector<std::uint8_t> is_reflex(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    if (helpers::is_reflex(input[prev_id[i]], input[i], input[next_id[i]], view) ||
        helpers::are_collinear(input[prev_id[i]], input[i], input[next_id[i]], view)) {
      reflex_indices.push_back(i);
      is_reflex[i] = 1;
    }
  }

  auto lambda_modify_reflex_status = [&](std::size_t idx) {
    bool is_now_reflex = helpers::is_reflex(input[prev_id[idx]], input[idx], input[next_id[idx]], view) ||
                         helpers::are_collinear(input[prev_id[idx]], input[idx], input[next_id[idx]], view);

    if (!is_now_reflex && is_reflex[idx]) {  // the opposite can never happen by a theorem: once an ear is cut,
                                             // the prev/next vertex can become convex, but never reflex
      auto it = std::find(reflex_indices.begin(), reflex_indices.end(), idx);
      if (it != reflex_indices.end()) {
        // remove with a quick O(1) swap and pop
        *it = reflex_indices.back();
        reflex_indices.pop_back();
      }

      is_reflex[idx] = 0;
    }
  };

  // main loop: while we have more than 3 vertices, try to find an ear and clip it
  std::size_t i = 0, i_prev = prev_id[i], i_next = next_id[i];
  while (n > 3) {
    if (!is_reflex[i]) {  // the triplet could be an ear
      PointT const& p_cur = input[i];
      PointT const& p_prev = input[i_prev];
      PointT const& p_next = input[i_next];

      // it can happen that this vertex is not reflex, but also not convex!
      // the points may be collinear, making the triangle check fail, we want to avoid that
      if (!helpers::are_collinear(p_prev, p_cur, p_next, view)) {
        // check if the triangle is an ear
        bool is_ear = true;
        for (auto const& j : reflex_indices) {
          if (j == i_prev || j == i_next) {  // quick exit (we don't care of the prev/next indices to be reflex)
            continue;
          }
          if (helpers::is_point_in_triangle(p_prev, p_cur, p_next, input[j], view)) {
            is_ear = false;
            break;
          }
        }

        if (is_ear) {
          triangles.push_back({i_prev, i, i_next});

          // set the prev/next indices to skip the current vertex
          next_id[i_prev] = i_next;
          prev_id[i_next] = i_prev;

          // update reflex status of the previous and next vertices
          lambda_modify_reflex_status(i_prev);
          lambda_modify_reflex_status(i_next);

          --n;
        }
      }
    }

    // update the index to the next vertex
    i = i_next;
    i_prev = prev_id[i];
    i_next = next_id[i];
  }

  // exactly 3 vertices remain, still linked via prev_id/next_id — the loop above only clips ears down to
  // n == 3 and never emits this last, leftover triangle itself.
  triangles.push_back({i_prev, i, i_next});

  // transform the deque in vector (1 allocation, using move)
  std::vector<std::array<std::size_t, 3>> triangles_output(std::make_move_iterator(triangles.begin()),
                                                           std::make_move_iterator(triangles.end()));

  return triangles_output;
}

template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point2D> const& input,
                                                                            View2D const& view);
template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point3D> const& input,
                                                                            View2D const& view);

// EarClippingBestFit: like EarClipping, but each outer-loop iteration does a full lap over the
// *current* ring to find the best-scoring valid ear (by shape quality, not just the first one found)
// before clipping exactly one. Guaranteed termination via the same Two Ears Theorem EarClipping relies
// on -- this only ever reorders which valid ear gets picked, never rejects one outright, so it can't
// get stuck the way a strict angle/area floor would. Trades speed for shape: unconditionally ~O(n^2)
// (a full O(current n) rescan per clip, every time), where EarClipping's O(n^2) is only a worst case.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<PointT> const& input,
                                                                            View2D const& view) {
  std::size_t n = input.size();

  // temporary container for output (excellent for unknown size list)
  std::deque<std::array<std::size_t, 3>> triangles;

  // temporary containers for calculations
  //  \_ previous and next indices (keep them as they are)
  std::vector<std::size_t> next_id(n), prev_id(n);
  for (std::size_t i = 0; i < n; ++i) {
    next_id[i] = (i + 1) % n;
    prev_id[i] = (i + n - 1) % n;
  }

  //  \_ reflex indices to analyze (reduce with a "swap and pop", the order doesn't matter)
  // NOTE: this set also includes exactly-flat (180 degree, collinear-with-neighbors) vertices, not
  // just strictly-reflex ones -- see the matching comment in ear_clipping_triangulation() for why a
  // flat vertex must still be scanned as a potential ear-blocker even though it's never a valid ear.
  std::deque<std::size_t> reflex_indices;
  std::vector<std::uint8_t> is_reflex(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    if (helpers::is_reflex(input[prev_id[i]], input[i], input[next_id[i]], view) ||
        helpers::are_collinear(input[prev_id[i]], input[i], input[next_id[i]], view)) {
      reflex_indices.push_back(i);
      is_reflex[i] = 1;
    }
  }

  auto lambda_modify_reflex_status = [&](std::size_t idx) {
    bool is_now_reflex = helpers::is_reflex(input[prev_id[idx]], input[idx], input[next_id[idx]], view) ||
                         helpers::are_collinear(input[prev_id[idx]], input[idx], input[next_id[idx]], view);

    if (!is_now_reflex && is_reflex[idx]) {  // the opposite can never happen by a theorem: once an ear is cut,
                                             // the prev/next vertex can become convex, but never reflex
      auto it = std::find(reflex_indices.begin(), reflex_indices.end(), idx);
      if (it != reflex_indices.end()) {
        // remove with a quick O(1) swap and pop
        *it = reflex_indices.back();
        reflex_indices.pop_back();
      }

      is_reflex[idx] = 0;
    }
  };

  auto lambda_ear_quality = [&view](PointT const& a, PointT const& b, PointT const& c) -> double {
    auto [x0, y0] = view.xy(a);  // p_prev
    auto [x1, y1] = view.xy(b);  // p_cur
    auto [x2, y2] = view.xy(c);  // p_next

    // loose collinearity check already computes
    double area2 = std::abs(helpers::area2(a, b, c, view));

    double a2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);  // |cur  - next|^2, opposite p_prev
    double b2 = (x2 - x0) * (x2 - x0) + (y2 - y0) * (y2 - y0);  // |prev - next|^2, opposite p_cur
    double c2 = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);  // |cur  - prev|^2, opposite p_next

    // the real formula is
    //    ear_quality = 4*sqrt(3)* abs(cross(a,b,c)/2) / (a*a + b*b + c*c)
    // however we remove 4*, sqrt(3), and /2 to save time
    // because we only need this value for comparison and the real variables are the
    // numerator cross(a,b,c) and the denominator (a2 + b2 + c2)
    return area2 / (a2 + b2 + c2);
  };

  // main loop: while we have more than 3 vertices, try to find an ear and clip it
  std::size_t i = 0, i_prev = prev_id[i], i_next = next_id[i];
  while (n > 3) {  // LOOP(1): main of the Ear Cutting Algorithm

    double best_ear_score = -1.0;  // sin(min-angle) or similar, always >= 0 for a valid ear
    std::array<std::size_t, 3> best_ear;
    std::size_t start = i;

    do {                    // LOOP(2): improvement step making this a greedy algorithm with same O(n2)
                            //          finds the best ear amongst all possible ones (based on area)
      if (!is_reflex[i]) {  // the triplet could be an ear
        PointT const& p_cur = input[i];
        PointT const& p_prev = input[i_prev];
        PointT const& p_next = input[i_next];

        // it can happen that this vertex is not reflex, but also not convex!
        // the points may be collinear, making the triangle check fail, we want to avoid that
        if (!helpers::are_collinear(p_prev, p_cur, p_next, view)) {
          // check if the triangle is an ear
          bool is_ear = true;
          for (auto const& j : reflex_indices) {
            if (j == i_prev || j == i_next) {  // quick exit (we don't care of the prev/next indices to be reflex)
              continue;
            }
            if (helpers::is_point_in_triangle(p_prev, p_cur, p_next, input[j], view)) {
              is_ear = false;
              break;
            }
          }

          if (is_ear) {
            // find the best cuttable ear (by area)
            double score = lambda_ear_quality(p_prev, p_cur, p_next);
            if (compare(score, best_ear_score) > 0) {
              best_ear_score = score;
              best_ear = {i_prev, i, i_next};
            }
          }
        }
      }

      i = i_next, i_prev = prev_id[i], i_next = next_id[i];

    } while (i != start);

    i_prev = best_ear[0], i = best_ear[1], i_next = best_ear[2];

    triangles.push_back(best_ear);

    // set the prev/next indices to skip the current vertex
    next_id[i_prev] = i_next;
    prev_id[i_next] = i_prev;

    // update reflex status of the previous and next vertices
    lambda_modify_reflex_status(i_prev);
    lambda_modify_reflex_status(i_next);

    // remove one vertex from the list
    --n;

    // update the index to the next vertex
    i = i_next, i_prev = prev_id[i], i_next = next_id[i];
  }

  // exactly 3 vertices remain, still linked via prev_id/next_id — the loop above only clips ears down to
  // n == 3 and never emits this last, leftover triangle itself.
  triangles.push_back({i_prev, i, i_next});

  // transform the deque in vector (1 allocation, using move)
  std::vector<std::array<std::size_t, 3>> triangles_output(std::make_move_iterator(triangles.begin()),
                                                           std::make_move_iterator(triangles.end()));

  return triangles_output;
}

template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<Point2D> const& input,
                                                                                     View2D const& view);
template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<Point3D> const& input,
                                                                                     View2D const& view);

template <typename PointT>
std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<PointT> const& input,
                                                                       View2D const& view) {
  throw std::runtime_error("not yet implemented");
}

template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<Point2D> const& input,
                                                                                View2D const& view);
template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<Point3D> const& input,
                                                                                View2D const& view);

template <typename PointT>
std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<PointT> const& input, View2D const& view) {
  throw std::runtime_error("not yet implemented");
}

template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point2D> const& input,
                                                                        View2D const& view);
template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point3D> const& input,
                                                                        View2D const& view);

template <typename PointT>
std::vector<std::array<PointT, 3>> triangulate_impl(std::vector<PointT> const& input, View2D const& view,
                                                    TriangulationParams const& settings) {
  if (input.size() < 3) {
    throw std::invalid_argument("triangulate: input must have at least 3 points");
  }

  if (input.size() == 3) {
    return {std::array<PointT, 3>{input[0], input[1], input[2]}};
  }

  // Working copy: Collinearity::Enforce may shrink it, Winding::Enforce may reverse it — every check below
  // and the final index-back-into-points step at the bottom all operate on this, never on the original
  // `input`.
  std::vector<PointT> working = input;

  switch (settings.collinearity) {
    case TriangulationParams::Collinearity::Guaranteed: {
      break;
    }
    case TriangulationParams::Collinearity::Assert: {
      if (detail::view::has_collinears(working, view)) {
        throw std::invalid_argument("triangulate: input has collinear (or duplicate) points");
      }
      break;
    }
    case TriangulationParams::Collinearity::Enforce: {
      if (detail::view::has_collinears(working, view)) {
        // remove_collinear() also catches duplicates as a byproduct: are_collinear(p0, p1, p2) is trivially
        // true whenever any two of the three coincide, so a run of duplicate points collapses right along
        // with genuinely collinear ones — no separate remove_consecutive_duplicates() pass needed.
        working = remove_collinear(std::move(working));
        if (working.size() < 3) {
          throw std::invalid_argument("triangulate: fewer than 3 points remain after removing collinear points");
        }
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown collinearity strategy");
    }
  }

  switch (settings.ccw_winding) {
    case TriangulationParams::Winding::Guaranteed: {
      break;
    }
    case TriangulationParams::Winding::Assert: {
      if (!detail::view::is_ccw(working, view)) {
        throw std::invalid_argument("triangulate: input is not CCW wound");
      }
      break;
    }
    case TriangulationParams::Winding::Enforce: {
      if (!detail::view::is_ccw(working, view)) {
        std::reverse(working.begin(), working.end());
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown winding strategy");
    }
  }

  // param check and creation of simple loops
  std::vector<std::vector<PointT>> simple_loops;
  switch (settings.simplicity) {
    case TriangulationParams::Simplicity::Guaranteed: {
      simple_loops.push_back(working);
      break;
    }
    case TriangulationParams::Simplicity::Assert: {
      if (!detail::view::is_simple(working, view)) {
        throw std::invalid_argument("triangulate: input is not simple");
      }
      simple_loops.push_back(working);
      break;
    }
    case TriangulationParams::Simplicity::Enforce: {
      if (detail::view::is_simple(working, view)) {
        simple_loops.push_back(working);
      } else if constexpr (std::is_same_v<PointT, Point3D>) {
        // TODO: simplify_rings() only ever returns 2D-projected rings (Point2D), so unprojecting them back
        // to Point3D needs plane-aware reconstruction not wired up here yet.
        throw std::runtime_error("not yet implemented");
      } else {  // simplified rings are also guaranteed to be CCW sorted
        for (auto const& loop : detail::view::simplify_rings(working, {}, view)) {
          simple_loops.push_back(loop);
        }
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown simplicity strategy");
    }
  }

  std::vector<std::array<std::size_t, 3>> tri_indices;

  switch (settings.strategy) {
    case TriangulationParams::Strategy::EarClipping: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = ear_clipping_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    case TriangulationParams::Strategy::EarClippingBestFit: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = ear_clipping_best_fit_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    case TriangulationParams::Strategy::MonotonePolygon: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = monotone_polygon_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    case TriangulationParams::Strategy::Delaunay: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = delaunay_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown triangulation strategy");
    }
  }

  std::vector<std::array<PointT, 3>> result;
  result.reserve(tri_indices.size());
  for (auto const& tri : tri_indices) {
    result.push_back({working[tri[0]], working[tri[1]], working[tri[2]]});
  }
  return result;
}

template std::vector<std::array<Point2D, 3>> triangulate_impl(std::vector<Point2D> const& input, View2D const& view,
                                                              TriangulationParams const& settings);
template std::vector<std::array<Point3D, 3>> triangulate_impl(std::vector<Point3D> const& input, View2D const& view,
                                                              TriangulationParams const& settings);
}  // namespace view
}  // namespace detail

std::vector<Triangle2D> triangulate(std::vector<Point2D> const& input, TriangulationParams const& settings) {
  auto tris = detail::view::triangulate_impl(input, View2D::XY(), settings);
  std::vector<Triangle2D> result;
  result.reserve(tris.size());
  for (auto const& t : tris) {
    result.push_back(Triangle2D::Make(t[0], t[1], t[2]));
  }
  return result;
}

namespace {

// PointT-specific segment-containment test, dispatched by overload resolution -- native per dimension,
// deliberately not View2D-projected (see AdjacencyViolation's own docs for why that would be wrong here).
bool segment_contains(Point2D const& a, Point2D const& b, Point2D const& v) {
  return LineSegment2D::Make(a, b).Contains(v);
}
bool segment_contains(Point3D const& a, Point3D const& b, Point3D const& v) {
  return LineSegment3D::Make(a, b).Contains(v);
}

template <typename PointT>
std::vector<AdjacencyViolation<PointT>> validate_adjacency_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  std::vector<AdjacencyViolation<PointT>> violations;

  struct Edge {
    std::size_t facet;
    PointT a, b;
  };
  std::vector<Edge> edges;
  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    auto const& ring = facet_rings[f];
    std::size_t n = ring.size();
    for (std::size_t i = 0; i < n; ++i) {
      edges.push_back({f, ring[i], ring[(i + 1) % n]});
    }
  }

  // Pass 1: non-manifold edges -- group edges that are the exact same undirected segment; a group of
  // more than 2 means more than 1 facet neighbors that edge (a normal boundary edge groups to 1, a
  // normal shared interior edge groups to 2).
  std::vector<bool> used(edges.size(), false);
  for (std::size_t i = 0; i < edges.size(); ++i) {
    if (used[i]) {
      continue;
    }
    std::vector<std::size_t> group{i};
    used[i] = true;
    for (std::size_t j = i + 1; j < edges.size(); ++j) {
      if (used[j]) {
        continue;
      }
      bool same_dir = edges[i].a.AlmostEquals(edges[j].a) && edges[i].b.AlmostEquals(edges[j].b);
      bool rev_dir = edges[i].a.AlmostEquals(edges[j].b) && edges[i].b.AlmostEquals(edges[j].a);
      if (same_dir || rev_dir) {
        group.push_back(j);
        used[j] = true;
      }
    }

    if (group.size() > 2) {
      std::vector<std::size_t> facet_indices;
      for (auto gi : group) {
        facet_indices.push_back(edges[gi].facet);
      }
      violations.push_back({edges[group[0]].a, edges[group[0]].b, facet_indices, true, edges[group[0]].a});
    }
  }

  // Pass 2: T-junctions -- a vertex from some facet lying in the interior of another facet's edge
  // (excluding that edge's own two endpoints).
  std::vector<PointT> unique_vertices;
  std::vector<std::size_t> vertex_owner;
  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    for (auto const& p : facet_rings[f]) {
      bool found = false;
      for (auto const& q : unique_vertices) {
        if (p.AlmostEquals(q)) {
          found = true;
          break;
        }
      }
      if (!found) {
        unique_vertices.push_back(p);
        vertex_owner.push_back(f);
      }
    }
  }

  for (auto const& e : edges) {
    for (std::size_t vi = 0; vi < unique_vertices.size(); ++vi) {
      auto const& v = unique_vertices[vi];
      if (v.AlmostEquals(e.a) || v.AlmostEquals(e.b)) {
        continue;
      }
      if (segment_contains(e.a, e.b, v)) {
        violations.push_back({e.a, e.b, {e.facet, vertex_owner[vi]}, false, v});
      }
    }
  }

  return violations;
}

template <typename PointT>
std::vector<std::vector<PointT>> fix_adjacency_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  auto violations = validate_adjacency_impl(facet_rings);

  for (auto const& v : violations) {
    if (v.is_non_manifold) {
      throw std::invalid_argument("fix_adjacency: edge (" + v.edge_p0.ToWkt() + " -> " + v.edge_p1.ToWkt() +
                                  ") is shared by " + std::to_string(v.facet_indices.size()) +
                                  " facets (max 2 allowed) -- not automatically fixable");
    }
  }

  struct SpliceKey {
    std::size_t facet;
    PointT a, b;
  };
  std::vector<std::pair<SpliceKey, std::vector<PointT>>> splices;

  for (auto const& v : violations) {
    std::size_t coarse_facet = v.facet_indices[0];

    bool found = false;
    for (auto& [key, pts] : splices) {
      if (key.facet == coarse_facet && key.a.AlmostEquals(v.edge_p0) && key.b.AlmostEquals(v.edge_p1)) {
        pts.push_back(v.on_vertex);
        found = true;
        break;
      }
    }
    if (!found) {
      splices.push_back({{coarse_facet, v.edge_p0, v.edge_p1}, {v.on_vertex}});
    }
  }

  std::vector<std::vector<PointT>> new_perimeters = facet_rings;

  for (auto const& [key, pts] : splices) {
    auto& perim = new_perimeters[key.facet];
    for (std::size_t i = 0; i < perim.size(); ++i) {
      if (perim[i].AlmostEquals(key.a)) {
        std::vector<PointT> sorted_pts = pts;
        std::sort(sorted_pts.begin(), sorted_pts.end(), [&](PointT const& x, PointT const& y) {
          return key.a.DistanceTo(x) < key.a.DistanceTo(y);
        });
        perim.insert(perim.begin() + static_cast<std::ptrdiff_t>(i) + 1, sorted_pts.begin(), sorted_pts.end());
        break;
      }
    }
  }

  // NOTE: deliberately NOT rebuilt via Polygon2D/3D::Make() -- Make() unconditionally strips collinear
  // points, which would immediately undo the splice above. These raw rings are the fix.
  return new_perimeters;
}

// View2D to project a ring through for the diagonal-cut math below -- native XY for 2D, PCA-fitted
// dominant-axis view for 3D (same pattern triangulate(vector<Point3D>, settings) uses when no normal
// is supplied), dispatched by overload resolution like segment_contains() above.
View2D view_for_ring(std::vector<Point2D> const&) { return View2D::XY(); }

View2D view_for_ring(std::vector<Point3D> const& ring) {
  auto frame = principal_axes(ring);
  Axis dax = frame.Z.DominantAxis();
  return (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
}

// Standard proper (strict) segment-segment intersection: both segments must straddle each other.
// Touching only at a shared endpoint, or running collinear/overlapping, does NOT count -- a valid
// polygon diagonal is allowed to meet other edges only at its own two endpoints.
template <typename PointT>
bool segments_properly_intersect(PointT const& p1, PointT const& q1, PointT const& p2, PointT const& q2,
                                 View2D const& view) {
  double d1 = detail::view::helpers::area2(p2, q2, p1, view);
  double d2 = detail::view::helpers::area2(p2, q2, q1, view);
  double d3 = detail::view::helpers::area2(p1, q1, p2, view);
  double d4 = detail::view::helpers::area2(p1, q1, q2, view);
  bool straddles_p2q2 = (compare(d1, 0.0) > 0 && compare(d2, 0.0) < 0) || (compare(d1, 0.0) < 0 && compare(d2, 0.0) > 0);
  bool straddles_p1q1 = (compare(d3, 0.0) > 0 && compare(d4, 0.0) < 0) || (compare(d3, 0.0) < 0 && compare(d4, 0.0) > 0);
  return straddles_p2q2 && straddles_p1q1;
}

// Is (ring[i], ring[j]) a valid polygon diagonal? i is always a just-spliced, exactly-flat (180 degree)
// vertex here, never a normal sharp one -- so unlike the general O'Rourke-style Diagonal()/InCone() test,
// there's no "is b within i's interior angle cone" step: a flat vertex's cone IS the whole interior
// half-plane, so any b on the interior side works there. What's still needed: (1) the segment can't
// properly cross any OTHER edge of the ring, (2) it can't run collinear along i's own (flat) edge --
// a "diagonal" along the boundary itself doesn't cut anything, (3) its midpoint must be strictly inside
// the ring (winding-number test, robust to the ring's own other flat/collinear vertices).
template <typename PointT>
bool is_valid_diagonal(std::vector<PointT> const& ring, std::size_t i, std::size_t j, View2D const& view) {
  std::size_t n = ring.size();
  std::size_t i_next = (i + 1) % n;
  std::size_t i_prev = (i + n - 1) % n;
  if (j == i || j == i_next || j == i_prev) {
    return false;
  }

  PointT const& a = ring[i];
  PointT const& b = ring[j];

  if (detail::view::helpers::are_collinear(ring[i_prev], a, b, view)) {
    return false;
  }

  for (std::size_t k = 0; k < n; ++k) {
    std::size_t k_next = (k + 1) % n;
    if (k == i || k == j || k_next == i || k_next == j) {
      continue;
    }
    if (segments_properly_intersect(a, b, ring[k], ring[k_next], view)) {
      return false;
    }
  }

  double mx = (view.x(a) + view.x(b)) / 2.0;
  double my = (view.y(a) + view.y(b)) / 2.0;
  return detail::view::polygon_contains(ring, std::vector<std::vector<PointT>>{}, view, mx, my);
}

// Splits a simple ring into two simple rings sharing the diagonal (ring[i], ring[j]) as an edge.
template <typename PointT>
std::pair<std::vector<PointT>, std::vector<PointT>> split_ring_at(std::vector<PointT> const& ring, std::size_t i,
                                                                   std::size_t j) {
  std::size_t n = ring.size();
  std::vector<PointT> ring_a, ring_b;
  for (std::size_t k = i;; k = (k + 1) % n) {
    ring_a.push_back(ring[k]);
    if (k == j) {
      break;
    }
  }
  for (std::size_t k = j;; k = (k + 1) % n) {
    ring_b.push_back(ring[k]);
    if (k == i) {
      break;
    }
  }
  return {ring_a, ring_b};
}

// The Polygon2D/3D flavor of fix_adjacency(): unlike the raw splice fix_adjacency_impl() above (still
// used by the Triangle2D/3D overload as a first step before re-triangulating), this actually splits a
// coarse facet into multiple facets -- one straight diagonal cut per spliced T-junction vertex, to its
// nearest ring vertex that forms a valid diagonal. This is the standard "diagonal-to-nearest-vertex"
// polygon-splitting technique (a guaranteed-valid diagonal always exists from any vertex of a simple
// polygon with >= 4 vertices), same guarantee ear-clipping itself relies on. An unaffected facet passes
// through unchanged.
template <typename PointT>
std::vector<std::vector<PointT>> split_facets_at_junctions_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  auto violations = validate_adjacency_impl(facet_rings);

  for (auto const& v : violations) {
    if (v.is_non_manifold) {
      throw std::invalid_argument("fix_adjacency: edge (" + v.edge_p0.ToWkt() + " -> " + v.edge_p1.ToWkt() +
                                  ") is shared by " + std::to_string(v.facet_indices.size()) +
                                  " facets (max 2 allowed) -- not automatically fixable");
    }
  }

  struct SpliceKey {
    std::size_t facet;
    PointT a, b;
  };
  std::vector<std::pair<SpliceKey, std::vector<PointT>>> splices;

  for (auto const& v : violations) {
    std::size_t coarse_facet = v.facet_indices[0];

    bool found = false;
    for (auto& [key, pts] : splices) {
      if (key.facet == coarse_facet && key.a.AlmostEquals(v.edge_p0) && key.b.AlmostEquals(v.edge_p1)) {
        pts.push_back(v.on_vertex);
        found = true;
        break;
      }
    }
    if (!found) {
      splices.push_back({{coarse_facet, v.edge_p0, v.edge_p1}, {v.on_vertex}});
    }
  }

  std::vector<std::vector<PointT>> result;

  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    std::vector<PointT> ring = facet_rings[f];
    std::vector<PointT> spliced_points;

    for (auto const& [key, pts] : splices) {
      if (key.facet != f) {
        continue;
      }
      for (std::size_t i = 0; i < ring.size(); ++i) {
        if (ring[i].AlmostEquals(key.a)) {
          std::vector<PointT> sorted_pts = pts;
          std::sort(sorted_pts.begin(), sorted_pts.end(), [&](PointT const& x, PointT const& y) {
            return key.a.DistanceTo(x) < key.a.DistanceTo(y);
          });
          ring.insert(ring.begin() + static_cast<std::ptrdiff_t>(i) + 1, sorted_pts.begin(), sorted_pts.end());
          spliced_points.insert(spliced_points.end(), sorted_pts.begin(), sorted_pts.end());
          break;
        }
      }
    }

    if (spliced_points.empty()) {
      result.push_back(std::move(ring));
      continue;
    }

    View2D view = view_for_ring(ring);
    std::vector<std::vector<PointT>> pieces = {std::move(ring)};

    for (auto const& v : spliced_points) {
      for (std::size_t pi = 0; pi < pieces.size(); ++pi) {
        auto& piece = pieces[pi];

        std::size_t vi = piece.size();
        for (std::size_t k = 0; k < piece.size(); ++k) {
          if (piece[k].AlmostEquals(v)) {
            vi = k;
            break;
          }
        }
        if (vi == piece.size()) {
          continue;  // v doesn't belong to this piece -- keep searching the others
        }

        std::vector<std::size_t> candidates;
        for (std::size_t k = 0; k < piece.size(); ++k) {
          if (k != vi) {
            candidates.push_back(k);
          }
        }
        std::sort(candidates.begin(), candidates.end(), [&](std::size_t ca, std::size_t cb) {
          return v.DistanceTo(piece[ca]) < v.DistanceTo(piece[cb]);
        });

        std::size_t chosen = piece.size();
        for (auto c : candidates) {
          if (is_valid_diagonal(piece, vi, c, view)) {
            chosen = c;
            break;
          }
        }
        if (chosen == piece.size()) {
          throw std::logic_error("fix_adjacency: found no valid diagonal from a spliced T-junction vertex -- "
                                 "every simple polygon with >= 4 vertices has one, so this indicates a bug");
        }

        auto [ring_a, ring_b] = split_ring_at(piece, vi, chosen);
        pieces.erase(pieces.begin() + static_cast<std::ptrdiff_t>(pi));
        pieces.push_back(std::move(ring_a));
        pieces.push_back(std::move(ring_b));
        break;  // this spliced vertex is handled -- move on to the next one
      }
    }

    result.insert(result.end(), std::make_move_iterator(pieces.begin()), std::make_move_iterator(pieces.end()));
  }

  return result;
}

}  // namespace

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Polygon2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Triangle2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<std::vector<Point2D>> const& facet_rings) {
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Polygon3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Triangle3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<std::vector<Point3D>> const& facet_rings) {
  return validate_adjacency_impl(facet_rings);
}

std::vector<std::vector<Point2D>> fix_adjacency(std::vector<Polygon2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return split_facets_at_junctions_impl(facet_rings);
}

std::vector<std::vector<Point3D>> fix_adjacency(std::vector<Polygon3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return split_facets_at_junctions_impl(facet_rings);
}

std::vector<Triangle2D> fix_adjacency(std::vector<Triangle2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }

  auto fixed_rings = fix_adjacency_impl(facet_rings);

  std::vector<Triangle2D> result;
  TriangulationParams guaranteed_collinearity;
  guaranteed_collinearity.collinearity = TriangulationParams::Collinearity::Guaranteed;
  for (auto const& ring : fixed_rings) {
    if (ring.size() == 3) {
      result.push_back(Triangle2D::Make(ring[0], ring[1], ring[2]));
      continue;
    }
    auto sub_triangles = triangulate(ring, guaranteed_collinearity);
    result.insert(result.end(), std::make_move_iterator(sub_triangles.begin()), std::make_move_iterator(sub_triangles.end()));
  }
  return result;
}

std::vector<Triangle3D> fix_adjacency(std::vector<Triangle3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }

  auto fixed_rings = fix_adjacency_impl(facet_rings);

  std::vector<Triangle3D> result;
  TriangulationParams guaranteed_collinearity;
  guaranteed_collinearity.collinearity = TriangulationParams::Collinearity::Guaranteed;
  for (auto const& ring : fixed_rings) {
    if (ring.size() == 3) {
      result.push_back(Triangle3D::Make(ring[0], ring[1], ring[2]));
      continue;
    }
    // The ring may now span more than 3 points (still planar -- the splice only added collinear
    // points), so fit its normal via PCA rather than assuming a caller-supplied one.
    auto sub_triangles = triangulate(ring, guaranteed_collinearity);
    result.insert(result.end(), std::make_move_iterator(sub_triangles.begin()), std::make_move_iterator(sub_triangles.end()));
  }
  return result;
}

std::vector<Triangle2D> triangulate(std::vector<Polygon2D> const& polygons, TriangulationParams const& settings) {
  switch (settings.conformity) {
    case TriangulationParams::AdjacencyConformity::Guaranteed: {
      std::vector<Triangle2D> result;
      for (auto const& poly : polygons) {
        auto tris = triangulate(poly.Perimeter(), settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    case TriangulationParams::AdjacencyConformity::Assert: {
      auto violations = validate_adjacency(polygons);
      if (!violations.empty()) {
        auto const& v = violations.front();
        throw std::invalid_argument(
            "triangulate: facets violate adjacency conformity at edge (" + v.edge_p0.ToWkt() + " -> " +
            v.edge_p1.ToWkt() + ") -- " +
            (v.is_non_manifold ? "shared by " + std::to_string(v.facet_indices.size()) + " facets (max 2 allowed)"
                                : "vertex " + v.on_vertex.ToWkt() + " lies on this edge (a T-junction)"));
      }
      std::vector<Triangle2D> result;
      for (auto const& poly : polygons) {
        auto tris = triangulate(poly.Perimeter(), settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    case TriangulationParams::AdjacencyConformity::Enforce: {
      // fix_adjacency() now actually splits a coarse facet via a diagonal cut per T-junction vertex
      // (§10.5), rather than just splicing a flat vertex into its ring, so every returned ring is
      // already a simple polygon with no leftover collinear points -- no need to force
      // Collinearity::Guaranteed here the way an earlier version of this function had to.
      auto fixed_rings = fix_adjacency(polygons);

      std::vector<Triangle2D> result;
      for (auto const& ring : fixed_rings) {
        auto tris = triangulate(ring, settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown adjacency conformity");
    }
  }
}

namespace detail {

template <typename PointT>
void assert_adjacency(std::vector<AdjacencyViolation<PointT>> const& violations) {
  if (violations.empty()) {
    return;
  }
  auto const& v = violations.front();
  std::string facets_str;
  for (std::size_t i = 0; i < v.facet_indices.size(); ++i) {
    facets_str += std::to_string(v.facet_indices[i]);
    if (i + 1 < v.facet_indices.size()) {
      facets_str += ", ";
    }
  }

  if (v.is_non_manifold) {
    throw std::invalid_argument("facet(s) " + facets_str + " have an edge in common (" + v.edge_p0.ToWkt() + " -> " +
                                v.edge_p1.ToWkt() + ") -- we only allow max 2 facets to share an edge");
  }
  throw std::invalid_argument("facet " + std::to_string(v.facet_indices[0]) + "'s edge (" + v.edge_p0.ToWkt() +
                              " -> " + v.edge_p1.ToWkt() + ") has facet " + std::to_string(v.facet_indices[1]) +
                              "'s vertex " + v.on_vertex.ToWkt() + " lying on it (a T-junction) -- a vertex may "
                              "only touch a neighbor's edge at that edge's own start/end");
}

template void assert_adjacency(std::vector<AdjacencyViolation<Point2D>> const&);
template void assert_adjacency(std::vector<AdjacencyViolation<Point3D>> const&);

}  // namespace detail

}  // namespace geometry

}  // namespace geompp
