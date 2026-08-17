#include "calc_utils/polygon_ops2d.hpp"

#include "calc_utils/polygon_queries2d.hpp"
#include "calc_utils/self_intersections2d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

namespace geompp {

inline namespace geometry {
namespace detail {

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

  auto split = detail::split_segments_at_crossings(segs);

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

}  // namespace

// Promoted out of the anonymous namespace above (unlike classify_and_orient/select_face/interior_sample_point,
// which stay boolean_op-only internal-linkage helpers): trace_directed_boundary and package_result_rings
// below are pure "cancel/trace/group" primitives with no boolean_op-specific dependency, declared in
// calc_utils/polygon_ops2d.hpp so calc_utils/polygonization2d.cpp can reuse them for the PlanarBoundaryExtraction
// polygonize() strategy and for merge() -- both need the exact same "chain directed edges into closed loops,
// group by containment" job this file already implements and tests for boolean_op.

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

namespace {

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

  auto split = detail::split_segments_at_crossings(segs);
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

}  // namespace detail

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

}  // namespace geometry

}  // namespace geompp
