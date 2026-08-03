"""
line segment2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestLineSegment2D:
    @pytest.fixture
    def seg(self):
        return geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0))

    def test_endpoints(self, seg):
        assert approx(seg.first.x, 0)
        assert approx(seg.last.x, 4)

    def test_length(self, seg):
        assert approx(seg.length(), 4.0)

    def test_contains(self, seg):
        assert seg.contains(geompp.Point2D(2, 0))
        assert not seg.contains(geompp.Point2D(2, 1))

    def test_interpolate(self, seg):
        mid = seg.interpolate(0.5)
        assert approx(mid.x, 2) and approx(mid.y, 0)

    def test_distance_to(self, seg):
        assert approx(seg.distance_to(geompp.Point2D(2, 3)), 3.0)

    def test_intersection_with_segment(self):
        s1 = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(2, 2))
        s2 = geompp.LineSegment2D.make(geompp.Point2D(0, 2), geompp.Point2D(2, 0))
        assert s1.intersects(s2)
        hit = s1.intersection(s2)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 1) and approx(hit.y, 1)

    def test_no_intersection(self):
        s1 = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s2 = geompp.LineSegment2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not s1.intersects(s2)
        assert s1.intersection(s2) is None

    def test_location(self, seg):
        assert approx(seg.location(geompp.Point2D(0, 0)), 0.0)
        assert approx(seg.location(geompp.Point2D(4, 0)), 1.0)
        assert approx(seg.location(geompp.Point2D(2, 0)), 0.5)
        assert math.isinf(seg.location(geompp.Point2D(2, 1)))

    def test_project_onto(self, seg):
        assert seg.project_onto(geompp.Point2D(2, 3)).almost_equals(geompp.Point2D(2, 0))
        assert seg.project_onto(geompp.Point2D(-1, 2)).almost_equals(geompp.Point2D(0, 0))
        assert seg.project_onto(geompp.Point2D(5, 2)).almost_equals(geompp.Point2D(4, 0))

    def test_is_left(self, seg):
        # fixture: P0=(0,0), P1=(4,0). Direction=(4,0).
        # formula: (4)*p.y - (0)*p.x = 4*p.y  → left iff p.y > 0
        assert seg.is_left(geompp.Point2D(2, 1))    # above the rightward segment → left
        assert not seg.is_left(geompp.Point2D(2, -1))  # below → right
        assert not seg.is_left(geompp.Point2D(2, 0))   # on the axis → not left

        # upward segment: P0=(0,0), P1=(0,1). formula: -p.x → left iff p.x < 0
        up = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert up.is_left(geompp.Point2D(-1, 0))    # left of upward → left
        assert not up.is_left(geompp.Point2D(1, 0)) # right of upward → right

        # diagonal: P0=(0,0), P1=(1,1). formula: p.y - p.x → left iff p.y > p.x
        diag = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert diag.is_left(geompp.Point2D(0, 1))    # above y=x → left
        assert not diag.is_left(geompp.Point2D(1, 0))  # below y=x → right

        # reversed segment: P0=(4,0), P1=(0,0). formula: -4*p.y → left iff p.y < 0
        rev = geompp.LineSegment2D.make(geompp.Point2D(4, 0), geompp.Point2D(0, 0))
        assert rev.is_left(geompp.Point2D(2, -1))   # below → left when going leftward
        assert not rev.is_left(geompp.Point2D(2, 1))  # above → right when going leftward

    def test_wkt_roundtrip(self, seg):
        seg2 = geompp.LineSegment2D.from_wkt(seg.to_wkt())
        assert seg.almost_equals(seg2)

    def test_from_wkt_whitespace(self):
        expected = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert expected.almost_equals(geompp.LineSegment2D.from_wkt("LINESTRING (0 0,1 1)"))
        assert expected.almost_equals(geompp.LineSegment2D.from_wkt("LINESTRING (  0 0  ,  1  1  )"))

    def test_to_line(self, seg):
        l = seg.to_line()
        assert isinstance(l, geompp.Line2D)
        assert l.contains(seg.first)
        assert l.contains(seg.last)

    def test_to_file_from_file(self, seg):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            seg.to_file(path)
            assert os.path.exists(path)
            seg2 = geompp.LineSegment2D.from_file(path)
            assert seg.almost_equals(seg2)
        finally:
            os.unlink(path)

    def test_reversed(self):
        s = geompp.LineSegment2D.make(geompp.Point2D(1, 2), geompp.Point2D(3, 4))
        r = s.reversed()
        assert isinstance(r, geompp.LineSegment2D)
        assert r.first.almost_equals(s.last)
        assert r.last.almost_equals(s.first)
        assert approx(s.length(), r.length())
        rr = r.reversed()
        assert rr.first.almost_equals(s.first)
        assert rr.last.almost_equals(s.last)

    def test_intersects_line(self, seg):
        # seg goes (0,0)→(4,0); a vertical line at x=2 hits it
        l_hit = geompp.Line2D.make(geompp.Point2D(2, -1), geompp.Point2D(2, 1))
        assert seg.intersects(l_hit)
        hit = seg.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 2) and approx(hit.y, 0)
        # A line parallel and above — no intersection
        l_miss = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(4, 1))
        assert not seg.intersects(l_miss)
        assert seg.intersection(l_miss) is None

    def test_intersects_ray(self, seg):
        # ray pointing downward, hitting the segment at (2, 0)
        r_hit = geompp.Ray2D.make(geompp.Point2D(2, 3), geompp.Vector2D(0, -1))
        assert seg.intersects(r_hit)
        hit = seg.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 2) and approx(hit.y, 0)
        # ray pointing away — no intersection
        r_miss = geompp.Ray2D.make(geompp.Point2D(2, 3), geompp.Vector2D(0, 1))
        assert not seg.intersects(r_miss)
        assert seg.intersection(r_miss) is None

class TestLineSegment2DOverlap:
    def test_overlap_with_collinear_line(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert seg.overlaps(x)
        ov = seg.overlap(x)
        assert ov == seg

    def test_no_overlap_offset_line(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        x_off = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not seg.overlaps(x_off)
        assert seg.overlap(x_off) is None

    def test_overlap_ray_covers_segment(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert seg.overlaps(r)
        assert seg.overlap(r) == seg

    def test_overlap_ray_partial(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(4, 0), geompp.Vector2D(1, 0))
        assert seg.overlaps(r)
        ov = seg.overlap(r)
        assert ov is not None
        assert ov == geompp.LineSegment2D.make(geompp.Point2D(4, 0), geompp.Point2D(6, 0))

    def test_no_overlap_ray_after_segment(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(7, 0), geompp.Vector2D(1, 0))
        assert not seg.overlaps(r)
        assert seg.overlap(r) is None

    def test_overlap_segment_partial(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert a.overlaps(b)
        ov = a.overlap(b)
        assert ov is not None
        assert ov == geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(5, 0))

    def test_no_overlap_disjoint_segments(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(6, 0), geompp.Point2D(9, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_touching_endpoint(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_perpendicular(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(2, -1), geompp.Point2D(2, 1))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

class TestLineSegment2DTouch:
    def test_touch_with_line_first_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(2, 3))
        assert s.touches(x)
        t = s.touch(x)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_ray_endpoint(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 2))
        assert s.touches(r)
        t = s.touch(r)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_segment_t_junction(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 3))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_segment_collinear_endpoint(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point2D(5, 0)

    def test_touch_with_segment_overlap_no_touch(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert not a.touches(b)
        assert a.touch(b) is None
