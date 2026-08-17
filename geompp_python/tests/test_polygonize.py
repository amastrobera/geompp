"""
polygonize() / merge() binding tests.
"""

import pytest
import geompp

from ._helpers import approx


def _grid_triangles(rows, cols, skip=frozenset()):
    """Axis-aligned unit-square triangles for a rows x cols grid, split along one diagonal per square,
    skipping any (r, c) cell in `skip` -- same convention as the C++ test suite's BuildGridTriangles."""
    triangles = []
    for r in range(rows):
        for c in range(cols):
            if (r, c) in skip:
                continue
            p00 = geompp.Point2D(c, r)
            p10 = geompp.Point2D(c + 1, r)
            p11 = geompp.Point2D(c + 1, r + 1)
            p01 = geompp.Point2D(c, r + 1)
            triangles.append(geompp.Triangle2D.make(p00, p10, p11))
            triangles.append(geompp.Triangle2D.make(p00, p11, p01))
    return triangles


class TestPolygonize:
    def test_default_strategy_is_hertel_mehlhorn(self):
        assert geompp.PolygonizationParams().strategy == geompp.PolygonizationStrategy.HertelMehlhorn

    def test_unit_square_from_two_triangles_returns_single_quad(self):
        polys = geompp.polygonize(_grid_triangles(1, 1))
        assert len(polys) == 1
        assert polys[0].size() == 4
        assert approx(polys[0].area(), 1.0)

    def test_planar_boundary_extraction_grid_with_center_hole(self):
        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.PlanarBoundaryExtraction)
        polys = geompp.polygonize(_grid_triangles(3, 3, skip={(1, 1)}), settings)
        assert len(polys) == 1
        assert approx(polys[0].area(), 8.0)  # 9 (outer) - 1 (hole)

    def test_planar_quads_two_triangles_returns_single_quad(self):
        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.PlanarQuads)
        polys = geompp.polygonize(_grid_triangles(1, 1), settings)
        assert len(polys) == 1
        assert polys[0].size() == 4

    def test_hertel_mehlhorn_2x2_grid_merges_into_one_convex_piece(self):
        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.HertelMehlhorn)
        polys = geompp.polygonize(_grid_triangles(2, 2), settings)
        assert len(polys) == 1
        assert approx(polys[0].area(), 4.0)

    def test_l_shape_hertel_mehlhorn_preserves_shared_t_junction_vertex(self):
        # 2x2 grid, top-left cell skipped -- an L-shape with a reflex vertex at (1, 1). HertelMehlhorn
        # returns 2 convex pieces (a 2x1 rectangle and a 1x1 square) whose shared corner sits exactly at
        # the midpoint of the rectangle's top edge. Regression test: polygonize() must not silently drop
        # that vertex as collinear-on-its-own-ring, or Mesh2D.polygonize()/PolyMesh2D would reject the
        # result as a T-junction once welded together (see test_mesh.py's mirror of this same case).
        polys = geompp.polygonize(_grid_triangles(2, 2, skip={(1, 0)}))
        assert len(polys) == 2

        rectangle = next(p for p in polys if approx(p.area(), 2.0))
        assert rectangle.size() == 6  # 4 real corners + (1, 0) and (1, 1), deliberately not simplified
        assert any(v.almost_equals(geompp.Point2D(1, 1)) for v in rectangle.perimeter())

    def test_empty_input_raises(self):
        with pytest.raises(ValueError):
            geompp.polygonize([])

    def test_non_manifold_input_raises(self):
        a = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1))
        b = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1))
        c = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -2))
        with pytest.raises(ValueError):
            geompp.polygonize([a, b, c])

    def test_3d_tilted_square_from_two_triangles(self):
        # A unit square on the tilted plane z = x -- side p00->p10 has length sqrt(2), side p00->p01 has
        # length 1, and they're perpendicular: area sqrt(2), not 1 (see the C++ test suite's version of
        # this fixture for the full derivation).
        p00 = geompp.Point3D(0, 0, 0)
        p10 = geompp.Point3D(1, 0, 1)
        p11 = geompp.Point3D(1, 1, 1)
        p01 = geompp.Point3D(0, 1, 0)
        triangles = [geompp.Triangle3D.make(p00, p10, p11), geompp.Triangle3D.make(p00, p11, p01)]

        polys = geompp.polygonize(triangles)

        assert len(polys) == 1
        assert polys[0].size() == 4
        assert approx(polys[0].area(), 2.0 ** 0.5)


class TestMerge:
    def test_empty_input_returns_empty(self):
        assert geompp.merge([]) == []

    def test_two_touching_squares_returns_single_merged_outer(self):
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2), geompp.Point2D(0, 2)])
        b = geompp.Polygon2D.make([geompp.Point2D(2, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 2), geompp.Point2D(2, 2)])

        result = geompp.merge([a, b])

        assert len(result) == 1
        assert approx(result[0].area(), 8.0)

    def test_three_squares_preserves_shared_t_junction_vertex(self):
        # a and b share a full edge (x=1, y:0-1) and merge into a 2x1 rectangle; c only touches the
        # merged piece at the single point (1, 1) (no full shared edge with a or b). Regression test for
        # the same class of bug as test_l_shape_hertel_mehlhorn_preserves_shared_t_junction_vertex above,
        # but via merge()'s own packaging: the merged rectangle must still carry (1, 1) as an explicit
        # vertex, or c's own corner would land mid-edge on a T-junction once both pieces shared a mesh.
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        c = geompp.Polygon2D.make([geompp.Point2D(1, 1), geompp.Point2D(1.5, 1), geompp.Point2D(1.5, 1.5), geompp.Point2D(1, 1.5)])

        result = geompp.merge([a, b, c])

        rectangle = next((p for p in result if approx(p.area(), 2.0)), None)
        assert rectangle is not None
        assert any(v.almost_equals(geompp.Point2D(1, 1)) for v in rectangle.perimeter())
        assert approx(sum(p.area() for p in result), 2.25)  # 2.0 (rectangle) + 0.25 (c)

    def test_two_disjoint_squares_returns_both_unchanged(self):
        a = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        b = geompp.Polygon2D.make([geompp.Point2D(10, 0), geompp.Point2D(11, 0), geompp.Point2D(11, 1), geompp.Point2D(10, 1)])

        result = geompp.merge([a, b])

        assert len(result) == 2
        assert approx(sum(p.area() for p in result), 2.0)

    def test_touching_holes_merge_into_one_bigger_hole(self):
        # Two 2x2 squares merging along x=2, each with a hole reaching that same shared seam -- the two
        # holes touching along it get detected and unioned into one (mirrors the C++ test of the same
        # name; holes must be given in CW order, Polygon2D.make() rejects a CCW hole outright).
        a = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2), geompp.Point2D(0, 2)],
            [[geompp.Point2D(1, 0.5), geompp.Point2D(1, 1.5), geompp.Point2D(2, 1.5), geompp.Point2D(2, 0.5)]],
        )
        b = geompp.Polygon2D.make(
            [geompp.Point2D(2, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 2), geompp.Point2D(2, 2)],
            [[geompp.Point2D(2, 0.5), geompp.Point2D(2, 1.5), geompp.Point2D(3, 1.5), geompp.Point2D(3, 0.5)]],
        )

        result = geompp.merge([a, b])

        assert len(result) == 1
        assert result[0].has_holes()
        assert len(result[0].holes()) == 1
        assert approx(result[0].area(), 6.0)  # 8 (outer) - 2 (merged 1x2 hole)

    def test_3d_two_parallel_same_normal_different_offset_squares_stay_unmerged(self):
        # Two unit squares sharing normal (0,0,1) but at different z-offsets: Plane.almost_equals'
        # offset check must keep them in separate plane groups, even though they'd share a
        # normal-only hash bucket.
        a = geompp.Polygon3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
                                   geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        b = geompp.Polygon3D.make([geompp.Point3D(0, 0, 5), geompp.Point3D(1, 0, 5),
                                   geompp.Point3D(1, 1, 5), geompp.Point3D(0, 1, 5)])

        result = geompp.merge([a, b])

        assert len(result) == 2
