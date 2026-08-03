"""
polyline expansion binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPolylineExpansionParams:
    def test_defaults(self):
        # min_segment_length defaults to DOUBLE_EPSILON, which tracks the current DECIMAL_PRECISION
        # (10 ** -precision) rather than a fixed constant.
        geompp.set_decimal_precision(geompp.DP_THREE)
        p = geompp.PolylineExpansionParams()
        assert p.smoothness == 0.5
        assert p.mode == geompp.PolylineExpansionMode.FixedSegments
        assert p.segments_per_corner == 4
        assert p.min_distance == 0.1
        assert p.min_segment_length == pytest.approx(10 ** -geompp.DP_THREE)

    def test_explicit_construction_and_readwrite(self):
        p = geompp.PolylineExpansionParams(smoothness=1.0, mode=geompp.PolylineExpansionMode.MinDistance,
                                           min_distance=0.2, min_segment_length=0.05)
        assert p.smoothness == 1.0
        assert p.mode == geompp.PolylineExpansionMode.MinDistance
        assert p.min_distance == 0.2
        assert p.min_segment_length == 0.05
        p.segments_per_corner = 10
        assert p.segments_per_corner == 10

class TestPolylineExpansionFreeFunction:
    def test_two_point_input_returns_unchanged(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 1)]
        result = geompp.polyline_expansion(pts, geompp.PolylineExpansionParams())
        assert len(result) == 2
        assert result[0].almost_equals(pts[0])
        assert result[1].almost_equals(pts[1])

    def test_smooths_inner_corner(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2)]
        settings = geompp.PolylineExpansionParams(smoothness=1.0, segments_per_corner=3)
        result = geompp.polyline_expansion(pts, settings)
        assert result[0].almost_equals(geompp.Point2D(0, 0))
        assert result[-1].almost_equals(geompp.Point2D(2, 2))
        assert any(p.almost_equals(geompp.Point2D(1, 0)) for p in result)
        assert any(p.almost_equals(geompp.Point2D(2, 1)) for p in result)

    def test_multiple_corners_each_uses_its_own_original_knots(self):
        # Regression guard: p0/p1 for each corner must come from the original input, not from the
        # already-built output — see the C++ test of the same name for the full rationale. Two
        # consecutive right-angle corners sharing a length-2 edge, smoothness=1.0, so both corners'
        # trims meet exactly at the shared edge's midpoint (2, 1), deduplicated to one occurrence.
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2), geompp.Point2D(0, 2)]
        settings = geompp.PolylineExpansionParams(smoothness=1.0, segments_per_corner=3)
        result = geompp.polyline_expansion(pts, settings)
        assert result[0].almost_equals(geompp.Point2D(0, 0))
        assert result[-1].almost_equals(geompp.Point2D(0, 2))
        midpoint_count = sum(1 for p in result if p.almost_equals(geompp.Point2D(2, 1)))
        assert midpoint_count == 1
        assert any(p.almost_equals(geompp.Point2D(1, 2)) for p in result)

    def test_min_segment_length_skips_short_corner(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0.5, 0), geompp.Point2D(0.5, 0.5)]
        settings = geompp.PolylineExpansionParams(smoothness=1.0, segments_per_corner=4, min_segment_length=1.0)
        result = geompp.polyline_expansion(pts, settings)
        assert len(result) == len(pts)
        for a, b in zip(result, pts):
            assert a.almost_equals(b)
