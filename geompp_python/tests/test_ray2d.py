"""
ray2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestRay2D:
    @pytest.fixture
    def ray(self):
        return geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))

    def test_is_ahead(self, ray):
        assert ray.is_ahead(geompp.Point2D(1, 0))
        assert not ray.is_ahead(geompp.Point2D(-1, 0))

    def test_is_behind(self, ray):
        assert ray.is_behind(geompp.Point2D(-1, 0))

    def test_contains(self, ray):
        assert ray.contains(geompp.Point2D(5, 0))
        assert not ray.contains(geompp.Point2D(-1, 0))

    def test_distance_to(self, ray):
        assert approx(ray.distance_to(geompp.Point2D(3, 4)), 4.0)
        assert approx(ray.distance_to(geompp.Point2D(3, 0)), 0.0)

    def test_project_onto(self, ray):
        assert ray.project_onto(geompp.Point2D(3, 5)).almost_equals(geompp.Point2D(3, 0))
        assert ray.project_onto(geompp.Point2D(-2, 3)).almost_equals(geompp.Point2D(0, 0))

    def test_intersection_with_segment(self, ray):
        s = geompp.LineSegment2D.make(geompp.Point2D(3, -1), geompp.Point2D(3, 1))
        assert ray.intersects(s)
        hit = ray.intersection(s)
        assert hit is not None
        assert approx(hit.x, 3) and approx(hit.y, 0)

    def test_to_line(self, ray):
        l = ray.to_line()
        assert isinstance(l, geompp.Line2D)
        assert l.contains(ray.origin)

    def test_almost_equals(self, ray):
        ray2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert ray.almost_equals(ray2)
        ray3 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert not ray.almost_equals(ray3)

    def test_wkt_roundtrip(self, ray):
        wkt = ray.to_wkt()
        ray2 = geompp.Ray2D.from_wkt(wkt)
        assert ray.almost_equals(ray2)

    def test_from_wkt_whitespace(self, ray):
        assert ray.almost_equals(geompp.Ray2D.from_wkt("RAY (0 0,1 0)"))
        assert ray.almost_equals(geompp.Ray2D.from_wkt("RAY (  0 0  ,  1  0  )"))

    def test_to_file_from_file(self, ray):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            ray.to_file(path)
            assert os.path.exists(path)
            ray2 = geompp.Ray2D.from_file(path)
            assert ray.almost_equals(ray2)
        finally:
            os.unlink(path)

    def test_intersects_ray(self, ray):
        # two rays from origin pointing right and upward at (5,0) — they share origin
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        # rays from different origins that cross
        r_hit = geompp.Ray2D.make(geompp.Point2D(3, -2), geompp.Vector2D(0, 1))
        assert ray.intersects(r_hit)
        hit = ray.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # two parallel rays — no intersection
        r_miss = geompp.Ray2D.make(geompp.Point2D(0, 1), geompp.Vector2D(1, 0))
        assert not ray.intersects(r_miss)
        assert ray.intersection(r_miss) is None

    def test_intersection_with_line(self, ray):
        l_hit = geompp.Line2D.make(geompp.Point2D(5, -1), geompp.Point2D(5, 1))
        assert ray.intersects(l_hit)
        hit = ray.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 5) and approx(hit.y, 0)
        # line parallel to ray — no intersection
        l_miss = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not ray.intersects(l_miss)
        assert ray.intersection(l_miss) is None

class TestRay2DOverlap:
    def test_overlap_with_collinear_line(self):
        r = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert r.overlaps(x)
        ov = r.overlap(x)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_no_overlap_perpendicular_line(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        y = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not r.overlaps(y)
        assert r.overlap(y) is None

    def test_overlap_same_direction_rays(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert r1.overlaps(r2)
        ov = r1.overlap(r2)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_overlap_anti_parallel_rays(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r3 = geompp.Ray2D.make(geompp.Point2D(5, 0), geompp.Vector2D(-1, 0))
        assert r1.overlaps(r3)
        ov = r1.overlap(r3)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment2D)

    def test_no_overlap_touching_anti_parallel(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r4 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(-1, 0))
        assert not r1.overlaps(r4)
        assert r1.overlap(r4) is None

    def test_overlap_segment_inside_ray(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(4, 0))
        assert r.overlaps(seg)
        ov = r.overlap(seg)
        assert ov == seg

    def test_no_overlap_segment_before_ray(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(-3, 0), geompp.Point2D(-1, 0))
        assert not r.overlaps(seg)
        assert r.overlap(seg) is None

    def test_no_overlap_segment_touching_ray_origin(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg_touch = geompp.LineSegment2D.make(geompp.Point2D(-1, 0), geompp.Point2D(1, 0))
        assert not r.overlaps(seg_touch)
        assert r.overlap(seg_touch) is None

class TestRay2DTouch:
    def test_touch_with_line_origin_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(0, 1))
        assert r.touches(x)
        t = r.touch(x)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_line_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        assert not r.touches(x)
        assert r.touch(x) is None

    def test_touch_with_ray_same_origin_different_dir(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point2D(0, 0)

    def test_touch_with_ray_anti_parallel_same_origin(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(-1, 0))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point2D(0, 0)

    def test_touch_with_ray_anti_parallel_overlapping_no_touch(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(3, 0), geompp.Vector2D(-1, 0))
        assert not r1.touches(r2)
        assert r1.touch(r2) is None

    def test_touch_with_segment_endpoint(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 2))
        assert r.touches(s)
        t = r.touch(s)
        assert t is not None
        assert t == geompp.Point2D(3, 0)
