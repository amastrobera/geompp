"""
mesh binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestMesh2D:
    def test_from_triangles_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.Mesh2D.from_triangles([])

    def test_from_triangles_non_manifold_edge_raises(self):
        # Three triangles all sharing the exact same edge (0,0)-(1,0) -- a full edge with 3 neighbors.
        a = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, 1))
        b = geompp.Triangle2D.make(geompp.Point2D(1, 0), geompp.Point2D(0, 0), geompp.Point2D(0.5, -1))
        c = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0.5, -2))
        with pytest.raises(ValueError):
            geompp.Mesh2D.from_triangles([a, b, c])

    def test_from_triangles_single(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        mesh = geompp.Mesh2D.from_triangles([t])
        assert mesh.size() == 1
        assert approx(mesh.area(), 0.5)

    def test_getitem_out_of_range_raises(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        mesh = geompp.Mesh2D.from_triangles([t])
        with pytest.raises(IndexError):
            mesh[1]

    def test_shared_edge_welds_and_preserves_faces(self):
        # Regression test for the vertex-index off-by-one bug: two triangles sharing an
        # edge must weld to 4 unique vertices and each face must round-trip correctly.
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.Mesh2D.from_triangles([t0, t1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 1.0)
        assert t0.almost_equals(mesh[0])
        assert t1.almost_equals(mesh[1])

    def test_len_and_iteration(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.Mesh2D.from_triangles([t0, t1])
        assert len(mesh) == 2
        faces = list(mesh)
        assert len(faces) == 2
        assert faces[0].almost_equals(mesh[0])
        assert faces[1].almost_equals(mesh[1])

    def test_connect_preserves_size_and_area(self):
        t0 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1))
        t1 = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1))
        mesh = geompp.Mesh2D.from_triangles([t0, t1])
        connected = mesh.connect()
        assert connected.size() == mesh.size()
        assert approx(connected.area(), mesh.area())

class TestMesh3D:
    def test_from_triangles_empty_raises(self):
        with pytest.raises(ValueError):
            geompp.Mesh3D.from_triangles([])

    def test_from_triangles_non_manifold_edge_raises(self):
        # Three triangles all sharing the exact same edge (0,0,0)-(1,0,0), fanned into three planes.
        a = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, 1, 0))
        b = geompp.Triangle3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(0.5, 0, 1))
        c = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0.5, -1, 0))
        with pytest.raises(ValueError):
            geompp.Mesh3D.from_triangles([a, b, c])

    def test_from_triangles_single(self):
        t = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.Mesh3D.from_triangles([t])
        assert mesh.size() == 1
        assert approx(mesh.area(), 0.5)

    def test_shared_edge_welds_and_preserves_faces(self):
        t0 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0))
        t1 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.Mesh3D.from_triangles([t0, t1])

        assert mesh.size() == 2
        assert approx(mesh.area(), 1.0)
        assert t0.almost_equals(mesh[0])
        assert t1.almost_equals(mesh[1])

    def test_connect_preserves_size_and_area(self):
        t0 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0))
        t1 = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.Mesh3D.from_triangles([t0, t1])
        connected = mesh.connect()
        assert connected.size() == mesh.size()
        assert approx(connected.area(), mesh.area())
