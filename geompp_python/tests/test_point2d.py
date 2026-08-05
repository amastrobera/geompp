"""
point2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPoint2D:
    def test_construction(self):
        p = geompp.Point2D(1.0, 2.0)
        assert approx(p.x, 1.0)
        assert approx(p.y, 2.0)

    def test_construction_from_vector(self):
        v = geompp.Vector2D(3.0, -4.5)
        p = geompp.Point2D(v)
        assert approx(p.x, 3.0)
        assert approx(p.y, -4.5)
        # round-trip through ToVector
        assert geompp.Point2D(p.to_vector()) == p

    def test_zero(self):
        z = geompp.Point2D.zero()
        assert approx(z.x, 0.0) and approx(z.y, 0.0)

    def test_add_vector(self):
        p = geompp.Point2D(1, 2) + geompp.Vector2D(3, 4)
        assert approx(p.x, 4) and approx(p.y, 6)

    def test_iadd_vector(self):
        p = geompp.Point2D(1.0, 2.0)
        p += geompp.Vector2D(0.5, -1.0)
        assert isinstance(p, geompp.Point2D)
        assert approx(p.x, 1.5) and approx(p.y, 1.0)

    def test_iadd_zero_vector_unchanged(self):
        p = geompp.Point2D(3.0, 4.0)
        p += geompp.Vector2D(0, 0)
        assert approx(p.x, 3.0) and approx(p.y, 4.0)

    def test_sub_point_gives_vector(self):
        v = geompp.Point2D(4, 6) - geompp.Point2D(1, 2)
        assert isinstance(v, geompp.Vector2D)
        assert approx(v.x, 3) and approx(v.y, 4)

    def test_sub_vector_gives_point(self):
        p = geompp.Point2D(4, 6) - geompp.Vector2D(1, 2)
        assert isinstance(p, geompp.Point2D)
        assert approx(p.x, 3) and approx(p.y, 4)

    def test_scalar_mul(self):
        p = geompp.Point2D(1, 2) * 3
        assert approx(p.x, 3) and approx(p.y, 6)

    def test_scalar_div(self):
        p = geompp.Point2D(4, 6) / 2
        assert approx(p.x, 2) and approx(p.y, 3)

    def test_distance_to(self):
        d = geompp.Point2D(0, 0).distance_to(geompp.Point2D(3, 4))
        assert approx(d, 5.0)

    def test_almost_equals(self):
        p = geompp.Point2D(1.0, 2.0)
        assert p.almost_equals(geompp.Point2D(1.0, 2.0))
        assert not p.almost_equals(geompp.Point2D(1.0, 2.1))

    def test_almost_equals_with_epsilon(self):
        p = geompp.Point2D(1.0, 2.0)
        assert p.almost_equals(geompp.Point2D(1.0, 2.05), 0.1)

    def test_eq(self):
        assert geompp.Point2D(1, 2) == geompp.Point2D(1, 2)
        assert not (geompp.Point2D(1, 2) == geompp.Point2D(1, 3))

    def test_wkt_roundtrip(self):
        p = geompp.Point2D(3.5, 7.0)
        wkt = p.to_wkt()
        assert "POINT" in wkt
        p2 = geompp.Point2D.from_wkt(wkt)
        assert p.almost_equals(p2)

    def test_from_wkt_whitespace(self):
        assert geompp.Point2D(0, 1).almost_equals(geompp.Point2D.from_wkt("POINT (  0  1  )"))

    def test_repr(self):
        assert "POINT" in repr(geompp.Point2D(1, 2))

    def test_to_vector(self):
        v = geompp.Point2D(3.0, 4.0).to_vector()
        assert isinstance(v, geompp.Vector2D)
        assert approx(v.x, 3.0) and approx(v.y, 4.0)

    def test_to_file_from_file(self):
        p = geompp.Point2D(1.5, 2.5)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            p.to_file(path)
            assert os.path.exists(path)
            p2 = geompp.Point2D.from_file(path)
            assert p.almost_equals(p2)
        finally:
            os.unlink(path)
