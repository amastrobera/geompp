"""
ray3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestRay3D:
    def test_is_ahead(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r.is_ahead(geompp.Point3D(0, 0, 5))
        assert r.is_behind(geompp.Point3D(0, 0, -1))

    def test_contains(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert r.contains(geompp.Point3D(3,0,0))
        assert not r.contains(geompp.Point3D(3,1,0))
        assert not r.contains(geompp.Point3D(-1,0,0))

    def test_distance_to(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert approx(r.distance_to(geompp.Point3D(3,4,0)), 4.0)
        assert approx(r.distance_to(geompp.Point3D(-2,0,0)), 2.0)

    def test_distance_to_line3d(self):
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # line crosses ray at origin
        line_hit = geompp.Line3D.make(geompp.Point3D(0, -1, 0), geompp.Point3D(0, 1, 0))
        assert approx(ray.distance_to(line_hit), 0.0)
        # parallel line, offset by 5 in Y
        line_parallel = geompp.Line3D.make(geompp.Point3D(0, 5, 0), geompp.Point3D(1, 5, 0))
        assert approx(ray.distance_to(line_parallel), 5.0)
        # overlapping (collinear) line → 0
        line_overlap = geompp.Line3D.make(geompp.Point3D(-2, 0, 0), geompp.Point3D(7, 0, 0))
        assert approx(ray.distance_to(line_overlap), 0.0)
        assert ray.distance(line_overlap) is None

    def test_distance_to_ray3d(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # crossing rays
        r2 = geompp.Ray3D.make(geompp.Point3D(3, 1, 0), geompp.Vector3D(0, -1, 0))
        assert approx(r1.distance_to(r2), 0.0)
        # skew (offset in Z)
        r_skew = geompp.Ray3D.make(geompp.Point3D(0, 0, 4), geompp.Vector3D(0, 1, 0))
        assert approx(r1.distance_to(r_skew), 4.0)
        # overlapping (collinear) rays, same direction
        r_overlap = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(r1.distance_to(r_overlap), 0.0)
        assert r1.distance(r_overlap) is None
        # parallel distinct rays
        r_parallel = geompp.Ray3D.make(geompp.Point3D(0, 3, 0), geompp.Vector3D(1, 0, 0))
        assert approx(r1.distance_to(r_parallel), 3.0)

    def test_distance_to_segment3d(self):
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # segment crossing the ray at (2,0,0)
        seg_cross = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(ray.distance_to(seg_cross), 0.0)
        # parallel segment offset in Z
        seg_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 5), geompp.Point3D(4, 0, 5))
        assert approx(ray.distance_to(seg_parallel), 5.0)
        # collinear (overlap) → 0
        seg_overlap = geompp.LineSegment3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(3, 0, 0))
        assert approx(ray.distance_to(seg_overlap), 0.0)
        assert ray.distance(seg_overlap) is None

    def test_project_onto(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert r.project_onto(geompp.Point3D(3,5,0)).almost_equals(geompp.Point3D(3,0,0))
        assert r.project_onto(geompp.Point3D(-2,3,0)).almost_equals(geompp.Point3D(0,0,0))

    def test_to_line(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        l = r.to_line()
        assert isinstance(l, geompp.Line3D)
        assert l.contains(r.origin)

    def test_almost_equals(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r1.almost_equals(r2)
        r3 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 1, 0))
        assert not r1.almost_equals(r3)

    def test_wkt_roundtrip(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 2, 3), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.from_wkt(r.to_wkt())
        assert r.almost_equals(r2)

    def test_from_wkt_whitespace(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r.almost_equals(geompp.Ray3D.from_wkt("RAY (0 0 0,1 0 0)"))
        assert r.almost_equals(geompp.Ray3D.from_wkt("RAY (  0 0 0  ,  1  0  0  )"))

    def test_to_file_from_file(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 2, 3), geompp.Vector3D(1, 0, 0))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            r.to_file(path)
            assert os.path.exists(path)
            r2 = geompp.Ray3D.from_file(path)
            assert r.almost_equals(r2)
        finally:
            os.unlink(path)

    def test_intersects_line3d(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        l_hit = geompp.Line3D.make(geompp.Point3D(3, -2, 0), geompp.Point3D(3, 2, 0))
        assert r.intersects(l_hit)
        hit = r.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # line parallel and offset — no intersection
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not r.intersects(l_miss)
        assert r.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(3, -2, 0), geompp.Vector3D(0, 1, 0))
        assert r1.intersects(r2)
        hit = r1.intersection(r2)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # two parallel rays — no intersection
        r_miss = geompp.Ray3D.make(geompp.Point3D(0, 1, 0), geompp.Vector3D(1, 0, 0))
        assert not r1.intersects(r_miss)
        assert r1.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(3, -2, 0), geompp.Point3D(3, 2, 0))
        assert r.intersects(s_hit)
        hit = r.intersection(s_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # segment behind the ray — no intersection
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(-3, -2, 0), geompp.Point3D(-3, 2, 0))
        assert not r.intersects(s_miss)
        assert r.intersection(s_miss) is None

class TestRay3DOverlap:
    def test_overlap_same_direction(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r1.overlaps(r2)
        ov = r1.overlap(r2)
        assert ov is not None
        assert isinstance(ov, geompp.Ray3D)

    def test_overlap_anti_parallel(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r3 = geompp.Ray3D.make(geompp.Point3D(5, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert r1.overlaps(r3)
        ov = r1.overlap(r3)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment3D)

    def test_no_overlap_touching(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r4 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert not r1.overlaps(r4)
        assert r1.overlap(r4) is None

    def test_overlap_segment_inside_ray(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 0))
        assert r.overlaps(seg)
        assert r.overlap(seg) == seg

    def test_no_overlap_segment_before_ray(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(-3, 0, 0), geompp.Point3D(-1, 0, 0))
        assert not r.overlaps(seg)
        assert r.overlap(seg) is None

class TestRay3DTouch:
    def test_touch_with_line_origin_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r.touches(x)
        t = r.touch(x)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)

    def test_touch_with_ray_same_origin(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point3D(0, 0, 0)

    def test_touch_with_ray_anti_parallel_same_origin(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point3D(0, 0, 0)

    def test_touch_with_segment_endpoint(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2))
        assert r.touches(s)
        t = r.touch(s)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)
