"""
wkt parser binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestWktParser:
    def _make_lsv_file(self, lines):
        f = tempfile.NamedTemporaryFile(mode="w", suffix=".lsv", delete=False)
        f.write("\n".join(lines) + "\n")
        f.close()
        return f.name

    def test_parse_points(self):
        path = self._make_lsv_file([
            "POINT (1 2)",
            "POINT (3 4)",
        ])
        try:
            parser = geompp.WktParser.open(path)
            results = []
            while parser.has_next():
                item = parser.next()
                if item is not None:
                    results.append(item)
            del parser  # force C++ destructor so the file handle is released on Windows
            assert len(results) == 2
            assert isinstance(results[0], geompp.Point2D)
            assert approx(results[0].x, 1) and approx(results[0].y, 2)
        finally:
            os.unlink(path)

    def test_context_manager(self):
        path = self._make_lsv_file(["POINT (0 0)"])
        try:
            with geompp.WktParser.open(path) as parser:
                item = parser.next()
                assert item is not None
            del parser  # force C++ destructor so the file handle is released on Windows
        finally:
            os.unlink(path)

    def test_get_returns_geometry(self):
        result = geompp.WktParser.from_wkt("POINT (1 2)")
        assert isinstance(result, geompp.Point2D)
        assert approx(result.x, 1) and approx(result.y, 2)

    def test_get_returns_none_on_unknown(self):
        result = geompp.WktParser.from_wkt("BOGUS (1 2)")
        assert result is None

    def test_to_wkt_point2d(self):
        p = geompp.Point2D(1.0, 2.0)
        assert geompp.WktParser.to_wkt(p) == p.to_wkt()

    def test_to_wkt_linesegment2d(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert geompp.WktParser.to_wkt(seg) == seg.to_wkt()

    def test_to_wkt_point3d(self):
        p = geompp.Point3D(1.0, 2.0, 3.0)
        assert geompp.WktParser.to_wkt(p) == p.to_wkt()

    def test_to_wkt_none_raises(self):
        with pytest.raises(Exception):
            geompp.WktParser.to_wkt(None)

    def test_to_wkt_roundtrip(self):
        wkt_in = "POINT (1 2)"
        geom = geompp.WktParser.from_wkt(wkt_in)
        assert geompp.WktParser.to_wkt(geom) == wkt_in

    def test_to_wkt_unsupported_type_raises(self):
        with pytest.raises(Exception):
            geompp.WktParser.to_wkt(42)

    def test_get_file_path(self):
        path = self._make_lsv_file(["POINT (1 2)"])
        try:
            parser = geompp.WktParser.open(path)
            fp = parser.get_file_path()
            assert isinstance(fp, str)
            assert len(fp) > 0
            del parser
        finally:
            os.unlink(path)
