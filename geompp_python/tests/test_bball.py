"""
bball binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestBBall2D:
    def test_constructor_center_radius(self):
        b = geompp.BBall2D(geompp.Point2D(1, 2), 5.0)
        assert b.center.almost_equals(geompp.Point2D(1, 2))
        assert approx(b.radius, 5.0)

    def test_constructor_from_single_point(self):
        b = geompp.BBall2D([geompp.Point2D(3, 4)])
        assert b.center.almost_equals(geompp.Point2D(3, 4))
        assert approx(b.radius, 0.0)

    def test_constructor_from_two_points(self):
        b = geompp.BBall2D([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        assert b.center.almost_equals(geompp.Point2D(2, 0))
        assert approx(b.radius, 2.0)

    def test_constructor_from_points_all_contained(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(2, 3), geompp.Point2D(-1, 1.5)]
        b = geompp.BBall2D(pts)
        for p in pts:
            assert b.contains(p)

    def test_contains_center(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert b.contains(geompp.Point2D(0, 0))

    def test_contains_boundary(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert b.contains(geompp.Point2D(3, 4))   # 3-4-5 right triangle
        assert b.contains(geompp.Point2D(5, 0))

    def test_contains_outside(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert not b.contains(geompp.Point2D(4, 4))   # dist ≈ 5.657
        assert not b.contains(geompp.Point2D(6, 0))

    def test_almost_equals(self):
        b1 = geompp.BBall2D(geompp.Point2D(1, 2), 3.0)
        b2 = geompp.BBall2D(geompp.Point2D(1, 2), 3.0)
        b3 = geompp.BBall2D(geompp.Point2D(0, 0), 1.0)
        assert b1.almost_equals(b2)
        assert not b1.almost_equals(b3)
        assert b1 == b2

class TestBBall3D:
    def test_constructor_center_radius(self):
        b = geompp.BBall3D(geompp.Point3D(1, 2, 3), 5.0)
        assert b.center.almost_equals(geompp.Point3D(1, 2, 3))
        assert approx(b.radius, 5.0)

    def test_constructor_from_single_point(self):
        b = geompp.BBall3D([geompp.Point3D(1, 2, 3)])
        assert b.center.almost_equals(geompp.Point3D(1, 2, 3))
        assert approx(b.radius, 0.0)

    def test_constructor_from_two_points(self):
        b = geompp.BBall3D([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        assert b.center.almost_equals(geompp.Point3D(2, 0, 0))
        assert approx(b.radius, 2.0)

    def test_constructor_from_points_all_contained(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(0, 3, 0), geompp.Point3D(0, 0, 2)]
        b = geompp.BBall3D(pts)
        for p in pts:
            assert b.contains(p)

    def test_contains_center(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert b.contains(geompp.Point3D(0, 0, 0))

    def test_contains_boundary(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert b.contains(geompp.Point3D(3, 4, 0))   # 3-4-5 in XY plane
        assert b.contains(geompp.Point3D(0, 0, 5))

    def test_contains_outside(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert not b.contains(geompp.Point3D(4, 4, 0))   # dist ≈ 5.657
        assert not b.contains(geompp.Point3D(0, 0, 6))

    def test_almost_equals(self):
        b1 = geompp.BBall3D(geompp.Point3D(1, 2, 3), 4.0)
        b2 = geompp.BBall3D(geompp.Point3D(1, 2, 3), 4.0)
        b3 = geompp.BBall3D(geompp.Point3D(0, 0, 0), 1.0)
        assert b1.almost_equals(b2)
        assert not b1.almost_equals(b3)
        assert b1 == b2
