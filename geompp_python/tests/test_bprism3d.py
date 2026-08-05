"""
bprism3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestBPrism3D:
    def test_empty_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([])

    def test_single_point_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([geompp.Point3D(3, 4, 5)])

    def test_two_points_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])

    def test_axis_aligned_box(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.volume, 24.0, eps=0.5)
        for pt in pts:
            assert p.contains(pt), f"prism must contain input point {pt}"

    def test_flat_cloud_w_is_epsilon(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        p = geompp.BPrism3D(pts)
        assert p.half_len_w > 0, "flat cloud must have non-zero w half-length"
        assert approx(p.half_len_w, 0.001, eps=0.01)

    def test_nonconvex_all_points_contained(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0),
            geompp.Point3D(3, 2, 0), geompp.Point3D(0, 2, 0),
            geompp.Point3D(1, 0.5, 1), geompp.Point3D(2, 1.5, 0.5),
        ]
        p = geompp.BPrism3D(pts)
        for pt in pts:
            assert p.contains(pt), f"prism must contain input point {pt}"

    def test_axes_are_unit_vectors(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.axis_u.length(), 1.0)
        assert approx(p.axis_v.length(), 1.0)
        assert approx(p.axis_w.length(), 1.0)

    def test_axes_orthogonal(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.axis_u.dot(p.axis_v), 0.0)
        assert approx(p.axis_u.dot(p.axis_w), 0.0)
        assert approx(p.axis_v.dot(p.axis_w), 0.0)

    def test_width_height_depth_volume(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.width, 2.0 * p.half_len_u)
        assert approx(p.height, 2.0 * p.half_len_v)
        assert approx(p.depth, 2.0 * p.half_len_w)
        assert approx(p.volume, p.width * p.height * p.depth, eps=0.5)

    def test_corners_eight_distinct_all_contained(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        corners = p.corners()
        assert len(corners) == 8
        for c in corners:
            assert p.contains(c), "each corner must be inside the prism"

    def test_contains_center(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert p.contains(p.center)

    def test_contains_outside_false(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert not p.contains(geompp.Point3D(10, 10, 10))

    def test_almost_equals(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p1 = geompp.BPrism3D(pts)
        p2 = geompp.BPrism3D(pts)
        assert p1.almost_equals(p2)
        assert p1 == p2

    def test_not_almost_equals_different(self):
        pts1 = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        pts2 = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0),
            geompp.Point3D(6, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        p1 = geompp.BPrism3D(pts1)
        p2 = geompp.BPrism3D(pts2)
        assert not p1.almost_equals(p2)
