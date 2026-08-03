"""
decimation binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestDecimationFreeFunctions:
    def test_dist_decimation_removes_clustered_points(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0.1, 0), geompp.Point2D(0.2, 0),
               geompp.Point2D(5, 0), geompp.Point2D(5.1, 0), geompp.Point2D(10, 0)]
        result = geompp.dist_decimation(pts, 1.0)
        assert len(result) == 3
        assert result[0].almost_equals(geompp.Point2D(0, 0))
        assert result[1].almost_equals(geompp.Point2D(5, 0))
        assert result[2].almost_equals(geompp.Point2D(10, 0))

    def test_dist_decimation_too_few_points_returns_unchanged(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 1)]
        result = geompp.dist_decimation(pts, 5.0)
        assert len(result) == 2

    def test_rdp_decimation_collinear_points_collapse_to_endpoints(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(2, 0),
               geompp.Point2D(3, 0), geompp.Point2D(4, 0)]
        result = geompp.rdp_decimation(pts, 0.5)
        assert len(result) == 2
        assert result[0].almost_equals(geompp.Point2D(0, 0))
        assert result[1].almost_equals(geompp.Point2D(4, 0))

    def test_rdp_decimation_keeps_peak_discards_shoulders(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 5),
               geompp.Point3D(6, 0, 0), geompp.Point3D(8, 0, 0)]
        result = geompp.rdp_decimation(pts, 2.0)
        assert len(result) == 3
        assert result[0].almost_equals(geompp.Point3D(0, 0, 0))
        assert result[1].almost_equals(geompp.Point3D(4, 0, 5))
        assert result[2].almost_equals(geompp.Point3D(8, 0, 0))

    def test_vw_decimation_keeps_high_area_vertex(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 5),
               geompp.Point2D(6, 0), geompp.Point2D(8, 0)]
        result = geompp.vw_decimation(pts, 6.0)
        assert len(result) == 3
        assert result[0].almost_equals(geompp.Point2D(0, 0))
        assert result[1].almost_equals(geompp.Point2D(4, 5))
        assert result[2].almost_equals(geompp.Point2D(8, 0))

    def test_vw_decimation_threshold_below_all_areas_keeps_all_points(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 5),
               geompp.Point3D(6, 0, 0), geompp.Point3D(8, 0, 0)]
        result = geompp.vw_decimation(pts, 1.0)
        assert len(result) == len(pts)

class TestPolylineDecimationParams:
    def test_defaults(self):
        p = geompp.PolylineDecimationParams()
        assert p.strategy == geompp.PolylineDecimationStrategy.RamerDouglasPeucker
        assert p.threshold == 0.5

    def test_explicit_construction_and_readwrite(self):
        p = geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.VisvalingamWhyatt, 2.0)
        assert p.strategy == geompp.PolylineDecimationStrategy.VisvalingamWhyatt
        assert p.threshold == 2.0
        p.threshold = 3.5
        assert p.threshold == 3.5
