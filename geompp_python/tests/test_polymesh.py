"""
polymesh binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPolyMesh2D:
    def test_from_polygons_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.PolyMesh2D.from_polygons([])

    def test_from_polygons_with_holes_raises(self):
        outer = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4), geompp.Point2D(0, 4)],
            [[geompp.Point2D(1, 1), geompp.Point2D(1, 2), geompp.Point2D(2, 2), geompp.Point2D(2, 1)]])
        with pytest.raises(ValueError):
            geompp.PolyMesh2D.from_polygons([outer])

    def test_from_polygons_t_junction_raises(self):
        # Two unit squares side by side, plus a roof triangle spanning both squares' top -- its base
        # edge passes straight through the squares' shared vertex. Classic T-junction.
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        with pytest.raises(ValueError):
            geompp.PolyMesh2D.from_polygons([p0, p1, roof])

    def test_from_polygons_t_junction_enforce_splits_and_welds(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        area_before = p0.area() + p1.area() + roof.area()

        mesh = geompp.PolyMesh2D.from_polygons([p0, p1, roof], geompp.AdjacencyConformity.Enforce)

        assert mesh.size() == 4  # p0, p1 pass through unchanged, roof splits into 2
        assert approx(mesh.area(), area_before)

    def test_from_polygons_t_junction_guaranteed_skips_check_and_succeeds(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        roof = geompp.Polygon2D.make([geompp.Point2D(0, 1), geompp.Point2D(2, 1), geompp.Point2D(1, 2)])
        geompp.PolyMesh2D.from_polygons([p0, p1, roof], geompp.AdjacencyConformity.Guaranteed)  # must not raise

    def test_from_polygons_with_holes_enforce_raises(self):
        # fix_adjacency() only round-trips through each facet's outer perimeter() -- a hole would
        # otherwise be silently dropped rather than raising, so Enforce rejects it outright instead.
        outer = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4), geompp.Point2D(0, 4)],
            [[geompp.Point2D(1, 1), geompp.Point2D(1, 2), geompp.Point2D(2, 2), geompp.Point2D(2, 1)]])
        with pytest.raises(ValueError):
            geompp.PolyMesh2D.from_polygons([outer], geompp.AdjacencyConformity.Enforce)

    def test_from_polygons_single_quad(self):
        p = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        mesh = geompp.PolyMesh2D.from_polygons([p])
        assert mesh.size() == 1
        assert approx(mesh.area(), 1.0)

    def test_shared_edge_welds_and_preserves_faces(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        mesh = geompp.PolyMesh2D.from_polygons([p0, p1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 2.0)
        assert p0.almost_equals(mesh[0])
        assert p1.almost_equals(mesh[1])

    def test_len_and_iteration(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        mesh = geompp.PolyMesh2D.from_polygons([p0])
        assert len(mesh) == 1
        faces = list(mesh)
        assert faces[0].almost_equals(mesh[0])

    def test_triangulate_shared_edge_preserves_total_area(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        mesh = geompp.PolyMesh2D.from_polygons([p0, p1])
        tri_mesh = mesh.triangulate()
        assert tri_mesh.size() == 4
        assert approx(tri_mesh.area(), mesh.area())

    # Regression test: PolyMesh2D.triangulate() used to triangulate the whole (shared, deduplicated)
    # vertex buffer as if it were a single ring, which only "worked" by coincidence for facets that
    # happened to share welded edges. Two disjoint facets — no welding to paper over the bug — exposes
    # it directly: the old code produced a wrong triangle count and a wrong total area.
    def test_triangulate_disjoint_facets_preserves_total_area(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(5, 5), geompp.Point2D(6, 5), geompp.Point2D(6, 6), geompp.Point2D(5, 6)])
        mesh = geompp.PolyMesh2D.from_polygons([p0, p1])
        tri_mesh = mesh.triangulate()
        assert tri_mesh.size() == 4
        assert approx(tri_mesh.area(), mesh.area())

class TestPolyMesh3D:
    def test_from_polygons_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.PolyMesh3D.from_polygons([])

    def test_from_polygons_with_holes_raises(self):
        outer = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)],
            [[geompp.Point3D(1, 1, 0), geompp.Point3D(1, 2, 0), geompp.Point3D(2, 2, 0), geompp.Point3D(2, 1, 0)]])
        with pytest.raises(ValueError):
            geompp.PolyMesh3D.from_polygons([outer])

    def test_from_polygons_t_junction_raises(self):
        # Two unit squares side by side (y=0 plane), plus a roof triangle spanning both squares' top.
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1), geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])
        with pytest.raises(ValueError):
            geompp.PolyMesh3D.from_polygons([p0, p1, roof])

    def test_from_polygons_t_junction_enforce_splits_and_welds(self):
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1), geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])
        area_before = p0.area() + p1.area() + roof.area()

        mesh = geompp.PolyMesh3D.from_polygons([p0, p1, roof], geompp.AdjacencyConformity.Enforce)

        assert mesh.size() == 4
        assert approx(mesh.area(), area_before)

    def test_from_polygons_t_junction_guaranteed_skips_check_and_succeeds(self):
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 1), geompp.Point3D(2, 0, 1), geompp.Point3D(2, 0, 0), geompp.Point3D(1, 0, 0)])
        roof = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 2), geompp.Point3D(2, 0, 1), geompp.Point3D(0, 0, 1)])
        geompp.PolyMesh3D.from_polygons([p0, p1, roof], geompp.AdjacencyConformity.Guaranteed)  # must not raise

    def test_from_polygons_with_holes_enforce_raises(self):
        outer = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)],
            [[geompp.Point3D(1, 1, 0), geompp.Point3D(1, 2, 0), geompp.Point3D(2, 2, 0), geompp.Point3D(2, 1, 0)]])
        with pytest.raises(ValueError):
            geompp.PolyMesh3D.from_polygons([outer], geompp.AdjacencyConformity.Enforce)

    def test_from_polygons_single_quad(self):
        p = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        mesh = geompp.PolyMesh3D.from_polygons([p])
        assert mesh.size() == 1
        assert approx(mesh.area(), 1.0)

    def test_shared_edge_welds_and_preserves_faces(self):
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(2, 1, 0), geompp.Point3D(1, 1, 0)])
        mesh = geompp.PolyMesh3D.from_polygons([p0, p1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 2.0)
        assert p0.almost_equals(mesh[0])
        assert p1.almost_equals(mesh[1])

    def test_triangulate_shared_edge_preserves_total_area(self):
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(1, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(2, 1, 0), geompp.Point3D(1, 1, 0)])
        mesh = geompp.PolyMesh3D.from_polygons([p0, p1])
        tri_mesh = mesh.triangulate()
        assert tri_mesh.size() == 4
        assert approx(tri_mesh.area(), mesh.area())

    def test_triangulate_disjoint_facets_preserves_total_area(self):
        p0 = geompp.Polygon3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        p1 = geompp.Polygon3D.make(
            [geompp.Point3D(5, 5, 0), geompp.Point3D(6, 5, 0), geompp.Point3D(6, 6, 0), geompp.Point3D(5, 6, 0)])
        mesh = geompp.PolyMesh3D.from_polygons([p0, p1])
        tri_mesh = mesh.triangulate()
        assert tri_mesh.size() == 4
        assert approx(tri_mesh.area(), mesh.area())
