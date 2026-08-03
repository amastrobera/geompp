"""
line segment3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestLineSegment3D:
    def test_length(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5))
        assert approx(s.length(), 5.0)

    def test_contains(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 4))
        assert s.contains(geompp.Point3D(0, 0, 2))

    def test_location(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert approx(s.location(geompp.Point3D(0,0,0)), 0.0)
        assert approx(s.location(geompp.Point3D(4,0,0)), 1.0)
        assert approx(s.location(geompp.Point3D(2,0,0)), 0.5)
        assert math.isinf(s.location(geompp.Point3D(2,1,0)))

    def test_distance_to(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert approx(s.distance_to(geompp.Point3D(2, 3, 0)), 3.0)
        assert approx(s.distance_to(geompp.Point3D(2, 0, 0)), 0.0)

    def test_distance_to_line3d(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # line crossing the segment at (2,0,0)
        line_cross = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(seg.distance_to(line_cross), 0.0)
        # parallel line offset by 3 in Y
        line_parallel = geompp.Line3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(1, 3, 0))
        assert approx(seg.distance_to(line_parallel), 3.0)
        # line containing the segment (overlap) → 0
        line_overlap = geompp.Line3D.make(geompp.Point3D(-2, 0, 0), geompp.Point3D(10, 0, 0))
        assert approx(seg.distance_to(line_overlap), 0.0)
        assert seg.distance(line_overlap) is None

    def test_distance_to_ray3d(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # ray crossing the segment at (2,0,0)
        ray_cross = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, -1, 0))
        assert approx(seg.distance_to(ray_cross), 0.0)
        # parallel ray, offset by 4 in Z
        ray_parallel = geompp.Ray3D.make(geompp.Point3D(0, 0, 4), geompp.Vector3D(1, 0, 0))
        assert approx(seg.distance_to(ray_parallel), 4.0)
        # ray contained on the segment's line (overlap) → 0
        ray_overlap = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(seg.distance_to(ray_overlap), 0.0)
        assert seg.distance(ray_overlap) is None

        # 2D top-down (XY) the ray crosses the segment at (2,0); in 3D the ray is at z=5
        # while the segment is at z=0 → non-parallel skew, real distance = 5.
        ray_top_cross_skew = geompp.Ray3D.make(geompp.Point3D(2, 3, 5), geompp.Vector3D(0, -1, 0))
        assert approx(seg.distance_to(ray_top_cross_skew), 5.0)

    def test_distance_to_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # crossing segment
        s_cross = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(s1.distance_to(s_cross), 0.0)
        # parallel segment offset in Y
        s_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(2, 3, 0))
        assert approx(s1.distance_to(s_parallel), 3.0)
        # collinear (overlap) → 0
        s_overlap = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(6, 0, 0))
        assert approx(s1.distance_to(s_overlap), 0.0)
        assert s1.distance(s_overlap) is None
        # skew segment
        s_skew = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 5), geompp.Point3D(2, 1, 5))
        assert approx(s1.distance_to(s_skew), 5.0)

    def test_interpolate(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert s.interpolate(0.0).almost_equals(geompp.Point3D(0,0,0))
        assert s.interpolate(1.0).almost_equals(geompp.Point3D(4,0,0))
        assert s.interpolate(0.5).almost_equals(geompp.Point3D(2,0,0))

    def test_project_onto(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert s.project_onto(geompp.Point3D(2,3,0)).almost_equals(geompp.Point3D(2,0,0))
        assert s.project_onto(geompp.Point3D(-1,2,0)).almost_equals(geompp.Point3D(0,0,0))
        assert s.project_onto(geompp.Point3D(5,2,0)).almost_equals(geompp.Point3D(4,0,0))

    def test_to_line(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l = s.to_line()
        assert isinstance(l, geompp.Line3D)
        assert l.contains(s.first)
        assert l.contains(s.last)

    def test_reversed(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(1, 2, 3), geompp.Point3D(4, 5, 6))
        r = s.reversed()
        assert isinstance(r, geompp.LineSegment3D)
        assert r.first.almost_equals(s.last)
        assert r.last.almost_equals(s.first)
        # length preserved
        assert approx(s.length(), r.length())
        # double reverse returns the original
        rr = r.reversed()
        assert rr.first.almost_equals(s.first)
        assert rr.last.almost_equals(s.last)

    def test_to_file_from_file(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(1, 2, 3), geompp.Point3D(4, 5, 6))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            s.to_file(path)
            assert os.path.exists(path)
            s2 = geompp.LineSegment3D.from_file(path)
            assert s.almost_equals(s2)
        finally:
            os.unlink(path)

    def test_intersects_line3d(self):
        # segment along X axis; line along Y axis at (2,0,0) — coplanar crossing
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l_hit = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert s.intersects(l_hit)
        hit = s.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_line3d(self):
        # segment along X; parallel line above — no hit
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not s.intersects(l_miss)
        assert s.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, -1, 0))
        assert s.intersects(r_hit)
        hit = s.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_ray3d(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_miss = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, 1, 0))
        assert not s.intersects(r_miss)
        assert s.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s2 = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert s1.intersects(s2)
        hit = s1.intersection(s2)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0))
        s2 = geompp.LineSegment3D.make(geompp.Point3D(3, -1, 0), geompp.Point3D(3, 1, 0))
        assert not s1.intersects(s2)
        assert s1.intersection(s2) is None

    def test_from_wkt_whitespace(self):
        expected = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        assert expected.almost_equals(geompp.LineSegment3D.from_wkt("LINESTRING (0 0 0,1 1 0)"))
        assert expected.almost_equals(geompp.LineSegment3D.from_wkt("LINESTRING (  0 0 0  ,  1  1  0  )"))

class TestLineSegment3DOverlap:
    def test_overlap_with_line(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert seg.overlaps(x)
        assert seg.overlap(x) == seg

    def test_no_overlap_offset_line(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        x_off = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(1, 1, 0))
        assert not seg.overlaps(x_off)
        assert seg.overlap(x_off) is None

    def test_overlap_segment_partial(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(7, 0, 0))
        assert a.overlaps(b)
        ov = a.overlap(b)
        assert ov == geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(5, 0, 0))

    def test_no_overlap_disjoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(6, 0, 0), geompp.Point3D(9, 0, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_touching_endpoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

class TestLineSegment3DTouch:
    def test_touch_with_line_first_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3))
        assert s.touches(x)
        t = s.touch(x)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)

    def test_touch_with_ray_endpoint(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2))
        assert s.touches(r)
        t = s.touch(r)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_segment_t_junction(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_segment_collinear_endpoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point3D(5, 0, 0)
