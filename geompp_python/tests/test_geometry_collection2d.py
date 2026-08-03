"""
geometry collection2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestGeometryCollection2D:
    def test_default_construction(self):
        gc = geompp.GeometryCollection2D()
        assert gc.size() == 0
        assert len(gc) == 0

    def test_add_point(self):
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        assert gc.size() == 1

    def test_add_multiple_types(self):
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(0, 0))
        gc.add(geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0)))
        gc.add(geompp.Triangle2D.make(
            geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1)
        ))
        assert gc.size() == 3
        assert len(gc) == 3

    def test_get_point(self):
        gc = geompp.GeometryCollection2D()
        p = geompp.Point2D(3, 4)
        gc.add(p)
        result = gc.get(0)
        assert isinstance(result, geompp.Point2D)
        assert result == p

    def test_getitem(self):
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        gc.add(geompp.Point2D(3, 4))
        assert isinstance(gc[0], geompp.Point2D)
        assert isinstance(gc[1], geompp.Point2D)
        assert isinstance(gc[-1], geompp.Point2D)

    def test_getitem_out_of_range(self):
        gc = geompp.GeometryCollection2D()
        with pytest.raises(IndexError):
            _ = gc[0]

    def test_get_line_segment(self):
        gc = geompp.GeometryCollection2D()
        seg = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(3, 4))
        gc.add(seg)
        result = gc.get(0)
        assert isinstance(result, geompp.LineSegment2D)

    def test_almost_equals_equal(self):
        gc1 = geompp.GeometryCollection2D()
        gc2 = geompp.GeometryCollection2D()
        gc1.add(geompp.Point2D(1, 2))
        gc2.add(geompp.Point2D(1, 2))
        assert gc1.almost_equals(gc2)
        assert gc1 == gc2

    def test_almost_equals_different(self):
        gc1 = geompp.GeometryCollection2D()
        gc2 = geompp.GeometryCollection2D()
        gc1.add(geompp.Point2D(1, 2))
        gc2.add(geompp.Point2D(9, 9))
        assert not gc1.almost_equals(gc2)

    def test_almost_equals_empty(self):
        gc1 = geompp.GeometryCollection2D()
        gc2 = geompp.GeometryCollection2D()
        assert gc1.almost_equals(gc2)
        assert gc1 == gc2

    def test_almost_equals_different_sizes(self):
        gc1 = geompp.GeometryCollection2D()
        gc2 = geompp.GeometryCollection2D()
        gc1.add(geompp.Point2D(1, 2))
        assert not gc1.almost_equals(gc2)

    def test_wkt_empty(self):
        gc = geompp.GeometryCollection2D()
        assert gc.to_wkt() == "GEOMETRYCOLLECTION EMPTY"
        assert str(gc) == "GEOMETRYCOLLECTION EMPTY"
        assert repr(gc) == "GEOMETRYCOLLECTION EMPTY"

    def test_wkt_with_geometries(self):
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        wkt = gc.to_wkt()
        assert "GEOMETRYCOLLECTION" in wkt
        assert "POINT" in wkt

    def test_nested_collection(self):
        inner = geompp.GeometryCollection2D()
        inner.add(geompp.Point2D(0, 0))
        outer = geompp.GeometryCollection2D()
        outer.add(inner)
        assert outer.size() == 1
        result = outer.get(0)
        assert isinstance(result, geompp.GeometryCollection2D)

    def test_copy_construction(self):
        gc1 = geompp.GeometryCollection2D()
        gc1.add(geompp.Point2D(5, 6))
        gc2 = geompp.GeometryCollection2D(gc1)
        assert gc1 == gc2

    def test_from_wkt_point(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection2D.from_wkt("GEOMETRYCOLLECTION (POINT (1 2))")
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Point2D)
        assert result == geompp.Point2D(1, 2)

    def test_from_wkt_invalid_throws(self):
        with pytest.raises(Exception):
            geompp.GeometryCollection2D.from_wkt("not a wkt")
        with pytest.raises(Exception):
            geompp.GeometryCollection2D.from_wkt("POINT (1 2)")

    def test_from_wkt_empty_throws(self):
        with pytest.raises(Exception):
            geompp.GeometryCollection2D.from_wkt("GEOMETRYCOLLECTION EMPTY")

    def test_roundtrip_wkt(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        gc2 = geompp.GeometryCollection2D.from_wkt(gc.to_wkt())
        assert gc == gc2

    def test_to_file(self):
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            gc.to_file(path)
            assert os.path.exists(path)
        finally:
            os.unlink(path)

    def test_add_line2d(self):
        gc = geompp.GeometryCollection2D()
        l = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        gc.add(l)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Line2D)

    def test_add_ray2d(self):
        gc = geompp.GeometryCollection2D()
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        gc.add(r)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Ray2D)

    def test_add_polyline2d(self):
        gc = geompp.GeometryCollection2D()
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 1)])
        gc.add(pl)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Polyline2D)

    def test_add_polygon2d(self):
        gc = geompp.GeometryCollection2D()
        poly = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])
        gc.add(poly)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Polygon2D)

    def test_from_file(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection2D()
        gc.add(geompp.Point2D(1, 2))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            gc.to_file(path)
            gc2 = geompp.GeometryCollection2D.from_file(path)
            assert gc == gc2
        finally:
            os.unlink(path)
