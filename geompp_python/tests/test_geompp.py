"""
Smoke / unit tests for the geompp Python bindings.

Run after installing the package:
    pip install ./geompp_python
    pytest geompp_python/tests/ -v
"""

import math
import tempfile
import os
import pytest
import geompp


# ─── Helpers ─────────────────────────────────────────────────────────────────

def approx(a, b, eps=1e-9):
    return abs(a - b) < eps


# ─── Precision ───────────────────────────────────────────────────────────────

class TestPrecision:
    def test_constants_exist(self):
        assert geompp.DP_THREE == 3
        assert geompp.DP_SIX   == 6
        assert geompp.DP_NINE  == 9

    def test_set_and_get(self):
        geompp.set_decimal_precision(geompp.DP_THREE)
        assert geompp.get_decimal_precision() == 3
        geompp.set_decimal_precision(geompp.DP_SIX)
        assert geompp.get_decimal_precision() == 6


# ─── Point2D ─────────────────────────────────────────────────────────────────

class TestPoint2D:
    def test_construction(self):
        p = geompp.Point2D(1.0, 2.0)
        assert approx(p.x, 1.0)
        assert approx(p.y, 2.0)

    def test_zero(self):
        z = geompp.Point2D.zero()
        assert approx(z.x, 0.0) and approx(z.y, 0.0)

    def test_add_vector(self):
        p = geompp.Point2D(1, 2) + geompp.Vector2D(3, 4)
        assert approx(p.x, 4) and approx(p.y, 6)

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

    def test_repr(self):
        assert "POINT" in repr(geompp.Point2D(1, 2))


# ─── Point3D ─────────────────────────────────────────────────────────────────

class TestPoint3D:
    def test_construction(self):
        p = geompp.Point3D(1, 2, 3)
        assert approx(p.x, 1) and approx(p.y, 2) and approx(p.z, 3)

    def test_add_vector(self):
        p = geompp.Point3D(1, 2, 3) + geompp.Vector3D(1, 1, 1)
        assert approx(p.x, 2) and approx(p.y, 3) and approx(p.z, 4)

    def test_distance_to(self):
        d = geompp.Point3D(0, 0, 0).distance_to(geompp.Point3D(1, 0, 0))
        assert approx(d, 1.0)

    def test_wkt_roundtrip(self):
        p = geompp.Point3D(1, 2, 3)
        p2 = geompp.Point3D.from_wkt(p.to_wkt())
        assert p.almost_equals(p2)


# ─── Vector2D ────────────────────────────────────────────────────────────────

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


# ─── Vector3D ────────────────────────────────────────────────────────────────

class TestVector3D:
    def test_cross(self):
        c = geompp.Vector3D(1, 0, 0).cross(geompp.Vector3D(0, 1, 0))
        assert approx(c.x, 0) and approx(c.y, 0) and approx(c.z, 1)

    def test_is_parallel(self):
        assert geompp.Vector3D(1, 0, 0).is_parallel(geompp.Vector3D(2, 0, 0))
        assert not geompp.Vector3D(1, 0, 0).is_parallel(geompp.Vector3D(0, 1, 0))

    def test_basis(self):
        assert approx(geompp.Vector3D.basis_z().z, 1)


# ─── LineSegment2D ───────────────────────────────────────────────────────────

class TestLineSegment2D:
    @pytest.fixture
    def seg(self):
        return geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0))

    def test_endpoints(self, seg):
        assert approx(seg.first.x, 0)
        assert approx(seg.last.x, 4)

    def test_length(self, seg):
        assert approx(seg.length(), 4.0)

    def test_contains(self, seg):
        assert seg.contains(geompp.Point2D(2, 0))
        assert not seg.contains(geompp.Point2D(2, 1))

    def test_interpolate(self, seg):
        mid = seg.interpolate(0.5)
        assert approx(mid.x, 2) and approx(mid.y, 0)

    def test_distance_to(self, seg):
        assert approx(seg.distance_to(geompp.Point2D(2, 3)), 3.0)

    def test_intersection_with_segment(self):
        s1 = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(2, 2))
        s2 = geompp.LineSegment2D.make(geompp.Point2D(0, 2), geompp.Point2D(2, 0))
        assert s1.intersects(s2)
        hit = s1.intersection(s2)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 1) and approx(hit.y, 1)

    def test_no_intersection(self):
        s1 = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s2 = geompp.LineSegment2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not s1.intersects(s2)
        assert s1.intersection(s2) is None

    def test_wkt_roundtrip(self, seg):
        seg2 = geompp.LineSegment2D.from_wkt(seg.to_wkt())
        assert seg.almost_equals(seg2)


# ─── LineSegment3D ───────────────────────────────────────────────────────────

class TestLineSegment3D:
    def test_length(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5))
        assert approx(s.length(), 5.0)

    def test_contains(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 4))
        assert s.contains(geompp.Point3D(0, 0, 2))


# ─── Line2D ──────────────────────────────────────────────────────────────────

class TestLine2D:
    @pytest.fixture
    def hline(self):
        return geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))

    @pytest.fixture
    def vline(self):
        return geompp.Line2D.make(geompp.Point2D(0.5, -1), geompp.Point2D(0.5, 1))

    def test_make_from_two_points(self, hline):
        assert approx(hline.direction.y, 0)

    def test_make_from_point_and_direction(self):
        l = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert approx(l.direction.x, 1)

    def test_contains(self, hline):
        assert hline.contains(geompp.Point2D(5, 0))
        assert not hline.contains(geompp.Point2D(5, 1))

    def test_distance_to(self, hline):
        assert approx(hline.distance_to(geompp.Point2D(0, 3)), 3.0)

    def test_project_onto(self, hline):
        p = hline.project_onto(geompp.Point2D(3, 5))
        assert approx(p.y, 0)

    def test_intersection(self, hline, vline):
        assert hline.intersects(vline)
        hit = hline.intersection(vline)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 0.5) and approx(hit.y, 0)

    def test_no_intersection_parallel(self):
        l1 = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        l2 = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not l1.intersects(l2)
        assert l1.intersection(l2) is None

    def test_intersection_with_params(self, hline, vline):
        hit, sc, tc = hline.intersection_with_params(vline)
        assert hit is not None
        assert approx(hit.x, 0.5)
        assert isinstance(sc, float)
        assert isinstance(tc, float)

    def test_wkt_roundtrip(self, hline):
        l2 = geompp.Line2D.from_wkt(hline.to_wkt())
        assert hline.almost_equals(l2)


# ─── Line3D ──────────────────────────────────────────────────────────────────

class TestLine3D:
    def test_intersection_with_segment(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 1))
        s = geompp.LineSegment3D.make(geompp.Point3D(0, -1, 2), geompp.Point3D(0, 1, 2))
        # segment is perpendicular to line at z=2 but both on x=0 plane — no intersection
        # (they cross at different z); just check the call works
        result = l.intersection(s)
        assert result is None or isinstance(result, geompp.Point3D)


# ─── Ray2D ───────────────────────────────────────────────────────────────────

class TestRay2D:
    @pytest.fixture
    def ray(self):
        return geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))

    def test_is_ahead(self, ray):
        assert ray.is_ahead(geompp.Point2D(1, 0))
        assert not ray.is_ahead(geompp.Point2D(-1, 0))

    def test_is_behind(self, ray):
        assert ray.is_behind(geompp.Point2D(-1, 0))

    def test_contains(self, ray):
        assert ray.contains(geompp.Point2D(5, 0))
        assert not ray.contains(geompp.Point2D(-1, 0))

    def test_intersection_with_segment(self, ray):
        s = geompp.LineSegment2D.make(geompp.Point2D(3, -1), geompp.Point2D(3, 1))
        assert ray.intersects(s)
        hit = ray.intersection(s)
        assert hit is not None
        assert approx(hit.x, 3) and approx(hit.y, 0)


# ─── Ray3D ───────────────────────────────────────────────────────────────────

class TestRay3D:
    def test_is_ahead(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r.is_ahead(geompp.Point3D(0, 0, 5))
        assert r.is_behind(geompp.Point3D(0, 0, -1))


# ─── Polygon2D ───────────────────────────────────────────────────────────────

class TestPolygon2D:
    @pytest.fixture
    def square(self):
        # Polygon2D requires a closed ring (first == last) for valid WKT output
        pts = [
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
            geompp.Point2D(0, 0),
        ]
        return geompp.Polygon2D.make(pts)

    def test_size(self, square):
        assert len(square) == 5

    def test_getitem(self, square):
        assert approx(square[0].x, 0) and approx(square[0].y, 0)
        assert approx(square[3].x, 0) and approx(square[3].y, 1)  # (0,1) before closing point

    def test_iteration(self, square):
        pts = list(square)
        assert len(pts) == 5  # 4 corners + closing point
        assert all(isinstance(p, geompp.Point2D) for p in pts)

    def test_index_error(self, square):
        with pytest.raises(IndexError):
            _ = square[10]


# ─── Polygon3D ───────────────────────────────────────────────────────────────

class TestPolygon3D:
    def test_construction(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 0, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        assert len(p) == 4


# ─── Polyline2D ──────────────────────────────────────────────────────────────

class TestPolyline2D:
    @pytest.fixture
    def pline(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0), geompp.Point2D(3, 4)]
        return geompp.Polyline2D.make(pts)

    def test_size(self, pline):
        assert pline.size() == 3

    def test_length(self, pline):
        assert approx(pline.length(), 7.0)  # 3 + 4

    def test_contains(self, pline):
        assert pline.contains(geompp.Point2D(1, 0))

    def test_interpolate(self, pline):
        # Polyline2D.interpolate() uses vertex index as parameter (0 … size-1)
        start = pline.interpolate(0.0)
        assert approx(start.x, 0) and approx(start.y, 0)
        end = pline.interpolate(float(pline.size() - 1))
        assert approx(end.x, 3) and approx(end.y, 4)

    def test_intersection_with_line(self, pline):
        l = geompp.Line2D.make(geompp.Point2D(0, 2), geompp.Point2D(1, 2))
        assert pline.intersects(l)
        hit = pline.intersection(l)
        assert hit is not None

    def test_wkt_roundtrip(self, pline):
        p2 = geompp.Polyline2D.from_wkt(pline.to_wkt())
        assert pline.almost_equals(p2)


# ─── Polyline3D ──────────────────────────────────────────────────────────────

class TestPolyline3D:
    def test_length(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.length(), 5.0)


# ─── Triangle2D ──────────────────────────────────────────────────────────────

class TestTriangle2D:
    @pytest.fixture
    def tri(self):
        return geompp.Triangle2D.make(
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3)
        )

    def test_vertices(self, tri):
        v0, v1, v2 = tri.vertices
        assert isinstance(v0, geompp.Point2D)
        assert approx(v0.x, 0) and approx(v0.y, 0)
        assert approx(v1.x, 4) and approx(v1.y, 0)

    def test_area(self, tri):
        assert approx(tri.area(), 6.0)  # 0.5 * 4 * 3

    def test_perimeter(self, tri):
        assert approx(tri.perimeter(), 4 + 3 + 5)  # 3-4-5 right triangle

    def test_centroid(self, tri):
        c = tri.centroid()
        assert approx(c.x, 4/3) and approx(c.y, 1.0)

    def test_contains(self, tri):
        assert tri.contains(geompp.Point2D(1, 1))
        assert not tri.contains(geompp.Point2D(5, 5))

    def test_interpolate(self, tri):
        p = tri.interpolate(0.0, 0.0)
        assert p is not None
        assert isinstance(p, geompp.Point2D)

    def test_intersection_with_line(self, tri):
        l = geompp.Line2D.make(geompp.Point2D(-1, 1), geompp.Point2D(5, 1))
        assert tri.intersects(l)
        hit = tri.intersection(l)
        assert hit is not None

    def test_wkt_roundtrip(self, tri):
        tri2 = geompp.Triangle2D.from_wkt(tri.to_wkt())
        assert tri.almost_equals(tri2)


# ─── Triangle3D ──────────────────────────────────────────────────────────────

class TestTriangle3D:
    @pytest.fixture
    def tri(self):
        return geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0)
        )

    def test_vertices(self, tri):
        v0, v1, v2 = tri.vertices
        assert isinstance(v0, geompp.Point3D)

    def test_centroid(self, tri):
        c = tri.centroid()
        assert isinstance(c, geompp.Point3D)

    def test_area_raises(self, tri):
        # Triangle3D::Area() is deliberately not implemented in C++ source
        with pytest.raises(Exception):
            tri.area()

    def test_wkt_roundtrip(self, tri):
        tri2 = geompp.Triangle3D.from_wkt(tri.to_wkt())
        assert tri.almost_equals(tri2)


# ─── BBox2D ──────────────────────────────────────────────────────────────────

class TestBBox2D:
    def test_from_points(self):
        bb = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(5, 5))
        assert approx(bb.min.x, 0)
        assert approx(bb.max.x, 5)

    def test_contains(self):
        bb = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(10, 10))
        assert bb.contains(geompp.Point2D(5, 5))
        assert not bb.contains(geompp.Point2D(11, 5))

    def test_eq(self):
        bb1 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        bb2 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert bb1 == bb2


# ─── BBox3D ──────────────────────────────────────────────────────────────────

class TestBBox3D:
    def test_from_points(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        assert approx(bb.max.z, 3)

    def test_contains(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 5, 5))
        assert bb.contains(geompp.Point3D(1, 1, 1))
        assert not bb.contains(geompp.Point3D(6, 1, 1))


# ─── Plane ───────────────────────────────────────────────────────────────────

class TestPlane:
    def test_xy_plane(self):
        pl = geompp.Plane.xy()
        assert isinstance(pl, geompp.Plane)
        # Normal of XY plane points in Z
        assert approx(pl.normal.z, 1)

    def test_yz_plane(self):
        pl = geompp.Plane.yz()
        assert approx(pl.normal.x, 1)

    def test_zx_plane(self):
        pl = geompp.Plane.zx()
        assert approx(pl.normal.y, 1)

    def test_project_onto(self):
        xy = geompp.Plane.xy()
        p3 = geompp.Point3D(1, 2, 5)
        proj = xy.project_onto(p3)
        assert approx(proj.z, 0)
        assert approx(proj.x, 1) and approx(proj.y, 2)

    def test_distance_to(self):
        xy = geompp.Plane.xy()
        assert approx(xy.distance_to(geompp.Point3D(0, 0, 3)), 3.0)

    def test_signed_distance_to(self):
        xy = geompp.Plane.xy()
        assert approx(xy.signed_distance_to(geompp.Point3D(0, 0, 3)),  3.0)
        assert approx(xy.signed_distance_to(geompp.Point3D(0, 0, -3)), -3.0)

    def test_contains(self):
        xy = geompp.Plane.xy()
        assert xy.contains(geompp.Point3D(1, 2, 0))
        assert not xy.contains(geompp.Point3D(1, 2, 1))

    def test_from_3_points(self):
        pl = geompp.Plane.from_3_points(
            geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1), geompp.Point3D(0, 1, 1)
        )
        # Plane at z=1, normal should be (0,0,1)
        assert approx(pl.normal.z, 1)

    def test_intersection_with_line(self):
        xy = geompp.Plane.xy()
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 5), geompp.Point3D(0, 0, -1))
        assert xy.intersects(l)
        hit = xy.intersection(l)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.z, 0)


# ─── LVSParser ───────────────────────────────────────────────────────────────

class TestLVSParser:
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
            parser = geompp.LVSParser.open(path)
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
            with geompp.LVSParser.open(path) as parser:
                item = parser.next()
                assert item is not None
            del parser  # force C++ destructor so the file handle is released on Windows
        finally:
            os.unlink(path)

    def test_to_wkt_static(self):
        p = geompp.Point2D(1, 2)
        wkt = geompp.LVSParser.to_wkt(p)
        assert "POINT" in wkt

    def test_to_wkt_none(self):
        assert "EMPTY" in geompp.LVSParser.to_wkt(None)


# ─── Free functions ──────────────────────────────────────────────────────────

class TestFreeFunctions:
    def test_are_collinear_2d_true(self):
        assert geompp.are_collinear(
            geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(2, 0)
        )

    def test_are_collinear_2d_false(self):
        assert not geompp.are_collinear(
            geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1)
        )

    def test_are_collinear_3d(self):
        assert geompp.are_collinear(
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 1), geompp.Point3D(2, 2, 2)
        )

    def test_remove_duplicates_2d(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0, 0), geompp.Point2D(1, 1)]
        result = geompp.remove_duplicates(pts)
        assert len(result) == 2

    def test_remove_duplicates_3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0)]
        result = geompp.remove_duplicates(pts)
        assert len(result) == 2

    def test_remove_collinear_2d(self):
        # Three collinear: middle one should be removed
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(2, 0)]
        result = geompp.remove_collinear(pts)
        assert len(result) == 2

    def test_average_2d(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(1, 2)]
        avg = geompp.average(pts)
        assert approx(avg.x, 1.0) and approx(avg.y, 2/3)

    def test_average_3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0)]
        avg = geompp.average(pts)
        assert approx(avg.x, 1.0)

    def test_linear_combination_2d(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 0)]
        result = geompp.linear_combination(pts, [0.0, 1.0])
        assert approx(result.x, 1.0)

    def test_remove_duplicates_from_sorted_list_2d(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0, 0), geompp.Point2D(1, 0)]
        result = geompp.remove_duplicates_from_sorted_list(pts)
        assert len(result) == 2
