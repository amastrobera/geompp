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

    def test_default_strategy_is_ear_clipping_best_fit(self):
        assert geompp.TriangulationParams().strategy == geompp.TriangulationStrategy.EarClippingBestFit

    def test_ear_clipping_best_fit_strategy_succeeds(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        settings = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.EarClippingBestFit)
        triangles = geompp.triangulate(pts, settings)
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), 8.0)

    def test_ear_clipping_best_fit_picks_different_diagonals_than_plain_ear_clipping(self):
        # A 5-pointed star: EarClipping (first valid ear in scan order) fans every triangle out from
        # one vertex; EarClippingBestFit (best-scoring valid ear each step) clips all 5 outer points
        # first, then fans only the remaining inner pentagon. Both are valid triangulations of the same
        # polygon -- same triangle count and total area -- but via genuinely different diagonals.
        star = [
            geompp.Point2D(3.0, 6.0), geompp.Point2D(2.29, 3.97), geompp.Point2D(0.15, 3.93),
            geompp.Point2D(1.86, 2.63), geompp.Point2D(1.24, 0.57), geompp.Point2D(3.0, 1.8),
            geompp.Point2D(4.76, 0.57), geompp.Point2D(4.14, 2.63), geompp.Point2D(5.85, 3.93),
            geompp.Point2D(3.71, 3.97),
        ]
        plain_settings = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.EarClipping)
        best_fit_settings = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.EarClippingBestFit)

        plain = geompp.triangulate(star, plain_settings)
        best_fit = geompp.triangulate(star, best_fit_settings)

        assert len(plain) == 8
        assert len(best_fit) == 8
        assert approx(sum(t.area() for t in plain), sum(t.area() for t in best_fit))

        plain_wkt = {t.to_wkt() for t in plain}
        best_fit_wkt = {t.to_wkt() for t in best_fit}
        assert plain_wkt != best_fit_wkt

    def test_validate_adjacency_conforming_returns_no_violations(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        assert geompp.validate_adjacency([p0, p1]) == []

    def test_validate_adjacency_t_junction_detects_violation(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])

        violations = geompp.validate_adjacency([p0, p1, roof])

        assert len(violations) > 0
        assert all(not v.is_non_manifold for v in violations)

    def test_validate_adjacency_non_manifold_edge_detects_violation(self):
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1)])
        c = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2)])

        violations = geompp.validate_adjacency([a, b, c])

        assert len(violations) > 0
        assert violations[0].is_non_manifold
        assert len(list(violations[0].facet_indices)) == 3

    def test_fix_adjacency_t_junction_splices_vertex_and_preserves_total_area(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        facets = [p0, p1, roof]
        area_before = sum(f.area() for f in facets)

        fixed = geompp.fix_adjacency(facets)

        assert geompp.validate_adjacency(fixed) == []
        guaranteed_collinearity = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Guaranteed)
        area_after = sum(t.area() for ring in fixed for t in geompp.triangulate(ring, guaranteed_collinearity))
        assert approx(area_before, area_after)

    def test_fix_adjacency_non_manifold_edge_raises(self):
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1)])
        c = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2)])
        with pytest.raises(ValueError):
            geompp.fix_adjacency([a, b, c])

    def test_triangulate_polygon_batch_default_enforce_fixes_t_junction(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])

        triangles = geompp.triangulate([p0, p1, roof])

        assert approx(sum(t.area() for t in triangles), 3.0)

    def test_triangulate_polygon_batch_assert_raises_on_t_junction(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        with pytest.raises(ValueError):
            geompp.triangulate([p0, p1, roof], geompp.AdjacencyConformity.Assert)
