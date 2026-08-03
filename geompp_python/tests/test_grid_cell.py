"""
grid cell binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestGridCell2D:
    def test_from_point_quantizes(self):
        cell = geompp.GridCell2D.from_point(geompp.Point2D(2.5, 3.5), 1.0)
        assert cell.x == 2 and cell.y == 3

    def test_from_point_nearby_points_same_cell(self):
        c1 = geompp.GridCell2D.from_point(geompp.Point2D(10.1, 10.1), 1.0)
        c2 = geompp.GridCell2D.from_point(geompp.Point2D(10.9, 10.9), 1.0)
        assert c1 == c2

    def test_from_point_distant_points_different_cell(self):
        c1 = geompp.GridCell2D.from_point(geompp.Point2D(0, 0), 1.0)
        c2 = geompp.GridCell2D.from_point(geompp.Point2D(100, 100), 1.0)
        assert not (c1 == c2)

    def test_from_point_default_epsilon(self):
        c1 = geompp.GridCell2D.from_point(geompp.Point2D(0, 0))
        c2 = geompp.GridCell2D.from_point(geompp.Point2D(0.0001, 0))
        assert c1 == c2

class TestGridCell3D:
    def test_from_point_quantizes(self):
        cell = geompp.GridCell3D.from_point(geompp.Point3D(2.5, 3.5, 4.5), 1.0)
        assert cell.x == 2 and cell.y == 3 and cell.z == 4

    def test_from_point_nearby_points_same_cell(self):
        c1 = geompp.GridCell3D.from_point(geompp.Point3D(10.1, 10.1, 10.1), 1.0)
        c2 = geompp.GridCell3D.from_point(geompp.Point3D(10.9, 10.9, 10.9), 1.0)
        assert c1 == c2

    def test_from_point_distant_points_different_cell(self):
        c1 = geompp.GridCell3D.from_point(geompp.Point3D(0, 0, 0), 1.0)
        c2 = geompp.GridCell3D.from_point(geompp.Point3D(100, 100, 100), 1.0)
        assert not (c1 == c2)
