"""
connected mesh binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestConnectedMesh2D:
    def test_from_triangles_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.ConnectedMesh2D.from_triangles([])

    def test_from_triangles_non_manifold_edge_raises(self):
        a = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1))
        b = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1))
        c = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, -2))
        with pytest.raises(ValueError):
            geompp.ConnectedMesh2D.from_triangles([a, b, c])

    def test_from_triangles_single(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t])
        assert mesh.size() == 1
        assert approx(mesh.area(), 0.5)

    def test_getitem_out_of_range_raises(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t])
        with pytest.raises(IndexError):
            mesh[1]

    def test_shared_edge_welds_and_preserves_faces(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t0, t1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 1.0)
        assert mesh[0].id() == 0
        assert t0.almost_equals(mesh[0].geometry())
        assert t1.almost_equals(mesh[1].geometry())

    def test_len_and_iteration(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t0, t1])
        assert len(mesh) == 2
        faces = list(mesh)
        assert len(faces) == 2
        assert faces[0].geometry().almost_equals(mesh[0].geometry())
        assert faces[1].geometry().almost_equals(mesh[1].geometry())

    def test_fan_all_faces_preserved(self):
        # Regression coverage for a fix in GridCellMapForConnectedMesh2D::Make() where
        # per-triangle adjacency indexing went out of bounds for any triangle beyond the
        # first — a fan of 4 triangles sharing an edge each exercises that path end-to-end.
        triangles = [
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1), geompp.Point2D(-1, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(-1, 1), geompp.Point2D(-1, 0)),
        ]
        mesh = geompp.ConnectedMesh2D.from_triangles(triangles)
        assert mesh.size() == 4
        for i, t in enumerate(triangles):
            assert t.almost_equals(mesh[i].geometry())

    # Fan of 4 triangles sharing the origin, laid out left-to-right (triangle i's THIRD edge is
    # welded to triangle i+1's FIRST edge). Starting at face 0 and always crossing THIRD, the walk
    # visits 0 -> 1 -> 2 -> 3, always entering the next face through its FIRST edge, and face 3's
    # THIRD edge is a boundary (end of the fan) — mirrors
    # ConnectedMesh2DTest.FaceView_Neighbor_WalksKnownAdjacencyPatternToExpectedTarget in C++.
    def test_faceview_neighbor_walks_known_adjacency_pattern(self):
        triangles = [
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1), geompp.Point2D(-1, 1)),
            geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(-1, 1), geompp.Point2D(-1, 0)),
        ]
        mesh = geompp.ConnectedMesh2D.from_triangles(triangles)

        face = mesh[0]
        assert face.id() == 0

        for expected_target in range(1, len(triangles)):
            assert face.neighbor_entry_edge(geompp.TriangleEdge.THIRD) == geompp.TriangleEdge.FIRST
            nxt = face.neighbor(geompp.TriangleEdge.THIRD)
            assert nxt is not None
            face = nxt
            assert face.id() == expected_target
            assert triangles[expected_target].almost_equals(face.geometry())

        # face 3 is the end of the fan: its THIRD edge has no twin.
        assert face.neighbor(geompp.TriangleEdge.THIRD) is None
        assert face.neighbor_entry_edge(geompp.TriangleEdge.THIRD) == geompp.TriangleEdge.INVALID

    def test_faceview_neighbor_single_triangle_all_edges_are_boundary(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t])
        face = mesh[0]
        for edge in (geompp.TriangleEdge.FIRST, geompp.TriangleEdge.SECOND, geompp.TriangleEdge.THIRD):
            assert face.neighbor(edge) is None
            assert face.neighbor_entry_edge(edge) == geompp.TriangleEdge.INVALID

    def test_faceview_neighbor_crossing_back_via_entry_edge_returns_to_origin(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t0, t1])

        face0 = mesh[0]
        entry_edge = face0.neighbor_entry_edge(geompp.TriangleEdge.THIRD)
        face1 = face0.neighbor(geompp.TriangleEdge.THIRD)
        assert face1 is not None
        assert face1.id() == 1

        back = face1.neighbor(entry_edge)
        assert back is not None
        assert back.id() == face0.id()
        assert face1.neighbor_entry_edge(entry_edge) == geompp.TriangleEdge.THIRD

    def test_polygonize_unit_square_from_two_triangles_returns_single_quad(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.ConnectedMesh2D.from_triangles([t0, t1])

        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.PlanarBoundaryExtraction)
        poly_mesh = mesh.polygonize(settings)

        assert poly_mesh.size() == 1
        assert approx(poly_mesh.area(), 1.0)
        assert poly_mesh[0].size() == 4

    def test_polygonize_2x2_grid_hertel_mehlhorn_merges_into_single_convex_piece(self):
        triangles = []
        for r in range(2):
            for c in range(2):
                p00 = geompp.Point2D(c, r)
                p10 = geompp.Point2D(c + 1, r)
                p11 = geompp.Point2D(c + 1, r + 1)
                p01 = geompp.Point2D(c, r + 1)
                triangles.append(geompp.Triangle2D.make(p00, p10, p11))
                triangles.append(geompp.Triangle2D.make(p00, p11, p01))
        mesh = geompp.ConnectedMesh2D.from_triangles(triangles)

        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.HertelMehlhorn)
        poly_mesh = mesh.polygonize(settings)

        assert poly_mesh.size() == 1
        assert approx(poly_mesh.area(), 4.0)
        assert poly_mesh[0].is_convex()

class TestConnectedMesh3D:
    def test_from_triangles_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.ConnectedMesh3D.from_triangles([])

    def test_from_triangles_non_manifold_edge_raises(self):
        a = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, 1, 0))
        b = geompp.Triangle3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, 0, 1))
        c = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, -1, 0))
        with pytest.raises(ValueError):
            geompp.ConnectedMesh3D.from_triangles([a, b, c])

    def test_from_triangles_single(self):
        t = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t])
        assert mesh.size() == 1
        assert approx(mesh.area(), 0.5)

    def test_getitem_out_of_range_raises(self):
        t = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t])
        with pytest.raises(IndexError):
            mesh[1]

    def test_shared_edge_welds_and_preserves_faces(self):
        t0 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0))
        t1 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t0, t1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 1.0)
        assert mesh[0].id() == 0
        assert t0.almost_equals(mesh[0].geometry())
        assert t1.almost_equals(mesh[1].geometry())

    def test_len_and_iteration(self):
        t0 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0))
        t1 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t0, t1])
        assert len(mesh) == 2
        faces = list(mesh)
        assert len(faces) == 2
        assert faces[0].geometry().almost_equals(mesh[0].geometry())
        assert faces[1].geometry().almost_equals(mesh[1].geometry())

    def test_fan_all_faces_preserved(self):
        # Regression coverage for a fix in GridCellMapForConnectedMesh3D::Make() where
        # per-triangle adjacency indexing went out of bounds for any triangle beyond the
        # first — a fan of 4 triangles sharing an edge each exercises that path end-to-end.
        triangles = [
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0), geompp.Point3D(-1, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(-1, 1, 0), geompp.Point3D(-1, 0, 0)),
        ]
        mesh = geompp.ConnectedMesh3D.from_triangles(triangles)
        assert mesh.size() == 4
        for i, t in enumerate(triangles):
            assert t.almost_equals(mesh[i].geometry())

    # Fan of 4 triangles sharing the origin, laid out left-to-right (triangle i's THIRD edge is
    # welded to triangle i+1's FIRST edge). Starting at face 0 and always crossing THIRD, the walk
    # visits 0 -> 1 -> 2 -> 3, always entering the next face through its FIRST edge, and face 3's
    # THIRD edge is a boundary (end of the fan) — mirrors
    # ConnectedMesh3DTest.FaceView_Neighbor_WalksKnownAdjacencyPatternToExpectedTarget in C++.
    def test_faceview_neighbor_walks_known_adjacency_pattern(self):
        triangles = [
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0), geompp.Point3D(-1, 1, 0)),
            geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(-1, 1, 0), geompp.Point3D(-1, 0, 0)),
        ]
        mesh = geompp.ConnectedMesh3D.from_triangles(triangles)

        face = mesh[0]
        assert face.id() == 0

        for expected_target in range(1, len(triangles)):
            assert face.neighbor_entry_edge(geompp.TriangleEdge.THIRD) == geompp.TriangleEdge.FIRST
            nxt = face.neighbor(geompp.TriangleEdge.THIRD)
            assert nxt is not None
            face = nxt
            assert face.id() == expected_target
            assert triangles[expected_target].almost_equals(face.geometry())

        # face 3 is the end of the fan: its THIRD edge has no twin.
        assert face.neighbor(geompp.TriangleEdge.THIRD) is None
        assert face.neighbor_entry_edge(geompp.TriangleEdge.THIRD) == geompp.TriangleEdge.INVALID

    def test_faceview_neighbor_single_triangle_all_edges_are_boundary(self):
        t = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t])
        face = mesh[0]
        for edge in (geompp.TriangleEdge.FIRST, geompp.TriangleEdge.SECOND, geompp.TriangleEdge.THIRD):
            assert face.neighbor(edge) is None
            assert face.neighbor_entry_edge(edge) == geompp.TriangleEdge.INVALID

    def test_faceview_neighbor_crossing_back_via_entry_edge_returns_to_origin(self):
        t0 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0))
        t1 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.ConnectedMesh3D.from_triangles([t0, t1])

        face0 = mesh[0]
        entry_edge = face0.neighbor_entry_edge(geompp.TriangleEdge.THIRD)
        face1 = face0.neighbor(geompp.TriangleEdge.THIRD)
        assert face1 is not None
        assert face1.id() == 1

        back = face1.neighbor(entry_edge)
        assert back is not None
        assert back.id() == face0.id()
        assert face1.neighbor_entry_edge(entry_edge) == geompp.TriangleEdge.THIRD

    def test_polygonize_tilted_square_from_two_triangles_returns_single_quad(self):
        p00 = geompp.Point3D(0, 0, 0)
        p10 = geompp.Point3D(1, 0, 1)
        p11 = geompp.Point3D(1, 1, 1)
        p01 = geompp.Point3D(0, 1, 0)
        t0 = geompp.Triangle3D.make(p00, p10, p11)
        t1 = geompp.Triangle3D.make(p00, p11, p01)
        mesh = geompp.ConnectedMesh3D.from_triangles([t0, t1])

        settings = geompp.PolygonizationParams(geompp.PolygonizationStrategy.PlanarBoundaryExtraction)
        poly_mesh = mesh.polygonize(settings)

        assert poly_mesh.size() == 1
        assert poly_mesh[0].size() == 4
        assert approx(poly_mesh.area(), 2.0 ** 0.5)
