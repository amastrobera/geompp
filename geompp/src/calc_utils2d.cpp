#include "calc_utils2d.hpp"

#include "calc_utils3d.hpp"
#include "line2d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "segment_iterator2d.hpp"
#include "sweep_line2d.hpp"
#include "triangle2d.hpp"
#include "triangle3d.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iterator>
#include <limits>
#include <map>
#include <numbers>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <variant>

namespace geompp {

inline namespace geometry {
namespace detail {

std::optional<Point2D> line_intersection(Point2D const& p0, Point2D const& p1, Point2D const& other_p0,
                                         Point2D const& other_p1, double& sc, double& tc) {
  try {
    // 2D intersection via perp-product:
    //   L(s) = p0 + s*(p1-p0),  L(t) = other_p0 + t*(other_p1-other_p0)
    //   s = -(w0 . vp) / (u . vp),  t = (w0 . up) / (v . up)
    auto u = p1 - p0;
    auto v = other_p1 - other_p0;
    auto vp = v.Perp();
    auto up = u.Perp();
    auto w0 = p0 - other_p0;

    if (compare(u.Dot(vp), 0) == 0 || compare(v.Dot(up), 0) == 0) {
      sc = tc = std::numeric_limits<double>::quiet_NaN();
      return std::nullopt;
    }

    sc = -w0.Dot(vp) / u.Dot(vp);
    tc = w0.Dot(up) / v.Dot(up);

    return p0 + (u * sc);

  } catch (...) {
    GEOMPP_LOG(WARNING) << "unexpected error while computing line intersection";
  }
  sc = tc = std::numeric_limits<double>::quiet_NaN();
  return std::nullopt;
}

// ------- free functions -------

namespace {

// Visitor for has_intersections(): stops the Shamos-Hoey sweep at the first crossing found; ignores
// segments simply becoming active (a plain existence check has nothing to do on OnStart).
struct FirstIntersectionVisitor2D {
  bool OnStart(auto const&) { return false; }

  // Same predicate the algorithm used to hardcode: exclude pairs that merely touch at a shared endpoint
  // (adjacent polygon edges, not a self-intersection), then the classic orientation-sign straddle test —
  // now the visitor's own choice, not something run_shamos_hoey forces on every caller.
  bool IsIntersecting(LineSegment2D const& a, LineSegment2D const& b) const {
    return !shares_endpoint(a, b) && intersect(a, b);
  }

  bool OnIntersection(auto const&, auto const&) { return true; }
};

// Visitor for find_intersections(): collects every confirmed crossing, merging segment ids into a single
// IntersectionEvent2D when 3+ segments cross at the same point (rather than emitting several 2-id events for
// that one point). Never stops the sweep early — it needs every crossing.
struct CollectIntersectionsVisitor2D {
  std::set<IntersectionEvent2D> Output;

  bool OnStart(auto const&) { return false; }

  // Excludes pairs that merely touch at a shared endpoint (adjacent polygon edges, not a self-
  // intersection) — the algorithm no longer does this filtering itself, so every visitor owns the choice.
  // Intersection() alone misses collinear, partially-overlapping segments — it returns nullopt for
  // parallel input (no unique point), so two segments that merely overlap along a shared sub-interval
  // would otherwise never generate an event. Overlap() finds that shared sub-segment directly (and
  // already discounts a mere shared-endpoint touch, returning nullopt for that), so its endpoints are
  // reported the same way a transversal crossing point would be.
  std::vector<Point2D> TestPair(LineSegment2D const& a, LineSegment2D const& b) const {
    if (shares_endpoint(a, b)) {
      return {};
    }
    if (auto p = a.Intersection(b)) {
      return {p.value()};
    }
    if (auto overlap = a.Overlap(b)) {
      return {overlap->First(), overlap->Last()};
    }
    return {};
  }

  bool OnIntersection(IntersectionEvent2D const& hit) {
    auto it = Output.find(hit);
    if (it == Output.end()) {
      Output.insert(hit);
      return false;
    }
    // are const so they must be extracted, modified, and re-inserted; also `vector::contains` doesn't exist
    IntersectionEvent2D updated = *it;
    Output.erase(it);
    for (auto id : hit.SegmentIds) {
      if (std::find(updated.SegmentIds.begin(), updated.SegmentIds.end(), id) == updated.SegmentIds.end()) {
        updated.SegmentIds.push_back(id);
      }
    }
    Output.insert(std::move(updated));
    return false;
  }
};

}  // namespace

template <SegmentList Segments>
bool has_intersections(Segments const& segments) {
  FirstIntersectionVisitor2D visitor;
  return run_shamos_hoey(segments, visitor);
}

template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections(Segments const& segments) {
  CollectIntersectionsVisitor2D visitor;
  run_bentley_ottmann(segments, visitor);
  return std::vector<IntersectionEvent2D>(visitor.Output.begin(), visitor.Output.end());
}

// for a free function template the instantiation deduces the parameter from the argument type:
template bool has_intersections(std::vector<LineSegment2D> const&);
template bool has_intersections(SegmentRange2D const&);

// for a free function template the instantiation deduces the parameter from the argument type:
template std::vector<IntersectionEvent2D> find_intersections(std::vector<LineSegment2D> const&);
template std::vector<IntersectionEvent2D> find_intersections(SegmentRange2D const&);

std::vector<LineSegment2D> split_segments_at_crossings(std::vector<LineSegment2D> const& segs) {
  // Qualified deliberately: an unqualified call would ADL onto geompp::find_intersections
  // (segs is std::vector<LineSegment2D>, and LineSegment2D lives in geompp) and, being a
  // non-template exact match, that overload wins over this template — silently returning
  // std::vector<Point2D> instead of std::vector<IntersectionEvent2D>.
  auto crossings = detail::find_intersections(segs);

  // map: segment index → crossing points on that segment
  // find_intersections()'s sweep already reports both transversal crossings AND collinear-overlap
  // endpoints (CollectIntersectionsVisitor2D::TestPair tries Intersection() then falls back to
  // Overlap()), so no separate all-pairs pass is needed here — everything below is O((n+k) log n), the
  // same bound the sweep itself guarantees.
  std::map<std::size_t, std::vector<Point2D>> seg_cp;
  for (auto const& ev : crossings) {
    for (auto id : ev.SegmentIds) {
      seg_cp[id].push_back(ev.Point);
    }
  }

  std::vector<LineSegment2D> split;
  split.reserve(segs.size() + crossings.size() * 2);
  for (std::size_t i = 0; i < segs.size(); ++i) {
    auto it = seg_cp.find(i);
    if (it == seg_cp.end()) {
      split.push_back(segs[i]);
      continue;
    }
    // sort crossing points by parameter t along this segment
    std::vector<Point2D> cps = it->second;
    std::sort(cps.begin(), cps.end(),
              [&](Point2D const& a, Point2D const& b) { return segs[i].Location(a) < segs[i].Location(b); });
    cps.erase(std::unique(cps.begin(), cps.end(), [](Point2D const& a, Point2D const& b) { return a.AlmostEquals(b); }),
              cps.end());

    Point2D prev = segs[i].First();
    for (auto const& cp : cps) {
      if (!prev.AlmostEquals(cp)) {
        split.push_back(LineSegment2D::Make(prev, cp));
      }
      prev = cp;
    }
    if (!prev.AlmostEquals(segs[i].Last())) {
      split.push_back(LineSegment2D::Make(prev, segs[i].Last()));
    }
  }
  return split;
}

// Everything in namespace view projects points through a View2D before operating on them.
// Grouped together since they all share that one dependency.
namespace view {

template <PointContainer Points>
std::vector<std::size_t> convex_hull_monotone_chain(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  if (n < 3) {
    return {};
  }

  std::vector<std::size_t> indices(n);
  std::iota(indices.begin(), indices.end(), std::size_t{0});

  std::sort(indices.begin(), indices.end(), [&](std::size_t i, std::size_t j) {
    double xi = view.x(points[i]), xj = view.x(points[j]);
    if (xi != xj) {
      return xi < xj;
    }
    return view.y(points[i]) < view.y(points[j]);
  });

  auto is_left = [&](std::size_t o, std::size_t a, std::size_t b) {
    auto [ox, oy] = view.xy(points[o]);
    auto [ax, ay] = view.xy(points[a]);
    auto [bx, by] = view.xy(points[b]);
    return compare((ax - ox) * (by - oy) - (ay - oy) * (bx - ox), 0) > 0;
  };

  std::vector<std::size_t> hull;
  hull.reserve(2 * n);

  // Lower hull
  for (std::size_t i = 0; i < n; ++i) {
    while (hull.size() >= 2 && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  // Upper hull
  std::size_t lower_hull_size = hull.size();
  for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(n) - 2; i >= 0; --i) {
    while (hull.size() > lower_hull_size && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  if (!hull.empty()) {
    hull.pop_back();
  }
  return hull;
}

// template specialization
template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point2D> const&, View2D const&);

template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point3D> const&, View2D const&);

}  // namespace view

template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points(R const& vertices, bool is_convex, V const& dir) {
  long const n = static_cast<long>(std::ranges::distance(vertices));
  if (n == 0) {
    throw std::invalid_argument("extreme_points: empty vertex range");
  }

  // projection of vertex i (cyclic index) onto the direction
  auto proj = [&](long i) -> double {
    return vertices[static_cast<std::size_t>(((i % n) + n) % n)].ToVector().Dot(dir);
  };

  // convex fast-path — Daniel Sunday's O(log n) binary search for the extreme vertex of a convex CCW
  // polygon in a direction (cf. the KACTL "extrVertex"). search(+1) returns argmax(proj); search(-1)
  // returns argmax(-proj) = argmin(proj). Ties (an edge perpendicular to dir) resolve to one endpoint.
  if (is_convex) {
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };

    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };                                 // maximise s * proj
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };                  // sign( g(j) - g(i) )
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };  // i is the peak
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = n;
      while (lo + 1 < hi) {
        long m = (lo + hi) / 2;
        if (extr(m)) {
          return static_cast<std::size_t>(m);
        }
        int ls = cmp(lo + 1, lo);
        int ms = cmp(m + 1, m);
        bool go_hi = (ls < ms) || (ls == ms && ls == cmp(lo, m));
        if (go_hi) {
          hi = m;
        } else {
          lo = m;
        }
      }
      return static_cast<std::size_t>(lo);
    };

    std::size_t max_i = search(1.0);
    std::size_t min_i = search(-1.0);
    return {min_i, max_i};
  }

  // brute force O(n): a concave ring has no monotone structure to exploit
  std::size_t min_i = 0, max_i = 0;
  double min_v = proj(0), max_v = proj(0);
  for (long i = 1; i < n; ++i) {
    double v = proj(i);
    if (v < min_v) {
      min_v = v;
      min_i = static_cast<std::size_t>(i);
    }
    if (v > max_v) {
      max_v = v;
      max_i = static_cast<std::size_t>(i);
    }
  }
  return {min_i, max_i};
}

template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point2D> const&, bool, Vector2D const&);
template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point3D> const&, bool, Vector3D const&);

namespace view {

template <PointContainer Points>
MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const& hull_indices, Points const& points,
                                        View2D const& view) {
  std::size_t m = hull_indices.size();

  double best_area = std::numeric_limits<double>::max();
  double best_ux = 1.0, best_uy = 0.0, best_vx = 0.0, best_vy = 1.0;
  double best_umin = 0.0, best_umax = 0.0, best_vmin = 0.0, best_vmax = 0.0;
  double best_ox = 0.0, best_oy = 0.0;

  for (std::size_t i = 0; i < m; ++i) {
    auto [p0x, p0y] = view.xy(points[hull_indices[i]]);
    auto [p1x, p1y] = view.xy(points[hull_indices[(i + 1) % m]]);

    double ex = p1x - p0x;
    double ey = p1y - p0y;
    double elen = std::sqrt(ex * ex + ey * ey);
    if (compare(elen, 0.0) == 0) {
      continue;
    }

    double ux = ex / elen;
    double uy = ey / elen;
    double vx = -uy;
    double vy = ux;

    double u_min = std::numeric_limits<double>::max();
    double u_max = -std::numeric_limits<double>::max();
    double v_min = std::numeric_limits<double>::max();
    double v_max = -std::numeric_limits<double>::max();

    for (std::size_t j = 0; j < m; ++j) {
      auto [qx, qy] = view.xy(points[hull_indices[j]]);
      double dx = qx - p0x;
      double dy = qy - p0y;
      double pu = dx * ux + dy * uy;
      double pv = dx * vx + dy * vy;
      if (pu < u_min) {
        u_min = pu;
      }
      if (pu > u_max) {
        u_max = pu;
      }
      if (pv < v_min) {
        v_min = pv;
      }
      if (pv > v_max) {
        v_max = pv;
      }
    }

    double rect_area = (u_max - u_min) * (v_max - v_min);
    if (compare(rect_area, best_area) < 0) {
      best_area = rect_area;
      best_ux = ux;
      best_uy = uy;
      best_vx = vx;
      best_vy = vy;
      best_umin = u_min;
      best_umax = u_max;
      best_vmin = v_min;
      best_vmax = v_max;
      best_ox = p0x;
      best_oy = p0y;
    }
  }

  double cu = (best_umin + best_umax) / 2.0;
  double cv = (best_vmin + best_vmax) / 2.0;

  return MinBoundingRectResult{
      best_ux, best_uy, best_vx, best_vy, (best_umax - best_umin) / 2.0, (best_vmax - best_vmin) / 2.0,
      cu,      cv,      best_ox, best_oy,
  };
}

template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point2D> const&,
                                                 View2D const&);
template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point3D> const&,
                                                 View2D const&);

}  // namespace view

std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points) {
  return view::convex_hull_monotone_chain(points, View2D::XY());
}

namespace view {

template <PointContainer Points>
std::vector<LineSegment2D> collect_ring_segments(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view) {
  std::vector<LineSegment2D> segs;
  int n = static_cast<int>(std::ranges::size(outer));
  segs.reserve(n);
  for (int i = 0; i < n; ++i) {
    segs.emplace_back(LineSegment2D::Make(Point2D(view.x(outer[i]), view.y(outer[i])),
                                          Point2D(view.x(outer[(i + 1) % n]), view.y(outer[(i + 1) % n]))));
  }
  for (auto const& hole : holes) {
    int nh = static_cast<int>(std::ranges::size(hole));
    for (int i = 0; i < nh; ++i) {
      segs.emplace_back(LineSegment2D::Make(Point2D(view.x(hole[i]), view.y(hole[i])),
                                            Point2D(view.x(hole[(i + 1) % nh]), view.y(hole[(i + 1) % nh]))));
    }
  }
  return segs;
}

template std::vector<LineSegment2D> collect_ring_segments(std::vector<Point2D> const&,
                                                          std::vector<std::vector<Point2D>> const&, View2D const&);
template std::vector<LineSegment2D> collect_ring_segments(std::vector<Point3D> const&,
                                                          std::vector<std::vector<Point3D>> const&, View2D const&);

template <PointContainer Points>
std::vector<std::vector<Point2D>> simplify_rings(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view) {
  auto segs = collect_ring_segments(outer, holes, view);

  if (segs.size() < 3) {
    throw std::invalid_argument("simplify_rings: need at least 3 segments");
  }

  auto split = split_segments_at_crossings(segs);

  // --- Stage 3: build planar graph, trace half-edge faces ---

  // Assign stable integer IDs to unique vertices (using rounded keys)
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  auto vkey = [scale](Point2D const& p) -> std::pair<long long, long long> {
    return {llround(p.x() * scale), llround(p.y() * scale)};
  };

  std::map<std::pair<long long, long long>, int> vid_map;
  std::vector<Point2D> verts;
  auto get_vid = [&](Point2D const& p) -> int {
    auto k = vkey(p);
    auto it = vid_map.find(k);
    if (it != vid_map.end()) {
      return it->second;
    }
    int id = static_cast<int>(verts.size());
    vid_map[k] = id;
    verts.push_back(p);
    return id;
  };

  // Undirected adjacency list — push all edges, deduplicate after sorting.
  std::vector<std::vector<int>> adj;
  for (auto const& seg : split) {
    int a = get_vid(seg.First());
    int b = get_vid(seg.Last());
    if (static_cast<int>(adj.size()) <= std::max(a, b)) {
      adj.resize(static_cast<std::size_t>(std::max(a, b)) + 1);
    }
    adj[a].push_back(b);
    adj[b].push_back(a);
  }
  adj.resize(verts.size());

  // Sort each neighbor list by polar angle CCW around the vertex, then deduplicate.
  // Precompute angles alongside neighbor IDs for O(log degree) lookup in the half-edge walk.
  int nv = static_cast<int>(verts.size());
  std::vector<std::vector<double>> adj_angles(nv);
  for (int v = 0; v < nv; ++v) {
    std::sort(adj[v].begin(), adj[v].end(), [&](int a, int b) {
      double ax = verts[a].x() - verts[v].x(), ay = verts[a].y() - verts[v].y();
      double bx = verts[b].x() - verts[v].x(), by = verts[b].y() - verts[v].y();
      return std::atan2(ay, ax) < std::atan2(by, bx);
    });
    adj[v].erase(std::unique(adj[v].begin(), adj[v].end()), adj[v].end());
    adj_angles[v].resize(adj[v].size());
    for (int k = 0; k < static_cast<int>(adj[v].size()); ++k) {
      adj_angles[v][k] = std::atan2(verts[adj[v][k]].y() - verts[v].y(), verts[adj[v][k]].x() - verts[v].x());
    }
  }

  // Walk half-edges: for directed (cur_from→cur_to), the next half-edge in the
  // same face is (cur_to→w) where w is the first CCW neighbor from cur_to after
  // the reversed-incoming direction (i.e., the direction back toward cur_from).
  std::set<std::pair<int, int>> used;
  std::vector<std::vector<Point2D>> rings;

  for (int u = 0; u < nv; ++u) {
    for (int v0 : adj[u]) {
      if (used.count({u, v0})) {
        continue;
      }

      std::vector<Point2D> ring;
      int cur_from = u, cur_to = v0;

      while (true) {
        auto edge = std::make_pair(cur_from, cur_to);
        if (used.count(edge)) {
          break;
        }
        used.insert(edge);
        ring.push_back(verts[cur_from]);

        double rev_angle = std::atan2(verts[cur_from].y() - verts[cur_to].y(), verts[cur_from].x() - verts[cur_to].x());

        auto const& angles = adj_angles[cur_to];
        auto const& nbrs = adj[cur_to];
        int deg = static_cast<int>(nbrs.size());
        int next_to = -1;

        if (deg > 0) {
          auto it = std::upper_bound(angles.begin(), angles.end(), rev_angle);
          if (it == angles.end()) {
            next_to = nbrs[0];
          } else {
            next_to = nbrs[static_cast<int>(it - angles.begin())];
          }
        }

        if (next_to == -1) {
          break;
        }
        cur_from = cur_to;
        cur_to = next_to;
      }

      if (ring.size() >= 3) {
        rings.push_back(std::move(ring));
      }
    }
  }

  // Exactly one of the traced rings is the unbounded "outside" face of the planar arrangement (a
  // topological fact — Euler's formula for a planar graph guarantees exactly one), and it's ALWAYS the
  // one with the opposite orientation sign from the real interior faces. But WHICH absolute sign (CW vs
  // CCW) means "interior" isn't fixed — it depends on how `outer` itself is embedded in this (view.x,
  // view.y) space: a validly-CCW outer ring normally embeds as positive-area, making interior faces come
  // out negative; a view that happens to mirror chirality (e.g. a Y-dominant-axis View2D for Polygon3D —
  // see its own callers' history) flips that. Calibrated here, once, against outer's own projected sign —
  // callers used to each duplicate this exact calibration (Polygon2D/3D::Simplify(), Polygon2D/3D::Area())
  // — so from here on this function always returns just the real interior faces, each already flipped to
  // positive (CCW) orientation, with the single unbounded outside face silently dropped.
  std::vector<Point2D> outer2d;
  {
    int no = static_cast<int>(std::ranges::size(outer));
    outer2d.reserve(no);
    for (int i = 0; i < no; ++i) {
      outer2d.emplace_back(view.x(outer[i]), view.y(outer[i]));
    }
  }
  // Qualified as geompp::signed_area (not a plain unqualified call): this function lives inside namespace
  // detail, so an unqualified call would be ambiguous between this scope's own detail::signed_area and the
  // public one pulled in via ADL on Point2D -- and outer2d/ring below are never proven collinear-free, so
  // the full remove_collinear-checking public path is the correct one to use anyway.
  bool flipped = compare(geompp::signed_area(outer2d), 0.0) < 0;

  std::vector<std::vector<Point2D>> interior_rings;
  interior_rings.reserve(rings.size());
  for (auto& ring : rings) {
    // signed_area() itself calls remove_collinear() and throws if that leaves fewer than 3 unique points —
    // a real possibility here (the half-edge walk only guarantees ring.size() >= 3 raw points, not 3+
    // genuinely non-collinear ones e.g. a degenerate zero-area sliver traced at a near-tangent crossing).
    // Caught per-ring (matches what every caller of this function used to do individually) so one
    // degenerate trace doesn't take down the whole decomposition.
    try {
      double sa = geompp::signed_area(ring);
      bool is_interior = flipped ? compare(sa, 0.0) > 0 : compare(sa, 0.0) < 0;
      if (!is_interior) {
        continue;  // the single unbounded outside face (or a degenerate zero-area trace)
      }
      if (!flipped) {
        std::reverse(ring.begin(), ring.end());
      }
      interior_rings.push_back(std::move(ring));
    } catch (std::runtime_error const& e) {
      GEOMPP_LOG(WARNING) << "simplify_rings: skipping degenerate ring (" << ring.size() << " pts): " << e.what();
    }
  }
  return interior_rings;
}

template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point2D> const&,
                                                          std::vector<std::vector<Point2D>> const&, View2D const&);

template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point3D> const&,
                                                          std::vector<std::vector<Point3D>> const&, View2D const&);

}  // namespace view

int winding_number(std::vector<Point2D> const& vertices, Point2D const& p) {
  int wn = 0;
  int n = static_cast<int>(vertices.size());
  for (int i1 = 0; i1 < n; ++i1) {
    int i2 = (i1 + 1) % n;
    auto const& v1 = vertices[i1];
    auto const& v2 = vertices[i2];
    if (compare(v1.y(), p.y()) <= 0) {   // edge starts below p
      if (compare(v2.y(), p.y()) > 0) {  //   ends above p (upward crossing)
        if (is_left(v1, v2, p)) {        //   p left of the edge
          ++wn;
        }
      }
    } else {                              // edge starts above p
      if (compare(v2.y(), p.y()) <= 0) {  //   ends below p (downward crossing)
        if (is_right(v1, v2, p)) {        //   p right of the edge
          --wn;
        }
      }
    }
  }
  return wn;
}

namespace {

bool select_face(BooleanOp op, bool in_subj, bool in_clip) {
  switch (op) {
    case BooleanOp::Union:
      return in_subj || in_clip;
    case BooleanOp::Intersection:
      return in_subj && in_clip;
    case BooleanOp::Difference:
      return in_subj && !in_clip;
    case BooleanOp::Xor:
      return in_subj != in_clip;
  }
  throw std::logic_error("invalid BooleanOp");
}

// A point guaranteed to sit just inside a simple ring, robust to concavity: nudges off the longest
// edge's midpoint (longest for numerical safety) along the interior-side normal, using CCW/CW to know
// which side "interior" is.
Point2D interior_sample_point(std::vector<Point2D> const& ring) {
  int n = static_cast<int>(ring.size());
  int best_i = 0;
  double best_len = -1.0;
  for (int i = 0; i < n; ++i) {
    double len = ring[i].DistanceTo(ring[(i + 1) % n]);
    if (len > best_len) {
      best_len = len;
      best_i = i;
    }
  }
  Point2D const& p0 = ring[best_i];
  Point2D const& p1 = ring[(best_i + 1) % n];
  Vector2D dir = (p1 - p0).Normalize();
  Vector2D normal = dir.Perp();  // left normal — interior side for a CCW ring
  // Qualified as geompp::are_ccw (not a plain unqualified call): `ring` here is a boolean-op result ring,
  // not proven collinear-free, so this must stay on the full remove_collinear-checking public path -- and
  // since this function itself lives inside namespace detail, an unqualified call would be ambiguous
  // between this scope's detail::are_ccw and the public one pulled in via ADL on Point2D.
  if (!geompp::are_ccw(ring)) {
    normal = -normal;  // interior is to the right for a CW ring
  }
  Point2D mid = LineSegment2D::Make(p0, p1).Interpolate(0.5);
  // Same reasoning as classify_and_orient's floor: winding_number()'s own comparisons use the current
  // DOUBLE_EPSILON as their tolerance, so this nudge must stay comfortably larger than it or the sample
  // point reads as "on the boundary" instead of "definitely inside." Requires DOUBLE_EPSILON to stay
  // small relative to the ring's own scale — same assumption this codebase's comparisons always make.
  double eps = std::max(best_len * 0.01, DOUBLE_EPSILON * 10);
  return mid + normal * eps;
}

// Classifies each split segment by sampling just left/right of its midpoint and keeps only the ones
// where op's truth table differs left vs. right (a genuine boundary of the result), oriented so the
// selected side ends up on the left.
//
// NOTE: this — like any classifier here — depends on split having no crossing in a fragment's interior.
// find_intersections' Bentley-Ottmann sweep used to occasionally MISS a genuine segment crossing (fixed via
// SweepLine2D::ReverseRun — see BooleanOp_MissedCrossing in test_polygon2d.cpp for the root cause), which
// would leave a fragment straddling the other operand's boundary and the midpoint sample would classify
// the whole straddling fragment by one side, dropping the part on the other.
std::vector<LineSegment2D> classify_and_orient(std::vector<LineSegment2D> const& split,
                                               std::vector<Point2D> const& subj_outer,
                                               std::vector<std::vector<Point2D>> const& subj_holes,
                                               std::vector<Point2D> const& clip_outer,
                                               std::vector<std::vector<Point2D>> const& clip_holes, BooleanOp op) {
  std::vector<LineSegment2D> result;
  result.reserve(split.size());

  for (auto const& seg : split) {
    Vector2D dir = (seg.Last() - seg.First()).Normalize();
    Vector2D normal = dir.Perp();
    Point2D mid = seg.Interpolate(0.5);
    // The floor must stay comfortably larger than DOUBLE_EPSILON: polygon_contains() below tests each
    // sample using the current DOUBLE_EPSILON as ITS tolerance, so a nudge smaller than that reads as
    // "on the boundary" rather than definitely inside/outside. This requires DOUBLE_EPSILON to be small
    // relative to the geometry's own scale — the same assumption every AlmostEquals-based comparison in
    // this codebase makes.
    double eps = std::max(seg.Length() * 0.01, DOUBLE_EPSILON * 10);
    Point2D left_pt = mid + normal * eps;
    Point2D right_pt = mid + normal * (-eps);

    bool left_in_subj = view::polygon_contains(subj_outer, subj_holes, View2D::XY(), left_pt.x(), left_pt.y());
    bool left_in_clip = view::polygon_contains(clip_outer, clip_holes, View2D::XY(), left_pt.x(), left_pt.y());
    bool right_in_subj = view::polygon_contains(subj_outer, subj_holes, View2D::XY(), right_pt.x(), right_pt.y());
    bool right_in_clip = view::polygon_contains(clip_outer, clip_holes, View2D::XY(), right_pt.x(), right_pt.y());

    bool left_sel = select_face(op, left_in_subj, left_in_clip);
    bool right_sel = select_face(op, right_in_subj, right_in_clip);

    if (left_sel == right_sel) {
      continue;  // not a boundary of the result — either fully inside or fully outside on both sides
    }
    result.push_back(left_sel ? seg : seg.Reversed());
  }
  return result;
}

// Half-edge walk over a DIRECTED edge set (unlike simplify_rings' tracer, each surviving edge here
// contributes only the one direction classify_and_orient chose, so the walk can never backtrack along
// it). Same angular "next half-edge in this face" rule, restricted to each vertex's outgoing edges.
std::vector<std::vector<Point2D>> trace_directed_boundary(std::vector<LineSegment2D> const& directed_edges) {
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  auto vkey = [scale](Point2D const& p) -> std::pair<long long, long long> {
    return {llround(p.x() * scale), llround(p.y() * scale)};
  };

  std::map<std::pair<long long, long long>, int> vid_map;
  std::vector<Point2D> verts;
  auto get_vid = [&](Point2D const& p) -> int {
    auto k = vkey(p);
    auto it = vid_map.find(k);
    if (it != vid_map.end()) {
      return it->second;
    }
    int id = static_cast<int>(verts.size());
    vid_map[k] = id;
    verts.push_back(p);
    return id;
  };

  std::vector<std::pair<int, int>> directed;
  directed.reserve(directed_edges.size());
  for (auto const& seg : directed_edges) {
    directed.push_back({get_vid(seg.First()), get_vid(seg.Last())});
  }

  std::vector<std::vector<int>> adj(verts.size());
  for (auto const& [a, b] : directed) {
    adj[a].push_back(b);
  }

  int nv = static_cast<int>(verts.size());
  std::vector<std::vector<double>> adj_angles(nv);
  for (int v = 0; v < nv; ++v) {
    std::sort(adj[v].begin(), adj[v].end(), [&](int a, int b) {
      double ax = verts[a].x() - verts[v].x(), ay = verts[a].y() - verts[v].y();
      double bx = verts[b].x() - verts[v].x(), by = verts[b].y() - verts[v].y();
      return std::atan2(ay, ax) < std::atan2(by, bx);
    });
    adj_angles[v].resize(adj[v].size());
    for (int k = 0; k < static_cast<int>(adj[v].size()); ++k) {
      adj_angles[v][k] = std::atan2(verts[adj[v][k]].y() - verts[v].y(), verts[adj[v][k]].x() - verts[v].x());
    }
  }

  std::set<std::pair<int, int>> used;
  std::vector<std::vector<Point2D>> rings;

  for (int u = 0; u < nv; ++u) {
    for (int v0 : adj[u]) {
      if (used.count({u, v0})) {
        continue;
      }

      std::vector<Point2D> ring;
      // vertex id -> index in `ring` where this walk first visited it. A vertex revisited mid-walk means
      // the walk has closed a sub-loop at a pinch point — two faces that only share that one vertex (e.g.
      // a bowtie's two lobes, or two Simplify()-decomposed pieces of the same operand touching at a
      // corner). Without this, the walk threads straight through and merges both faces into one
      // self-touching boundary instead of stopping there: the "next edge by angle" rule alone only stops
      // on an already-USED EDGE, not an already-visited VERTEX, and unlike simplify_rings' undirected
      // tracer (which always has a genuine twin half-edge to rotate from at every vertex), this directed,
      // single-half-edge-per-boundary-edge graph has no such twin to naturally break the tie. Peel the
      // closed portion off as its own ring and keep tracing the remainder from the repeated vertex —
      // handles 3+-way pinches too, one peel at a time.
      std::map<int, std::size_t> visit_pos;
      int cur_from = u, cur_to = v0;

      while (true) {
        auto edge = std::make_pair(cur_from, cur_to);
        if (used.count(edge)) {
          break;
        }
        used.insert(edge);

        auto seen = visit_pos.find(cur_from);
        if (seen != visit_pos.end()) {
          std::size_t const cut = seen->second;  // captured before erasing — `seen` itself is in range
          std::vector<Point2D> sub(ring.begin() + static_cast<std::ptrdiff_t>(cut), ring.end());
          if (sub.size() >= 3) {
            rings.push_back(std::move(sub));
          }
          ring.erase(ring.begin() + static_cast<std::ptrdiff_t>(cut), ring.end());
          for (auto vit = visit_pos.begin(); vit != visit_pos.end();) {
            if (vit->second >= cut) {
              vit = visit_pos.erase(vit);
            } else {
              ++vit;
            }
          }
        }
        visit_pos[cur_from] = ring.size();
        ring.push_back(verts[cur_from]);

        double rev_angle = std::atan2(verts[cur_from].y() - verts[cur_to].y(), verts[cur_from].x() - verts[cur_to].x());

        auto const& angles = adj_angles[cur_to];
        auto const& nbrs = adj[cur_to];
        int deg = static_cast<int>(nbrs.size());
        int next_to = -1;

        if (deg > 0) {
          auto it = std::upper_bound(angles.begin(), angles.end(), rev_angle);
          next_to = (it == angles.end()) ? nbrs[0] : nbrs[static_cast<int>(it - angles.begin())];
        }

        if (next_to == -1) {
          break;
        }
        cur_from = cur_to;
        cur_to = next_to;
      }

      if (ring.size() >= 3) {
        rings.push_back(std::move(ring));
      }
    }
  }
  return rings;
}

// Groups traced rings into {outer, holes} via a containment forest: a ring's immediate parent is the
// smallest-area other ring whose interior contains its sample point. Even-depth CCW rings are genuine
// outer boundaries; odd-depth CW rings are holes of their immediate (even-depth) parent. This works
// regardless of whether an outer and its hole ever share an edge — nesting is a geometric test, not a
// graph-connectivity one — which is exactly what's needed when one operand fully contains the other
// with no shared boundary.
std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> package_result_rings(
    std::vector<std::vector<Point2D>> const& raw_rings) {
  // Raw shoelace (no remove_collinear, so it never throws on a near-degenerate ring) — used both to
  // filter out zero-area slivers and to size rings for nesting. A boolean op can legitimately produce a
  // sliver ring at a grazing contact; it contributes no area and isn't a valid Polygon2D, so drop it
  // here rather than letting signed_area()/Make() throw downstream.
  auto raw_signed_area = [](std::vector<Point2D> const& r) {
    double a = 0.0;
    int m = static_cast<int>(r.size());
    for (int i = 0; i < m; ++i) {
      auto const& p = r[i];
      auto const& q = r[(i + 1) % m];
      a += p.x() * q.y() - q.x() * p.y();
    }
    return 0.5 * a;
  };

  std::vector<std::vector<Point2D>> rings;
  rings.reserve(raw_rings.size());
  for (auto const& r : raw_rings) {
    if (r.size() >= 3 && std::abs(raw_signed_area(r)) > DOUBLE_EPSILON) {
      rings.push_back(r);
    }
  }

  int n = static_cast<int>(rings.size());
  std::vector<Point2D> sample;
  sample.reserve(n);
  std::vector<double> abs_area(n);
  for (int i = 0; i < n; ++i) {
    sample.push_back(interior_sample_point(rings[i]));
    abs_area[i] = std::abs(raw_signed_area(rings[i]));
  }

  std::vector<int> parent(n, -1);
  for (int i = 0; i < n; ++i) {
    double best_area = std::numeric_limits<double>::infinity();
    for (int j = 0; j < n; ++j) {
      if (i == j || abs_area[j] <= abs_area[i]) {
        continue;  // a valid parent must strictly enclose i, so it must be strictly larger
      }
      if (winding_number(rings[j], sample[i]) != 0 && abs_area[j] < best_area) {
        parent[i] = j;
        best_area = abs_area[j];
      }
    }
  }

  std::vector<int> depth(n, -1);
  for (int i = 0; i < n; ++i) {
    if (depth[i] != -1) {
      continue;
    }
    std::vector<int> chain;
    int cur = i;
    while (cur != -1 && depth[cur] == -1) {
      chain.push_back(cur);
      cur = parent[cur];
    }
    int base_depth = (cur == -1) ? 0 : depth[cur] + 1;
    for (int k = static_cast<int>(chain.size()) - 1; k >= 0; --k) {
      depth[chain[k]] = base_depth + (static_cast<int>(chain.size()) - 1 - k);
    }
  }

  // Qualified as geompp::are_ccw for the same reason as interior_sample_point() above: rings[i] isn't
  // proven collinear-free, and an unqualified call here (inside namespace detail) would be ambiguous with
  // this scope's own detail::are_ccw.
  std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> result;
  std::map<int, std::size_t> outer_index;
  for (int i = 0; i < n; ++i) {
    if (depth[i] % 2 == 0 && geompp::are_ccw(rings[i])) {
      outer_index[i] = result.size();
      result.push_back({rings[i], {}});
    }
  }
  for (int i = 0; i < n; ++i) {
    if (depth[i] % 2 == 1 && !geompp::are_ccw(rings[i])) {
      auto it = outer_index.find(parent[i]);
      if (it != outer_index.end()) {
        result[it->second].second.push_back(rings[i]);
      }
    }
  }
  return result;
}

// A split fragment tagged with which operand's piece it came from. Meaningful only when every piece of
// both operands is simple and CCW-outer/CW-hole oriented (RingPieces' contract): under that convention a
// fragment's own-operand interior is always on its left, so classification never needs to probe it — only
// the OTHER operand needs a sample point, on each side.
struct SourceTaggedSegment {
  LineSegment2D Seg;
  bool FromSubject;
};

// Same crossing-split logic as split_segments_at_crossings, but over two operand segment pools kept
// separate so every output fragment can be tagged with its origin. Crossings are still found across the
// COMBINED pool in one sweep (subject-clip crossings are exactly what boolean ops care about), matching
// boolean_op's arrangement exactly — this only adds provenance to the result.
std::vector<SourceTaggedSegment> split_segments_at_crossings_tagged(std::vector<LineSegment2D> const& subj_segs,
                                                                    std::vector<LineSegment2D> const& clip_segs) {
  std::vector<LineSegment2D> segs = subj_segs;
  segs.insert(segs.end(), clip_segs.begin(), clip_segs.end());
  std::size_t const subj_count = subj_segs.size();

  auto crossings = detail::find_intersections(segs);

  std::map<std::size_t, std::vector<Point2D>> seg_cp;
  for (auto const& ev : crossings) {
    for (auto id : ev.SegmentIds) {
      seg_cp[id].push_back(ev.Point);
    }
  }

  std::vector<SourceTaggedSegment> split;
  split.reserve(segs.size() + crossings.size() * 2);
  for (std::size_t i = 0; i < segs.size(); ++i) {
    bool from_subj = i < subj_count;
    auto it = seg_cp.find(i);
    if (it == seg_cp.end()) {
      split.push_back({segs[i], from_subj});
      continue;
    }
    std::vector<Point2D> cps = it->second;
    std::sort(cps.begin(), cps.end(),
              [&](Point2D const& a, Point2D const& b) { return segs[i].Location(a) < segs[i].Location(b); });
    cps.erase(std::unique(cps.begin(), cps.end(), [](Point2D const& a, Point2D const& b) { return a.AlmostEquals(b); }),
              cps.end());

    Point2D prev = segs[i].First();
    for (auto const& cp : cps) {
      if (!prev.AlmostEquals(cp)) {
        split.push_back({LineSegment2D::Make(prev, cp), from_subj});
      }
      prev = cp;
    }
    if (!prev.AlmostEquals(segs[i].Last())) {
      split.push_back({LineSegment2D::Make(prev, segs[i].Last()), from_subj});
    }
  }
  return split;
}

// Removes internal seams: two fragments, same operand, exact reverses of each other (coincident edge
// shared by two of that operand's OWN pieces — e.g. two Simplify() faces touching along a whole edge, not
// just a point). Both sides of such a seam are interior to the operand, so it contributes nothing to any
// boolean op and must vanish before classify_and_orient_source_tagged trusts "own side = always interior".
// Without this, two same-operand pieces sharing an edge would each claim that edge as their own boundary,
// wrongly promoting an internal seam to a result boundary.
void cancel_coincident_same_operand_pairs(std::vector<SourceTaggedSegment>& segs) {
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  auto vkey = [scale](Point2D const& p) -> std::pair<long long, long long> {
    return {llround(p.x() * scale), llround(p.y() * scale)};
  };
  using PKey = std::pair<long long, long long>;

  std::multimap<std::pair<PKey, PKey>, std::size_t> groups;
  for (std::size_t i = 0; i < segs.size(); ++i) {
    PKey a = vkey(segs[i].Seg.First()), b = vkey(segs[i].Seg.Last());
    groups.insert({a < b ? std::make_pair(a, b) : std::make_pair(b, a), i});
  }

  std::vector<bool> cancelled(segs.size(), false);
  for (auto it = groups.begin(); it != groups.end();) {
    auto range_end = groups.upper_bound(it->first);
    std::vector<std::size_t> idxs;
    for (auto j = it; j != range_end; ++j) {
      idxs.push_back(j->second);
    }
    for (std::size_t a = 0; a < idxs.size(); ++a) {
      if (cancelled[idxs[a]]) {
        continue;
      }
      for (std::size_t b = a + 1; b < idxs.size(); ++b) {
        if (cancelled[idxs[b]]) {
          continue;
        }
        auto const& sa = segs[idxs[a]];
        auto const& sb = segs[idxs[b]];
        if (sa.FromSubject == sb.FromSubject && vkey(sa.Seg.First()) == vkey(sb.Seg.Last()) &&
            vkey(sa.Seg.Last()) == vkey(sb.Seg.First())) {
          cancelled[idxs[a]] = cancelled[idxs[b]] = true;
          break;
        }
      }
    }
    it = range_end;
  }

  std::vector<SourceTaggedSegment> kept;
  kept.reserve(segs.size());
  for (std::size_t i = 0; i < segs.size(); ++i) {
    if (!cancelled[i]) {
      kept.push_back(std::move(segs[i]));
    }
  }
  segs = std::move(kept);
}

bool piece_set_contains(RingPieces const& pieces, double px, double py) {
  for (auto const& [outer, holes] : pieces) {
    if (view::polygon_contains(outer, holes, View2D::XY(), px, py)) {
      return true;
    }
  }
  return false;
}

// Source-tagged counterpart of classify_and_orient: the segment's own-operand side is known outright
// (true on the left, false on the right — the CCW-outer/CW-hole convention every RingPieces entry is
// required to satisfy), so only the OTHER operand needs probing, left and right. Half the polygon_contains
// calls of classify_and_orient, and the own side no longer depends on the nudge epsilon at all.
std::vector<LineSegment2D> classify_and_orient_source_tagged(std::vector<SourceTaggedSegment> const& split,
                                                             RingPieces const& subj_pieces,
                                                             RingPieces const& clip_pieces, BooleanOp op) {
  std::vector<LineSegment2D> result;
  result.reserve(split.size());

  for (auto const& ts : split) {
    auto const& seg = ts.Seg;
    Vector2D dir = (seg.Last() - seg.First()).Normalize();
    Vector2D normal = dir.Perp();
    Point2D mid = seg.Interpolate(0.5);
    double eps = std::max(seg.Length() * 0.01, DOUBLE_EPSILON * 10);
    Point2D left_pt = mid + normal * eps;
    Point2D right_pt = mid + normal * (-eps);

    RingPieces const& other_pieces = ts.FromSubject ? clip_pieces : subj_pieces;
    bool left_in_other = piece_set_contains(other_pieces, left_pt.x(), left_pt.y());
    bool right_in_other = piece_set_contains(other_pieces, right_pt.x(), right_pt.y());

    bool left_in_subj = ts.FromSubject ? true : left_in_other;
    bool left_in_clip = ts.FromSubject ? left_in_other : true;
    bool right_in_subj = ts.FromSubject ? false : right_in_other;
    bool right_in_clip = ts.FromSubject ? right_in_other : false;

    bool left_sel = select_face(op, left_in_subj, left_in_clip);
    bool right_sel = select_face(op, right_in_subj, right_in_clip);

    if (left_sel == right_sel) {
      continue;
    }
    result.push_back(left_sel ? seg : seg.Reversed());
  }
  return result;
}

}  // namespace

std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> boolean_op(
    std::vector<Point2D> const& subj_outer, std::vector<std::vector<Point2D>> const& subj_holes,
    std::vector<Point2D> const& clip_outer, std::vector<std::vector<Point2D>> const& clip_holes, BooleanOp op) {
  auto segs = view::collect_ring_segments(subj_outer, subj_holes, View2D::XY());
  auto clip_segs = view::collect_ring_segments(clip_outer, clip_holes, View2D::XY());
  segs.insert(segs.end(), clip_segs.begin(), clip_segs.end());

  auto split = split_segments_at_crossings(segs);
  auto directed = classify_and_orient(split, subj_outer, subj_holes, clip_outer, clip_holes, op);
  auto rings = trace_directed_boundary(directed);
  return package_result_rings(rings);
}

RingPieces boolean_op_multi(RingPieces const& subj_pieces, RingPieces const& clip_pieces, BooleanOp op) {
  std::vector<LineSegment2D> subj_segs, clip_segs;
  for (auto const& [outer, holes] : subj_pieces) {
    auto s = view::collect_ring_segments(outer, holes, View2D::XY());
    subj_segs.insert(subj_segs.end(), s.begin(), s.end());
  }
  for (auto const& [outer, holes] : clip_pieces) {
    auto s = view::collect_ring_segments(outer, holes, View2D::XY());
    clip_segs.insert(clip_segs.end(), s.begin(), s.end());
  }

  auto split = split_segments_at_crossings_tagged(subj_segs, clip_segs);
  cancel_coincident_same_operand_pairs(split);
  auto directed = classify_and_orient_source_tagged(split, subj_pieces, clip_pieces, op);
  auto rings = trace_directed_boundary(directed);
  return package_result_rings(rings);
}

namespace view {

template <PointContainer Points>
bool is_ccw(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  double area2 = 0.0;
  for (std::size_t i = 0; i < n; ++i) {
    double x0 = view.x(points[i]), y0 = view.y(points[i]);
    double x1 = view.x(points[(i + 1) % n]), y1 = view.y(points[(i + 1) % n]);
    area2 += x0 * y1 - x1 * y0;
  }
  return compare(area2, 0.0) > 0;
}

template bool is_ccw(std::vector<Point2D> const&, View2D const&);
template bool is_ccw(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool has_collinears(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  for (std::size_t i = 0; i < n; ++i) {
    auto [x0, y0] = view.xy(points[i]);
    auto [x1, y1] = view.xy(points[(i + 1) % n]);
    auto [x2, y2] = view.xy(points[(i + 2) % n]);

    // A duplicate needs no separate check: if points[i] == points[i+1] (or points[i+1] == points[i+2]),
    // one of the two edge vectors below is the zero vector, so the cross product is already trivially zero.
    double cross = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);  // zero turn at points[i+1]

    if (compare(cross, 0.0) == 0) {
      return true;
    }
  }
  return false;
}

template bool has_collinears(std::vector<Point2D> const&, View2D const&);
template bool has_collinears(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool is_convex(Points const& vertices, View2D const& view) {
  int n = static_cast<int>(vertices.size());
  bool seen_positive = false;
  bool seen_negative = false;
  for (int i = 0; i < n; ++i) {
    double x0 = view.x(vertices[i]), y0 = view.y(vertices[i]);
    double x1 = view.x(vertices[(i + 1) % n]), y1 = view.y(vertices[(i + 1) % n]);
    double x2 = view.x(vertices[(i + 2) % n]), y2 = view.y(vertices[(i + 2) % n]);
    double cross = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
    auto ord = compare(cross, 0.0);
    if (ord == 0) {
      continue;
    }
    if (ord > 0) {
      seen_positive = true;
    } else {
      seen_negative = true;
    }
    if (seen_positive && seen_negative) {
      return false;
    }
  }
  return true;
}

template bool is_convex(std::vector<Point2D> const&, View2D const&);
template bool is_convex(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool is_simple(Points const& points, View2D const& view) {
  auto segs = collect_ring_segments(points, std::vector<Points>{}, view);
  return !has_intersections(segs);
}

template bool is_simple(std::vector<Point2D> const&, View2D const&);
template bool is_simple(std::vector<Point3D> const&, View2D const&);

}  // namespace view

bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes) {
  if (!holes.empty() || vertices.size() < 3) {
    return false;
  }
  return view::is_convex(vertices, View2D::XY());
}

namespace view {

template <PointContainer Points>
bool is_on_perimeter(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  Point2D test_pt(px, py);
  auto check_ring = [&](auto const& ring) -> bool {
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      Point2D v1(view.x(ring[i]), view.y(ring[i]));
      Point2D v2(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]));
      if (LineSegment2D::Make(v1, v2).Contains(test_pt)) {
        return true;
      }
    }
    return false;
  };
  if (check_ring(outer)) {
    return true;
  }
  for (auto const& hole : holes) {
    if (check_ring(hole)) {
      return true;
    }
  }
  return false;
}

template bool is_on_perimeter(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                              double, double);
template bool is_on_perimeter(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                              double, double);

template <PointContainer Points>
bool polygon_contains(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  auto ring_winding = [&](auto const& ring) -> int {
    int wn = 0;
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      double v1y = view.y(ring[i]);
      double v2y = view.y(ring[(i + 1) % n]);
      if (compare(v1y, py) <= 0) {
        if (compare(v2y, py) > 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) > 0) {
            ++wn;
          }
        }
      } else {
        if (compare(v2y, py) <= 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) < 0) {
            --wn;
          }
        }
      }
    }
    return wn;
  };

  int wn = ring_winding(outer);
  if (wn == 0) {  // early terminate if outside
    return false;
  }

  for (auto const& hole : holes) {
    wn += ring_winding(hole);

    if (wn == 0) {  // since valid holes cannot overlap each other
                    // early terminate if inside one of the holes
      return false;
    }
  }
  return wn != 0;
}

template bool polygon_contains(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                               double, double);
template bool polygon_contains(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                               double, double);

template <PointContainer Points, Point P>
std::vector<std::pair<double, double>> compute_parametric_intersection_intervals(
    Points const& outer_coplanar_ccw, std::vector<Points> const& holes_coplanar_cw, bool is_convex_input,
    P const& line_p0, P const& line_p1, View2D const& view) {
  // Debug-mode invariant checks. Disabled in Release (NDEBUG defined). Qualified as geompp::are_ccw/
  // are_coplanar/are_cw (not plain unqualified calls): this function lives inside namespace detail, so an
  // unqualified call would be ambiguous between this scope's own detail:: overloads and the public ones
  // pulled in via ADL on Points::value_type -- and these asserts exist specifically to VERIFY the
  // collinear-free/CCW/coplanar invariant the caller claims, so the full-checking public path is correct.
  assert(geompp::are_ccw(outer_coplanar_ccw));
  if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
    assert(geompp::are_coplanar(outer_coplanar_ccw));
  }
  assert(!is_convex_input || is_convex(outer_coplanar_ccw, view));
  assert(!is_convex_input || holes_coplanar_cw.empty());
  for (auto const& hole : holes_coplanar_cw) {
    assert(geompp::are_cw(hole));
    if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
      assert(geompp::are_coplanar(hole));
    }
  }

  // actual algorithm

  // Project the line endpoints to 2D once; all geometry lives in this view's plane.
  // edge e(i): v_0 = outer[i], v_1 = outer[(i+1)%n]  (CCW order)
  //   ex = v1x - v0x,  ey = v1y - v0y
  //   outward normal for CCW ring: n = (ey, -ex)
  //   N = -n · (P0 - Vi)  =  -(ey, -ex) · (p0x - v0x,  p0y - v0y)
  //                       = -(ey * (p0x - v0x) + (-ex) * (p0y - v0y))
  //                       = -(p0x - v0x) * ey + (p0y - v0y)*ex
  //   D = n · (P1 - P0)   =  dx*ey - dy*ex
  double p0x = view.x(line_p0), p0y = view.y(line_p0);
  double dx = view.x(line_p1) - p0x;
  double dy = view.y(line_p1) - p0y;

  auto edge_ND = [&](P const& v0, P const& v1) -> std::pair<double, double> {
    double v0x = view.x(v0), v0y = view.y(v0);
    double ex = view.x(v1) - v0x, ey = view.y(v1) - v0y;
    double N = -(p0x - v0x) * ey + (p0y - v0y) * ex;
    double D = dx * ey - dy * ex;
    return {N, D};
  };

  std::vector<std::pair<double, double>> t_list;

  if (is_convex_input) {  // quick exit on entering and leaving edges
    double t_e = std::numeric_limits<double>::min();
    bool is_t_e_set = false;
    double t_l = std::numeric_limits<double>::max();
    bool is_t_l_set = false;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        if (compare(N, 0) < 0) {  // P0 is outside the edge e(i)
          break;                  // early terminate (valid for convex: one miss = total miss)

        } else {
          continue;  // ignore edge e(i)
        }
      }

      double t = N / D;
      if (D_compare_to_0 < 0) {  // the line is ENTERING polygon through the edge e(i)
        if (!is_t_e_set) {       // init
          t_e = t;
          is_t_e_set = true;

        } else {
          if (compare(t, t_e) > 0) {  // update our t_e to the greatest of the old and new value
            t_e = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }

      } else {              // the line is LEAVING polygon through the edge e(i), on D_compare_to_0 > 0
        if (!is_t_l_set) {  // init
          t_l = t;
          is_t_l_set = true;

        } else {
          if (compare(t, t_l) < 0) {  // update our t_l to the smallest of the old and new value
            t_l = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }
      }
    }

    // only if we have both an ENTERING edge and a LEAVING edge we have an intersection,
    // otherwise it's an overlap (OnPerimeter)
    if (is_t_e_set && is_t_l_set && compare(t_e, t_l) < 0) {
      t_list.emplace_back(t_e, t_l);
    }

  } else {  // Collect, Sort, Parity

    // we don't need to care about separating t_e from t_l
    // the Jordan Curve Theorem guarantees that an infinite line crossing a closed shape will always alternate:
    // Enter-> Leave-> Enter -> Leave, always paired up
    // (try it graphically on a piece of paper: outer CCW and inner CW guarantee this)

    // the list of ts will always have an even number
    // (mathematically guaranteed if we close the for-loop of points)
    // so we can expect to always have something like this once sorted (<t_e, t_l>, <t_e, t_l>, <t_e, t_l>, ...)

    // special case: overlap with vertex: the values of (sorted) t_e and t_l are equal: we will remove them

    std::vector<double> t_all;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];  // guarantees the loop of points to be closed
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        continue;                 // skip — parallel to one edge does not mean the line misses the whole concave polygon
      }

      double t = N / D;
      t_all.push_back(t);
    }

    // sort the list
    if (!t_all.empty()) {
      // sort ASC, using the std::partial_ordering and the optimized ranges algorithm
      std::ranges::sort(t_all);
      // remove consecutive duplicates
      remove_all_duplicated_elements(t_all);

      if (!t_all.empty()) {
        std::size_t m = t_all.size();
        if (m % 2 != 0) {  // impossible (mathematically) to get an odd number
          throw std::runtime_error("found an odd number of t_all intervals in sorted vector");
        }

        for (std::size_t i = 0; i < m - 1; i += 2) {
          t_list.emplace_back(t_all[i], t_all[i + 1]);
        }
      }
    }
  }

  return t_list;
}

std::vector<std::pair<double, double>> compute_intersection_intervals_2d(
    std::vector<Point2D> const& outer_coplanar_ccw, std::vector<std::vector<Point2D>> const& holes_coplanar_cw,
    bool is_convex_input, Point2D const& line_p0, Point2D const& line_p1, View2D const& view) {
  return compute_parametric_intersection_intervals(outer_coplanar_ccw, holes_coplanar_cw, is_convex_input, line_p0,
                                                   line_p1, view);
}

std::vector<std::pair<double, double>> compute_intersection_intervals_3d(
    std::vector<Point3D> const& outer_coplanar_ccw, std::vector<std::vector<Point3D>> const& holes_coplanar_cw,
    bool is_convex_input, Point3D const& line_p0, Point3D const& line_p1, View2D const& view) {
  return compute_parametric_intersection_intervals(outer_coplanar_ccw, holes_coplanar_cw, is_convex_input, line_p0,
                                                   line_p1, view);
}

template <PointContainer Points, Point P>
double distance_to(Points const& outer_loop, bool is_convex, P const& line_p0, P const& line_p1, View2D const& view) {
  std::size_t n = std::ranges::size(outer_loop);
  if (n == 0) {
    throw std::invalid_argument("distance_to: empty vertex range");
  }

  double lx0 = view.x(line_p0), ly0 = view.y(line_p0);
  double ldx = view.x(line_p1) - lx0, ldy = view.y(line_p1) - ly0;
  double llen = std::sqrt(ldx * ldx + ldy * ldy);
  if (compare(llen, 0.0) == 0) {
    throw std::invalid_argument("distance_to: line_p0, line_p1 project onto a zero-length line");
  }
  double nx = -ldy / llen, ny = ldx / llen;  // unit, perpendicular to the (projected) line

  auto signed_dist = [&](std::size_t i) {
    return (view.x(outer_loop[i]) - lx0) * nx + (view.y(outer_loop[i]) - ly0) * ny;
  };

  // convex: binary search based O(LogN) algorithm
  if (is_convex) {
    // Daniel Sunday's O(log n) extreme-vertex binary search (cf. detail::extreme_points), reimplemented
    // here on view-projected scalars: view.x()/y() have no native-vector equivalent to dot a direction
    // against, so the native detail::extreme_points can't be reused for the projected case.
    long const ln = static_cast<long>(n);
    auto proj = [&](long i) { return signed_dist(static_cast<std::size_t>(((i % ln) + ln) % ln)); };
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };
    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = ln;
      while (lo + 1 < hi) {
        long m = (lo + hi) / 2;
        if (extr(m)) {
          return static_cast<std::size_t>(m);
        }
        int ls = cmp(lo + 1, lo);
        int ms = cmp(m + 1, m);
        bool go_hi = (ls < ms) || (ls == ms && ls == cmp(lo, m));
        if (go_hi) {
          hi = m;
        } else {
          lo = m;
        }
      }
      return static_cast<std::size_t>(lo);
    };

    double d_max = signed_dist(search(1.0));
    double d_min = signed_dist(search(-1.0));

    // polygon straddles the line if the extreme signed offsets have opposite sign (or touch)
    if (compare(d_min, 0) <= 0 && compare(d_max, 0) >= 0) {
      return 0.0;
    }

    return std::min(std::abs(d_min), std::abs(d_max));
  }

  // non convex: brute force O(N) algorithm
  double min_d = std::numeric_limits<double>::infinity();
  for (std::size_t i = 0; i < n; ++i) {
    double d0 = signed_dist(i);
    double d1 = signed_dist((i + 1) % n);

    // the signed distance is affine along the edge, so its rate of change is exactly d1 - d0;
    // solving d0 + t*(d1-d0) = 0 gives the parametric crossing point without any extra dot products
    double denom = d1 - d0;
    if (compare(denom, 0.0) != 0) {
      double t = -d0 / denom;
      if (is_in_range(t, 0.0, 1.0)) {
        return 0.0;  // this edge crosses the line
      }
    }

    // no crossing: the closest point on this edge to the line is one of its endpoints
    min_d = std::min({min_d, std::abs(d0), std::abs(d1)});
  }

  return min_d;
}

template double distance_to(std::vector<Point2D> const&, bool, Point2D const&, Point2D const&, View2D const&);
template double distance_to(std::vector<Point3D> const&, bool, Point3D const&, Point3D const&, View2D const&);

}  // namespace view

double distance_to(std::vector<Point2D> const& outer_loop, bool is_convex, Point2D const& line_p0,
                   Point2D const& line_p1) {
  return view::distance_to(outer_loop, is_convex, line_p0, line_p1, View2D::XY());
}

namespace view {

namespace {

// Cross-product orientation tests, projected through `view` (dimension-agnostic: works for Point2D or Point3D).
template <Point P>
bool is_left(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross =
      (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) - (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) > 0;
}

template <Point P>
bool is_right(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross =
      (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) - (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) < 0;
}

// tells if v1 is above v2, with respect to point p (ref)
template <Point P>
bool is_above(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_left(p, v1, v2, view);
}
// tells if v1 is below v2, with respect to point p (ref)
template <Point P>
bool is_below(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_right(p, v1, v2, view);
}

/// @brief finds the index of outer_loop corresponding to the RIGHT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t r_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("r_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local max
  if (is_below(p, outer_loop[1], outer_loop[0], view) && !is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "above" wins
      return is_above(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (dnC && !is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the max tangent point
      return c % n;
    }
    // no max yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool upA = is_above(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (upA) {
      if (dnC) {
        b = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (!dnC) {
        a = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (r_tangent)");
}

/// @brief finds the index of outer_loop corresponding to the LEFT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t l_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("l_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local min
  if (!is_below(p, outer_loop[1], outer_loop[0], view) && is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "below" wins
      return is_below(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (!dnC && is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the min tangent point
      return c % n;
    }
    // no min yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool dnA = is_below(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (dnA) {
      if (!dnC) {
        b = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (dnC) {
        a = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (l_tangent)");
}

// Reduces `loop` to its convex hull (projected through `view`) when it isn't already convex. Returns the hull
// points alongside the indices (into the ORIGINAL loop) each hull point came from, so callers can map hull-local
// results back to the caller's index space. When `already_convex`, both are trivial identity pass-throughs.
template <PointContainer Points>
std::pair<std::vector<std::ranges::range_value_t<Points>>, std::vector<std::size_t>> to_convex_loop(
    Points const& loop, bool already_convex, View2D const& view) {
  using PointT = std::ranges::range_value_t<Points>;

  std::vector<std::size_t> idx;
  if (already_convex) {
    idx.resize(loop.size());
    std::iota(idx.begin(), idx.end(), std::size_t{0});
  } else {
    idx = convex_hull_monotone_chain(loop, view);
  }

  std::vector<PointT> pts;
  pts.reserve(idx.size());
  for (auto i : idx) {
    pts.push_back(loop[i]);
  }
  return {std::move(pts), std::move(idx)};
}

}  // namespace

template <PointContainer Points, Point P>
std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(Points const& outer_loop, bool is_convex, P const& p,
                                                             View2D const& view) {
  // if the polygon is convex we can use the binary search, O(logN)
  if (is_convex) {
    return {l_tangent(outer_loop, p, view), r_tangent(outer_loop, p, view)};
  }

  // otherwise reduce to the convex hull first — a tangent from an external point can only ever touch a hull
  // vertex — then map the hull-local result back to an index into the original outer_loop
  auto [hull_pts, hull_idx] = to_convex_loop(outer_loop, /*already_convex=*/false, view);
  return {hull_idx[l_tangent(hull_pts, p, view)], hull_idx[r_tangent(hull_pts, p, view)]};
}

template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const&, bool, Point2D const&,
                                                                      View2D const&);
template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point3D> const&, bool, Point3D const&,
                                                                      View2D const&);

template <PointContainer Points>
std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(Points const& loop1, bool is_convex1, Points const& loop2,
                                                            bool is_convex2, View2D const& view) {
  auto [cv1, idx1] = to_convex_loop(loop1, is_convex1, view);
  auto [cv2, idx2] = to_convex_loop(loop2, is_convex2, view);

  std::size_t m = cv1.size();
  std::size_t n = cv2.size();
  if (m < 3 || n < 3) {
    throw std::invalid_argument("poly_poly_RL_tangent_to requires polygons with at least 3 vertices");
  }

  std::size_t i1 = r_tangent(cv1, cv2[0], view);
  std::size_t i2 = l_tangent(cv2, cv1[i1], view);

  std::size_t max_iter = m * n + 1;
  bool done = false;
  while (!done && max_iter-- > 0) {
    done = true;

    std::size_t guard1 = 0;
    while (!is_left(cv2[i2], cv1[i1], cv1[(i1 + 1) % m], view)) {
      i1 = (i1 + 1) % m;
      if (++guard1 > m) {
        throw std::logic_error("poly_poly_RL_tangent_to: i1 walk failed to converge");
      }
    }

    std::size_t guard2 = 0;
    while (!is_right(cv1[i1], cv2[i2], cv2[(i2 + n - 1) % n], view)) {
      i2 = (i2 + n - 1) % n;
      done = false;
      if (++guard2 > n) {
        throw std::logic_error("poly_poly_RL_tangent_to: i2 walk failed to converge");
      }
    }
  }
  if (!done) {
    throw std::logic_error("poly_poly_RL_tangent_to exceeded max iterations without converging");
  }

  return {idx1[i1], idx2[i2]};
}

template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const&, bool,
                                                                     std::vector<Point2D> const&, bool, View2D const&);
template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point3D> const&, bool,
                                                                     std::vector<Point3D> const&, bool, View2D const&);

}  // namespace view

std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const& outer_loop, bool is_convex,
                                                             Point2D const& p) {
  return view::point_poly_tangent_lr_to(outer_loop, is_convex, p, View2D::XY());
}

std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const& loop1, bool is_convex1,
                                                            std::vector<Point2D> const& loop2, bool is_convex2) {
  return view::poly_poly_RL_tangent_to(loop1, is_convex1, loop2, is_convex2, View2D::XY());
}

}  // namespace detail

ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line) {
  auto [min_i, max_i] = detail::extreme_points(polygon.Perimeter(), polygon.IsConvex(), line.Direction());
  return {polygon[min_i], polygon[max_i]};
}

double distance_to(Polygon2D const& polygon, Line2D const& line) {
  return detail::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.First(), line.Last());
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Point2D const& p) {
  auto [left_i, right_i] = detail::point_poly_tangent_lr_to(polygon.Perimeter(), polygon.IsConvex(), p);
  return {LineSegment2D::Make(p, polygon[left_i]), LineSegment2D::Make(p, polygon[right_i])};
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Polygon2D const& other) {
  auto [RL_poly_i, RL_other_i] =
      detail::poly_poly_RL_tangent_to(polygon.Perimeter(), polygon.IsConvex(), other.Perimeter(), other.IsConvex());
  auto [LR_other_i, LR_poly_i] =
      detail::poly_poly_RL_tangent_to(other.Perimeter(), other.IsConvex(), polygon.Perimeter(), polygon.IsConvex());
  return {LineSegment2D::Make(polygon[RL_poly_i], other[RL_other_i]),
          LineSegment2D::Make(polygon[LR_poly_i], other[LR_other_i])};
}

std::vector<LineSegment2D> to_segments(std::vector<Point2D> const& points) {
  SegmentRange2D range(points, true);
  std::vector<LineSegment2D> segments;
  segments.reserve(range.size());
  for (auto const& seg : range) {
    segments.push_back(seg);
  }
  return segments;
}

bool is_simple(std::vector<Point2D> const& points) { return detail::view::is_simple(points, View2D::XY()); }

template <PointContainer Points>
std::vector<Points> clip(Points const& clipper_loop, Points const& subject_loop) {
  using PointT = typename Points::value_type;

  // View2D has no copy-assignment (its defaulted move constructor suppresses it), so the branch below
  // is built via a single initializing return rather than default-construct-then-reassign.
  View2D view = [&]() -> View2D {
    if constexpr (std::is_same_v<PointT, Point3D>) {
      if (subject_loop.size() < 3) {
        throw std::invalid_argument("clip: subject_loop needs at least 3 points to establish a plane");
      }
      Points combined = subject_loop;
      combined.insert(combined.end(), clipper_loop.begin(), clipper_loop.end());
      if (!are_coplanar(combined)) {
        throw std::invalid_argument("clip: clipper_loop and subject_loop must be coplanar for Point3D input");
      }
      return View2D::OnPlane(Plane::From3Points(subject_loop[0], subject_loop[1], subject_loop[2]));
    } else {
      return View2D::XY();
    }
  }();

  auto project = [&](Points const& ring) {
    std::vector<Point2D> out;
    out.reserve(ring.size());
    for (auto const& p : ring) {
      out.push_back(Point2D(view.x(p), view.y(p)));
    }
    return out;
  };

  auto unproject = [&](std::vector<Point2D> const& ring2d) {
    Points out;
    out.reserve(ring2d.size());
    for (auto const& p : ring2d) {
      if constexpr (std::is_same_v<PointT, Point3D>) {
        out.push_back(view.xyz(p));
      } else {
        out.push_back(p);
      }
    }
    return out;
  };

  auto groups =
      detail::boolean_op(project(subject_loop), {}, project(clipper_loop), {}, detail::BooleanOp::Intersection);

  std::vector<Points> rings;
  rings.reserve(groups.size() * 2);
  for (auto& [outer, holes] : groups) {
    rings.push_back(unproject(outer));
    for (auto& hole : holes) {
      rings.push_back(unproject(hole));
    }
  }
  return rings;
}

template std::vector<std::vector<Point2D>> clip(std::vector<Point2D> const&, std::vector<Point2D> const&);
template std::vector<std::vector<Point3D>> clip(std::vector<Point3D> const&, std::vector<Point3D> const&);

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
  // input, never from the growing output buffer — see the doc comment in calc_utils2d.hpp for why
  // that matters (it's what keeps adjacent corners' trims from ever exceeding their shared edge).
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
