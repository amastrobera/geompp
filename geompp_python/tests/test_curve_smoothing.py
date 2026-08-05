"""
curve smoothing binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestBezierSmoothing2FreeFunction:
    def test_min_distance_not_positive_raises(self):
        p0, p1, p2 = geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)
        with pytest.raises(ValueError):
            geompp.bezier_smoothing_2(p0, p1, p2, 0.5, 0.0)

    def test_num_segments_less_than_one_raises(self):
        p0, p1, p2 = geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)
        with pytest.raises(ValueError):
            geompp.bezier_smoothing_2(p0, p1, p2, 0.5, 0)

    def test_endpoints_are_trimmed_tangents(self):
        # len1 == len2 == 2, smoothness=1.0 -> max_trim=1: T0=(1,0), T1=(2,1)
        p0, p1, p2 = geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)
        result = geompp.bezier_smoothing_2(p0, p1, p2, 1.0, 1.0)
        assert result[0].almost_equals(geompp.Point2D(1, 0))
        assert result[-1].almost_equals(geompp.Point2D(2, 1))

    def test_num_segments_overload_produces_exact_point_count(self):
        p0, p1, p2 = geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)
        result = geompp.bezier_smoothing_2(p0, p1, p2, 1.0, 3)
        assert len(result) == 4

    def test_smoothness_zero_collapses_to_corner(self):
        p0, p1, p2 = geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)
        result = geompp.bezier_smoothing_2(p0, p1, p2, 0.0, 0.5)
        assert all(p.almost_equals(p1) for p in result)

    def test_coincident_p0_p1_no_nan_collapses_to_corner(self):
        p1, p2 = geompp.Point2D(1, 1), geompp.Point2D(3, 1)
        result = geompp.bezier_smoothing_2(p1, p1, p2, 0.5, 0.1)
        for p in result:
            assert not math.isnan(p.x)
            assert not math.isnan(p.y)
            assert p.almost_equals(p1)

    def test_point3d_overload(self):
        p0 = geompp.Point3D(0, 0, 0)
        p1 = geompp.Point3D(2, 0, 0)
        p2 = geompp.Point3D(2, 2, 0)
        result = geompp.bezier_smoothing_2(p0, p1, p2, 1.0, 1.0)
        assert result[0].almost_equals(geompp.Point3D(1, 0, 0))
        assert result[-1].almost_equals(geompp.Point3D(2, 1, 0))
