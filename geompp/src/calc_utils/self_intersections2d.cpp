#include "calc_utils/self_intersections2d.hpp"

#include "calc_utils/convex_hull2d.hpp"
#include "geompp_log.hpp"
#include "segment_iterator2d.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <set>

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

}  // namespace detail

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

}  // namespace geometry

}  // namespace geompp
