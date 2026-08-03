"""
vector2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestVector2D:
    def test_length(self):
        assert approx(geompp.Vector2D(3, 4).length(), 5.0)

    def test_dot(self):
        assert approx(geompp.Vector2D(1, 0).dot(geompp.Vector2D(0, 1)), 0.0)
        assert approx(geompp.Vector2D(1, 0).dot(geompp.Vector2D(1, 0)), 1.0)

    def test_cross(self):
        # Cross product in 2D is the scalar z-component
        c = geompp.Vector2D(1, 0).cross(geompp.Vector2D(0, 1))
        assert approx(c, 1.0)

    def test_normalize(self):
        n = geompp.Vector2D(3, 4).normalize()
        assert approx(n.length(), 1.0)

    def test_basis(self):
        bx = geompp.Vector2D.basis_x()
        assert approx(bx.x, 1) and approx(bx.y, 0)
        by = geompp.Vector2D.basis_y()
        assert approx(by.x, 0) and approx(by.y, 1)

    def test_arithmetic(self):
        a = geompp.Vector2D(1, 2)
        b = geompp.Vector2D(3, 4)
        s = a + b
        assert approx(s.x, 4) and approx(s.y, 6)
        d = b - a
        assert approx(d.x, 2) and approx(d.y, 2)
        scaled = a * 2
        assert approx(scaled.x, 2) and approx(scaled.y, 4)
        divided = b / 2
        assert approx(divided.x, 1.5) and approx(divided.y, 2)

    def test_negation(self):
        v = -geompp.Vector2D(1, 2)
        assert approx(v.x, -1) and approx(v.y, -2)

    def test_perp(self):
        p = geompp.Vector2D(1, 0).perp()
        assert approx(p.x, 0) and approx(p.y, 1)

    def test_to_point(self):
        pt = geompp.Vector2D(3.0, 4.0).to_point()
        assert isinstance(pt, geompp.Point2D)
        assert approx(pt.x, 3.0) and approx(pt.y, 4.0)

    def test_almost_equals(self):
        v = geompp.Vector2D(1.0, 2.0)
        assert v.almost_equals(geompp.Vector2D(1.0, 2.0))
        assert not v.almost_equals(geompp.Vector2D(1.0, 2.1))

    def test_wkt_roundtrip(self):
        v = geompp.Vector2D(1.0, 2.0)
        wkt = v.to_wkt()
        v2 = geompp.Vector2D.from_wkt(wkt)
        assert v.almost_equals(v2)

    def test_from_wkt_whitespace(self):
        assert geompp.Vector2D(0, 1).almost_equals(geompp.Vector2D.from_wkt("VECTOR (  0  1  )"))

    def test_to_file_from_file(self):
        v = geompp.Vector2D(1.5, 2.5)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            v.to_file(path)
            assert os.path.exists(path)
            v2 = geompp.Vector2D.from_file(path)
            assert v.almost_equals(v2)
        finally:
            os.unlink(path)

    def test_is_parallel(self):
        assert geompp.Vector2D(1, 0).is_parallel(geompp.Vector2D(2, 0))
        assert geompp.Vector2D(1, 0).is_parallel(geompp.Vector2D(-3, 0))
        assert geompp.Vector2D(1, 1).is_parallel(geompp.Vector2D(2, 2))
        assert not geompp.Vector2D(1, 0).is_parallel(geompp.Vector2D(0, 1))
        assert not geompp.Vector2D(1, 0).is_parallel(geompp.Vector2D(1, 1))
