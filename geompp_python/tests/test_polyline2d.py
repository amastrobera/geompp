"""
polyline2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPolyline2D:
    @pytest.fixture
    def pline(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0), geompp.Point2D(3, 4)]
        return geompp.Polyline2D.make(pts)

    def test_size(self, pline):
        assert pline.size() == 3

    def test_length(self, pline):
        assert approx(pline.length(), 7.0)  # 3 + 4

    def test_contains(self, pline):
        assert pline.contains(geompp.Point2D(1, 0))

    def test_interpolate(self, pline):
        start = pline.interpolate(0.0)
        assert approx(start.x, 0) and approx(start.y, 0)
        end = pline.interpolate(1.0)
        assert approx(end.x, 3) and approx(end.y, 4)
        with pytest.raises(Exception):
            pline.interpolate(1.2)
        with pytest.raises(Exception):
            pline.interpolate(-0.1)

    def test_intersection_with_line(self, pline):
        l = geompp.Line2D.make(geompp.Point2D(0, 2), geompp.Point2D(1, 2))
        assert pline.intersects(l)
        hit = pline.intersection(l)
        assert hit is not None

    def test_wkt_roundtrip(self, pline):
        p2 = geompp.Polyline2D.from_wkt(pline.to_wkt())
        assert pline.almost_equals(p2)

    def test_from_wkt_whitespace(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1)])
        assert pl.almost_equals(geompp.Polyline2D.from_wkt("LINESTRING (0 0, 1 0, 1 1)"))
        assert pl.almost_equals(geompp.Polyline2D.from_wkt("LINESTRING (  0 0  ,  1 0  ,  1 1  )"))

    def test_location(self, pline):
        assert approx(pline.location(geompp.Point2D(0, 0)), 0.0)
        assert approx(pline.location(geompp.Point2D(3, 4)), 1.0)
        assert math.isinf(pline.location(geompp.Point2D(1, 1)))

    def test_distance_to(self, pline):
        assert approx(pline.distance_to(geompp.Point2D(0, 0)), 0.0)
        assert approx(pline.distance_to(geompp.Point2D(1, 0)), 0.0)
        assert approx(pline.distance_to(geompp.Point2D(0, 1)), 1.0)

    def test_project_onto(self, pline):
        assert pline.project_onto(geompp.Point2D(2, 3)).almost_equals(geompp.Point2D(3, 3))
        assert pline.project_onto(geompp.Point2D(6, 2)).almost_equals(geompp.Point2D(3, 2))

    def test_to_segments(self, pline):
        segs = pline.to_segments()
        assert len(segs) == 2  # 3 knots → 2 segments (open)
        assert all(isinstance(s, geompp.LineSegment2D) for s in segs)
        assert approx(segs[0].length(), 3.0)
        assert approx(segs[1].length(), 4.0)
        assert segs[0].first.almost_equals(geompp.Point2D(0, 0))
        assert segs[0].last.almost_equals(geompp.Point2D(3, 0))

    def test_intersects_ray(self, pline):
        # pline: (0,0)→(3,0)→(3,4). Ray pointing right at y=2 crosses vertical segment
        r_hit = geompp.Ray2D.make(geompp.Point2D(0, 2), geompp.Vector2D(1, 0))
        assert pline.intersects(r_hit)
        hit = pline.intersection(r_hit)
        assert hit is not None
        # ray pointing away — no hit
        r_miss = geompp.Ray2D.make(geompp.Point2D(5, 2), geompp.Vector2D(1, 0))
        assert not pline.intersects(r_miss)
        assert pline.intersection(r_miss) is None

    def test_intersects_segment(self, pline):
        s_hit = geompp.LineSegment2D.make(geompp.Point2D(1, -1), geompp.Point2D(1, 1))
        assert pline.intersects(s_hit)
        hit = pline.intersection(s_hit)
        assert hit is not None
        s_miss = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 4))
        assert not pline.intersects(s_miss)
        assert pline.intersection(s_miss) is None

    def test_intersects_polyline(self, pline):
        # crossing polyline
        other_hit = geompp.Polyline2D.make([geompp.Point2D(1, -1), geompp.Point2D(1, 1)])
        assert pline.intersects(other_hit)
        hit = pline.intersection(other_hit)
        assert hit is not None
        # non-crossing polyline
        other_miss = geompp.Polyline2D.make([geompp.Point2D(5, 0), geompp.Point2D(5, 4)])
        assert not pline.intersects(other_miss)
        assert pline.intersection(other_miss) is None

    def test_to_file_from_file(self, pline):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            pline.to_file(path)
            assert os.path.exists(path)
            pl2 = geompp.Polyline2D.from_file(path)
            assert pline.almost_equals(pl2)
        finally:
            os.unlink(path)

class TestPolyline2DConvexHull:
    def test_too_few_points_throws(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0)])
        with pytest.raises(Exception):
            pl.convex_hull()

    def test_three_points_returns_triangle(self):
        pl = geompp.Polyline2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(2, 3),
        ])
        hull = pl.convex_hull()
        assert hull.size() == 3

    def test_concave_path_inner_point_excluded(self):
        # simple path: outer corners with inner dip at (2,1) — hull is the 4 outer corners
        pl = geompp.Polyline2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4),
        ])
        hull = pl.convex_hull()
        assert hull.size() == 4
        expected = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                    geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        for e in expected:
            assert any(approx(e.x, hull[i].x) and approx(e.y, hull[i].y)
                       for i in range(hull.size())), f"{e} should be on hull"

class TestPolyline2DOverlap:
    def test_overlap_line_collinear_segment(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 3)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert pl.overlaps(line=line)
        result = pl.overlap(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0)))

    def test_overlap_line_perpendicular_no_overlap(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not pl.overlaps(line=line)
        assert pl.overlap(line=line) is None

    def test_overlap_ray_collinear_partial(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(6, 0)])
        ray = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert pl.overlaps(ray=ray)
        result = pl.overlap(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0)))

    def test_overlap_segment_partial(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(5, 0)])
        seg = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert pl.overlaps(segment=seg)
        result = pl.overlap(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(5, 0)))

    def test_overlap_polyline_shared_segment(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 3)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        assert pl1.overlaps(other=pl2)
        result = pl1.overlap(other=pl2)
        assert result is not None
        assert len(result) == 1

    def test_overlap_polyline_no_overlap(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(0, 1), geompp.Point2D(4, 1)])
        assert not pl1.overlaps(other=pl2)
        assert pl1.overlap(other=pl2) is None

class TestPolyline2DTouch:
    def test_touch_line_endpoint_on_line(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(2, 0), geompp.Point2D(2, 3)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert pl.touches(line=line)
        result = pl.touch(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(2, 0))

    def test_touch_line_collinear_not_touch(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert not pl.touches(line=line)
        assert pl.touch(line=line) is None

    def test_touch_ray_endpoint_on_ray(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 2)])
        ray = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert pl.touches(ray=ray)
        result = pl.touch(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_segment_t_junction(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 3)])
        seg = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        assert pl.touches(segment=seg)
        result = pl.touch(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_polyline_shared_endpoint(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(3, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 3)])
        assert pl1.touches(other=pl2)
        result = pl1.touch(other=pl2)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_polyline_disjoint(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(5, 0), geompp.Point2D(5, 3)])
        assert not pl1.touches(other=pl2)
        assert pl1.touch(other=pl2) is None

class TestPolyline2DReduce:
    def test_two_point_polyline_returns_unchanged(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 1)])
        assert pl.almost_equals(pl.reduce())

    def test_radial_distance(self):
        # Note: a "peak" shape (not a straight line) is deliberate — Polyline2D.make() prunes exactly
        # collinear knots at construction time, so a flat clustered dataset would collapse to its 2
        # endpoints regardless of what reduce() does, defeating the test.
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(0.1, 0.05), geompp.Point2D(0.2, -0.05),
                                     geompp.Point2D(5, 5), geompp.Point2D(5.1, 5.05), geompp.Point2D(10, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.RadialDistance, 1.0))
        assert reduced.size() == 3
        assert reduced[0].almost_equals(geompp.Point2D(0, 0))
        assert reduced[1].almost_equals(geompp.Point2D(5, 5))
        assert reduced[2].almost_equals(geompp.Point2D(10, 0))

    def test_ramer_douglas_peucker(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 5),
                                     geompp.Point2D(6, 0), geompp.Point2D(8, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.RamerDouglasPeucker, 2.0))
        expected = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 5), geompp.Point2D(8, 0)])
        assert reduced.almost_equals(expected)

    def test_visvalingam_whyatt(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 5),
                                     geompp.Point2D(6, 0), geompp.Point2D(8, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.VisvalingamWhyatt, 6.0))
        expected = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 5), geompp.Point2D(8, 0)])
        assert reduced.almost_equals(expected)

    def test_default_params_match_explicit_rdp_half_threshold(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0.01), geompp.Point2D(2, -0.01),
                                     geompp.Point2D(3, 0), geompp.Point2D(4, 0)])
        reduced_default = pl.reduce()
        reduced_explicit = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.RamerDouglasPeucker, 0.5))
        assert reduced_default.almost_equals(reduced_explicit)
        expected = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        assert reduced_default.almost_equals(expected)

class TestPolyline2DExpand:
    def test_default_params_work(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)])
        expanded = pl.expand()
        assert expanded.size() >= pl.size()

    def test_invalid_segments_per_corner_raises(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)])
        with pytest.raises(ValueError):
            pl.expand(geompp.PolylineExpansionParams(segments_per_corner=0))

    def test_min_distance_mode_smaller_min_distance_yields_more_points(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)])
        coarse = pl.expand(geompp.PolylineExpansionParams(
            smoothness=1.0, mode=geompp.PolylineExpansionMode.MinDistance, min_distance=1.0))
        fine = pl.expand(geompp.PolylineExpansionParams(
            smoothness=1.0, mode=geompp.PolylineExpansionMode.MinDistance, min_distance=0.5))
        assert coarse.size() < fine.size()
