"""
point3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPoint3D:
    def test_construction_from_vector(self):
        v = geompp.Vector3D(3.0, -4.5, 6.25)
        p = geompp.Point3D(v)
        assert approx(p.x, 3.0)
        assert approx(p.y, -4.5)
        assert approx(p.z, 6.25)
        # round-trip through ToVector
        assert geompp.Point3D(p.to_vector()) == p

    def test_construction(self):
        p = geompp.Point3D(1, 2, 3)
        assert approx(p.x, 1) and approx(p.y, 2) and approx(p.z, 3)

    def test_add_vector(self):
        p = geompp.Point3D(1, 2, 3) + geompp.Vector3D(1, 1, 1)
        assert approx(p.x, 2) and approx(p.y, 3) and approx(p.z, 4)

    def test_iadd_vector(self):
        p = geompp.Point3D(1.0, 2.0, 3.0)
        p += geompp.Vector3D(0.5, -1.0, 2.0)
        assert isinstance(p, geompp.Point3D)
        assert approx(p.x, 1.5) and approx(p.y, 1.0) and approx(p.z, 5.0)

    def test_iadd_zero_vector_unchanged(self):
        p = geompp.Point3D(1.0, 2.0, 3.0)
        p += geompp.Vector3D(0, 0, 0)
        assert approx(p.x, 1.0) and approx(p.y, 2.0) and approx(p.z, 3.0)

    def test_distance_to(self):
        d = geompp.Point3D(0, 0, 0).distance_to(geompp.Point3D(1, 0, 0))
        assert approx(d, 1.0)

    def test_scalar_div(self):
        p = geompp.Point3D(4, 6, 8) / 2
        assert approx(p.x, 2) and approx(p.y, 3) and approx(p.z, 4)

    def test_wkt_roundtrip(self):
        p = geompp.Point3D(1, 2, 3)
        p2 = geompp.Point3D.from_wkt(p.to_wkt())
        assert p.almost_equals(p2)

    def test_from_wkt_whitespace(self):
        assert geompp.Point3D(0, 1, 2).almost_equals(geompp.Point3D.from_wkt("POINT (  0  1  2  )"))

    def test_zero(self):
        z = geompp.Point3D.zero()
        assert approx(z.x, 0.0) and approx(z.y, 0.0) and approx(z.z, 0.0)

    def test_to_vector(self):
        v = geompp.Point3D(1.0, 2.0, 3.0).to_vector()
        assert isinstance(v, geompp.Vector3D)
        assert approx(v.x, 1.0) and approx(v.y, 2.0) and approx(v.z, 3.0)

    def test_almost_equals(self):
        p = geompp.Point3D(1.0, 2.0, 3.0)
        assert p.almost_equals(geompp.Point3D(1.0, 2.0, 3.0))
        assert not p.almost_equals(geompp.Point3D(1.0, 2.0, 3.1))

    def test_sub_point_gives_vector(self):
        v = geompp.Point3D(4, 5, 6) - geompp.Point3D(1, 2, 3)
        assert isinstance(v, geompp.Vector3D)
        assert approx(v.x, 3) and approx(v.y, 3) and approx(v.z, 3)

    def test_sub_vector_gives_point(self):
        p = geompp.Point3D(4, 5, 6) - geompp.Vector3D(1, 2, 3)
        assert isinstance(p, geompp.Point3D)
        assert approx(p.x, 3) and approx(p.y, 3) and approx(p.z, 3)

    def test_scalar_mul(self):
        p = geompp.Point3D(1, 2, 3) * 2
        assert approx(p.x, 2) and approx(p.y, 4) and approx(p.z, 6)

    def test_to_file_from_file(self):
        p = geompp.Point3D(1.0, 2.0, 3.0)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            p.to_file(path)
            assert os.path.exists(path)
            p2 = geompp.Point3D.from_file(path)
            assert p.almost_equals(p2)
        finally:
            os.unlink(path)
