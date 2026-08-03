"""
line2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestLine2D:
    @pytest.fixture
    def hline(self):
        return geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))

    @pytest.fixture
    def vline(self):
        return geompp.Line2D.make(geompp.Point2D(0.5, -1), geompp.Point2D(0.5, 1))

    def test_make_from_two_points(self, hline):
        assert approx(hline.direction.y, 0)

    def test_make_from_point_and_direction(self):
        l = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert approx(l.direction.x, 1)

    def test_contains(self, hline):
        assert hline.contains(geompp.Point2D(5, 0))
        assert not hline.contains(geompp.Point2D(5, 1))

    def test_distance_to(self, hline):
        assert approx(hline.distance_to(geompp.Point2D(0, 3)), 3.0)

    def test_project_onto(self, hline):
        p = hline.project_onto(geompp.Point2D(3, 5))
        assert approx(p.y, 0)

    def test_intersection(self, hline, vline):
        assert hline.intersects(vline)
        hit = hline.intersection(vline)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 0.5) and approx(hit.y, 0)

    def test_no_intersection_parallel(self):
        l1 = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        l2 = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not l1.intersects(l2)
        assert l1.intersection(l2) is None

    def test_intersection_with_params(self, hline, vline):
        hit, sc, tc = hline.intersection_with_params(vline)
        assert hit is not None
        assert approx(hit.x, 0.5)
        assert isinstance(sc, float)
        assert isinstance(tc, float)

    def test_wkt_roundtrip(self, hline):
        l2 = geompp.Line2D.from_wkt(hline.to_wkt())
        assert hline.almost_equals(l2)

    def test_from_wkt_whitespace(self):
        l = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert l.almost_equals(geompp.Line2D.from_wkt("LINE (0 0,1 1)"))
        assert l.almost_equals(geompp.Line2D.from_wkt("LINE (  0 0  ,  1  1  )"))

    def test_intersects_ray(self, hline):
        # ray pointing upward from below y=0, crossing hline at (3,0)
        r_hit = geompp.Ray2D.make(geompp.Point2D(3, -2), geompp.Vector2D(0, 1))
        assert hline.intersects(r_hit)
        hit = hline.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # ray pointing upward from (0,1) — parallel but offset, no intersection with hline (y=0)
        r_miss = geompp.Ray2D.make(geompp.Point2D(0, 1), geompp.Vector2D(0, 1))
        assert not hline.intersects(r_miss)
        assert hline.intersection(r_miss) is None

    def test_intersects_segment(self, hline):
        # vertical segment crossing y=0
        s_hit = geompp.LineSegment2D.make(geompp.Point2D(3, -1), geompp.Point2D(3, 1))
        assert hline.intersects(s_hit)
        hit = hline.intersection(s_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # segment entirely above — no hit
        s_miss = geompp.LineSegment2D.make(geompp.Point2D(0, 1), geompp.Point2D(4, 1))
        assert not hline.intersects(s_miss)
        assert hline.intersection(s_miss) is None

    def test_to_file_from_file(self, hline):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            hline.to_file(path)
            assert os.path.exists(path)
            l2 = geompp.Line2D.from_file(path)
            assert hline.almost_equals(l2)
        finally:
            os.unlink(path)

class TestLine2DOverlap:
    def test_overlap_same_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        x2 = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert x.overlaps(x2)
        ov = x.overlap(x2)
        assert ov is not None
        assert isinstance(ov, geompp.Line2D)

    def test_no_overlap_crossing(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        y = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not x.overlaps(y)
        assert x.overlap(y) is None

    def test_no_overlap_parallel_offset(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        x_off = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not x.overlaps(x_off)
        assert x.overlap(x_off) is None

    def test_overlap_with_collinear_ray(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        rx = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert x.overlaps(rx)
        ov = x.overlap(rx)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_no_overlap_perpendicular_ray(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        ry = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert not x.overlaps(ry)
        assert x.overlap(ry) is None

    def test_overlap_with_collinear_segment(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        assert x.overlaps(seg)
        ov = x.overlap(seg)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment2D)
        assert ov == seg

    def test_no_overlap_offset_segment(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        seg_off = geompp.LineSegment2D.make(geompp.Point2D(2, 1), geompp.Point2D(5, 1))
        assert not x.overlaps(seg_off)
        assert x.overlap(seg_off) is None

class TestLine2DTouch:
    def test_touch_with_ray_origin_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(3, 0), geompp.Vector2D(0, 1))
        assert x.touches(r)
        t = x.touch(r)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_ray_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        assert not x.touches(r)
        assert x.touch(r) is None

    def test_touch_with_segment_endpoint_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(2, 3))
        assert x.touches(s)
        t = x.touch(s)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_segment_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(1, 0), geompp.Point2D(4, 0))
        assert not x.touches(s)
        assert x.touch(s) is None
