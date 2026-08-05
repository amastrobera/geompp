"""
vector3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestVector3D:
    def test_cross(self):
        c = geompp.Vector3D(1, 0, 0).cross(geompp.Vector3D(0, 1, 0))
        assert approx(c.x, 0) and approx(c.y, 0) and approx(c.z, 1)

    def test_is_parallel(self):
        assert geompp.Vector3D(1, 0, 0).is_parallel(geompp.Vector3D(2, 0, 0))
        assert not geompp.Vector3D(1, 0, 0).is_parallel(geompp.Vector3D(0, 1, 0))

    def test_basis(self):
        assert approx(geompp.Vector3D.basis_z().z, 1)

    def test_dominant_axis(self):
        assert geompp.Vector3D(3, 1, 1).dominant_axis() == geompp.Axis.X
        assert geompp.Vector3D(-3, 1, 1).dominant_axis() == geompp.Axis.X
        assert geompp.Vector3D(1, 3, 1).dominant_axis() == geompp.Axis.Y
        assert geompp.Vector3D(1, 1, 3).dominant_axis() == geompp.Axis.Z
        assert geompp.Vector3D.basis_z().dominant_axis() == geompp.Axis.Z

    def test_to_point(self):
        pt = geompp.Vector3D(1.0, 2.0, 3.0).to_point()
        assert isinstance(pt, geompp.Point3D)
        assert approx(pt.x, 1.0) and approx(pt.y, 2.0) and approx(pt.z, 3.0)

    def test_length(self):
        assert approx(geompp.Vector3D(0, 3, 4).length(), 5.0)

    def test_dot(self):
        assert approx(geompp.Vector3D(1, 0, 0).dot(geompp.Vector3D(0, 1, 0)), 0.0)
        assert approx(geompp.Vector3D(1, 0, 0).dot(geompp.Vector3D(1, 0, 0)), 1.0)

    def test_perp(self):
        # perp of (1,0,0) should be a vector perpendicular to it
        p = geompp.Vector3D(1, 0, 0).perp()
        assert isinstance(p, geompp.Vector3D)
        assert approx(geompp.Vector3D(1, 0, 0).dot(p), 0.0)

    def test_normalize(self):
        n = geompp.Vector3D(3, 4, 0).normalize()
        assert approx(n.length(), 1.0)

    def test_arithmetic(self):
        a = geompp.Vector3D(1, 2, 3)
        b = geompp.Vector3D(4, 5, 6)
        s = a + b
        assert approx(s.x, 5) and approx(s.y, 7) and approx(s.z, 9)
        d = b - a
        assert approx(d.x, 3) and approx(d.y, 3) and approx(d.z, 3)
        scaled = a * 2
        assert approx(scaled.x, 2) and approx(scaled.y, 4) and approx(scaled.z, 6)
        rscaled = 2 * a
        assert approx(rscaled.x, 2) and approx(rscaled.y, 4) and approx(rscaled.z, 6)
        divided = b / 2
        assert approx(divided.x, 2) and approx(divided.y, 2.5) and approx(divided.z, 3)

    def test_negation(self):
        v = -geompp.Vector3D(1, 2, 3)
        assert approx(v.x, -1) and approx(v.y, -2) and approx(v.z, -3)

    def test_eq(self):
        v = geompp.Vector3D(1.0, 2.0, 3.0)
        assert v == geompp.Vector3D(1.0, 2.0, 3.0)
        assert not (v == geompp.Vector3D(1.0, 2.0, 3.1))

    def test_almost_equals(self):
        v = geompp.Vector3D(1.0, 2.0, 3.0)
        assert v.almost_equals(geompp.Vector3D(1.0, 2.0, 3.0))
        assert not v.almost_equals(geompp.Vector3D(1.0, 2.0, 3.1))

    def test_wkt_roundtrip(self):
        v = geompp.Vector3D(1.0, 2.0, 3.0)
        wkt = v.to_wkt()
        v2 = geompp.Vector3D.from_wkt(wkt)
        assert v.almost_equals(v2)

    def test_from_wkt_whitespace(self):
        assert geompp.Vector3D(0, 1, 2).almost_equals(geompp.Vector3D.from_wkt("VECTOR (  0  1  2  )"))

    def test_to_file_from_file(self):
        v = geompp.Vector3D(1.0, 2.0, 3.0)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            v.to_file(path)
            assert os.path.exists(path)
            v2 = geompp.Vector3D.from_file(path)
            assert v.almost_equals(v2)
        finally:
            os.unlink(path)

    def test_basis_x_and_basis_y(self):
        bx = geompp.Vector3D.basis_x()
        assert approx(bx.x, 1) and approx(bx.y, 0) and approx(bx.z, 0)
        by = geompp.Vector3D.basis_y()
        assert approx(by.x, 0) and approx(by.y, 1) and approx(by.z, 0)
