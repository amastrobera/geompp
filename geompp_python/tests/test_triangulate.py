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
        assert all(len(list(v.facet_indices)) <= 1 for v in violations)

    def test_validate_adjacency_non_manifold_edge_detects_violation(self):
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1)])
        c = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2)])

        violations = geompp.validate_adjacency([a, b, c])

        assert len(violations) > 0
        assert len(list(violations[0].facet_indices)) == 3  # 3+ facets sharing an edge == non-manifold

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

    def test_fix_adjacency_t_junction_splits_coarse_facet_instead_of_just_splicing(self):
        # roof has exactly 1 T-junction (the shared p0/p1 corner (1,1) lands on its base edge).
        # fix_adjacency() cuts a diagonal from that spliced vertex to its nearest valid ring vertex --
        # here that's roof's apex, so roof splits clean into 2 triangles.
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        facets = [p0, p1, roof]

        fixed = geompp.fix_adjacency(facets)

        assert len(fixed) == 4  # p0, p1 pass through unchanged, roof splits into 2
        assert all(len(ring) <= 4 for ring in fixed)
        assert geompp.validate_adjacency(fixed) == []

    def test_fix_adjacency_multiple_t_junctions_on_one_edge_splits_into_several_pieces(self):
        # The "T-junction house": a wide base spanning both squares plus their side overhangs, with 3
        # foreign vertices ((0,0), (1,0), (2,0)) landing on its single top edge -- each cuts its own
        # diagonal, carving the base into 4 rectangular strips instead of one 7-vertex polygon.
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        # Base is deliberately NOT centered on the middle spliced vertex (1,0) -- a symmetric base
        # (e.g. -0.5..2.5, center x=1.0) makes that vertex exactly equidistant from both bottom
        # corners, an undefined tie for split_facets_at_junctions_impl's nearest-valid-diagonal search.
        base = geompp.Polygon2D.make([geompp.Point2D(-0.7, -1.2), geompp.Point2D(2.5, -1.2),
                                      geompp.Point2D(2.5, 0), geompp.Point2D(-0.7, 0)])
        facets = [base, p0, p1]
        area_before = sum(f.area() for f in facets)

        fixed = geompp.fix_adjacency(facets)

        assert len(fixed) == 6  # base -> 4 strips, p0/p1 pass through unchanged (2)
        assert geompp.validate_adjacency(fixed) == []

        guaranteed_collinearity = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Guaranteed)
        area_after = sum(t.area() for ring in fixed for t in geompp.triangulate(ring, guaranteed_collinearity))
        assert approx(area_before, area_after)

    def test_fix_adjacency_non_manifold_edge_no_longer_raises_produces_degenerate_ring(self):
        # AdjacencyViolation.is_non_manifold was removed (see CHANGELOG [0.18.0] Removed) --
        # fix_adjacency() no longer refuses a non-manifold-edge input up front. It now splices
        # on_vertex (for a non-manifold violation, just a reused edge endpoint, not a real foreign
        # vertex) right next to itself, producing a ring with a coincident/zero-length-edge vertex
        # pair instead of raising. Documents the current (not ideal) behavior.
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1)])
        c = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2)])

        fixed = geompp.fix_adjacency([a, b, c])

        found_degenerate_edge = False
        for ring in fixed:
            n = len(ring)
            for i in range(n):
                if ring[i].almost_equals(ring[(i + 1) % n]):
                    found_degenerate_edge = True
        assert found_degenerate_edge

    def test_fix_adjacency_triangle_t_junction_retriangulates_and_preserves_total_area(self):
        # Big triangle a sitting on two small triangles b, c -- b and c's shared vertex (2,0) lies in
        # the interior of a's base edge (0,0)-(4,0), a T-junction. a can't just absorb (2,0) and stay
        # a triangle, so fix_adjacency() must re-triangulate it into 2 triangles covering the same area.
        a = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(2, 3))
        b = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, -1.5), geompp.Point2D(2, 0))
        c = geompp.Triangle2D.make(geompp.Point2D(2, 0), geompp.Point2D(3, -1.5), geompp.Point2D(4, 0))
        facets = [a, b, c]
        area_before = sum(f.area() for f in facets)
        assert approx(area_before, 9.0)

        assert len(geompp.validate_adjacency(facets)) > 0

        fixed = geompp.fix_adjacency(facets)

        assert len(fixed) == 4  # a -> 2 triangles, b and c pass through unchanged
        assert geompp.validate_adjacency(fixed) == []
        assert approx(sum(t.area() for t in fixed), area_before)

    def test_fix_adjacency_triangle_non_manifold_edge_raises_confusing_runtime_error(self):
        # Unlike the Polygon2D overload above, this one still raises -- but no longer the clear
        # ValueError naming the real problem (edge shared by 3+ facets). fix_adjacency_impl's splice
        # inserts on_vertex (a reused edge endpoint for a non-manifold violation) right next to
        # itself, and the coincident-point pair then trips a degeneracy guard somewhere downstream
        # in re-triangulation, surfacing as an unrelated-looking RuntimeError about two points being
        # too close. Documents the current (confusing but non-silent) behavior.
        a = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1))
        b = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1))
        c = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2))
        with pytest.raises(RuntimeError):
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
            geompp.triangulate([p0, p1, roof], geompp.TriangulationParams(conformity=geompp.AdjacencyConformity.Assert))

    # ── native 3D adjacency (validate_adjacency / fix_adjacency), not View2D-projected ─────────────

    def test_validate_adjacency_3d_polygon_t_junction_detects_violation(self):
        # Two unit squares side by side (y=0 plane), plus a roof triangle spanning both squares' top --
        # its base edge passes straight through the squares' shared vertex. Native 3D, not View2D.
        p0 = geompp.Polygon3D.make([geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1),
                                    geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make([geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1),
                                    geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make([geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])

        violations = geompp.validate_adjacency([p0, p1, roof])

        assert len(violations) > 0
        assert all(len(list(v.facet_indices)) <= 1 for v in violations)

    def test_fix_adjacency_3d_polygon_t_junction_splices_vertex_and_preserves_total_area(self):
        p0 = geompp.Polygon3D.make([geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1),
                                    geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make([geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1),
                                    geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make([geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])
        facets = [p0, p1, roof]
        area_before = sum(f.area() for f in facets)

        fixed = geompp.fix_adjacency(facets)

        assert geompp.validate_adjacency(fixed) == []
        guaranteed_collinearity = geompp.TriangulationParams(collinearity=geompp.TriangulationCollinearity.Guaranteed)
        area_after = sum(t.area() for ring in fixed for t in geompp.triangulate(ring, geompp.Vector3D(0, -1, 0), guaranteed_collinearity))
        assert approx(area_before, area_after)

    def test_fix_adjacency_3d_polygon_t_junction_splits_coarse_facet_instead_of_just_splicing(self):
        p0 = geompp.Polygon3D.make([geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1),
                                    geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make([geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1),
                                    geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make([geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])
        facets = [p0, p1, roof]

        fixed = geompp.fix_adjacency(facets)

        assert len(fixed) == 4  # p0, p1 pass through unchanged, roof splits into 2
        assert all(len(ring) <= 4 for ring in fixed)
        assert geompp.validate_adjacency(fixed) == []

    def test_fix_adjacency_3d_triangle_t_junction_retriangulates_and_preserves_total_area(self):
        # Native 3D analog of the 2D "big triangle over two small triangles" case (y=0 plane).
        # Triangle3D.make() has no winding constraint, unlike Polygon3D.make() above.
        a = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(2, 0, 3))
        b = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, -1.5), geompp.Point3D(2, 0, 0))
        c = geompp.Triangle3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(3, 0, -1.5), geompp.Point3D(4, 0, 0))
        facets = [a, b, c]
        area_before = sum(f.area() for f in facets)
        assert approx(area_before, 9.0)

        assert len(geompp.validate_adjacency(facets)) > 0

        fixed = geompp.fix_adjacency(facets)

        assert len(fixed) == 4
        assert geompp.validate_adjacency(fixed) == []
        assert approx(sum(t.area() for t in fixed), area_before)

    def test_fix_adjacency_3d_triangle_non_manifold_edge_raises_confusing_runtime_error(self):
        # Same as test_fix_adjacency_triangle_non_manifold_edge_raises_confusing_runtime_error above,
        # native 3D: still raises, but now a RuntimeError from a downstream degeneracy guard rather
        # than the clear ValueError naming the real problem, since is_non_manifold was removed.
        a = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, 1, 0))
        b = geompp.Triangle3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, 0, 1))
        c = geompp.Triangle3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, -1, 0))
        with pytest.raises(RuntimeError):
            geompp.fix_adjacency([a, b, c])

    def test_fix_adjacency_3d_non_manifold_edge_no_longer_raises_produces_degenerate_ring(self):
        # Polygon3D overload: same as test_fix_adjacency_non_manifold_edge_no_longer_raises_produces_degenerate_ring
        # above, native 3D.
        a = geompp.Polygon3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, 1, 0)])
        b = geompp.Polygon3D.make([geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, 0, 1)])
        c = geompp.Polygon3D.make([geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, -1, 0)])

        fixed = geompp.fix_adjacency([a, b, c])

        found_degenerate_edge = False
        for ring in fixed:
            n = len(ring)
            for i in range(n):
                if ring[i].almost_equals(ring[(i + 1) % n]):
                    found_degenerate_edge = True
        assert found_degenerate_edge

    def test_triangulate_monotone_polygon_square(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)]
        params = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.MonotonePolygon)
        tris = geompp.triangulate(pts, params)
        assert len(tris) == 2
        assert abs(sum(t.area() for t in tris) - 1.0) < 1e-6

    def test_triangulate_constrained_delaunay_square(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)]
        params = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.ConstrainedDelaunay)
        tris = geompp.triangulate(pts, params)
        assert len(tris) == 2
        assert abs(sum(t.area() for t in tris) - 1.0) < 1e-6

    # ── TriangulationParams.monotonicity (partition into y-monotone pieces) ────────────────────────

    def test_monotonicity_defaults_to_guaranteed(self):
        assert geompp.TriangulationParams().monotonicity == geompp.TriangulationMonotonicity.Guaranteed

    def test_monotonicity_assert_raises_on_non_monotone_star(self):
        # Same 5-pointed star as visual_doc_and_sample_code.md §12.2 -- fails is_axis_monotone on
        # every axis.
        star = [
            geompp.Point2D(3.0, 6.0), geompp.Point2D(2.29, 3.97), geompp.Point2D(0.15, 3.93),
            geompp.Point2D(1.86, 2.63), geompp.Point2D(1.24, 0.57), geompp.Point2D(3.0, 1.8),
            geompp.Point2D(4.76, 0.57), geompp.Point2D(4.14, 2.63), geompp.Point2D(5.85, 3.93),
            geompp.Point2D(3.71, 3.97),
        ]
        settings = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.MonotonePolygon,
                                               monotonicity=geompp.TriangulationMonotonicity.Assert)
        with pytest.raises(ValueError):
            geompp.triangulate(star, settings)

    def test_monotonicity_enforce_on_non_monotone_star_matches_ear_clipping_area(self):
        star = [
            geompp.Point2D(3.0, 6.0), geompp.Point2D(2.29, 3.97), geompp.Point2D(0.15, 3.93),
            geompp.Point2D(1.86, 2.63), geompp.Point2D(1.24, 0.57), geompp.Point2D(3.0, 1.8),
            geompp.Point2D(4.76, 0.57), geompp.Point2D(4.14, 2.63), geompp.Point2D(5.85, 3.93),
            geompp.Point2D(3.71, 3.97),
        ]
        enforced = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.MonotonePolygon,
                                               monotonicity=geompp.TriangulationMonotonicity.Enforce)
        tris = geompp.triangulate(star, enforced)
        assert len(tris) == len(star) - 2
        for t in tris:
            assert t.area() > 0.0

        reference = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.EarClippingBestFit)
        ref_tris = geompp.triangulate(star, reference)
        assert approx(sum(t.area() for t in tris), sum(t.area() for t in ref_tris))

    def test_monotonicity_enforce_on_already_monotone_comb_matches_guaranteed_exactly(self):
        # Same 3-tooth comb as §12.2 -- already y-monotone, so Enforce must take the exact same code
        # path as Guaranteed and produce an identical triangulation.
        comb = [
            geompp.Point2D(5, 0), geompp.Point2D(5, 10), geompp.Point2D(4, 10), geompp.Point2D(4, 9),
            geompp.Point2D(3, 9), geompp.Point2D(3, 10), geompp.Point2D(2, 10), geompp.Point2D(2, 9),
            geompp.Point2D(1, 9), geompp.Point2D(1, 10), geompp.Point2D(0, 10), geompp.Point2D(0, 0),
        ]
        guaranteed = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.MonotonePolygon,
                                                 monotonicity=geompp.TriangulationMonotonicity.Guaranteed)
        guaranteed_tris = geompp.triangulate(comb, guaranteed)

        enforced = geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.MonotonePolygon,
                                               monotonicity=geompp.TriangulationMonotonicity.Enforce)
        enforced_tris = geompp.triangulate(comb, enforced)

        assert len(guaranteed_tris) == len(enforced_tris)
        assert [t.to_wkt() for t in guaranteed_tris] == [t.to_wkt() for t in enforced_tris]


# ── ConstrainedDelaunay strategy / delaunay() point cloud ───────────────────────────────────────────
# Mirrors geompp_tests/src/test_calc_utils2d.cpp / test_calc_utils3d.cpp, region
# "ConstrainedDelaunay strategy / delaunay() point cloud".

def _p2(coords):
    return [geompp.Point2D(x, y) for x, y in coords]


def _p3(coords):
    return [geompp.Point3D(x, y, z) for x, y, z in coords]


def _ccw(t):
    a, b, c = t.vertices
    return (a, b, c) if t.is_ccw() else (a, c, b)


def _total_area(tris):
    return sum(t.area() for t in tris)


def _is_globally_delaunay(tris, points):
    for t in tris:
        a, b, c = _ccw(t)
        for p in points:
            if geompp.in_circumcircle(a, b, c, p):
                return False
    return True


_STAR = [(3.0, 6.0), (2.29, 3.97), (0.15, 3.93), (1.86, 2.63), (1.24, 0.57),
         (3.0, 1.8), (4.76, 0.57), (4.14, 2.63), (5.85, 3.93), (3.71, 3.97)]

_COMB = [(5, 0), (5, 10), (4, 10), (4, 9), (3, 9), (3, 10),
         (2, 10), (2, 9), (1, 9), (1, 10), (0, 10), (0, 0)]


def _cdt_params():
    return geompp.TriangulationParams(strategy=geompp.TriangulationStrategy.ConstrainedDelaunay)


class TestConstrainedDelaunayAndDelaunay:
    def test_strategy_enum_ordinal_and_rename(self):
        assert int(geompp.TriangulationStrategy.ConstrainedDelaunay) == 3
        assert not hasattr(geompp.TriangulationStrategy, "Delaunay")

    def test_cdt_comb_stays_inside_polygon(self):
        comb = _p2(_COMB)
        poly = geompp.Polygon2D.make(comb)
        tris = geompp.triangulate(comb, _cdt_params())
        assert len(tris) == 10  # n - 2, not unconstrained Delaunay's 14 hull triangles
        assert approx(_total_area(tris), 48.0)
        assert approx(_total_area(tris), poly.area())
        for t in tris:
            assert t.area() > 0.0
            assert poly.contains(t.centroid()), t.to_wkt()

    def test_cdt_star_stays_inside_polygon(self):
        star = _p2(_STAR)
        poly = geompp.Polygon2D.make(star)
        tris = geompp.triangulate(star, _cdt_params())
        assert len(tris) == 8  # n - 2, not unconstrained Delaunay's 13
        assert abs(_total_area(tris) - poly.area()) < 1e-6
        for t in tris:
            assert t.area() > 0.0
            assert poly.contains(t.centroid()), t.to_wkt()

    def test_cdt_tilted_comb_3d(self):
        comb = _p3([(0, 0, 0), (5, 0, 0), (5, 0, 10), (4, 0, 10), (4, 0, 9), (3, 0, 9), (3, 0, 10),
                    (2, 0, 10), (2, 0, 9), (1, 0, 9), (1, 0, 10), (0, 0, 10)])
        tris = geompp.triangulate(comb, _cdt_params())
        assert len(tris) == 10
        assert abs(_total_area(tris) - 48.0) < 1e-6

    def test_delaunay_square_two_triangles(self):
        tris = geompp.delaunay(_p2([(0, 0), (1, 0), (1, 1), (0, 1)]))
        assert len(tris) == 2
        assert all(isinstance(t, geompp.Triangle2D) for t in tris)
        assert abs(_total_area(tris) - 1.0) < 1e-6

    def test_delaunay_star_points_cover_convex_hull(self):
        star = _p2(_STAR)
        tris = geompp.delaunay(star)
        assert len(tris) == 13  # 2n - h - 2 = 20 - 5 - 2
        hull = geompp.Polygon2D.make(geompp.convex_hull(star))
        assert abs(_total_area(tris) - hull.area()) < 1e-6
        assert _is_globally_delaunay(tris, star)
        for t in tris:
            assert t.is_ccw()

    def test_delaunay_square_with_center_four_triangles(self):
        tris = geompp.delaunay(_p2([(0, 0), (2, 0), (2, 2), (0, 2), (1, 1)]))
        assert len(tris) == 4
        assert abs(_total_area(tris) - 4.0) < 1e-6

    def test_delaunay_duplicates_ignored(self):
        pts = _p2([(0, 0), (1, 0), (1, 1), (0, 1), (1, 1), (0, 0), (0.0001, 0.0)])
        tris = geompp.delaunay(pts)
        assert len(tris) == 2
        assert abs(_total_area(tris) - 1.0) < 1e-6

    def test_delaunay_all_collinear_returns_empty(self):
        assert geompp.delaunay(_p2([(0, 0), (1, 1), (2, 2), (3, 3)])) == []

    def test_delaunay_fewer_than_three_points_raises(self):
        with pytest.raises(ValueError):
            geompp.delaunay(_p2([(0, 0), (1, 0)]))
        with pytest.raises(ValueError):
            geompp.delaunay(_p3([(0, 0, 0), (1, 0, 0)]))
        with pytest.raises(ValueError):
            geompp.delaunay(_p3([(0, 0, 0), (1, 0, 0)]), geompp.Vector3D(0, 0, 1))

    def test_delaunay_grid_5x5(self):
        pts = _p2([(i, j) for i in range(5) for j in range(5)])
        tris = geompp.delaunay(pts)
        assert len(tris) == 32  # 2n - h - 2 = 50 - 16 - 2
        assert abs(_total_area(tris) - 16.0) < 1e-6
        for t in tris:
            assert t.area() > 0.0

    def test_delaunay_no_input_point_inside_any_circumcircle(self):
        pts = _p2([(0, 0), (4, 0.3), (6.2, 2.1), (5.1, 5.4), (1.7, 6.2), (-1.2, 3.1),
                   (2.3, 2.2), (3.6, 3.9), (1.1, 1.4)])
        tris = geompp.delaunay(pts)
        assert len(tris) > 0
        for t in tris:
            a, b, c = _ccw(t)
            for p in pts:
                assert geompp.in_circumcircle(a, b, c, p) is False

    def test_delaunay_3d_terrain_with_normal(self):
        pts = _p3([(0, 0, 1.0), (4, 0, 2.0), (4, 4, 0.5), (0, 4, 3.0),
                   (1, 1, 1.7), (3, 1.2, 0.2), (2, 3, 2.4), (1.1, 2.6, 0.9)])
        tris = geompp.delaunay(pts, geompp.Vector3D(0, 0, 1))
        assert len(tris) == 10  # 2n - h - 2 = 16 - 4 - 2
        for t in tris:
            assert isinstance(t, geompp.Triangle3D)
            for v in t.vertices:
                assert any(p.almost_equals(v) for p in pts), "vertex not lifted back to an input point"

    def test_delaunay_3d_pca_overload_tilted_square_with_center(self):
        pts = _p3([(0, 0, 0), (0, 2, 0), (2, 0, -2), (2, 2, -2), (1, 1, -1)])
        tris = geompp.delaunay(pts)
        assert len(tris) == 4
        assert all(isinstance(t, geompp.Triangle3D) for t in tris)
        assert abs(_total_area(tris) - 4.0 * math.sqrt(2.0)) < 1e-6
