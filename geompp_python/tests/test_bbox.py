"""
bbox binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestBBox2D:
    def test_from_points(self):
        bb = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(5, 5))
        assert approx(bb.min.x, 0)
        assert approx(bb.max.x, 5)

    def test_contains(self):
        bb = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(10, 10))
        assert bb.contains(geompp.Point2D(5, 5))
        assert not bb.contains(geompp.Point2D(11, 5))

    def test_eq(self):
        bb1 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        bb2 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert bb1 == bb2

    def test_from_line_segment2d(self):
        s = geompp.LineSegment2D.make(geompp.Point2D(-1, -2), geompp.Point2D(3, 4))
        bb = geompp.BBox2D(s)
        assert approx(bb.min.x, -1) and approx(bb.min.y, -2)
        assert approx(bb.max.x,  3) and approx(bb.max.y,  4)

    def test_from_polyline2d(self):
        pts = [geompp.Point2D(0, 5), geompp.Point2D(3, 0), geompp.Point2D(1, 2)]
        bb = geompp.BBox2D(geompp.Polyline2D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 5)

    def test_from_polygon2d(self):
        sq = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 3), geompp.Point2D(0, 3),
        ])
        bb = geompp.BBox2D(sq)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 4) and approx(bb.max.y, 3)

    def test_from_triangle2d(self):
        tri = geompp.Triangle2D.make(
            geompp.Point2D(0, 0), geompp.Point2D(3, 0), geompp.Point2D(0, 4)
        )
        bb = geompp.BBox2D(tri)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 4)

    def test_almost_equals(self):
        bb1 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        bb2 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert bb1.almost_equals(bb2)
        bb3 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(2, 2))
        assert not bb1.almost_equals(bb3)

class TestBBox3D:
    def test_from_points(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        assert approx(bb.max.z, 3)

    def test_contains(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 5, 5))
        assert bb.contains(geompp.Point3D(1, 1, 1))
        assert not bb.contains(geompp.Point3D(6, 1, 1))

    def test_from_line_segment(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(-1, -2, -3), geompp.Point3D(3, 4, 5))
        bb = geompp.BBox3D(s)
        assert approx(bb.min.x, -1) and approx(bb.min.y, -2) and approx(bb.min.z, -3)
        assert approx(bb.max.x,  3) and approx(bb.max.y,  4) and approx(bb.max.z,  5)

    def test_from_polyline(self):
        pts = [geompp.Point3D(0, 5, 1), geompp.Point3D(3, 0, 4), geompp.Point3D(1, 2, 0)]
        bb = geompp.BBox3D(geompp.Polyline3D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0) and approx(bb.min.z, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 5) and approx(bb.max.z, 4)

    def test_from_polygon(self):
        # ZX projection: reversed order so the triangle is CCW
        pts = [geompp.Point3D(4, 3, 5), geompp.Point3D(4, 0, 1), geompp.Point3D(0, 0, 1)]
        bb = geompp.BBox3D(geompp.Polygon3D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.z, 1)
        assert approx(bb.max.x, 4) and approx(bb.max.z, 5)

    def test_from_triangle(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(0, 3, 4)
        )
        bb = geompp.BBox3D(tri)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0) and approx(bb.min.z, 0)
        assert approx(bb.max.x, 2) and approx(bb.max.y, 3) and approx(bb.max.z, 4)

    def test_almost_equals(self):
        bb1 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        bb2 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        assert bb1.almost_equals(bb2)
        bb3 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 4))
        assert not bb1.almost_equals(bb3)
