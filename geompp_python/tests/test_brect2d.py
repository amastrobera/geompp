"""
brect2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestBRect2D:
    def test_empty_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([])

    def test_single_point_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([geompp.Point2D(3, 4)])

    def test_two_points_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])

    def test_axis_aligned_rectangle_center(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.center.almost_equals(geompp.Point2D(2, 1))
        assert approx(r.area, 8.0)
        assert approx(r.width, 4.0)
        assert approx(r.height, 2.0)

    def test_all_points_contained(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 2), geompp.Point2D(0, 2),
               geompp.Point2D(1, 0.5), geompp.Point2D(2, 1.5)]
        r = geompp.BRect2D(pts)
        for p in pts:
            assert r.contains(p), f"OBB must contain input point {p}"

    def test_axes_are_unit_vectors(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0),
               geompp.Point2D(2, 1), geompp.Point2D(0, 1)]
        r = geompp.BRect2D(pts)
        assert approx(r.axis_u.length(), 1.0)
        assert approx(r.axis_v.length(), 1.0)

    def test_axes_orthogonal(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert approx(r.axis_u.dot(r.axis_v), 0.0)

    def test_corners_returns_four_points(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        corners = r.corners()
        assert len(corners) == 4
        for c in corners:
            assert r.contains(c)

    def test_contains_center(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.contains(geompp.Point2D(2, 1))

    def test_contains_boundary(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.contains(geompp.Point2D(0, 0))    # corner
        assert r.contains(geompp.Point2D(4, 1))    # edge midpoint

    def test_contains_outside(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert not r.contains(geompp.Point2D(5, 1))
        assert not r.contains(geompp.Point2D(2, 3))

    def test_almost_equals(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r1 = geompp.BRect2D(pts)
        r2 = geompp.BRect2D(pts)
        assert r1.almost_equals(r2)
        assert r1 == r2

    def test_not_almost_equals_different(self):
        pts1 = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        pts2 = [geompp.Point2D(0, 0), geompp.Point2D(6, 0),
                geompp.Point2D(6, 2), geompp.Point2D(0, 2)]
        r1 = geompp.BRect2D(pts1)
        r2 = geompp.BRect2D(pts2)
        assert not r1.almost_equals(r2)
