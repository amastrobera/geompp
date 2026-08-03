"""
triangulate binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestTriangulate:
    def test_2d_default_settings(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        triangles = geompp.triangulate(pts)
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 8.0)

    def test_3d_with_explicit_normal(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(4, 2, 0), geompp.Point3D(0, 2, 0)]
        triangles = geompp.triangulate(pts, geompp.Vector3D(0, 0, 1))
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 8.0)

    def test_3d_without_normal_fits_via_pca(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(4, 2, 0), geompp.Point3D(0, 2, 0)]
        triangles = geompp.triangulate(pts)
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 8.0)

    def test_fewer_than_three_points_raises(self):
        with pytest.raises(ValueError):
            geompp.triangulate([geompp.Point2D(0, 0), geompp.Point2D(1, 0)])

    def test_winding_assert_raises_on_clockwise_input(self):
        cw_square = [geompp.Point2D(0, 0), geompp.Point2D(0, 1), geompp.Point2D(1, 1), geompp.Point2D(1, 0)]
        settings = geompp.TriangulationParams(ccw_winding=geompp.TriangulationWinding.Assert)
        with pytest.raises(ValueError):
            geompp.triangulate(cw_square, settings)

    def test_winding_enforce_fixes_clockwise_input(self):
        cw_square = [geompp.Point2D(0, 0), geompp.Point2D(0, 1), geompp.Point2D(1, 1), geompp.Point2D(1, 0)]
        settings = geompp.TriangulationParams(ccw_winding=geompp.TriangulationWinding.Enforce)
        triangles = geompp.triangulate(cw_square, settings)
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 1.0)

    def test_collinearity_assert_raises_on_collinear_point(self):
        with_collinear = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 0),
                          geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        settings = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Assert)
        with pytest.raises(ValueError):
            geompp.triangulate(with_collinear, settings)

    def test_collinearity_enforce_removes_collinear_point(self):
        with_collinear = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 0),
                          geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        settings = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Enforce)
        triangles = geompp.triangulate(with_collinear, settings)
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 16.0)

    def test_simplicity_assert_raises_on_self_intersecting_input(self):
        bowtie = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1), geompp.Point2D(1, 1)]
        settings = geompp.TriangulationParams(simplicity=geompp.TriangulationSimplicity.Assert)
        with pytest.raises(ValueError):
            geompp.triangulate(bowtie, settings)

    def test_collinearity_guaranteed_with_midpoint_vertex_does_not_hang(self):
        # Regression test: a 4x4 square with a redundant vertex (2, 0) sitting exactly on the middle
        # of the bottom edge -- neither reflex nor a normal convex ear. With collinearity Guaranteed
        # (skip the check, so the redundant point survives into ear_clipping_triangulation), a stale
        # index bug in the collinear-vertex skip branch could previously hang the algorithm forever.
        square_with_midpoint = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(4, 0),
                                geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        settings = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Guaranteed)
        triangles = geompp.triangulate(square_with_midpoint, settings)
        assert len(triangles) == 3
        assert approx(sum(t.area() for t in triangles), 16.0)

    def test_collinearity_guaranteed_with_midpoint_at_ring_start_does_not_hang(self):
        # Same shape, rotated so the collinear vertex is at index 0 -- the exact case that hung.
        square_with_midpoint = [geompp.Point2D(2, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
                                geompp.Point2D(0, 4), geompp.Point2D(0, 0)]
        settings = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Guaranteed)
        triangles = geompp.triangulate(square_with_midpoint, settings)
        assert len(triangles) == 3
        assert approx(sum(t.area() for t in triangles), 16.0)
