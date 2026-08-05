"""
geometry collection3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestGeometryCollection3D:
    def test_default_construction(self):
        gc = geompp.GeometryCollection3D()
        assert gc.size() == 0

    def test_add_point(self):
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(1, 2, 3))
        assert gc.size() == 1

    def test_add_multiple_types(self):
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(0, 0, 0))
        gc.add(geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0)))
        gc.add(geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0)
        ))
        assert gc.size() == 3

    def test_get_point(self):
        gc = geompp.GeometryCollection3D()
        p = geompp.Point3D(1, 2, 3)
        gc.add(p)
        result = gc.get(0)
        assert isinstance(result, geompp.Point3D)
        assert result == p

    def test_getitem(self):
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(1, 2, 3))
        assert isinstance(gc[0], geompp.Point3D)
        assert isinstance(gc[-1], geompp.Point3D)

    def test_almost_equals_equal(self):
        gc1 = geompp.GeometryCollection3D()
        gc2 = geompp.GeometryCollection3D()
        gc1.add(geompp.Point3D(1, 2, 3))
        gc2.add(geompp.Point3D(1, 2, 3))
        assert gc1.almost_equals(gc2)
        assert gc1 == gc2

    def test_almost_equals_empty(self):
        gc1 = geompp.GeometryCollection3D()
        gc2 = geompp.GeometryCollection3D()
        assert gc1.almost_equals(gc2)

    def test_wkt_empty(self):
        gc = geompp.GeometryCollection3D()
        assert gc.to_wkt() == "GEOMETRYCOLLECTION EMPTY"

    def test_wkt_with_geometries(self):
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(1, 2, 3))
        wkt = gc.to_wkt()
        assert "GEOMETRYCOLLECTION" in wkt
        assert "POINT" in wkt

    def test_nested_collection(self):
        inner = geompp.GeometryCollection3D()
        inner.add(geompp.Point3D(0, 0, 0))
        outer = geompp.GeometryCollection3D()
        outer.add(inner)
        assert outer.size() == 1
        result = outer.get(0)
        assert isinstance(result, geompp.GeometryCollection3D)

    def test_from_wkt_point(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection3D.from_wkt("GEOMETRYCOLLECTION (POINT (1 2 3))")
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Point3D)
        assert result == geompp.Point3D(1, 2, 3)

    def test_from_wkt_invalid_throws(self):
        with pytest.raises(Exception):
            geompp.GeometryCollection3D.from_wkt("not a wkt")
        with pytest.raises(Exception):
            geompp.GeometryCollection3D.from_wkt("POINT (1 2 3)")

    def test_from_wkt_empty_throws(self):
        with pytest.raises(Exception):
            geompp.GeometryCollection3D.from_wkt("GEOMETRYCOLLECTION EMPTY")

    def test_roundtrip_wkt(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(1, 2, 3))
        gc2 = geompp.GeometryCollection3D.from_wkt(gc.to_wkt())
        assert gc == gc2

    def test_add_line3d(self):
        gc = geompp.GeometryCollection3D()
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        gc.add(l)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Line3D)

    def test_add_ray3d(self):
        gc = geompp.GeometryCollection3D()
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        gc.add(r)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Ray3D)

    def test_add_polyline3d(self):
        gc = geompp.GeometryCollection3D()
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 1)])
        gc.add(pl)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Polyline3D)

    def test_add_polygon3d(self):
        gc = geompp.GeometryCollection3D()
        poly = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])
        gc.add(poly)
        assert gc.size() == 1
        result = gc.get(0)
        assert isinstance(result, geompp.Polygon3D)

    def test_to_file_from_file(self):
        geompp.set_decimal_precision(4)
        gc = geompp.GeometryCollection3D()
        gc.add(geompp.Point3D(1, 2, 3))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            gc.to_file(path)
            assert os.path.exists(path)
            gc2 = geompp.GeometryCollection3D.from_file(path)
            assert gc == gc2
        finally:
            os.unlink(path)
