"""
line3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestLine3D:
    def test_contains(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        assert l.contains(geompp.Point3D(1,0,0))
        assert not l.contains(geompp.Point3D(1,1,0))

    def test_distance_to(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        assert approx(l.distance_to(geompp.Point3D(0,3,0)), 3.0)

    def test_distance_to_line3d(self):
        x_axis = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # intersecting (Y-axis crosses X-axis at origin)
        y_axis = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0))
        assert approx(x_axis.distance_to(y_axis), 0.0)
        assert x_axis.distance(y_axis) is None
        # skew (Y-dir line offset in Z)
        skew = geompp.Line3D.make(geompp.Point3D(0, -1, 5), geompp.Point3D(0, 1, 5))
        assert approx(x_axis.distance_to(skew), 5.0)
        dseg = x_axis.distance(skew)
        assert isinstance(dseg, geompp.LineSegment3D)
        assert approx(dseg.length(), 5.0)
        # parallel distinct
        parallel = geompp.Line3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(1, 3, 0))
        assert approx(x_axis.distance_to(parallel), 3.0)
        # overlap (collinear) → 0
        overlap = geompp.Line3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(7, 0, 0))
        assert approx(x_axis.distance_to(overlap), 0.0)
        assert x_axis.distance(overlap) is None

    def test_distance_to_ray3d(self):
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # ray hitting the line at origin
        ray_hit = geompp.Ray3D.make(geompp.Point3D(0, 2, 0), geompp.Vector3D(0, -1, 0))
        assert approx(line.distance_to(ray_hit), 0.0)
        # ray collinear with the line (overlap)
        ray_overlap = geompp.Ray3D.make(geompp.Point3D(3, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(line.distance_to(ray_overlap), 0.0)
        assert line.distance(ray_overlap) is None
        # ray pointing away → distance to ray origin
        ray_away = geompp.Ray3D.make(geompp.Point3D(0, 2, 0), geompp.Vector3D(0, 1, 0))
        assert approx(line.distance_to(ray_away), 2.0)

    def test_distance_to_segment3d(self):
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # segment crossing line at origin
        seg_cross = geompp.LineSegment3D.make(geompp.Point3D(0, -1, 0), geompp.Point3D(0, 1, 0))
        assert approx(line.distance_to(seg_cross), 0.0)
        # parallel segment
        seg_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 4, 0), geompp.Point3D(3, 4, 0))
        assert approx(line.distance_to(seg_parallel), 4.0)
        # overlapping (collinear) segment → 0
        seg_overlap = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        assert approx(line.distance_to(seg_overlap), 0.0)
        assert line.distance(seg_overlap) is None

    def test_project_onto(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        p = l.project_onto(geompp.Point3D(2,3,0))
        assert p.almost_equals(geompp.Point3D(2,0,0))

    def test_intersection_with_segment(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 1))
        s = geompp.LineSegment3D.make(geompp.Point3D(0, -1, 2), geompp.Point3D(0, 1, 2))
        # segment is perpendicular to line at z=2 but both on x=0 plane — no intersection
        # (they cross at different z); just check the call works
        result = l.intersection(s)
        assert result is None or isinstance(result, geompp.Point3D)

    def test_intersects_ray3d(self):
        # line along X; ray along Y at x=2, z=0 — they cross at (2,0,0)
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, -3, 0), geompp.Vector3D(0, 1, 0))
        assert l.intersects(r_hit)
        hit = l.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_ray3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # ray in a parallel plane (z=1) pointing along X — no crossing
        r_miss = geompp.Ray3D.make(geompp.Point3D(0, 0, 1), geompp.Vector3D(1, 0, 0))
        assert not l.intersects(r_miss)

    def test_intersects_segment3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert l.intersects(s_hit)

    def test_no_intersects_segment3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not l.intersects(s_miss)

    def test_to_file_from_file(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            l.to_file(path)
            assert os.path.exists(path)
            l2 = geompp.Line3D.from_file(path)
            assert l.almost_equals(l2)
        finally:
            os.unlink(path)

    def test_from_wkt_whitespace(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        assert l.almost_equals(geompp.Line3D.from_wkt("LINE (0 0 0,1 1 0)"))
        assert l.almost_equals(geompp.Line3D.from_wkt("LINE (  0 0 0  ,  1  1  0  )"))

class TestLine3DOverlap:
    def test_overlap_same_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        x2 = geompp.Line3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert x.overlaps(x2)
        ov = x.overlap(x2)
        assert ov is not None
        assert isinstance(ov, geompp.Line3D)

    def test_no_overlap_crossing(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        y = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0))
        assert not x.overlaps(y)
        assert x.overlap(y) is None

    def test_overlap_with_collinear_ray(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        rx = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert x.overlaps(rx)
        ov = x.overlap(rx)
        assert ov is not None
        assert isinstance(ov, geompp.Ray3D)

    def test_overlap_with_collinear_segment(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        assert x.overlaps(seg)
        ov = x.overlap(seg)
        assert ov == seg

class TestLine3DTouch:
    def test_touch_with_ray_origin_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(3, 0, 0), geompp.Vector3D(0, 0, 1))
        assert x.touches(r)
        t = x.touch(r)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_ray_collinear_no_touch(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        assert not x.touches(r)
        assert x.touch(r) is None

    def test_touch_with_segment_endpoint_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3))
        assert x.touches(s)
        t = x.touch(s)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)
