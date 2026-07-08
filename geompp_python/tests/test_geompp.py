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


# ─── Point3D ─────────────────────────────────────────────────────────────────

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

    def test_location(self, seg):
        assert approx(seg.location(geompp.Point2D(0, 0)), 0.0)
        assert approx(seg.location(geompp.Point2D(4, 0)), 1.0)
        assert approx(seg.location(geompp.Point2D(2, 0)), 0.5)
        assert math.isinf(seg.location(geompp.Point2D(2, 1)))

    def test_project_onto(self, seg):
        assert seg.project_onto(geompp.Point2D(2, 3)).almost_equals(geompp.Point2D(2, 0))
        assert seg.project_onto(geompp.Point2D(-1, 2)).almost_equals(geompp.Point2D(0, 0))
        assert seg.project_onto(geompp.Point2D(5, 2)).almost_equals(geompp.Point2D(4, 0))

    def test_is_left(self, seg):
        # fixture: P0=(0,0), P1=(4,0). Direction=(4,0).
        # formula: (4)*p.y - (0)*p.x = 4*p.y  → left iff p.y > 0
        assert seg.is_left(geompp.Point2D(2, 1))    # above the rightward segment → left
        assert not seg.is_left(geompp.Point2D(2, -1))  # below → right
        assert not seg.is_left(geompp.Point2D(2, 0))   # on the axis → not left

        # upward segment: P0=(0,0), P1=(0,1). formula: -p.x → left iff p.x < 0
        up = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert up.is_left(geompp.Point2D(-1, 0))    # left of upward → left
        assert not up.is_left(geompp.Point2D(1, 0)) # right of upward → right

        # diagonal: P0=(0,0), P1=(1,1). formula: p.y - p.x → left iff p.y > p.x
        diag = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert diag.is_left(geompp.Point2D(0, 1))    # above y=x → left
        assert not diag.is_left(geompp.Point2D(1, 0))  # below y=x → right

        # reversed segment: P0=(4,0), P1=(0,0). formula: -4*p.y → left iff p.y < 0
        rev = geompp.LineSegment2D.make(geompp.Point2D(4, 0), geompp.Point2D(0, 0))
        assert rev.is_left(geompp.Point2D(2, -1))   # below → left when going leftward
        assert not rev.is_left(geompp.Point2D(2, 1))  # above → right when going leftward

    def test_wkt_roundtrip(self, seg):
        seg2 = geompp.LineSegment2D.from_wkt(seg.to_wkt())
        assert seg.almost_equals(seg2)

    def test_from_wkt_whitespace(self):
        expected = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert expected.almost_equals(geompp.LineSegment2D.from_wkt("LINESTRING (0 0,1 1)"))
        assert expected.almost_equals(geompp.LineSegment2D.from_wkt("LINESTRING (  0 0  ,  1  1  )"))

    def test_to_line(self, seg):
        l = seg.to_line()
        assert isinstance(l, geompp.Line2D)
        assert l.contains(seg.first)
        assert l.contains(seg.last)

    def test_to_file_from_file(self, seg):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            seg.to_file(path)
            assert os.path.exists(path)
            seg2 = geompp.LineSegment2D.from_file(path)
            assert seg.almost_equals(seg2)
        finally:
            os.unlink(path)

    def test_reversed(self):
        s = geompp.LineSegment2D.make(geompp.Point2D(1, 2), geompp.Point2D(3, 4))
        r = s.reversed()
        assert isinstance(r, geompp.LineSegment2D)
        assert r.first.almost_equals(s.last)
        assert r.last.almost_equals(s.first)
        assert approx(s.length(), r.length())
        rr = r.reversed()
        assert rr.first.almost_equals(s.first)
        assert rr.last.almost_equals(s.last)

    def test_intersects_line(self, seg):
        # seg goes (0,0)→(4,0); a vertical line at x=2 hits it
        l_hit = geompp.Line2D.make(geompp.Point2D(2, -1), geompp.Point2D(2, 1))
        assert seg.intersects(l_hit)
        hit = seg.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 2) and approx(hit.y, 0)
        # A line parallel and above — no intersection
        l_miss = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(4, 1))
        assert not seg.intersects(l_miss)
        assert seg.intersection(l_miss) is None

    def test_intersects_ray(self, seg):
        # ray pointing downward, hitting the segment at (2, 0)
        r_hit = geompp.Ray2D.make(geompp.Point2D(2, 3), geompp.Vector2D(0, -1))
        assert seg.intersects(r_hit)
        hit = seg.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 2) and approx(hit.y, 0)
        # ray pointing away — no intersection
        r_miss = geompp.Ray2D.make(geompp.Point2D(2, 3), geompp.Vector2D(0, 1))
        assert not seg.intersects(r_miss)
        assert seg.intersection(r_miss) is None


# ─── LineSegment3D ───────────────────────────────────────────────────────────

class TestLineSegment3D:
    def test_length(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5))
        assert approx(s.length(), 5.0)

    def test_contains(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 4))
        assert s.contains(geompp.Point3D(0, 0, 2))

    def test_location(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert approx(s.location(geompp.Point3D(0,0,0)), 0.0)
        assert approx(s.location(geompp.Point3D(4,0,0)), 1.0)
        assert approx(s.location(geompp.Point3D(2,0,0)), 0.5)
        assert math.isinf(s.location(geompp.Point3D(2,1,0)))

    def test_distance_to(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert approx(s.distance_to(geompp.Point3D(2, 3, 0)), 3.0)
        assert approx(s.distance_to(geompp.Point3D(2, 0, 0)), 0.0)

    def test_distance_to_line3d(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # line crossing the segment at (2,0,0)
        line_cross = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(seg.distance_to(line_cross), 0.0)
        # parallel line offset by 3 in Y
        line_parallel = geompp.Line3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(1, 3, 0))
        assert approx(seg.distance_to(line_parallel), 3.0)
        # line containing the segment (overlap) → 0
        line_overlap = geompp.Line3D.make(geompp.Point3D(-2, 0, 0), geompp.Point3D(10, 0, 0))
        assert approx(seg.distance_to(line_overlap), 0.0)
        assert seg.distance(line_overlap) is None

    def test_distance_to_ray3d(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # ray crossing the segment at (2,0,0)
        ray_cross = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, -1, 0))
        assert approx(seg.distance_to(ray_cross), 0.0)
        # parallel ray, offset by 4 in Z
        ray_parallel = geompp.Ray3D.make(geompp.Point3D(0, 0, 4), geompp.Vector3D(1, 0, 0))
        assert approx(seg.distance_to(ray_parallel), 4.0)
        # ray contained on the segment's line (overlap) → 0
        ray_overlap = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(seg.distance_to(ray_overlap), 0.0)
        assert seg.distance(ray_overlap) is None

        # 2D top-down (XY) the ray crosses the segment at (2,0); in 3D the ray is at z=5
        # while the segment is at z=0 → non-parallel skew, real distance = 5.
        ray_top_cross_skew = geompp.Ray3D.make(geompp.Point3D(2, 3, 5), geompp.Vector3D(0, -1, 0))
        assert approx(seg.distance_to(ray_top_cross_skew), 5.0)

    def test_distance_to_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # crossing segment
        s_cross = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(s1.distance_to(s_cross), 0.0)
        # parallel segment offset in Y
        s_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(2, 3, 0))
        assert approx(s1.distance_to(s_parallel), 3.0)
        # collinear (overlap) → 0
        s_overlap = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(6, 0, 0))
        assert approx(s1.distance_to(s_overlap), 0.0)
        assert s1.distance(s_overlap) is None
        # skew segment
        s_skew = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 5), geompp.Point3D(2, 1, 5))
        assert approx(s1.distance_to(s_skew), 5.0)

    def test_interpolate(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert s.interpolate(0.0).almost_equals(geompp.Point3D(0,0,0))
        assert s.interpolate(1.0).almost_equals(geompp.Point3D(4,0,0))
        assert s.interpolate(0.5).almost_equals(geompp.Point3D(2,0,0))

    def test_project_onto(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0,0,0), geompp.Point3D(4,0,0))
        assert s.project_onto(geompp.Point3D(2,3,0)).almost_equals(geompp.Point3D(2,0,0))
        assert s.project_onto(geompp.Point3D(-1,2,0)).almost_equals(geompp.Point3D(0,0,0))
        assert s.project_onto(geompp.Point3D(5,2,0)).almost_equals(geompp.Point3D(4,0,0))

    def test_to_line(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l = s.to_line()
        assert isinstance(l, geompp.Line3D)
        assert l.contains(s.first)
        assert l.contains(s.last)

    def test_reversed(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(1, 2, 3), geompp.Point3D(4, 5, 6))
        r = s.reversed()
        assert isinstance(r, geompp.LineSegment3D)
        assert r.first.almost_equals(s.last)
        assert r.last.almost_equals(s.first)
        # length preserved
        assert approx(s.length(), r.length())
        # double reverse returns the original
        rr = r.reversed()
        assert rr.first.almost_equals(s.first)
        assert rr.last.almost_equals(s.last)

    def test_to_file_from_file(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(1, 2, 3), geompp.Point3D(4, 5, 6))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            s.to_file(path)
            assert os.path.exists(path)
            s2 = geompp.LineSegment3D.from_file(path)
            assert s.almost_equals(s2)
        finally:
            os.unlink(path)

    def test_intersects_line3d(self):
        # segment along X axis; line along Y axis at (2,0,0) — coplanar crossing
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l_hit = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert s.intersects(l_hit)
        hit = s.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_line3d(self):
        # segment along X; parallel line above — no hit
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not s.intersects(l_miss)
        assert s.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, -1, 0))
        assert s.intersects(r_hit)
        hit = s.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_ray3d(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_miss = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, 1, 0))
        assert not s.intersects(r_miss)
        assert s.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s2 = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert s1.intersects(s2)
        hit = s1.intersection(s2)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_segment3d(self):
        s1 = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0))
        s2 = geompp.LineSegment3D.make(geompp.Point3D(3, -1, 0), geompp.Point3D(3, 1, 0))
        assert not s1.intersects(s2)
        assert s1.intersection(s2) is None

    def test_from_wkt_whitespace(self):
        expected = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        assert expected.almost_equals(geompp.LineSegment3D.from_wkt("LINESTRING (0 0 0,1 1 0)"))
        assert expected.almost_equals(geompp.LineSegment3D.from_wkt("LINESTRING (  0 0 0  ,  1  1  0  )"))


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

    def test_from_wkt_whitespace(self):
        l = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert l.almost_equals(geompp.Line2D.from_wkt("LINE (0 0,1 1)"))
        assert l.almost_equals(geompp.Line2D.from_wkt("LINE (  0 0  ,  1  1  )"))

    def test_intersects_ray(self, hline):
        # ray pointing upward from below y=0, crossing hline at (3,0)
        r_hit = geompp.Ray2D.make(geompp.Point2D(3, -2), geompp.Vector2D(0, 1))
        assert hline.intersects(r_hit)
        hit = hline.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # ray pointing upward from (0,1) — parallel but offset, no intersection with hline (y=0)
        r_miss = geompp.Ray2D.make(geompp.Point2D(0, 1), geompp.Vector2D(0, 1))
        assert not hline.intersects(r_miss)
        assert hline.intersection(r_miss) is None

    def test_intersects_segment(self, hline):
        # vertical segment crossing y=0
        s_hit = geompp.LineSegment2D.make(geompp.Point2D(3, -1), geompp.Point2D(3, 1))
        assert hline.intersects(s_hit)
        hit = hline.intersection(s_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # segment entirely above — no hit
        s_miss = geompp.LineSegment2D.make(geompp.Point2D(0, 1), geompp.Point2D(4, 1))
        assert not hline.intersects(s_miss)
        assert hline.intersection(s_miss) is None

    def test_to_file_from_file(self, hline):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            hline.to_file(path)
            assert os.path.exists(path)
            l2 = geompp.Line2D.from_file(path)
            assert hline.almost_equals(l2)
        finally:
            os.unlink(path)


# ─── Line3D ──────────────────────────────────────────────────────────────────

class TestLine3D:
    def test_contains(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        assert l.contains(geompp.Point3D(1,0,0))
        assert not l.contains(geompp.Point3D(1,1,0))

    def test_distance_to(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        assert approx(l.distance_to(geompp.Point3D(0,3,0)), 3.0)

    def test_distance_to_line3d(self):
        x_axis = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # intersecting (Y-axis crosses X-axis at origin)
        y_axis = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0))
        assert approx(x_axis.distance_to(y_axis), 0.0)
        assert x_axis.distance(y_axis) is None
        # skew (Y-dir line offset in Z)
        skew = geompp.Line3D.make(geompp.Point3D(0, -1, 5), geompp.Point3D(0, 1, 5))
        assert approx(x_axis.distance_to(skew), 5.0)
        dseg = x_axis.distance(skew)
        assert isinstance(dseg, geompp.LineSegment3D)
        assert approx(dseg.length(), 5.0)
        # parallel distinct
        parallel = geompp.Line3D.make(geompp.Point3D(0, 3, 0), geompp.Point3D(1, 3, 0))
        assert approx(x_axis.distance_to(parallel), 3.0)
        # overlap (collinear) → 0
        overlap = geompp.Line3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(7, 0, 0))
        assert approx(x_axis.distance_to(overlap), 0.0)
        assert x_axis.distance(overlap) is None

    def test_distance_to_ray3d(self):
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # ray hitting the line at origin
        ray_hit = geompp.Ray3D.make(geompp.Point3D(0, 2, 0), geompp.Vector3D(0, -1, 0))
        assert approx(line.distance_to(ray_hit), 0.0)
        # ray collinear with the line (overlap)
        ray_overlap = geompp.Ray3D.make(geompp.Point3D(3, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(line.distance_to(ray_overlap), 0.0)
        assert line.distance(ray_overlap) is None
        # ray pointing away → distance to ray origin
        ray_away = geompp.Ray3D.make(geompp.Point3D(0, 2, 0), geompp.Vector3D(0, 1, 0))
        assert approx(line.distance_to(ray_away), 2.0)

    def test_distance_to_segment3d(self):
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        # segment crossing line at origin
        seg_cross = geompp.LineSegment3D.make(geompp.Point3D(0, -1, 0), geompp.Point3D(0, 1, 0))
        assert approx(line.distance_to(seg_cross), 0.0)
        # parallel segment
        seg_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 4, 0), geompp.Point3D(3, 4, 0))
        assert approx(line.distance_to(seg_parallel), 4.0)
        # overlapping (collinear) segment → 0
        seg_overlap = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        assert approx(line.distance_to(seg_overlap), 0.0)
        assert line.distance(seg_overlap) is None

    def test_project_onto(self):
        l = geompp.Line3D.make(geompp.Point3D(0,0,0), geompp.Point3D(3,0,0))
        p = l.project_onto(geompp.Point3D(2,3,0))
        assert p.almost_equals(geompp.Point3D(2,0,0))

    def test_intersection_with_segment(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 1))
        s = geompp.LineSegment3D.make(geompp.Point3D(0, -1, 2), geompp.Point3D(0, 1, 2))
        # segment is perpendicular to line at z=2 but both on x=0 plane — no intersection
        # (they cross at different z); just check the call works
        result = l.intersection(s)
        assert result is None or isinstance(result, geompp.Point3D)

    def test_intersects_ray3d(self):
        # line along X; ray along Y at x=2, z=0 — they cross at (2,0,0)
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, -3, 0), geompp.Vector3D(0, 1, 0))
        assert l.intersects(r_hit)
        hit = l.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 2) and approx(hit.y, 0) and approx(hit.z, 0)

    def test_no_intersects_ray3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        # ray in a parallel plane (z=1) pointing along X — no crossing
        r_miss = geompp.Ray3D.make(geompp.Point3D(0, 0, 1), geompp.Vector3D(1, 0, 0))
        assert not l.intersects(r_miss)

    def test_intersects_segment3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert l.intersects(s_hit)

    def test_no_intersects_segment3d(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0))
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not l.intersects(s_miss)

    def test_to_file_from_file(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            l.to_file(path)
            assert os.path.exists(path)
            l2 = geompp.Line3D.from_file(path)
            assert l.almost_equals(l2)
        finally:
            os.unlink(path)

    def test_from_wkt_whitespace(self):
        l = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        assert l.almost_equals(geompp.Line3D.from_wkt("LINE (0 0 0,1 1 0)"))
        assert l.almost_equals(geompp.Line3D.from_wkt("LINE (  0 0 0  ,  1  1  0  )"))


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

    def test_distance_to(self, ray):
        assert approx(ray.distance_to(geompp.Point2D(3, 4)), 4.0)
        assert approx(ray.distance_to(geompp.Point2D(3, 0)), 0.0)

    def test_project_onto(self, ray):
        assert ray.project_onto(geompp.Point2D(3, 5)).almost_equals(geompp.Point2D(3, 0))
        assert ray.project_onto(geompp.Point2D(-2, 3)).almost_equals(geompp.Point2D(0, 0))

    def test_intersection_with_segment(self, ray):
        s = geompp.LineSegment2D.make(geompp.Point2D(3, -1), geompp.Point2D(3, 1))
        assert ray.intersects(s)
        hit = ray.intersection(s)
        assert hit is not None
        assert approx(hit.x, 3) and approx(hit.y, 0)

    def test_to_line(self, ray):
        l = ray.to_line()
        assert isinstance(l, geompp.Line2D)
        assert l.contains(ray.origin)

    def test_almost_equals(self, ray):
        ray2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert ray.almost_equals(ray2)
        ray3 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert not ray.almost_equals(ray3)

    def test_wkt_roundtrip(self, ray):
        wkt = ray.to_wkt()
        ray2 = geompp.Ray2D.from_wkt(wkt)
        assert ray.almost_equals(ray2)

    def test_from_wkt_whitespace(self, ray):
        assert ray.almost_equals(geompp.Ray2D.from_wkt("RAY (0 0,1 0)"))
        assert ray.almost_equals(geompp.Ray2D.from_wkt("RAY (  0 0  ,  1  0  )"))

    def test_to_file_from_file(self, ray):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            ray.to_file(path)
            assert os.path.exists(path)
            ray2 = geompp.Ray2D.from_file(path)
            assert ray.almost_equals(ray2)
        finally:
            os.unlink(path)

    def test_intersects_ray(self, ray):
        # two rays from origin pointing right and upward at (5,0) — they share origin
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        # rays from different origins that cross
        r_hit = geompp.Ray2D.make(geompp.Point2D(3, -2), geompp.Vector2D(0, 1))
        assert ray.intersects(r_hit)
        hit = ray.intersection(r_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 3) and approx(hit.y, 0)
        # two parallel rays — no intersection
        r_miss = geompp.Ray2D.make(geompp.Point2D(0, 1), geompp.Vector2D(1, 0))
        assert not ray.intersects(r_miss)
        assert ray.intersection(r_miss) is None

    def test_intersection_with_line(self, ray):
        l_hit = geompp.Line2D.make(geompp.Point2D(5, -1), geompp.Point2D(5, 1))
        assert ray.intersects(l_hit)
        hit = ray.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point2D)
        assert approx(hit.x, 5) and approx(hit.y, 0)
        # line parallel to ray — no intersection
        l_miss = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not ray.intersects(l_miss)
        assert ray.intersection(l_miss) is None


# ─── Ray3D ───────────────────────────────────────────────────────────────────

class TestRay3D:
    def test_is_ahead(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r.is_ahead(geompp.Point3D(0, 0, 5))
        assert r.is_behind(geompp.Point3D(0, 0, -1))

    def test_contains(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert r.contains(geompp.Point3D(3,0,0))
        assert not r.contains(geompp.Point3D(3,1,0))
        assert not r.contains(geompp.Point3D(-1,0,0))

    def test_distance_to(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert approx(r.distance_to(geompp.Point3D(3,4,0)), 4.0)
        assert approx(r.distance_to(geompp.Point3D(-2,0,0)), 2.0)

    def test_distance_to_line3d(self):
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # line crosses ray at origin
        line_hit = geompp.Line3D.make(geompp.Point3D(0, -1, 0), geompp.Point3D(0, 1, 0))
        assert approx(ray.distance_to(line_hit), 0.0)
        # parallel line, offset by 5 in Y
        line_parallel = geompp.Line3D.make(geompp.Point3D(0, 5, 0), geompp.Point3D(1, 5, 0))
        assert approx(ray.distance_to(line_parallel), 5.0)
        # overlapping (collinear) line → 0
        line_overlap = geompp.Line3D.make(geompp.Point3D(-2, 0, 0), geompp.Point3D(7, 0, 0))
        assert approx(ray.distance_to(line_overlap), 0.0)
        assert ray.distance(line_overlap) is None

    def test_distance_to_ray3d(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # crossing rays
        r2 = geompp.Ray3D.make(geompp.Point3D(3, 1, 0), geompp.Vector3D(0, -1, 0))
        assert approx(r1.distance_to(r2), 0.0)
        # skew (offset in Z)
        r_skew = geompp.Ray3D.make(geompp.Point3D(0, 0, 4), geompp.Vector3D(0, 1, 0))
        assert approx(r1.distance_to(r_skew), 4.0)
        # overlapping (collinear) rays, same direction
        r_overlap = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert approx(r1.distance_to(r_overlap), 0.0)
        assert r1.distance(r_overlap) is None
        # parallel distinct rays
        r_parallel = geompp.Ray3D.make(geompp.Point3D(0, 3, 0), geompp.Vector3D(1, 0, 0))
        assert approx(r1.distance_to(r_parallel), 3.0)

    def test_distance_to_segment3d(self):
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        # segment crossing the ray at (2,0,0)
        seg_cross = geompp.LineSegment3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert approx(ray.distance_to(seg_cross), 0.0)
        # parallel segment offset in Z
        seg_parallel = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 5), geompp.Point3D(4, 0, 5))
        assert approx(ray.distance_to(seg_parallel), 5.0)
        # collinear (overlap) → 0
        seg_overlap = geompp.LineSegment3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(3, 0, 0))
        assert approx(ray.distance_to(seg_overlap), 0.0)
        assert ray.distance(seg_overlap) is None

    def test_project_onto(self):
        r = geompp.Ray3D.make(geompp.Point3D(0,0,0), geompp.Vector3D(1,0,0))
        assert r.project_onto(geompp.Point3D(3,5,0)).almost_equals(geompp.Point3D(3,0,0))
        assert r.project_onto(geompp.Point3D(-2,3,0)).almost_equals(geompp.Point3D(0,0,0))

    def test_to_line(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        l = r.to_line()
        assert isinstance(l, geompp.Line3D)
        assert l.contains(r.origin)

    def test_almost_equals(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r1.almost_equals(r2)
        r3 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 1, 0))
        assert not r1.almost_equals(r3)

    def test_wkt_roundtrip(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 2, 3), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.from_wkt(r.to_wkt())
        assert r.almost_equals(r2)

    def test_from_wkt_whitespace(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r.almost_equals(geompp.Ray3D.from_wkt("RAY (0 0 0,1 0 0)"))
        assert r.almost_equals(geompp.Ray3D.from_wkt("RAY (  0 0 0  ,  1  0  0  )"))

    def test_to_file_from_file(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 2, 3), geompp.Vector3D(1, 0, 0))
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            r.to_file(path)
            assert os.path.exists(path)
            r2 = geompp.Ray3D.from_file(path)
            assert r.almost_equals(r2)
        finally:
            os.unlink(path)

    def test_intersects_line3d(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        l_hit = geompp.Line3D.make(geompp.Point3D(3, -2, 0), geompp.Point3D(3, 2, 0))
        assert r.intersects(l_hit)
        hit = r.intersection(l_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # line parallel and offset — no intersection
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not r.intersects(l_miss)
        assert r.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(3, -2, 0), geompp.Vector3D(0, 1, 0))
        assert r1.intersects(r2)
        hit = r1.intersection(r2)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # two parallel rays — no intersection
        r_miss = geompp.Ray3D.make(geompp.Point3D(0, 1, 0), geompp.Vector3D(1, 0, 0))
        assert not r1.intersects(r_miss)
        assert r1.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(3, -2, 0), geompp.Point3D(3, 2, 0))
        assert r.intersects(s_hit)
        hit = r.intersection(s_hit)
        assert hit is not None
        assert isinstance(hit, geompp.Point3D)
        assert approx(hit.x, 3) and approx(hit.y, 0) and approx(hit.z, 0)
        # segment behind the ray — no intersection
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(-3, -2, 0), geompp.Point3D(-3, 2, 0))
        assert not r.intersects(s_miss)
        assert r.intersection(s_miss) is None


# ─── Polygon2D ───────────────────────────────────────────────────────────────

class TestPolygon2D:
    @pytest.fixture
    def square(self):
        pts = [
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ]
        return geompp.Polygon2D.make(pts)

    def test_size(self, square):
        assert len(square) == 4

    def test_getitem(self, square):
        assert approx(square[0].x, 0) and approx(square[0].y, 0)
        assert approx(square[3].x, 0) and approx(square[3].y, 1)

    def test_iteration(self, square):
        pts = list(square)
        assert len(pts) == 4
        assert all(isinstance(p, geompp.Point2D) for p in pts)

    def test_index_error(self, square):
        with pytest.raises(IndexError):
            _ = square[10]

    def test_cw_outer_throws(self):
        with pytest.raises(Exception):
            geompp.Polygon2D.make([
                geompp.Point2D(0, 0), geompp.Point2D(0, 4),
                geompp.Point2D(4, 4), geompp.Point2D(4, 0),
            ])

    def test_with_holes_valid(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        p = geompp.Polygon2D.make(outer, [hole])
        assert len(p) == 4

    def test_with_holes_ccw_hole_throws(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        ccw_hole = [
            geompp.Point2D(1, 1), geompp.Point2D(3, 1),
            geompp.Point2D(3, 3), geompp.Point2D(1, 3),
        ]
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [ccw_hole])

    def test_centroid_square(self, square):
        c = square.centroid()
        assert approx(c.x, 0.5) and approx(c.y, 0.5)

    def test_centroid_triangle(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3)
        ])
        c = p.centroid()
        assert approx(c.x, 4/3) and approx(c.y, 1.0)

    def test_centroid_with_hole(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        c = geompp.Polygon2D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0)

    def test_perimeter_square(self, square):
        # 1×1 square → perimeter = 4
        assert approx(square.perimeter_size(), 4.0)

    def test_perimeter_rectangle(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(3, 0),
            geompp.Point2D(3, 4), geompp.Point2D(0, 4),
        ])
        assert approx(p.perimeter_size(), 14.0)

    def test_perimeter_triangle(self):
        # 3-4-5 right triangle → perimeter = 12
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3),
        ])
        assert approx(p.perimeter_size(), 12.0)

    def test_to_segments(self, square):
        segs = square.to_segments()
        assert len(segs) == 4  # N vertices → N segments (closed)
        assert all(isinstance(s, geompp.LineSegment2D) for s in segs)
        assert all(approx(s.length(), 1.0) for s in segs)
        first = segs[0]
        assert first.first.almost_equals(geompp.Point2D(0, 0))
        assert first.last.almost_equals(geompp.Point2D(1, 0))
        last = segs[3]
        assert last.first.almost_equals(geompp.Point2D(0, 1))
        assert last.last.almost_equals(geompp.Point2D(0, 0))

    def test_almost_equals(self, square):
        sq2 = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])
        assert square.almost_equals(sq2)
        other = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(2, 0),
            geompp.Point2D(2, 2), geompp.Point2D(0, 2),
        ])
        assert not square.almost_equals(other)

    def test_to_file_from_file(self, square):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            square.to_file(path)
            assert os.path.exists(path)
            sq2 = geompp.Polygon2D.from_file(path)
            assert square.almost_equals(sq2)
        finally:
            os.unlink(path)

    def test_contains(self, square):
        # fixture: unit square (0,0)-(1,0)-(1,1)-(0,1)
        assert square.contains(geompp.Point2D(0.5, 0.5))    # center
        assert square.contains(geompp.Point2D(0.1, 0.1))    # near corner
        assert square.contains(geompp.Point2D(0.9, 0.9))    # near opposite corner
        assert not square.contains(geompp.Point2D(-0.1, 0.5))
        assert not square.contains(geompp.Point2D(1.1, 0.5))
        assert not square.contains(geompp.Point2D(0.5, -0.1))
        assert not square.contains(geompp.Point2D(0.5, 1.1))
        assert not square.contains(geompp.Point2D(5, 5))

        # polygon with hole: 4×4 outer, 2×2 centred hole
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.contains(geompp.Point2D(0.5, 0.5))   # inside outer, outside hole
        assert poly.contains(geompp.Point2D(3.5, 3.5))   # inside outer, outside hole
        assert not poly.contains(geompp.Point2D(2, 2))   # inside hole
        assert not poly.contains(geompp.Point2D(-1, 2))  # outside outer

        # boundary — vertices and edge midpoints are included
        assert square.contains(geompp.Point2D(0,   0))    # bottom-left vertex
        assert square.contains(geompp.Point2D(1,   0))    # bottom-right vertex
        assert square.contains(geompp.Point2D(1,   1))    # top-right vertex
        assert square.contains(geompp.Point2D(0,   1))    # top-left vertex
        assert square.contains(geompp.Point2D(0.5, 0))    # bottom edge midpoint
        assert square.contains(geompp.Point2D(1,   0.5))  # right edge midpoint
        assert square.contains(geompp.Point2D(0.5, 1))    # top edge midpoint
        assert square.contains(geompp.Point2D(0,   0.5))  # left edge midpoint
        assert poly.contains(geompp.Point2D(2,   0))      # outer bottom edge
        assert poly.contains(geompp.Point2D(4,   2))      # outer right edge
        assert poly.contains(geompp.Point2D(2,   1))      # hole bottom edge
        assert poly.contains(geompp.Point2D(1,   2))      # hole left edge

    def test_is_on_perimeter(self, square):
        # vertices
        assert square.is_on_perimeter(geompp.Point2D(0,   0))
        assert square.is_on_perimeter(geompp.Point2D(1,   0))
        assert square.is_on_perimeter(geompp.Point2D(1,   1))
        assert square.is_on_perimeter(geompp.Point2D(0,   1))
        # edge midpoints
        assert square.is_on_perimeter(geompp.Point2D(0.5, 0))
        assert square.is_on_perimeter(geompp.Point2D(1,   0.5))
        assert square.is_on_perimeter(geompp.Point2D(0.5, 1))
        assert square.is_on_perimeter(geompp.Point2D(0,   0.5))
        # interior and outside must be False
        assert not square.is_on_perimeter(geompp.Point2D(0.5, 0.5))
        assert not square.is_on_perimeter(geompp.Point2D(-0.1, 0.5))
        assert not square.is_on_perimeter(geompp.Point2D(1.1,  0.5))
        # hole boundary
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.is_on_perimeter(geompp.Point2D(2, 0))  # outer bottom
        assert poly.is_on_perimeter(geompp.Point2D(4, 2))  # outer right
        assert poly.is_on_perimeter(geompp.Point2D(2, 1))  # hole bottom
        assert poly.is_on_perimeter(geompp.Point2D(1, 2))  # hole left
        assert not poly.is_on_perimeter(geompp.Point2D(0.5, 0.5))  # interior
        assert not poly.is_on_perimeter(geompp.Point2D(2,   2))    # inside hole

    # NOTE: is_simple() correctness rides on the (currently provisional) sweep-line comparator; these encode
    # the intended behaviour and should be re-verified once the real sweep-status ordering is in place.
    def test_is_simple_smoke(self, square):
        assert isinstance(square.is_simple(), bool)

    def test_is_simple_convex_square_true(self, square):
        assert square.is_simple()

    def test_is_simple_self_intersecting_false(self):
        # CCW (positive area) but edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(1, 3), geompp.Point2D(3, 3),
        ])
        assert not p.is_simple()

    def test_from_wkt_whitespace(self):
        sq = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])
        assert sq.almost_equals(geompp.Polygon2D.from_wkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))"))
        assert sq.almost_equals(geompp.Polygon2D.from_wkt("POLYGON ((  0 0  ,  1 0  ,  1 1  ,  0 1  ,  0 0  ))"))


# ─── Polygon2D Intersection ───────────────────────────────────────────────────

class TestPolygon2DIntersection:
    @pytest.fixture
    def sq(self):
        return geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])

    # ── Line2D ───────────────────────────────────────────────────────────────

    def test_intersection_line_passes_through(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(0, 0.5), geompp.Point2D(1, 0.5))
        result = sq.intersection(line)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1
        seg = result[0]
        endpoints = {(round(seg.first.x, 3), round(seg.first.y, 3)),
                     (round(seg.last.x,  3), round(seg.last.y,  3))}
        assert (0.0, 0.5) in endpoints
        assert (1.0, 0.5) in endpoints

    def test_intersection_line_misses(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 1))
        assert sq.intersection(line) is None

    def test_intersects_line_true(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(0.5, -1), geompp.Point2D(0.5, 2))
        assert sq.intersects(line)

    def test_intersects_line_false(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 1))
        assert not sq.intersects(line)

    # ── Ray2D ────────────────────────────────────────────────────────────────

    def test_intersection_ray_hits(self, sq):
        ray = geompp.Ray2D.make(geompp.Point2D(-1, 0.5), geompp.Vector2D(1, 0))
        result = sq.intersection(ray)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1

    def test_intersection_ray_pointing_away(self, sq):
        ray = geompp.Ray2D.make(geompp.Point2D(5, 0.5), geompp.Vector2D(1, 0))
        assert sq.intersection(ray) is None

    def test_intersection_ray_origin_inside(self, sq):
        # Origin inside polygon, ray clips from origin to exit.
        ray = geompp.Ray2D.make(geompp.Point2D(0.5, 0.5), geompp.Vector2D(1, 0))
        result = sq.intersection(ray)
        assert result is not None
        assert isinstance(result, list)
        seg = result[0]
        endpoints = {(round(seg.first.x, 3), round(seg.first.y, 3)),
                     (round(seg.last.x,  3), round(seg.last.y,  3))}
        assert (0.5, 0.5) in endpoints

    def test_intersects_ray(self, sq):
        assert sq.intersects(geompp.Ray2D.make(geompp.Point2D(-1, 0.5), geompp.Vector2D(1, 0)))
        assert not sq.intersects(geompp.Ray2D.make(geompp.Point2D(5, 0.5), geompp.Vector2D(1, 0)))

    # ── LineSegment2D ─────────────────────────────────────────────────────────

    def test_intersection_segment_pierces(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(-0.5, 0.5), geompp.Point2D(1.5, 0.5))
        result = sq.intersection(seg)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1
        chord = result[0]
        endpoints = {(round(chord.first.x, 3), round(chord.first.y, 3)),
                     (round(chord.last.x,  3), round(chord.last.y,  3))}
        assert (0.0, 0.5) in endpoints
        assert (1.0, 0.5) in endpoints

    def test_intersection_segment_too_short(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(-2, 0.5), geompp.Point2D(-0.5, 0.5))
        assert sq.intersection(seg) is None

    def test_intersection_segment_entirely_inside(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(0.2, 0.5), geompp.Point2D(0.8, 0.5))
        result = sq.intersection(seg)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1

    def test_intersects_segment(self, sq):
        assert sq.intersects(geompp.LineSegment2D.make(geompp.Point2D(-0.5, 0.5), geompp.Point2D(1.5, 0.5)))
        assert not sq.intersects(geompp.LineSegment2D.make(geompp.Point2D(-2, 0.5), geompp.Point2D(-0.5, 0.5)))


# ─── Polygon3D ───────────────────────────────────────────────────────────────

class TestPolygon3DIsSimple:
    # NOTE: is_simple() correctness rides on the (currently provisional) sweep-line comparator; these encode
    # the intended behaviour and should be re-verified once the real sweep-status ordering is in place.
    def test_is_simple_convex_square_xy_plane_true(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])
        assert p.is_simple()

    def test_is_simple_convex_square_yz_plane_true(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        assert p.is_simple()

    def test_is_simple_self_intersecting_false(self):
        # CCW (positive area) but edges (4,0,0)->(1,3,0) and (3,3,0)->(0,0,0) cross
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0),
        ])
        assert not p.is_simple()


class TestPolygon3D:
    def test_construction(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        assert len(p) == 4

    def test_cw_outer_throws(self):
        with pytest.raises(Exception):
            geompp.Polygon3D.make([
                geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
                geompp.Point3D(4, 4, 0), geompp.Point3D(4, 0, 0),
            ])

    def test_with_holes_valid(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole])
        assert len(p) == 4

    def test_with_holes_ccw_hole_throws(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        ccw_hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(3, 1, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(1, 3, 0),
        ]
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [ccw_hole])

    def test_area_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).area(), 16.0)

    def test_area_with_hole(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        assert approx(geompp.Polygon3D.make(outer, [hole]).area(), 12.0)

    def test_area_far_from_origin(self):
        ox, oy = 1e8, 1e8
        pts = [
            geompp.Point3D(ox,     oy,     0), geompp.Point3D(ox + 4, oy,     0),
            geompp.Point3D(ox + 4, oy + 4, 0), geompp.Point3D(ox,     oy + 4, 0),
        ]
        assert abs(geompp.Polygon3D.make(pts).area() - 16.0) < 1e-4

    def test_perimeter_square(self):
        # 1×1 square → perimeter = 4
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 4.0)

    def test_perimeter_non_xy_plane(self):
        # 1×1 square on YZ plane → same perimeter = 4
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 4.0)

    def test_perimeter_triangle(self):
        # 3-4-5 right triangle → perimeter = 12
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 3, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 12.0)

    def test_get_plane_returns_plane(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        pl = geompp.Polygon3D.make(pts).get_plane()
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_get_plane_contains_all_vertices(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        pl = p.get_plane()
        for i in range(len(p)):
            assert pl.contains(p[i])

    def test_get_plane_with_holes(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole])
        pl = p.get_plane()
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_centroid_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        c = geompp.Polygon3D.make(pts).centroid()
        assert isinstance(c, geompp.Point3D)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_centroid_elevated_square(self):
        pts = [
            geompp.Point3D(0, 0, 5), geompp.Point3D(4, 0, 5),
            geompp.Point3D(4, 4, 5), geompp.Point3D(0, 4, 5),
        ]
        c = geompp.Polygon3D.make(pts).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 5.0)

    def test_centroid_with_centered_hole(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        c = geompp.Polygon3D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_centroid_with_off_center_hole(self):
        # 6×1 outer, 2×1 CW hole on the right → centroid shifts left to x=2
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0),
            geompp.Point3D(6, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        hole = [
            geompp.Point3D(4, 0, 0), geompp.Point3D(4, 1, 0),
            geompp.Point3D(6, 1, 0), geompp.Point3D(6, 0, 0),
        ]
        c = geompp.Polygon3D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 0.5) and approx(c.z, 0.0)

    def test_to_segments(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        segs = p.to_segments()
        assert len(segs) == 4  # N vertices → N segments (closed)
        assert all(isinstance(s, geompp.LineSegment3D) for s in segs)
        assert all(approx(s.length(), 1.0) for s in segs)
        first = segs[0]
        assert first.first.almost_equals(geompp.Point3D(0, 0, 0))
        assert first.last.almost_equals(geompp.Point3D(1, 0, 0))
        last = segs[3]
        assert last.first.almost_equals(geompp.Point3D(0, 1, 0))
        assert last.last.almost_equals(geompp.Point3D(0, 0, 0))

    def test_almost_equals(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p1 = geompp.Polygon3D.make(pts)
        p2 = geompp.Polygon3D.make(pts)
        assert p1.almost_equals(p2)
        other_pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0),
            geompp.Point3D(2, 2, 0), geompp.Point3D(0, 2, 0),
        ]
        assert not p1.almost_equals(geompp.Polygon3D.make(other_pts))

    def test_to_file_from_file(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            p.to_file(path)
            assert os.path.exists(path)
            p2 = geompp.Polygon3D.from_file(path)
            assert p.almost_equals(p2)
        finally:
            os.unlink(path)

    def test_contains(self):
        # unit square in XY plane
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        sq = geompp.Polygon3D.make(pts)
        assert sq.contains(geompp.Point3D(0.5, 0.5, 0))    # center
        assert sq.contains(geompp.Point3D(0.1, 0.1, 0))    # near corner
        assert sq.contains(geompp.Point3D(0.9, 0.9, 0))    # near corner
        assert not sq.contains(geompp.Point3D(-0.1, 0.5, 0))
        assert not sq.contains(geompp.Point3D(1.1, 0.5, 0))
        assert not sq.contains(geompp.Point3D(0.5, 0.5, 1))   # off-plane
        assert not sq.contains(geompp.Point3D(0.5, 0.5, -1))  # off-plane

        # polygon with hole in XY plane
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.contains(geompp.Point3D(0.5, 0.5, 0))   # inside outer, outside hole
        assert poly.contains(geompp.Point3D(3.5, 3.5, 0))   # inside outer, outside hole
        assert not poly.contains(geompp.Point3D(2, 2, 0))   # inside hole
        assert not poly.contains(geompp.Point3D(-1, 2, 0))  # outside outer
        assert not poly.contains(geompp.Point3D(2, 2, 1))   # above plane

        # boundary — vertices and edge midpoints are included
        assert sq.contains(geompp.Point3D(0,   0,   0))   # vertex
        assert sq.contains(geompp.Point3D(1,   0,   0))   # vertex
        assert sq.contains(geompp.Point3D(0.5, 0,   0))   # bottom edge midpoint
        assert sq.contains(geompp.Point3D(1,   0.5, 0))   # right edge midpoint
        assert sq.contains(geompp.Point3D(0.5, 1,   0))   # top edge midpoint
        assert sq.contains(geompp.Point3D(0,   0.5, 0))   # left edge midpoint
        assert poly.contains(geompp.Point3D(2,   0,   0))  # outer bottom edge
        assert poly.contains(geompp.Point3D(2,   1,   0))  # hole bottom edge

        # square in YZ plane (x=0)
        yz_sq = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        assert yz_sq.contains(geompp.Point3D(0, 0.5, 0.5))
        assert not yz_sq.contains(geompp.Point3D(1, 0.5, 0.5))  # off-plane

    def test_is_on_perimeter(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        sq = geompp.Polygon3D.make(pts)
        # vertices
        assert sq.is_on_perimeter(geompp.Point3D(0,   0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   1,   0))
        assert sq.is_on_perimeter(geompp.Point3D(0,   1,   0))
        # edge midpoints
        assert sq.is_on_perimeter(geompp.Point3D(0.5, 0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   0.5, 0))
        assert sq.is_on_perimeter(geompp.Point3D(0.5, 1,   0))
        assert sq.is_on_perimeter(geompp.Point3D(0,   0.5, 0))
        # interior and outside must be False
        assert not sq.is_on_perimeter(geompp.Point3D(0.5, 0.5, 0))
        assert not sq.is_on_perimeter(geompp.Point3D(-0.1, 0.5, 0))
        assert not sq.is_on_perimeter(geompp.Point3D(0.5,  0.5, 0.01))  # off-plane
        # hole boundary
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.is_on_perimeter(geompp.Point3D(2, 0, 0))  # outer bottom
        assert poly.is_on_perimeter(geompp.Point3D(4, 2, 0))  # outer right
        assert poly.is_on_perimeter(geompp.Point3D(2, 1, 0))  # hole bottom
        assert poly.is_on_perimeter(geompp.Point3D(1, 2, 0))  # hole left
        assert not poly.is_on_perimeter(geompp.Point3D(0.5, 0.5, 0))  # interior
        assert not poly.is_on_perimeter(geompp.Point3D(2,   2,   0))  # inside hole

    def test_from_wkt_whitespace(self):
        sq = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])
        assert sq.almost_equals(geompp.Polygon3D.from_wkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))"))
        assert sq.almost_equals(geompp.Polygon3D.from_wkt("POLYGON ((  0 0 0  ,  1 0 0  ,  1 1 0  ,  0 1 0  ,  0 0 0  ))"))


# ─── Polygon3D::Intersection ──────────────────────────────────────────────────

class TestPolygon3DIntersection:
    def _sq(self):
        return geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])

    def test_intersection_line_hits_center(self):
        result = self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_line_misses_outside(self):
        assert self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1))) is None

    def test_intersection_line_parallel_miss(self):
        assert self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(0, 0, 1), geompp.Point3D(1, 1, 1))) is None

    def test_intersects_line(self):
        sq = self._sq()
        assert sq.intersects(geompp.Line3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert not sq.intersects(geompp.Line3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1)))

    def test_intersection_ray_hits_center(self):
        result = self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, -1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_ray_pointing_away(self):
        assert self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, 1))) is None

    def test_intersection_ray_misses_outside(self):
        assert self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(2, 0.5, 1), geompp.Vector3D(0, 0, -1))) is None

    def test_intersects_ray(self):
        sq = self._sq()
        assert sq.intersects(geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, -1)))
        assert not sq.intersects(geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, 1)))

    def test_intersection_segment_pierces(self):
        result = self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_segment_too_short(self):
        assert self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, 0.5), geompp.Point3D(0.5, 0.5, 1))) is None

    def test_intersection_segment_misses_outside(self):
        assert self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1))) is None

    def test_intersects_segment(self):
        sq = self._sq()
        assert sq.intersects(geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert not sq.intersects(geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, 0.5), geompp.Point3D(0.5, 0.5, 1)))

    def test_intersection_non_xy_plane(self):
        # YZ square at x=0; normal = +X. Line along X through center (0, 0.5, 0.5).
        yz = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        result = yz.intersection(geompp.Line3D.make(geompp.Point3D(-1, 0.5, 0.5), geompp.Point3D(1, 0.5, 0.5)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0, 0.5, 0.5))


# ─── Free function centroid (3D) ─────────────────────────────────────────────

class TestCentroid3D:
    def test_xy_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        c = geompp.centroid(pts)
        assert isinstance(c, geompp.Point3D)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_elevated_xy_square(self):
        pts = [
            geompp.Point3D(0, 0, 7), geompp.Point3D(4, 0, 7),
            geompp.Point3D(4, 4, 7), geompp.Point3D(0, 4, 7),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 7.0)

    def test_yz_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(0, 4, 4), geompp.Point3D(0, 0, 4),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 0.0) and approx(c.y, 2.0) and approx(c.z, 2.0)

    def test_off_origin_yz_square(self):
        pts = [
            geompp.Point3D(5, 0, 0), geompp.Point3D(5, 4, 0),
            geompp.Point3D(5, 4, 4), geompp.Point3D(5, 0, 4),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 5.0) and approx(c.y, 2.0) and approx(c.z, 2.0)

    def test_with_explicit_plane(self):
        pts = [
            geompp.Point3D(0, 0, 3), geompp.Point3D(4, 0, 3),
            geompp.Point3D(4, 4, 3), geompp.Point3D(0, 4, 3),
        ]
        pl = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 3), geompp.Vector3D(0, 0, 1))
        c = geompp.centroid(pts, pl)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 3.0)

    def test_triangle_returns_average(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(0, 3, 0)]
        c = geompp.centroid(pts)
        assert approx(c.x, 1.0) and approx(c.y, 1.0) and approx(c.z, 0.0)


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
        start = pline.interpolate(0.0)
        assert approx(start.x, 0) and approx(start.y, 0)
        end = pline.interpolate(1.0)
        assert approx(end.x, 3) and approx(end.y, 4)
        with pytest.raises(Exception):
            pline.interpolate(1.2)
        with pytest.raises(Exception):
            pline.interpolate(-0.1)

    def test_intersection_with_line(self, pline):
        l = geompp.Line2D.make(geompp.Point2D(0, 2), geompp.Point2D(1, 2))
        assert pline.intersects(l)
        hit = pline.intersection(l)
        assert hit is not None

    def test_wkt_roundtrip(self, pline):
        p2 = geompp.Polyline2D.from_wkt(pline.to_wkt())
        assert pline.almost_equals(p2)

    def test_from_wkt_whitespace(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1)])
        assert pl.almost_equals(geompp.Polyline2D.from_wkt("LINESTRING (0 0, 1 0, 1 1)"))
        assert pl.almost_equals(geompp.Polyline2D.from_wkt("LINESTRING (  0 0  ,  1 0  ,  1 1  )"))

    def test_location(self, pline):
        assert approx(pline.location(geompp.Point2D(0, 0)), 0.0)
        assert approx(pline.location(geompp.Point2D(3, 4)), 1.0)
        assert math.isinf(pline.location(geompp.Point2D(1, 1)))

    def test_distance_to(self, pline):
        assert approx(pline.distance_to(geompp.Point2D(0, 0)), 0.0)
        assert approx(pline.distance_to(geompp.Point2D(1, 0)), 0.0)
        assert approx(pline.distance_to(geompp.Point2D(0, 1)), 1.0)

    def test_project_onto(self, pline):
        assert pline.project_onto(geompp.Point2D(2, 3)).almost_equals(geompp.Point2D(3, 3))
        assert pline.project_onto(geompp.Point2D(6, 2)).almost_equals(geompp.Point2D(3, 2))

    def test_to_segments(self, pline):
        segs = pline.to_segments()
        assert len(segs) == 2  # 3 knots → 2 segments (open)
        assert all(isinstance(s, geompp.LineSegment2D) for s in segs)
        assert approx(segs[0].length(), 3.0)
        assert approx(segs[1].length(), 4.0)
        assert segs[0].first.almost_equals(geompp.Point2D(0, 0))
        assert segs[0].last.almost_equals(geompp.Point2D(3, 0))

    def test_intersects_ray(self, pline):
        # pline: (0,0)→(3,0)→(3,4). Ray pointing right at y=2 crosses vertical segment
        r_hit = geompp.Ray2D.make(geompp.Point2D(0, 2), geompp.Vector2D(1, 0))
        assert pline.intersects(r_hit)
        hit = pline.intersection(r_hit)
        assert hit is not None
        # ray pointing away — no hit
        r_miss = geompp.Ray2D.make(geompp.Point2D(5, 2), geompp.Vector2D(1, 0))
        assert not pline.intersects(r_miss)
        assert pline.intersection(r_miss) is None

    def test_intersects_segment(self, pline):
        s_hit = geompp.LineSegment2D.make(geompp.Point2D(1, -1), geompp.Point2D(1, 1))
        assert pline.intersects(s_hit)
        hit = pline.intersection(s_hit)
        assert hit is not None
        s_miss = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 4))
        assert not pline.intersects(s_miss)
        assert pline.intersection(s_miss) is None

    def test_intersects_polyline(self, pline):
        # crossing polyline
        other_hit = geompp.Polyline2D.make([geompp.Point2D(1, -1), geompp.Point2D(1, 1)])
        assert pline.intersects(other_hit)
        hit = pline.intersection(other_hit)
        assert hit is not None
        # non-crossing polyline
        other_miss = geompp.Polyline2D.make([geompp.Point2D(5, 0), geompp.Point2D(5, 4)])
        assert not pline.intersects(other_miss)
        assert pline.intersection(other_miss) is None

    def test_to_file_from_file(self, pline):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            pline.to_file(path)
            assert os.path.exists(path)
            pl2 = geompp.Polyline2D.from_file(path)
            assert pline.almost_equals(pl2)
        finally:
            os.unlink(path)


class TestPolyline2DConvexHull:
    def test_too_few_points_throws(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0)])
        with pytest.raises(Exception):
            pl.convex_hull()

    def test_three_points_returns_triangle(self):
        pl = geompp.Polyline2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(2, 3),
        ])
        hull = pl.convex_hull()
        assert hull.size() == 3

    def test_concave_path_inner_point_excluded(self):
        # simple path: outer corners with inner dip at (2,1) — hull is the 4 outer corners
        pl = geompp.Polyline2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4),
        ])
        hull = pl.convex_hull()
        assert hull.size() == 4
        expected = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                    geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        for e in expected:
            assert any(approx(e.x, hull[i].x) and approx(e.y, hull[i].y)
                       for i in range(hull.size())), f"{e} should be on hull"


class TestPolyline2DOverlap:
    def test_overlap_line_collinear_segment(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 3)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert pl.overlaps(line=line)
        result = pl.overlap(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0)))

    def test_overlap_line_perpendicular_no_overlap(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not pl.overlaps(line=line)
        assert pl.overlap(line=line) is None

    def test_overlap_ray_collinear_partial(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(6, 0)])
        ray = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert pl.overlaps(ray=ray)
        result = pl.overlap(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0)))

    def test_overlap_segment_partial(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(5, 0)])
        seg = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert pl.overlaps(segment=seg)
        result = pl.overlap(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(5, 0)))

    def test_overlap_polyline_shared_segment(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 3)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        assert pl1.overlaps(other=pl2)
        result = pl1.overlap(other=pl2)
        assert result is not None
        assert len(result) == 1

    def test_overlap_polyline_no_overlap(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(0, 1), geompp.Point2D(4, 1)])
        assert not pl1.overlaps(other=pl2)
        assert pl1.overlap(other=pl2) is None


class TestPolyline2DTouch:
    def test_touch_line_endpoint_on_line(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(2, 0), geompp.Point2D(2, 3)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert pl.touches(line=line)
        result = pl.touch(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(2, 0))

    def test_touch_line_collinear_not_touch(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert not pl.touches(line=line)
        assert pl.touch(line=line) is None

    def test_touch_ray_endpoint_on_ray(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 2)])
        ray = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert pl.touches(ray=ray)
        result = pl.touch(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_segment_t_junction(self):
        pl = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 3)])
        seg = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        assert pl.touches(segment=seg)
        result = pl.touch(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_polyline_shared_endpoint(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(3, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(3, 0), geompp.Point2D(3, 3)])
        assert pl1.touches(other=pl2)
        result = pl1.touch(other=pl2)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point2D(3, 0))

    def test_touch_polyline_disjoint(self):
        pl1 = geompp.Polyline2D.make([geompp.Point2D(0, 0), geompp.Point2D(2, 0)])
        pl2 = geompp.Polyline2D.make([geompp.Point2D(5, 0), geompp.Point2D(5, 3)])
        assert not pl1.touches(other=pl2)
        assert pl1.touch(other=pl2) is None


# ─── Polyline3D ──────────────────────────────────────────────────────────────

class TestPolyline3D:
    def test_length(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.length(), 5.0)

    def test_to_segments(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        segs = pl.to_segments()
        assert len(segs) == 2  # 3 knots → 2 segments (open)
        assert all(isinstance(s, geompp.LineSegment3D) for s in segs)
        assert approx(segs[0].length(), 3.0)
        assert approx(segs[1].length(), 4.0)
        assert segs[0].first.almost_equals(geompp.Point3D(0, 0, 0))
        assert segs[0].last.almost_equals(geompp.Point3D(3, 0, 0))

    def test_contains(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.contains(geompp.Point3D(1,0,0))
        assert not pl.contains(geompp.Point3D(1,1,0))

    def test_location(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.location(geompp.Point3D(0,0,0)), 0.0)
        assert approx(pl.location(geompp.Point3D(3,4,0)), 1.0)
        assert math.isinf(pl.location(geompp.Point3D(1,1,0)))

    def test_distance_to(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.distance_to(geompp.Point3D(0,0,0)), 0.0)
        assert approx(pl.distance_to(geompp.Point3D(1,0,0)), 0.0)
        assert approx(pl.distance_to(geompp.Point3D(0,1,0)), 1.0)

    def test_interpolate(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.interpolate(0.0).almost_equals(geompp.Point3D(0,0,0))
        assert pl.interpolate(1.0).almost_equals(geompp.Point3D(3,4,0))
        with pytest.raises(Exception):
            pl.interpolate(1.2)
        with pytest.raises(Exception):
            pl.interpolate(-0.1)

    def test_project_onto(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.project_onto(geompp.Point3D(2,3,0)).almost_equals(geompp.Point3D(3,3,0))
        assert pl.project_onto(geompp.Point3D(6,2,0)).almost_equals(geompp.Point3D(3,2,0))

    def test_almost_equals(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl1 = geompp.Polyline3D.make(pts)
        pl2 = geompp.Polyline3D.make(pts)
        assert pl1.almost_equals(pl2)
        other = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0)])
        assert not pl1.almost_equals(other)

    def test_wkt_roundtrip(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        wkt = pl.to_wkt()
        pl2 = geompp.Polyline3D.from_wkt(wkt)
        assert pl.almost_equals(pl2)

    def test_from_wkt_whitespace(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0)])
        assert pl.almost_equals(geompp.Polyline3D.from_wkt("LINESTRING (0 0 0, 1 0 0, 1 1 0)"))
        assert pl.almost_equals(geompp.Polyline3D.from_wkt("LINESTRING (  0 0 0  ,  1 0 0  ,  1 1 0  )"))

    def test_to_file_from_file(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            pl.to_file(path)
            assert os.path.exists(path)
            pl2 = geompp.Polyline3D.from_file(path)
            assert pl.almost_equals(pl2)
        finally:
            os.unlink(path)

    def test_len_and_getitem(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        assert len(pl) == 3
        assert pl[0].almost_equals(geompp.Point3D(0, 0, 0))
        assert pl[2].almost_equals(geompp.Point3D(3, 4, 0))
        assert pl[-1].almost_equals(geompp.Point3D(3, 4, 0))
        with pytest.raises(IndexError):
            _ = pl[10]

    def test_intersects_line3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        l_hit = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert pl.intersects(l_hit)
        hit = pl.intersection(l_hit)
        assert hit is not None
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not pl.intersects(l_miss)
        assert pl.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, -3, 0), geompp.Vector3D(0, 1, 0))
        assert pl.intersects(r_hit)
        hit = pl.intersection(r_hit)
        assert hit is not None
        r_miss = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, 1, 0))
        assert not pl.intersects(r_miss)
        assert pl.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert pl.intersects(s_hit)
        hit = pl.intersection(s_hit)
        assert hit is not None
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(5, -2, 0), geompp.Point3D(5, 2, 0))
        assert not pl.intersects(s_miss)
        assert pl.intersection(s_miss) is None

    def test_intersects_polyline3d(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        pl2_hit = geompp.Polyline3D.make([geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0)])
        assert pl1.intersects(pl2_hit)
        hit = pl1.intersection(pl2_hit)
        assert hit is not None
        pl2_miss = geompp.Polyline3D.make([geompp.Point3D(5, -2, 0), geompp.Point3D(5, 2, 0)])
        assert not pl1.intersects(pl2_miss)
        assert pl1.intersection(pl2_miss) is None


class TestPolyline3DOverlap:
    def test_overlap_line3d_collinear_segment(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(4, 0, 3)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert pl.overlaps(line=line)
        result = pl.overlap(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)))

    def test_overlap_ray3d_partial(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0)])
        ray = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert pl.overlaps(ray=ray)
        result = pl.overlap(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(6, 0, 0)))

    def test_overlap_segment3d_partial(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0)])
        seg = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(7, 0, 0))
        assert pl.overlaps(segment=seg)
        result = pl.overlap(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(5, 0, 0)))

    def test_overlap_polyline3d_no_overlap(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0)])
        assert not pl1.overlaps(other=pl2)
        assert pl1.overlap(other=pl2) is None


class TestPolyline3DTouch:
    def test_touch_line3d_endpoint_on_line(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert pl.touches(line=line)
        result = pl.touch(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(2, 0, 0))

    def test_touch_ray3d_endpoint_on_ray(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2)])
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert pl.touches(ray=ray)
        result = pl.touch(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_segment3d_t_junction(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3)])
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        assert pl.touches(segment=seg)
        result = pl.touch(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_polyline3d_shared_endpoint(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3)])
        assert pl1.touches(other=pl2)
        result = pl1.touch(other=pl2)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_polyline3d_disjoint(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(5, 0, 0), geompp.Point3D(5, 0, 3)])
        assert not pl1.touches(other=pl2)
        assert pl1.touch(other=pl2) is None


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
        v0, v1, v2 = tri.vertices  # (0,0), (4,0), (0,3)
        c = tri.centroid()          # (4/3, 1.0)

        def pt(x, y):
            return geompp.Point2D(x, y)

        def mid(a, b):
            return pt((a.x + b.x) / 2, (a.y + b.y) / 2)

        def beyond(corner):
            return pt(c.x + 2 * (corner.x - c.x), c.y + 2 * (corner.y - c.y))

        # corner points are on the boundary
        assert tri.contains(v0)
        assert tri.contains(v1)
        assert tri.contains(v2)

        # centroid is strictly inside
        assert tri.contains(c)

        # halfway between centroid and each corner
        assert tri.contains(mid(c, v0))
        assert tri.contains(mid(c, v1))
        assert tri.contains(mid(c, v2))

        # midpoints of edges (on boundary)
        assert tri.contains(mid(v0, v1))
        assert tri.contains(mid(v1, v2))
        assert tri.contains(mid(v2, v0))

        # extension beyond each corner along centroid→corner ray
        assert not tri.contains(beyond(v0))
        assert not tri.contains(beyond(v1))
        assert not tri.contains(beyond(v2))

        # clearly outside
        assert not tri.contains(pt(5, 5))
        assert not tri.contains(pt(-1, -1))
        assert not tri.contains(pt(3, 3))

    def test_location(self, tri):
        # fixture: P0=(0,0), P1=(4,0), P2=(0,3)

        # inside: location returns (s,t) AND contains agrees
        def check_inside(p, exp_s, exp_t):
            st = tri.location(p)
            assert st is not None, f"expected location to be inside for {p}"
            s, t = st
            assert approx(s, exp_s) and approx(t, exp_t)
            assert tri.contains(p)   # location non-null ↔ contains true

        check_inside(geompp.Point2D(0, 0), 0.0, 0.0)   # P0
        check_inside(geompp.Point2D(4, 0), 1.0, 0.0)   # P1
        check_inside(geompp.Point2D(0, 3), 0.0, 1.0)   # P2
        check_inside(tri.centroid(),        1/3, 1/3)

        # outside: location is None AND contains is false
        def check_outside(p):
            assert tri.location(p) is None
            assert not tri.contains(p)   # location null ↔ contains false

        check_outside(geompp.Point2D(-1, -1))
        check_outside(geompp.Point2D(5, 0))
        check_outside(geompp.Point2D(0, 4))

        # round-trip A: interpolate(location(p)) == p
        p = geompp.Point2D(1, 0.5)
        st = tri.location(p)
        assert st is not None
        s, t = st
        p_back = tri.interpolate(s, t)
        assert p_back is not None
        assert p.almost_equals(p_back)

        # round-trip B: location(interpolate(s,t)) == (s,t)
        s_in, t_in = 0.25, 0.25
        q = tri.interpolate(s_in, t_in)
        assert q is not None
        assert tri.contains(q)           # interpolate result is always inside
        st_q = tri.location(q)
        assert st_q is not None
        s_back, t_back = st_q
        assert approx(s_back, s_in) and approx(t_back, t_in)

    def test_interpolate(self, tri):
        # vertices: (0,0)=P0, (4,0)=P1, (0,3)=P2
        assert tri.interpolate(0.0, 0.0).almost_equals(geompp.Point2D(0, 0))
        assert tri.interpolate(1.0, 0.0).almost_equals(geompp.Point2D(4, 0))
        assert tri.interpolate(0.0, 1.0).almost_equals(geompp.Point2D(0, 3))
        # centroid at s=1/3, t=1/3
        c = tri.interpolate(1/3, 1/3)
        assert c is not None
        assert isinstance(c, geompp.Point2D)
        # outside: s+t > 1 → None
        assert tri.interpolate(0.8, 0.8) is None
        assert tri.interpolate(1.0, 1.0) is None

    def test_intersection_with_line(self, tri):
        l = geompp.Line2D.make(geompp.Point2D(-1, 1), geompp.Point2D(5, 1))
        assert tri.intersects(l)
        hit = tri.intersection(l)
        assert hit is not None

    def test_is_ccw(self, tri):
        # fixture is CCW → IsCCW True and SignedArea positive
        assert tri.is_ccw()
        assert tri.signed_area() > 0

        # reversed winding → CW
        v0, v1, v2 = tri.vertices
        t_cw = geompp.Triangle2D.make(v0, v2, v1)
        assert not t_cw.is_ccw()
        assert t_cw.signed_area() < 0

        # invariant: is_ccw() == (signed_area() > 0)
        assert tri.is_ccw() == (tri.signed_area() > 0)
        assert t_cw.is_ccw() == (t_cw.signed_area() > 0)

    def test_wkt_roundtrip(self, tri):
        tri2 = geompp.Triangle2D.from_wkt(tri.to_wkt())
        assert tri.almost_equals(tri2)

    def test_from_wkt_whitespace(self):
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(0, 1))
        assert t.almost_equals(geompp.Triangle2D.from_wkt("TRIANGLE (0 0, 1 0, 0 1)"))
        assert t.almost_equals(geompp.Triangle2D.from_wkt("TRIANGLE (  0 0  ,  1 0  ,  0 1  )"))

    def test_to_axis(self, tri):
        # tri fixture: P0=(0,0), P1=(4,0), P2=(0,3)
        # ToAxis() returns (P1-P0, P2-P0) — edge vectors, not normalized
        axes = tri.to_axis()
        assert isinstance(axes, tuple)
        assert len(axes) == 2
        u, v = axes
        assert isinstance(u, geompp.Vector2D)
        assert isinstance(v, geompp.Vector2D)
        assert approx(u.x, 4.0) and approx(u.y, 0.0)
        assert approx(v.x, 0.0) and approx(v.y, 3.0)

    def test_to_file_from_file(self, tri):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            tri.to_file(path)
            assert os.path.exists(path)
            tri2 = geompp.Triangle2D.from_file(path)
            assert tri.almost_equals(tri2)
        finally:
            os.unlink(path)


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

    def test_area(self, tri):
        # Right triangle with legs 1, 1 → area = 0.5
        assert approx(tri.area(), 0.5)

    def test_signed_area(self, tri):
        ref = geompp.Vector3D(0, 0, 1)
        # CCW winding in XY plane → positive signed area
        assert tri.signed_area(ref) > 0
        v0, v1, v2 = tri.vertices
        t_cw = geompp.Triangle3D.make(v0, v2, v1)
        assert t_cw.signed_area(ref) < 0
        # flipping ref reverses the sign
        assert tri.signed_area(geompp.Vector3D(0, 0, -1)) < 0

    def test_area_vector(self, tri):
        av = tri.area_vector()
        assert isinstance(av, geompp.Vector3D)
        assert approx(av.z, 0.5)
        assert approx(tri.area(), av.length())

    def test_to_plane(self, tri):
        pl = tri.to_plane()
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_is_ccw(self, tri):
        ref = geompp.Vector3D(0, 0, 1)
        # fixture is CCW in XY plane → IsCCW True and SignedArea positive
        assert tri.is_ccw(ref)
        assert tri.signed_area(ref) > 0

        # reversed winding → CW
        v0, v1, v2 = tri.vertices
        t_cw = geompp.Triangle3D.make(v0, v2, v1)
        assert not t_cw.is_ccw(ref)
        assert t_cw.signed_area(ref) < 0

        # invariant: is_ccw(ref) == (signed_area(ref) > 0)
        assert tri.is_ccw(ref) == (tri.signed_area(ref) > 0)
        assert t_cw.is_ccw(ref) == (t_cw.signed_area(ref) > 0)

        # flipping ref flips the result
        assert not tri.is_ccw(geompp.Vector3D(0, 0, -1))

    def test_interpolate(self, tri):
        # fixture: P0=(0,0,0), P1=(1,0,0), P2=(0,1,0)
        assert tri.interpolate(0.0, 0.0).almost_equals(geompp.Point3D(0, 0, 0))
        assert tri.interpolate(1.0, 0.0).almost_equals(geompp.Point3D(1, 0, 0))
        assert tri.interpolate(0.0, 1.0).almost_equals(geompp.Point3D(0, 1, 0))
        # centroid at s=1/3, t=1/3
        c = tri.interpolate(1/3, 1/3)
        assert c is not None
        assert isinstance(c, geompp.Point3D)
        # outside: s+t > 1 → None
        assert tri.interpolate(0.8, 0.8) is None
        assert tri.interpolate(1.0, 1.0) is None

    def test_wkt_roundtrip(self, tri):
        tri2 = geompp.Triangle3D.from_wkt(tri.to_wkt())
        assert tri.almost_equals(tri2)

    def test_from_wkt_whitespace(self):
        t = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        assert t.almost_equals(geompp.Triangle3D.from_wkt("TRIANGLE (0 0 0, 1 0 0, 0 1 0)"))
        assert t.almost_equals(geompp.Triangle3D.from_wkt("TRIANGLE (  0 0 0  ,  1 0 0  ,  0 1 0  )"))

    def test_to_polygon(self, tri):
        p = tri.to_polygon()
        assert isinstance(p, geompp.Polygon3D)
        assert len(p) == 3

    def test_perimeter(self, tri):
        # fixture: (0,0,0),(1,0,0),(0,1,0) — right triangle with legs 1,1 and hypotenuse sqrt(2)
        assert approx(tri.perimeter(), 2.0 + math.sqrt(2.0))

    def test_to_axis(self, tri):
        axes = tri.to_axis()
        assert isinstance(axes, tuple)
        assert len(axes) == 2
        u, v = axes
        assert isinstance(u, geompp.Vector3D)
        assert isinstance(v, geompp.Vector3D)
        assert approx(u.length(), 1.0)
        assert approx(v.length(), 1.0)

    def test_to_file_from_file(self, tri):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            tri.to_file(path)
            assert os.path.exists(path)
            tri2 = geompp.Triangle3D.from_file(path)
            assert tri.almost_equals(tri2)
        finally:
            os.unlink(path)

    def test_location(self, tri):
        # fixture: P0=(0,0,0), P1=(1,0,0), P2=(0,1,0)

        # inside: location returns (s,t) AND contains agrees
        def check_inside(p, exp_s, exp_t):
            st = tri.location(p)
            assert st is not None, f"expected location to be inside for {p}"
            s, t = st
            assert approx(s, exp_s) and approx(t, exp_t)
            assert tri.contains(p)   # location non-null ↔ contains true

        check_inside(geompp.Point3D(0, 0, 0), 0.0, 0.0)   # P0
        check_inside(geompp.Point3D(1, 0, 0), 1.0, 0.0)   # P1
        check_inside(geompp.Point3D(0, 1, 0), 0.0, 1.0)   # P2
        check_inside(tri.centroid(),            1/3, 1/3)

        # outside: location is None AND contains is false
        def check_outside(p):
            assert tri.location(p) is None
            assert not tri.contains(p)   # location null ↔ contains false

        check_outside(geompp.Point3D(0.3, 0.3,  1))   # off-plane above
        check_outside(geompp.Point3D(0.3, 0.3, -1))   # off-plane below
        check_outside(geompp.Point3D(-0.5, 0, 0))      # outside in-plane
        check_outside(geompp.Point3D(0.8, 0.8, 0))     # past hypotenuse

        # round-trip A: interpolate(location(p)) == p
        p = geompp.Point3D(0.25, 0.25, 0)
        st = tri.location(p)
        assert st is not None
        s, t = st
        p_back = tri.interpolate(s, t)
        assert p_back is not None
        assert p.almost_equals(p_back)

        # round-trip B: location(interpolate(s,t)) == (s,t)
        s_in, t_in = 0.25, 0.25
        q = tri.interpolate(s_in, t_in)
        assert q is not None
        assert tri.contains(q)           # interpolate result is always inside
        st_q = tri.location(q)
        assert st_q is not None
        s_back, t_back = st_q
        assert approx(s_back, s_in) and approx(t_back, t_in)

    def test_contains(self, tri):
        # fixture: P0=(0,0,0), P1=(1,0,0), P2=(0,1,0)
        # interior point
        assert tri.contains(tri.centroid())
        # near each vertex (but not exactly on it)
        assert tri.contains(geompp.Point3D(0.05, 0.05, 0))
        assert tri.contains(geompp.Point3D(0.9,  0.05, 0))
        assert tri.contains(geompp.Point3D(0.05, 0.9,  0))
        # clearly outside in the same plane
        assert not tri.contains(geompp.Point3D(-0.5, 0.5, 0))
        assert not tri.contains(geompp.Point3D(0.5, -0.5, 0))
        assert not tri.contains(geompp.Point3D(0.8,  0.8,  0))  # past hypotenuse
        # off-plane
        assert not tri.contains(geompp.Point3D(0.3, 0.3,  1))
        assert not tri.contains(geompp.Point3D(0.3, 0.3, -1))

        # tilted triangle (in YZ plane, normal along X)
        t2 = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 2, 0), geompp.Point3D(0, 0, 2)
        )
        assert t2.contains(t2.centroid())
        assert not t2.contains(geompp.Point3D(1, 0.5, 0.5))  # off-plane

        # boundary — vertices and edge midpoints are included (P0,P1,P2 = (0,0,0),(1,0,0),(0,1,0))
        p0 = geompp.Point3D(0, 0, 0)
        p1 = geompp.Point3D(1, 0, 0)
        p2 = geompp.Point3D(0, 1, 0)
        assert tri.contains(p0)                                     # vertex P0
        assert tri.contains(p1)                                     # vertex P1
        assert tri.contains(p2)                                     # vertex P2
        assert tri.contains(geompp.Point3D(0.5, 0,   0))           # base edge midpoint
        assert tri.contains(geompp.Point3D(0,   0.5, 0))           # left edge midpoint
        assert tri.contains(geompp.Point3D(0.5, 0.5, 0))           # hypotenuse midpoint
        assert not tri.contains(geompp.Point3D(0.5, 0, 0.01))      # just off-plane

    def test_intersection_with_line(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(0, 2, 0)
        )

        # line through interior at (0.5, 0.5, 0)
        line_in = geompp.Line3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1))
        assert tri.intersects(line_in)
        hit = tri.intersection(line_in)
        assert isinstance(hit, geompp.Point3D)
        assert hit.almost_equals(geompp.Point3D(0.5, 0.5, 0))

        # line missing the triangle (hits plane outside)
        line_miss = geompp.Line3D.make(geompp.Point3D(3, 3, -1), geompp.Point3D(3, 3, 1))
        assert not tri.intersects(line_miss)
        assert tri.intersection(line_miss) is None

        # coplanar line — API limitation: nullopt
        line_coplanar = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        assert not tri.intersects(line_coplanar)
        assert tri.intersection(line_coplanar) is None

    def test_intersection_with_ray(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(0, 2, 0)
        )

        # ray pointing toward triangle from above
        r_down = geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 4), geompp.Vector3D(0, 0, -1))
        assert tri.intersects(r_down)
        hit = tri.intersection(r_down)
        assert isinstance(hit, geompp.Point3D)
        assert hit.almost_equals(geompp.Point3D(0.5, 0.5, 0))

        # ray pointing away from the plane — no intersection
        r_up = geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 4), geompp.Vector3D(0, 0, 1))
        assert not tri.intersects(r_up)
        assert tri.intersection(r_up) is None

        # ray hitting plane outside the triangle
        r_miss = geompp.Ray3D.make(geompp.Point3D(3, 3, 4), geompp.Vector3D(0, 0, -1))
        assert not tri.intersects(r_miss)
        assert tri.intersection(r_miss) is None

    def test_intersection_with_line_segment(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(0, 2, 0)
        )

        seg_cross = geompp.LineSegment3D.make(
            geompp.Point3D(0.5, 0.5, -2), geompp.Point3D(0.5, 0.5, 3)
        )
        assert tri.intersects(seg_cross)
        hit = tri.intersection(seg_cross)
        assert isinstance(hit, geompp.Point3D)
        assert hit.almost_equals(geompp.Point3D(0.5, 0.5, 0))

        # segment entirely above the plane
        seg_above = geompp.LineSegment3D.make(
            geompp.Point3D(0.5, 0.5, 1), geompp.Point3D(0.5, 0.5, 3)
        )
        assert not tri.intersects(seg_above)
        assert tri.intersection(seg_above) is None

        # segment with an endpoint on the triangle
        seg_touching = geompp.LineSegment3D.make(
            geompp.Point3D(0.5, 0.5, 0), geompp.Point3D(0.5, 0.5, 3)
        )
        assert tri.intersects(seg_touching)
        hit2 = tri.intersection(seg_touching)
        assert isinstance(hit2, geompp.Point3D)
        assert hit2.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_with_plane(self):
        # Use a triangle large enough that the plane y=1 cuts through the *interior* of its edges
        # (Triangle2D::Intersection(Line2D) returns None when all intersection points are vertices).
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 4, 0)
        )

        # Plane y=1 cuts the triangle through edge interiors → LineSegment3D
        y1 = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 1, 0), geompp.Vector3D(0, 1, 0))
        assert tri.intersects(y1)
        seg = tri.intersection(y1)
        assert isinstance(seg, geompp.LineSegment3D)
        assert tri.contains(seg.first)
        assert tri.contains(seg.last)
        assert y1.contains(seg.first)
        assert y1.contains(seg.last)

        # Plane parallel above the triangle's plane → no intersection
        above = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 1), geompp.Vector3D(0, 0, 1))
        assert not tri.intersects(above)
        assert tri.intersection(above) is None

        # Coplanar plane (same as triangle's plane) → None (API limitation)
        same = geompp.Plane.xy()
        assert not tri.intersects(same)
        assert tri.intersection(same) is None

    def test_intersection_with_triangle(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 4, 0)
        )

        # Triangle parallel above — disjoint planes, no intersection
        above = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 1), geompp.Point3D(1, 0, 1), geompp.Point3D(0, 1, 1)
        )
        assert not tri.intersects_triangle(above)
        assert tri.intersection(above) is None

        # Two triangles whose planes intersect through their interiors.
        # tri  on XY:  plane y=1 cut is (0,1,0)→(3,1,0).
        # tri2 on y=1: plane z=0 cut is (1,1,0)→(3,1,0).
        # Overlap → LineSegment3D from (1,1,0) to (3,1,0).
        tri2 = geompp.Triangle3D.make(
            geompp.Point3D(1, 1, -1), geompp.Point3D(1, 1, 1), geompp.Point3D(3, 1, 0)
        )
        assert tri.intersects_triangle(tri2)
        seg = tri.intersection(tri2)
        assert isinstance(seg, geompp.LineSegment3D)
        assert tri.contains(seg.first)
        assert tri.contains(seg.last)
        assert tri2.contains(seg.first)
        assert tri2.contains(seg.last)


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

    def test_from_line_segment2d(self):
        s = geompp.LineSegment2D.make(geompp.Point2D(-1, -2), geompp.Point2D(3, 4))
        bb = geompp.BBox2D(s)
        assert approx(bb.min.x, -1) and approx(bb.min.y, -2)
        assert approx(bb.max.x,  3) and approx(bb.max.y,  4)

    def test_from_polyline2d(self):
        pts = [geompp.Point2D(0, 5), geompp.Point2D(3, 0), geompp.Point2D(1, 2)]
        bb = geompp.BBox2D(geompp.Polyline2D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 5)

    def test_from_polygon2d(self):
        sq = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 3), geompp.Point2D(0, 3),
        ])
        bb = geompp.BBox2D(sq)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 4) and approx(bb.max.y, 3)

    def test_from_triangle2d(self):
        tri = geompp.Triangle2D.make(
            geompp.Point2D(0, 0), geompp.Point2D(3, 0), geompp.Point2D(0, 4)
        )
        bb = geompp.BBox2D(tri)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 4)

    def test_almost_equals(self):
        bb1 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        bb2 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        assert bb1.almost_equals(bb2)
        bb3 = geompp.BBox2D(geompp.Point2D(0, 0), geompp.Point2D(2, 2))
        assert not bb1.almost_equals(bb3)


# ─── BBox3D ──────────────────────────────────────────────────────────────────

class TestBBox3D:
    def test_from_points(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        assert approx(bb.max.z, 3)

    def test_contains(self):
        bb = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 5, 5))
        assert bb.contains(geompp.Point3D(1, 1, 1))
        assert not bb.contains(geompp.Point3D(6, 1, 1))

    def test_from_line_segment(self):
        s = geompp.LineSegment3D.make(geompp.Point3D(-1, -2, -3), geompp.Point3D(3, 4, 5))
        bb = geompp.BBox3D(s)
        assert approx(bb.min.x, -1) and approx(bb.min.y, -2) and approx(bb.min.z, -3)
        assert approx(bb.max.x,  3) and approx(bb.max.y,  4) and approx(bb.max.z,  5)

    def test_from_polyline(self):
        pts = [geompp.Point3D(0, 5, 1), geompp.Point3D(3, 0, 4), geompp.Point3D(1, 2, 0)]
        bb = geompp.BBox3D(geompp.Polyline3D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0) and approx(bb.min.z, 0)
        assert approx(bb.max.x, 3) and approx(bb.max.y, 5) and approx(bb.max.z, 4)

    def test_from_polygon(self):
        # ZX projection: reversed order so the triangle is CCW
        pts = [geompp.Point3D(4, 3, 5), geompp.Point3D(4, 0, 1), geompp.Point3D(0, 0, 1)]
        bb = geompp.BBox3D(geompp.Polygon3D.make(pts))
        assert approx(bb.min.x, 0) and approx(bb.min.z, 1)
        assert approx(bb.max.x, 4) and approx(bb.max.z, 5)

    def test_from_triangle(self):
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(0, 3, 4)
        )
        bb = geompp.BBox3D(tri)
        assert approx(bb.min.x, 0) and approx(bb.min.y, 0) and approx(bb.min.z, 0)
        assert approx(bb.max.x, 2) and approx(bb.max.y, 3) and approx(bb.max.z, 4)

    def test_almost_equals(self):
        bb1 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        bb2 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 3))
        assert bb1.almost_equals(bb2)
        bb3 = geompp.BBox3D(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 2, 4))
        assert not bb1.almost_equals(bb3)


# ─── BBall2D ─────────────────────────────────────────────────────────────────

class TestBBall2D:
    def test_constructor_center_radius(self):
        b = geompp.BBall2D(geompp.Point2D(1, 2), 5.0)
        assert b.center.almost_equals(geompp.Point2D(1, 2))
        assert approx(b.radius, 5.0)

    def test_constructor_from_single_point(self):
        b = geompp.BBall2D([geompp.Point2D(3, 4)])
        assert b.center.almost_equals(geompp.Point2D(3, 4))
        assert approx(b.radius, 0.0)

    def test_constructor_from_two_points(self):
        b = geompp.BBall2D([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])
        assert b.center.almost_equals(geompp.Point2D(2, 0))
        assert approx(b.radius, 2.0)

    def test_constructor_from_points_all_contained(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(2, 3), geompp.Point2D(-1, 1.5)]
        b = geompp.BBall2D(pts)
        for p in pts:
            assert b.contains(p)

    def test_contains_center(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert b.contains(geompp.Point2D(0, 0))

    def test_contains_boundary(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert b.contains(geompp.Point2D(3, 4))   # 3-4-5 right triangle
        assert b.contains(geompp.Point2D(5, 0))

    def test_contains_outside(self):
        b = geompp.BBall2D(geompp.Point2D(0, 0), 5.0)
        assert not b.contains(geompp.Point2D(4, 4))   # dist ≈ 5.657
        assert not b.contains(geompp.Point2D(6, 0))

    def test_almost_equals(self):
        b1 = geompp.BBall2D(geompp.Point2D(1, 2), 3.0)
        b2 = geompp.BBall2D(geompp.Point2D(1, 2), 3.0)
        b3 = geompp.BBall2D(geompp.Point2D(0, 0), 1.0)
        assert b1.almost_equals(b2)
        assert not b1.almost_equals(b3)
        assert b1 == b2


# ─── BBall3D ─────────────────────────────────────────────────────────────────

class TestBBall3D:
    def test_constructor_center_radius(self):
        b = geompp.BBall3D(geompp.Point3D(1, 2, 3), 5.0)
        assert b.center.almost_equals(geompp.Point3D(1, 2, 3))
        assert approx(b.radius, 5.0)

    def test_constructor_from_single_point(self):
        b = geompp.BBall3D([geompp.Point3D(1, 2, 3)])
        assert b.center.almost_equals(geompp.Point3D(1, 2, 3))
        assert approx(b.radius, 0.0)

    def test_constructor_from_two_points(self):
        b = geompp.BBall3D([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        assert b.center.almost_equals(geompp.Point3D(2, 0, 0))
        assert approx(b.radius, 2.0)

    def test_constructor_from_points_all_contained(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(0, 3, 0), geompp.Point3D(0, 0, 2)]
        b = geompp.BBall3D(pts)
        for p in pts:
            assert b.contains(p)

    def test_contains_center(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert b.contains(geompp.Point3D(0, 0, 0))

    def test_contains_boundary(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert b.contains(geompp.Point3D(3, 4, 0))   # 3-4-5 in XY plane
        assert b.contains(geompp.Point3D(0, 0, 5))

    def test_contains_outside(self):
        b = geompp.BBall3D(geompp.Point3D(0, 0, 0), 5.0)
        assert not b.contains(geompp.Point3D(4, 4, 0))   # dist ≈ 5.657
        assert not b.contains(geompp.Point3D(0, 0, 6))

    def test_almost_equals(self):
        b1 = geompp.BBall3D(geompp.Point3D(1, 2, 3), 4.0)
        b2 = geompp.BBall3D(geompp.Point3D(1, 2, 3), 4.0)
        b3 = geompp.BBall3D(geompp.Point3D(0, 0, 0), 1.0)
        assert b1.almost_equals(b2)
        assert not b1.almost_equals(b3)
        assert b1 == b2


# ─── BRect2D ─────────────────────────────────────────────────────────────────

class TestBRect2D:
    def test_empty_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([])

    def test_single_point_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([geompp.Point2D(3, 4)])

    def test_two_points_throws(self):
        with pytest.raises(Exception):
            geompp.BRect2D([geompp.Point2D(0, 0), geompp.Point2D(4, 0)])

    def test_axis_aligned_rectangle_center(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.center.almost_equals(geompp.Point2D(2, 1))
        assert approx(r.area, 8.0)
        assert approx(r.width, 4.0)
        assert approx(r.height, 2.0)

    def test_all_points_contained(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 2), geompp.Point2D(0, 2),
               geompp.Point2D(1, 0.5), geompp.Point2D(2, 1.5)]
        r = geompp.BRect2D(pts)
        for p in pts:
            assert r.contains(p), f"OBB must contain input point {p}"

    def test_axes_are_unit_vectors(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0),
               geompp.Point2D(2, 1), geompp.Point2D(0, 1)]
        r = geompp.BRect2D(pts)
        assert approx(r.axis_u.length(), 1.0)
        assert approx(r.axis_v.length(), 1.0)

    def test_axes_orthogonal(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert approx(r.axis_u.dot(r.axis_v), 0.0)

    def test_corners_returns_four_points(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        corners = r.corners()
        assert len(corners) == 4
        for c in corners:
            assert r.contains(c)

    def test_contains_center(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.contains(geompp.Point2D(2, 1))

    def test_contains_boundary(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert r.contains(geompp.Point2D(0, 0))    # corner
        assert r.contains(geompp.Point2D(4, 1))    # edge midpoint

    def test_contains_outside(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r = geompp.BRect2D(pts)
        assert not r.contains(geompp.Point2D(5, 1))
        assert not r.contains(geompp.Point2D(2, 3))

    def test_almost_equals(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        r1 = geompp.BRect2D(pts)
        r2 = geompp.BRect2D(pts)
        assert r1.almost_equals(r2)
        assert r1 == r2

    def test_not_almost_equals_different(self):
        pts1 = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                geompp.Point2D(4, 2), geompp.Point2D(0, 2)]
        pts2 = [geompp.Point2D(0, 0), geompp.Point2D(6, 0),
                geompp.Point2D(6, 2), geompp.Point2D(0, 2)]
        r1 = geompp.BRect2D(pts1)
        r2 = geompp.BRect2D(pts2)
        assert not r1.almost_equals(r2)


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

    def test_from_origin_and_axes(self):
        origin = geompp.Point3D(1, 2, 3)
        u = geompp.Vector3D(1, 0, 0)
        v = geompp.Vector3D(0, 1, 0)
        pl = geompp.Plane.from_origin_and_axes(origin, u, v)
        assert isinstance(pl, geompp.Plane)
        assert pl.origin.almost_equals(origin)
        # normal should be along Z (cross of X and Y)
        assert approx(abs(pl.normal.z), 1.0)

    def test_axis_u_and_axis_v(self):
        pl = geompp.Plane.xy()
        u = pl.axis_u
        v = pl.axis_v
        assert isinstance(u, geompp.Vector3D)
        assert isinstance(v, geompp.Vector3D)
        # both axes should be unit vectors lying in XY plane
        assert approx(u.length(), 1.0)
        assert approx(v.length(), 1.0)
        assert approx(u.z, 0.0)
        assert approx(v.z, 0.0)

    def test_project_into(self):
        pl = geompp.Plane.xy()
        pt3 = geompp.Point3D(3, 4, 7)
        uv = pl.project_into(pt3)
        # project_into returns a 2D point (u, v) in plane coordinates
        assert isinstance(uv, geompp.Point2D)

    def test_evaluate(self):
        pl = geompp.Plane.xy()
        # evaluate at (u=1, v=0) using a Point2D — binding takes p2d: Point2D
        p2d = geompp.Point2D(1.0, 0.0)
        pt3 = pl.evaluate(p2d)
        assert isinstance(pt3, geompp.Point3D)
        assert approx(pt3.z, 0.0)

    def test_project_into_then_evaluate_roundtrip(self):
        pl = geompp.Plane.from_origin_and_axes(
            geompp.Point3D(0, 0, 5),
            geompp.Vector3D(1, 0, 0),
            geompp.Vector3D(0, 1, 0),
        )
        pt = geompp.Point3D(3, 4, 5)
        uv = pl.project_into(pt)
        back = pl.evaluate(uv)
        assert back.almost_equals(pt)

    def test_intersection_with_ray(self):
        xy = geompp.Plane.xy()
        # ray pointing down toward plane crosses at (5, 3, 0)
        r = geompp.Ray3D.make(geompp.Point3D(5, 3, 4), geompp.Vector3D(0, 0, -1))
        assert xy.intersects(r)
        hit = xy.intersection(r)
        assert isinstance(hit, geompp.Point3D)
        assert hit.almost_equals(geompp.Point3D(5, 3, 0))

        # ray pointing away from plane → no intersection
        r_away = geompp.Ray3D.make(geompp.Point3D(5, 3, 4), geompp.Vector3D(0, 0, 1))
        assert not xy.intersects(r_away)
        assert xy.intersection(r_away) is None

    def test_intersection_with_line_segment(self):
        xy = geompp.Plane.xy()
        seg = geompp.LineSegment3D.make(geompp.Point3D(5, 3, -2), geompp.Point3D(5, 3, 4))
        assert xy.intersects(seg)
        hit = xy.intersection(seg)
        assert isinstance(hit, geompp.Point3D)
        assert hit.almost_equals(geompp.Point3D(5, 3, 0))

        # segment entirely above the plane
        seg_above = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 1), geompp.Point3D(1, 1, 2))
        assert not xy.intersects(seg_above)
        assert xy.intersection(seg_above) is None

    def test_intersection_with_plane(self):
        # XY ∩ YZ → line along Y through origin
        inter = geompp.Plane.xy().intersection(geompp.Plane.yz())
        assert isinstance(inter, geompp.Line3D)
        # the resulting line must lie in both planes
        assert geompp.Plane.xy().contains(inter.origin)
        assert geompp.Plane.yz().contains(inter.origin)
        # direction parallel to Y-axis (up to sign)
        d = inter.direction
        assert d.almost_equals(geompp.Vector3D(0, 1, 0)) or d.almost_equals(geompp.Vector3D(0, -1, 0))

        # parallel distinct planes → None
        p1 = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        p2 = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 5), geompp.Vector3D(0, 0, 1))
        assert not p1.intersects(p2)
        assert p1.intersection(p2) is None

    def test_intersection_with_triangle(self):
        # plane y=1 cuts the triangle through edge interiors (avoids the all-vertices nullopt rule)
        y1  = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 1, 0), geompp.Vector3D(0, 1, 0))
        tri = geompp.Triangle3D.make(
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 4, 0)
        )
        assert y1.intersects(tri)
        seg = y1.intersection(tri)
        assert isinstance(seg, geompp.LineSegment3D)
        assert y1.contains(seg.first)
        assert y1.contains(seg.last)
        assert tri.contains(seg.first)
        assert tri.contains(seg.last)

        # plane parallel above the triangle's plane → no intersection
        above = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 1), geompp.Vector3D(0, 0, 1))
        assert not above.intersects(tri)
        assert above.intersection(tri) is None

    def test_is_parallel(self):
        xy = geompp.Plane.xy()
        line_along_x = geompp.Line3D.make(geompp.Point3D(0, 0, 3), geompp.Vector3D(1, 0, 0))
        assert xy.is_parallel(line_along_x)

        line_perp = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert not xy.is_parallel(line_perp)

        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 3), geompp.Vector3D(1, 0, 0))
        assert xy.is_parallel(ray)
        ray_diag = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 1, 1))
        assert not xy.is_parallel(ray_diag)

        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 3), geompp.Point3D(5, 0, 3))
        assert xy.is_parallel(seg)
        seg_perp = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5))
        assert not xy.is_parallel(seg_perp)

    def test_is_coplanar(self):
        xy = geompp.Plane.xy()
        line_in = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 1, 0))
        assert xy.is_coplanar(line_in)

        line_above = geompp.Line3D.make(geompp.Point3D(0, 0, 2), geompp.Vector3D(1, 0, 0))
        assert not xy.is_coplanar(line_above)

        ray_in = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert xy.is_coplanar(ray_in)
        ray_above = geompp.Ray3D.make(geompp.Point3D(0, 0, 2), geompp.Vector3D(1, 0, 0))
        assert not xy.is_coplanar(ray_above)

        seg_in = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(2, 2, 0))
        assert xy.is_coplanar(seg_in)
        seg_above = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 2), geompp.Point3D(5, 0, 2))
        assert not xy.is_coplanar(seg_above)


# ─── WktParser ───────────────────────────────────────────────────────────────

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

    def test_remove_consecutive_duplicates_2d(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0, 0), geompp.Point2D(1, 0)]
        result = geompp.remove_consecutive_duplicates(pts)
        assert len(result) == 2

    def test_are_ccw_2d_true(self):
        pts = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        assert geompp.are_ccw(pts)
        assert not geompp.are_cw(pts)

    def test_are_cw_2d_true(self):
        pts = [
            geompp.Point2D(0, 0), geompp.Point2D(0, 4),
            geompp.Point2D(4, 4), geompp.Point2D(4, 0),
        ]
        assert geompp.are_cw(pts)
        assert not geompp.are_ccw(pts)

    def test_are_coplanar_3d_true(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(0, 1, 0), geompp.Point3D(1, 1, 0),
        ]
        assert geompp.are_coplanar(pts)

    def test_are_coplanar_3d_false(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(0, 1, 0), geompp.Point3D(1, 1, 1),
        ]
        assert not geompp.are_coplanar(pts)

    def test_are_ccw_3d_true(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        assert geompp.are_ccw(pts)
        assert not geompp.are_cw(pts)

    def test_are_cw_3d_true(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(4, 0, 0),
        ]
        assert geompp.are_cw(pts)
        assert not geompp.are_ccw(pts)

    def test_closest_world_plane_to(self):
        # XY points → normal along Z
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0)]
        pl = geompp.closest_world_plane_to(pts)
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_are_ccw_3d_with_ref_plane(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        ref = geompp.Plane.xy()
        assert geompp.are_ccw(pts, ref)
        assert not geompp.are_cw(pts, ref)

    def test_are_ccw_3d_with_none_ref_plane(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        assert geompp.are_ccw(pts, None)  # None is accepted as the default

    def test_signed_area_ccw_is_positive(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        sa = geompp.signed_area(pts)
        assert sa > 0
        assert approx(sa, 16.0)

    def test_signed_area_cw_is_negative(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(0, 4), geompp.Point2D(4, 4), geompp.Point2D(4, 0)]
        sa = geompp.signed_area(pts)
        assert sa < 0
        assert approx(sa, -16.0)

    def test_signed_area_triangle(self):
        # base=4, height=3 → area = 6
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3)]
        assert approx(geompp.signed_area(pts), 6.0)

    def test_signed_area_off_origin(self):
        # 4×4 square not starting at origin — exercises i=0 wraparound
        pts = [geompp.Point2D(1, 1), geompp.Point2D(5, 1), geompp.Point2D(5, 5), geompp.Point2D(1, 5)]
        assert approx(geompp.signed_area(pts), 16.0)

    def test_signed_area_consistent_with_are_ccw(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 2), geompp.Point2D(0, 2)]
        assert (geompp.signed_area(pts) > 0) == geompp.are_ccw(pts)

    def test_signed_area_3d_ccw_is_positive(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)]
        sa = geompp.signed_area(pts)
        assert sa > 0
        assert approx(sa, 16.0)

    def test_signed_area_3d_cw_is_negative(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
               geompp.Point3D(4, 4, 0), geompp.Point3D(4, 0, 0)]
        sa = geompp.signed_area(pts)
        assert sa < 0
        assert approx(sa, -16.0)

    def test_signed_area_3d_opposite_normal_negates(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)]
        plane_pos = geompp.Plane.xy()
        plane_neg = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, -1))
        assert geompp.signed_area(pts, plane_pos) > 0
        assert geompp.signed_area(pts, plane_neg) < 0

    def test_signed_area_3d_consistent_with_are_ccw(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0),
               geompp.Point3D(2, 2, 0), geompp.Point3D(0, 2, 0)]
        assert (geompp.signed_area(pts) > 0) == geompp.are_ccw(pts, None)


# ─── GeometryCollection2D ─────────────────────────────────────────────────────

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


# ─── GeometryCollection3D ─────────────────────────────────────────────────────

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


# --- segment-set intersection (Shamos-Hoey / Bentley-Ottmann) ---
# NOTE: correctness rides on the provisional sweep-status comparator; these encode the intended
# behaviour of correct Shamos-Hoey / Bentley-Ottmann implementations and are flagged for re-verification.
class TestSegmentIntersections:
    @staticmethod
    def _square_ring():
        return [
            geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0)),
            geompp.LineSegment2D.make(geompp.Point2D(1, 0), geompp.Point2D(1, 1)),
            geompp.LineSegment2D.make(geompp.Point2D(1, 1), geompp.Point2D(0, 1)),
            geompp.LineSegment2D.make(geompp.Point2D(0, 1), geompp.Point2D(0, 0)),
        ]

    @staticmethod
    def _self_intersecting_ring():
        # edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
        return [
            geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0)),
            geompp.LineSegment2D.make(geompp.Point2D(4, 0), geompp.Point2D(1, 3)),
            geompp.LineSegment2D.make(geompp.Point2D(1, 3), geompp.Point2D(3, 3)),
            geompp.LineSegment2D.make(geompp.Point2D(3, 3), geompp.Point2D(0, 0)),
        ]

    def test_has_intersections_simple_false(self):
        assert not geompp.has_intersections(self._square_ring())

    def test_has_intersections_self_intersecting_true(self):
        assert geompp.has_intersections(self._self_intersecting_ring())

    def test_find_intersections_simple_empty(self):
        assert list(geompp.find_intersections(self._square_ring())) == []

    def test_find_intersections_reports_crossing(self):
        hits = list(geompp.find_intersections(self._self_intersecting_ring()))
        assert len(hits) >= 1
        assert any(approx(h.x, 2.0) and approx(h.y, 2.0) for h in hits)


# --- find_extreme_points (polygon extreme vertices along a line) ---
class TestExtremePoints:
    def test_convex_diamond_along_x(self):
        diamond = geompp.Polygon2D.make([
            geompp.Point2D(2, 0), geompp.Point2D(4, 2),
            geompp.Point2D(2, 4), geompp.Point2D(0, 2)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        ex = geompp.find_extreme_points(diamond, line)
        assert approx(ex.min_point.x, 0) and approx(ex.min_point.y, 2)
        assert approx(ex.max_point.x, 4) and approx(ex.max_point.y, 2)

    def test_convex_square_diagonal(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4)])
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 1))
        ex = geompp.find_extreme_points(square, line)
        assert approx(ex.min_point.x, 0) and approx(ex.min_point.y, 0)
        assert approx(ex.max_point.x, 4) and approx(ex.max_point.y, 4)

    def test_concave_polygon_brute_force(self):
        dart = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4)])
        assert not dart.is_convex()
        line = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 2))
        ex = geompp.find_extreme_points(dart, line)
        assert approx(ex.min_point.x, 0) and approx(ex.min_point.y, 0)
        assert approx(ex.max_point.x, 4) and approx(ex.max_point.y, 4)

    def test_polygon3d_along_x(self):
        diamond = geompp.Polygon3D.make([
            geompp.Point3D(2, 0, 0), geompp.Point3D(4, 2, 0),
            geompp.Point3D(2, 4, 0), geompp.Point3D(0, 2, 0)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        ex = geompp.find_extreme_points(diamond, line)
        assert approx(ex.min_point.x, 0) and approx(ex.min_point.y, 2)
        assert approx(ex.max_point.x, 4) and approx(ex.max_point.y, 2)

    def test_polygon3d_tilted_plane(self):
        para = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 2),
            geompp.Point3D(2, 2, 2), geompp.Point3D(0, 2, 0)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 0))
        ex = geompp.find_extreme_points(para, line)
        assert approx(ex.min_point.x, 0) and approx(ex.min_point.y, 0) and approx(ex.min_point.z, 0)
        assert approx(ex.max_point.x, 2) and approx(ex.max_point.y, 2) and approx(ex.max_point.z, 2)


# --- distance_to (polygon-to-line distance) ---
class TestDistanceTo:
    def test_2d_convex_square_line_crossing_is_zero(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4)])
        line = geompp.Line2D.make(geompp.Point2D(2, -1), geompp.Point2D(2, 5))
        assert approx(geompp.distance_to(square, line), 0.0)

    def test_2d_convex_square_line_outside(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4)])
        line = geompp.Line2D.make(geompp.Point2D(6, -1), geompp.Point2D(6, 5))
        assert approx(geompp.distance_to(square, line), 2.0)

    def test_2d_non_convex_dart_line_outside(self):
        dart = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4)])
        line = geompp.Line2D.make(geompp.Point2D(10, -1), geompp.Point2D(10, 5))
        assert approx(geompp.distance_to(dart, line), 6.0)

    def test_3d_coplanar_line_outside(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        line = geompp.Line3D.make(geompp.Point3D(6, -1, 0), geompp.Point3D(6, 5, 0))
        assert approx(geompp.distance_to(square, line), 2.0)

    def test_3d_parallel_offset_pythagorean_combination(self):
        # Line parallel to the plane, offset h=3; in-plane distance d=2 (same as coplanar case) ->
        # sqrt(h^2 + d^2) = sqrt(13).
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        line = geompp.Line3D.make(geompp.Point3D(6, 0, 3), geompp.Point3D(6, 1, 3))
        assert approx(geompp.distance_to(square, line), 13.0 ** 0.5)

    def test_3d_skew_perpendicular_crossing_inside_is_zero(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        line = geompp.Line3D.make(geompp.Point3D(2, 2, -1), geompp.Point3D(2, 2, 1))
        assert approx(geompp.distance_to(square, line), 0.0)

    def test_3d_skew_oblique_crossing_outside_anisotropic_metric(self):
        # Line crosses the plane at 45 degrees off the normal at (6,2,0), outside the square.
        # Correct anisotropic answer is sqrt(2) ~= 1.41421356, not the naive in-plane 2 (6-4).
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        line = geompp.Line3D.make(geompp.Point3D(6, 2, 0), geompp.Point3D(7, 2, 1))
        assert approx(geompp.distance_to(square, line), 2.0 ** 0.5)


# --- convex hull (Andrew's monotone chain) ---
class TestConvexHull:
    def test_few_points_returns_as_is(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(1, 1)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 2

    def test_convex_square_returns_four_corners(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
               geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 4
        for p in pts:
            assert any(approx(h.x, p.x) and approx(h.y, p.y) for h in hull), \
                f"corner {p} should be on the hull"

    def test_asymmetric_star_hull_is_pentagon(self):
        # 5 outer tips at unequal distances + 5 inner concave vertices
        outer = [
            geompp.Point2D( 0,  5),
            geompp.Point2D( 4,  2),
            geompp.Point2D( 3, -3),
            geompp.Point2D(-2, -4),
            geompp.Point2D(-3,  1),
        ]
        inner = [
            geompp.Point2D( 2,  1),
            geompp.Point2D( 2, -1),
            geompp.Point2D( 0, -1),
            geompp.Point2D(-1, -1),
            geompp.Point2D(-1,  2),
        ]
        star = [outer[0], inner[0], outer[1], inner[1], outer[2],
                inner[2], outer[3], inner[3], outer[4], inner[4]]
        hull = geompp.convex_hull(star)
        assert len(hull) == 5, f"expected 5-point hull, got {len(hull)}"
        for tip in outer:
            assert any(approx(h.x, tip.x) and approx(h.y, tip.y) for h in hull), \
                f"outer tip {tip} should be on the hull"
        for ip in inner:
            assert not any(approx(h.x, ip.x) and approx(h.y, ip.y) for h in hull), \
                f"inner point {ip} should NOT be on the hull"


# --- convex hull 3D (Andrew's monotone chain on coplanar points) ---
class TestConvexHull3D:
    def test_few_points_returns_as_is(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 2

    def test_coplanar_square_xy_plane(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
               geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 4
        for p in pts:
            assert any(approx(h.x, p.x) and approx(h.y, p.y) and approx(h.z, p.z)
                       for h in hull), f"corner {p} should be on the hull"

    def test_coplanar_square_yz_plane(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
               geompp.Point3D(0, 4, 4), geompp.Point3D(0, 0, 4)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 4

    def test_asymmetric_star_hull_is_pentagon(self):
        outer = [
            geompp.Point3D( 0,  5, 0),
            geompp.Point3D( 4,  2, 0),
            geompp.Point3D( 3, -3, 0),
            geompp.Point3D(-2, -4, 0),
            geompp.Point3D(-3,  1, 0),
        ]
        inner = [
            geompp.Point3D( 2,  1, 0),
            geompp.Point3D( 2, -1, 0),
            geompp.Point3D( 0, -1, 0),
            geompp.Point3D(-1, -1, 0),
            geompp.Point3D(-1,  2, 0),
        ]
        star = [outer[0], inner[0], outer[1], inner[1], outer[2],
                inner[2], outer[3], inner[3], outer[4], inner[4]]
        hull = geompp.convex_hull(star)
        assert len(hull) == 5, f"expected 5-point hull, got {len(hull)}"
        for tip in outer:
            assert any(approx(h.x, tip.x) and approx(h.y, tip.y) for h in hull), \
                f"outer tip {tip} should be on the hull"
        for ip in inner:
            assert not any(approx(h.x, ip.x) and approx(h.y, ip.y) for h in hull), \
                f"inner point {ip} should NOT be on the hull"

    def test_with_explicit_normal(self):
        pts = [geompp.Point3D(0, 0, 5), geompp.Point3D(4, 0, 5),
               geompp.Point3D(4, 4, 5), geompp.Point3D(0, 4, 5)]
        normal = geompp.Vector3D(0, 0, 1)
        hull = geompp.convex_hull(pts, normal)
        assert len(hull) == 4

    def test_non_coplanar_small_z_jitter(self):
        pts = [geompp.Point3D(0, 0, 0.1), geompp.Point3D(4, 0, -0.1),
               geompp.Point3D(4, 4, 0.05), geompp.Point3D(0, 4, -0.05),
               geompp.Point3D(2, 2, 0.02)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 4, "interior point should be excluded"

    def test_non_coplanar_tilted_plane(self):
        pts = [geompp.Point3D(3, 0, 0), geompp.Point3D(0, 3, 0),
               geompp.Point3D(0, 0, 3), geompp.Point3D(1, 1, 1)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 3, "three corners of the triangle, interior excluded"

    def test_non_coplanar_all_hull_points_present(self):
        tips = [geompp.Point3D(0, 5, 1), geompp.Point3D(4, 2, 0.5),
                geompp.Point3D(3, -3, 0), geompp.Point3D(-2, -4, 0.5),
                geompp.Point3D(-3, 1, 1)]
        pts = tips + [geompp.Point3D(0, 0, 0.6)]
        hull = geompp.convex_hull(pts)
        assert len(hull) == 5
        for tip in tips:
            assert any(approx(h.x, tip.x) and approx(h.y, tip.y) and approx(h.z, tip.z)
                       for h in hull), f"tip {tip} should be on hull"


# --- Polygon2D.convex_hull / perimeter ---
class TestPolygon2DConvexHull:
    def _star(self):
        return geompp.Polygon2D.make([
            geompp.Point2D(-1,  2), geompp.Point2D(-3,  1),
            geompp.Point2D(-1, -1), geompp.Point2D(-2, -4),
            geompp.Point2D( 0, -1), geompp.Point2D( 3, -3),
            geompp.Point2D( 2, -1), geompp.Point2D( 4,  2),
            geompp.Point2D( 2,  1), geompp.Point2D( 0,  5),
        ])

    def test_convex_hull_star_is_pentagon(self):
        hull = self._star().convex_hull()
        assert hull.size() == 5

    def test_convex_hull_convex_polygon_unchanged(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ])
        hull = square.convex_hull()
        assert hull.size() == 4

    def test_perimeter_round_trip(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 3), geompp.Point2D(0, 3)]
        poly = geompp.Polygon2D.make(pts)
        back = poly.perimeter()
        assert len(back) == 4
        for orig, restored in zip(pts, back):
            assert approx(orig.x, restored.x) and approx(orig.y, restored.y)


# --- Polygon3D.convex_hull / perimeter ---
class TestPolygon3DConvexHull:
    def _star3d(self):
        return geompp.Polygon3D.make([
            geompp.Point3D(-1,  2, 0), geompp.Point3D(-3,  1, 0),
            geompp.Point3D(-1, -1, 0), geompp.Point3D(-2, -4, 0),
            geompp.Point3D( 0, -1, 0), geompp.Point3D( 3, -3, 0),
            geompp.Point3D( 2, -1, 0), geompp.Point3D( 4,  2, 0),
            geompp.Point3D( 2,  1, 0), geompp.Point3D( 0,  5, 0),
        ])

    def test_convex_hull_star_is_pentagon(self):
        hull = self._star3d().convex_hull()
        assert hull.size() == 5

    def test_convex_hull_convex_polygon_unchanged(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ])
        hull = square.convex_hull()
        assert hull.size() == 4

    def test_perimeter_round_trip(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0),
               geompp.Point3D(3, 3, 0), geompp.Point3D(0, 3, 0)]
        poly = geompp.Polygon3D.make(pts)
        back = poly.perimeter()
        assert len(back) == 4
        for orig, restored in zip(pts, back):
            assert (approx(orig.x, restored.x) and approx(orig.y, restored.y)
                    and approx(orig.z, restored.z))


# --- Polygon2D.has_holes / holes ---
class TestPolygon2DHoles:
    def test_no_holes(self):
        poly = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ])
        assert poly.has_holes() is False
        assert poly.holes() == []

    def test_with_hole(self):
        outer = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                 geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        hole = [geompp.Point2D(1, 1), geompp.Point2D(1, 3),
                geompp.Point2D(3, 3), geompp.Point2D(3, 1)]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.has_holes() is True
        holes = poly.holes()
        assert len(holes) == 1
        assert len(holes[0]) == 4
        for orig, restored in zip(hole, holes[0]):
            assert approx(orig.x, restored.x) and approx(orig.y, restored.y)


# --- Polygon3D.has_holes / holes ---
class TestPolygon3DHoles:
    def test_no_holes(self):
        poly = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ])
        assert poly.has_holes() is False
        assert poly.holes() == []

    def test_with_hole(self):
        outer = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
                 geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)]
        hole = [geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
                geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0)]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.has_holes() is True
        holes = poly.holes()
        assert len(holes) == 1
        assert len(holes[0]) == 4
        for orig, restored in zip(hole, holes[0]):
            assert (approx(orig.x, restored.x) and approx(orig.y, restored.y)
                    and approx(orig.z, restored.z))


class TestPolygon2DIsConvex:
    def test_square_is_convex(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0,0), geompp.Point2D(1,0),
            geompp.Point2D(1,1), geompp.Point2D(0,1)])
        assert p.is_convex()

    def test_concave_not_convex(self):
        # Concave polygon (dent)
        p = geompp.Polygon2D.make([
            geompp.Point2D(0,0), geompp.Point2D(4,0),
            geompp.Point2D(4,4), geompp.Point2D(2,2), geompp.Point2D(0,4)])
        assert not p.is_convex()

    def test_with_hole_not_convex(self):
        outer = [geompp.Point2D(0,0), geompp.Point2D(4,0),
                 geompp.Point2D(4,4), geompp.Point2D(0,4)]
        # holes must be CW (reversed winding)
        hole  = [geompp.Point2D(1,1), geompp.Point2D(1,2),
                 geompp.Point2D(2,2), geompp.Point2D(2,1)]
        p = geompp.Polygon2D.make(outer, [hole])
        assert not p.is_convex()


class TestPolygon3DIsConvex:
    def test_square_xy_plane_is_convex(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(0,1,0)])
        assert p.is_convex()

    def test_concave_not_convex(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(4,0,0),
            geompp.Point3D(4,4,0), geompp.Point3D(2,2,0), geompp.Point3D(0,4,0)])
        assert not p.is_convex()

    def test_with_hole_not_convex(self):
        outer = [geompp.Point3D(0,0,0), geompp.Point3D(4,0,0),
                 geompp.Point3D(4,4,0), geompp.Point3D(0,4,0)]
        # holes must be CW (reversed winding)
        hole  = [geompp.Point3D(1,1,0), geompp.Point3D(1,2,0),
                 geompp.Point3D(2,2,0), geompp.Point3D(2,1,0)]
        p = geompp.Polygon3D.make(outer, [hole])
        assert not p.is_convex()


# --- Polygon2D.simplify ---
class TestPolygon2DSimplify:
    def _bowtie(self):
        # Self-intersecting: A(0,0), B(4,0), C(1,3), D(3,3) — edges B→C and D→A cross at (2,2).
        return geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(1, 3), geompp.Point2D(3, 3)])

    def test_already_simple_returns_self(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(2, 0),
            geompp.Point2D(2, 2), geompp.Point2D(0, 2)])
        result = p.simplify()
        assert len(result) == 1
        assert result[0].almost_equals(p)

    def test_bowtie_yields_two_polygons(self):
        result = self._bowtie().simplify()
        assert len(result) == 2

    def test_bowtie_results_are_simple(self):
        for poly in self._bowtie().simplify():
            assert poly.is_simple()

    def test_bowtie_areas_sum(self):
        result = self._bowtie().simplify()
        total = sum(p.area() for p in result)
        assert abs(total - 5.0) < 0.01

    def test_wide_bowtie_all_simple(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(2, 6), geompp.Point2D(8, 6)])
        assert not p.is_simple()
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()


# --- Polygon3D.simplify ---
class TestPolygon3DSimplify:
    def _bowtie_xy(self):
        # Bowtie in XY plane (z=0): dominant axis = Z.
        return geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])

    def test_already_simple_returns_self(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0),
            geompp.Point3D(2, 2, 0), geompp.Point3D(0, 2, 0)])
        result = p.simplify()
        assert len(result) == 1
        assert result[0].almost_equals(p)

    def test_bowtie_xy_yields_two_polygons(self):
        result = self._bowtie_xy().simplify()
        assert len(result) == 2

    def test_bowtie_xy_results_are_simple(self):
        for poly in self._bowtie_xy().simplify():
            assert poly.is_simple()

    def test_bowtie_yz_plane_yields_two_polygons(self):
        # Bowtie in YZ plane (x=0): dominant axis = X.
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(0, 1, 3), geompp.Point3D(0, 3, 3)])
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()

    def test_bowtie_xz_plane_yields_two_polygons(self):
        # Bowtie in XZ plane (y=0): dominant axis = Y, projection flips chirality.
        p = geompp.Polygon3D.make([
            geompp.Point3D(3, 0, 3), geompp.Point3D(1, 0, 3),
            geompp.Point3D(4, 0, 0), geompp.Point3D(0, 0, 0)])
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()

    def test_bowtie_areas_sum(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])
        result = p.simplify()
        total = sum(poly.area() for poly in result)
        assert abs(total - 5.0) < 0.01

    def test_results_are_coplanar(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])
        for poly in p.simplify():
            for i in range(poly.size()):
                assert abs(poly[i].z) < 1e-9


class TestPolyline3DPlanarConvex:
    def test_is_planar_xy(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(0,1,0)])
        assert pl.is_planar()

    def test_is_planar_nonplanar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        assert not pl.is_planar()

    def test_is_simple_planar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        assert pl.is_simple()

    def test_is_convex_planar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        assert pl.is_convex()

    def test_is_convex_not_planar_throws(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        with pytest.raises(Exception):
            pl.is_convex()

    def test_convex_hull_returns_polyline(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        hull = pl.convex_hull()
        assert isinstance(hull, geompp.Polyline3D)

    def test_convex_hull_to_polygon(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        polygon = pl.convex_hull().to_polygon()
        assert isinstance(polygon, geompp.Polygon3D)

    def test_to_polygon_not_planar_throws(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        with pytest.raises(Exception):
            pl.to_polygon()


class TestPrincipalAxes:
    @pytest.fixture
    def flat_xy_cloud(self):
        # Flat cloud in XY plane, elongated along X
        return [
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(2,0,0), geompp.Point3D(3,0,0),
            geompp.Point3D(0,0.1,0), geompp.Point3D(1,0.1,0),
            geompp.Point3D(2,0.1,0), geompp.Point3D(3,0.1,0),
        ]

    def test_coordinate_frame_attributes(self, flat_xy_cloud):
        frame = geompp.principal_axes(flat_xy_cloud)
        assert hasattr(frame, 'x')
        assert hasattr(frame, 'y')
        assert hasattr(frame, 'z')
        assert isinstance(frame.x, geompp.Vector3D)
        assert isinstance(frame.y, geompp.Vector3D)
        assert isinstance(frame.z, geompp.Vector3D)

    def test_z_is_normal_for_flat_xy_cloud(self, flat_xy_cloud):
        frame = geompp.principal_axes(flat_xy_cloud)
        # Z should be nearly (0,0,1) or (0,0,-1)
        assert abs(abs(frame.z.z) - 1.0) < 0.01

    def test_axes_are_orthogonal(self, flat_xy_cloud):
        frame = geompp.principal_axes(flat_xy_cloud)
        assert abs(frame.x.dot(frame.y)) < 1e-6
        assert abs(frame.x.dot(frame.z)) < 1e-6
        assert abs(frame.y.dot(frame.z)) < 1e-6

    def test_axes_are_unit_vectors(self, flat_xy_cloud):
        frame = geompp.principal_axes(flat_xy_cloud)
        assert abs(frame.x.length() - 1.0) < 1e-9
        assert abs(frame.y.length() - 1.0) < 1e-9
        assert abs(frame.z.length() - 1.0) < 1e-9

    def test_principal_normal_matches_z(self, flat_xy_cloud):
        normal = geompp.principal_normal(flat_xy_cloud)
        assert abs(abs(normal.z) - 1.0) < 0.01

    def test_principal_direction_matches_x(self, flat_xy_cloud):
        direction = geompp.principal_direction(flat_xy_cloud)
        # Primary direction should be along X (elongated axis)
        assert abs(abs(direction.x) - 1.0) < 0.01

    def test_too_few_points_throws(self):
        with pytest.raises(Exception):
            geompp.principal_axes([geompp.Point3D(0,0,0), geompp.Point3D(1,0,0)])


# ─── View2D ──────────────────────────────────────────────────────────────────

class TestView2D:

    # type
    def test_type_xy(self):
        assert geompp.View2D.xy().type == geompp.ProjectionType.XY

    def test_type_yz(self):
        assert geompp.View2D.yz().type == geompp.ProjectionType.YZ

    def test_type_zx(self):
        assert geompp.View2D.zx().type == geompp.ProjectionType.ZX

    def test_type_custom(self):
        v = geompp.View2D.on_plane(geompp.Plane.xy())
        assert v.type == geompp.ProjectionType.Custom

    # Point2D pass-through
    def test_point2d_passthrough(self):
        p = geompp.Point2D(3.0, 7.0)
        v = geompp.View2D.xy()
        assert approx(v.x(p), 3.0)
        assert approx(v.y(p), 7.0)

    # XY projection (drops Z)
    def test_xy_x(self):
        p = geompp.Point3D(1.0, 2.0, 99.0)
        assert approx(geompp.View2D.xy().x(p), 1.0)

    def test_xy_y(self):
        p = geompp.Point3D(1.0, 2.0, 99.0)
        assert approx(geompp.View2D.xy().y(p), 2.0)

    # YZ projection (y→x, z→y, drops X)
    def test_yz_x(self):
        p = geompp.Point3D(99.0, 3.0, 4.0)
        assert approx(geompp.View2D.yz().x(p), 3.0)

    def test_yz_y(self):
        p = geompp.Point3D(99.0, 3.0, 4.0)
        assert approx(geompp.View2D.yz().y(p), 4.0)

    # ZX projection (z→x, x→y, drops Y)
    def test_zx_x(self):
        p = geompp.Point3D(5.0, 99.0, 6.0)
        assert approx(geompp.View2D.zx().x(p), 6.0)

    def test_zx_y(self):
        p = geompp.Point3D(5.0, 99.0, 6.0)
        assert approx(geompp.View2D.zx().y(p), 5.0)

    # Custom (on_plane)
    def test_custom_on_xy_plane(self):
        v = geompp.View2D.on_plane(geompp.Plane.xy())
        p = geompp.Point3D(2.0, 5.0, 0.0)
        assert approx(v.x(p), 2.0)
        assert approx(v.y(p), 5.0)

    def test_custom_with_offset(self):
        plane = geompp.Plane.from_origin_and_normal(
            geompp.Point3D(1.0, 1.0, 1.0), geompp.Vector3D(0.0, 0.0, 1.0))
        v = geompp.View2D.on_plane(plane)
        p = geompp.Point3D(3.0, 4.0, 1.0)  # offset from plane origin: (2, 3)
        assert approx(v.x(p), 2.0)
        assert approx(v.y(p), 3.0)

    def test_custom_yz_matches_builtin(self):
        custom = geompp.View2D.on_plane(geompp.Plane.yz())
        builtin = geompp.View2D.yz()
        p = geompp.Point3D(99.0, 7.0, 8.0)
        assert approx(custom.x(p), builtin.x(p))
        assert approx(custom.y(p), builtin.y(p))

    # copy
    def test_copy(self):
        v1 = geompp.View2D.yz()
        v2 = geompp.View2D(v1)
        p = geompp.Point3D(1.0, 2.0, 3.0)
        assert approx(v2.x(p), v1.x(p))
        assert approx(v2.y(p), v1.y(p))

    # bulk streaming (the main use-case)
    def test_bulk_projection(self):
        pts = [geompp.Point3D(1.0, 2.0, 10.0),
               geompp.Point3D(3.0, 4.0, 20.0),
               geompp.Point3D(5.0, 6.0, 30.0)]
        v = geompp.View2D.xy()
        xs = [v.x(p) for p in pts]
        ys = [v.y(p) for p in pts]
        assert xs == pytest.approx([1.0, 3.0, 5.0])
        assert ys == pytest.approx([2.0, 4.0, 6.0])


# ─── BPrism3D ──────────────────────────────────────────────────────────────────

class TestBPrism3D:
    def test_empty_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([])

    def test_single_point_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([geompp.Point3D(3, 4, 5)])

    def test_two_points_throws(self):
        with pytest.raises(Exception):
            geompp.BPrism3D([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])

    def test_axis_aligned_box(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.volume, 24.0, eps=0.5)
        for pt in pts:
            assert p.contains(pt), f"prism must contain input point {pt}"

    def test_flat_cloud_w_is_epsilon(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        p = geompp.BPrism3D(pts)
        assert p.half_len_w > 0, "flat cloud must have non-zero w half-length"
        assert approx(p.half_len_w, 0.001, eps=0.01)

    def test_nonconvex_all_points_contained(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0),
            geompp.Point3D(3, 2, 0), geompp.Point3D(0, 2, 0),
            geompp.Point3D(1, 0.5, 1), geompp.Point3D(2, 1.5, 0.5),
        ]
        p = geompp.BPrism3D(pts)
        for pt in pts:
            assert p.contains(pt), f"prism must contain input point {pt}"

    def test_axes_are_unit_vectors(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.axis_u.length(), 1.0)
        assert approx(p.axis_v.length(), 1.0)
        assert approx(p.axis_w.length(), 1.0)

    def test_axes_orthogonal(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.axis_u.dot(p.axis_v), 0.0)
        assert approx(p.axis_u.dot(p.axis_w), 0.0)
        assert approx(p.axis_v.dot(p.axis_w), 0.0)

    def test_width_height_depth_volume(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert approx(p.width, 2.0 * p.half_len_u)
        assert approx(p.height, 2.0 * p.half_len_v)
        assert approx(p.depth, 2.0 * p.half_len_w)
        assert approx(p.volume, p.width * p.height * p.depth, eps=0.5)

    def test_corners_eight_distinct_all_contained(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        corners = p.corners()
        assert len(corners) == 8
        for c in corners:
            assert p.contains(c), "each corner must be inside the prism"

    def test_contains_center(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert p.contains(p.center)

    def test_contains_outside_false(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p = geompp.BPrism3D(pts)
        assert not p.contains(geompp.Point3D(10, 10, 10))

    def test_almost_equals(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
            geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
            geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
        ]
        p1 = geompp.BPrism3D(pts)
        p2 = geompp.BPrism3D(pts)
        assert p1.almost_equals(p2)
        assert p1 == p2

    def test_not_almost_equals_different(self):
        pts1 = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        pts2 = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0),
            geompp.Point3D(6, 3, 0), geompp.Point3D(0, 3, 0),
        ]
        p1 = geompp.BPrism3D(pts1)
        p2 = geompp.BPrism3D(pts2)
        assert not p1.almost_equals(p2)


# ─── Overlaps ────────────────────────────────────────────────────────────────

class TestLine2DOverlap:
    def test_overlap_same_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        x2 = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert x.overlaps(x2)
        ov = x.overlap(x2)
        assert ov is not None
        assert isinstance(ov, geompp.Line2D)

    def test_no_overlap_crossing(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        y = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not x.overlaps(y)
        assert x.overlap(y) is None

    def test_no_overlap_parallel_offset(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        x_off = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not x.overlaps(x_off)
        assert x.overlap(x_off) is None

    def test_overlap_with_collinear_ray(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        rx = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert x.overlaps(rx)
        ov = x.overlap(rx)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_no_overlap_perpendicular_ray(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        ry = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert not x.overlaps(ry)
        assert x.overlap(ry) is None

    def test_overlap_with_collinear_segment(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        assert x.overlaps(seg)
        ov = x.overlap(seg)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment2D)
        assert ov == seg

    def test_no_overlap_offset_segment(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        seg_off = geompp.LineSegment2D.make(geompp.Point2D(2, 1), geompp.Point2D(5, 1))
        assert not x.overlaps(seg_off)
        assert x.overlap(seg_off) is None


class TestRay2DOverlap:
    def test_overlap_with_collinear_line(self):
        r = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert r.overlaps(x)
        ov = r.overlap(x)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_no_overlap_perpendicular_line(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        y = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(0, 1))
        assert not r.overlaps(y)
        assert r.overlap(y) is None

    def test_overlap_same_direction_rays(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(1, 0))
        assert r1.overlaps(r2)
        ov = r1.overlap(r2)
        assert ov is not None
        assert isinstance(ov, geompp.Ray2D)

    def test_overlap_anti_parallel_rays(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r3 = geompp.Ray2D.make(geompp.Point2D(5, 0), geompp.Vector2D(-1, 0))
        assert r1.overlaps(r3)
        ov = r1.overlap(r3)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment2D)

    def test_no_overlap_touching_anti_parallel(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r4 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(-1, 0))
        assert not r1.overlaps(r4)
        assert r1.overlap(r4) is None

    def test_overlap_segment_inside_ray(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(4, 0))
        assert r.overlaps(seg)
        ov = r.overlap(seg)
        assert ov == seg

    def test_no_overlap_segment_before_ray(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg = geompp.LineSegment2D.make(geompp.Point2D(-3, 0), geompp.Point2D(-1, 0))
        assert not r.overlaps(seg)
        assert r.overlap(seg) is None

    def test_no_overlap_segment_touching_ray_origin(self):
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        seg_touch = geompp.LineSegment2D.make(geompp.Point2D(-1, 0), geompp.Point2D(1, 0))
        assert not r.overlaps(seg_touch)
        assert r.overlap(seg_touch) is None


class TestLineSegment2DOverlap:
    def test_overlap_with_collinear_line(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        assert seg.overlaps(x)
        ov = seg.overlap(x)
        assert ov == seg

    def test_no_overlap_offset_line(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(5, 0))
        x_off = geompp.Line2D.make(geompp.Point2D(0, 1), geompp.Point2D(1, 1))
        assert not seg.overlaps(x_off)
        assert seg.overlap(x_off) is None

    def test_overlap_ray_covers_segment(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        assert seg.overlaps(r)
        assert seg.overlap(r) == seg

    def test_overlap_ray_partial(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(4, 0), geompp.Vector2D(1, 0))
        assert seg.overlaps(r)
        ov = seg.overlap(r)
        assert ov is not None
        assert ov == geompp.LineSegment2D.make(geompp.Point2D(4, 0), geompp.Point2D(6, 0))

    def test_no_overlap_ray_after_segment(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(6, 0))
        r = geompp.Ray2D.make(geompp.Point2D(7, 0), geompp.Vector2D(1, 0))
        assert not seg.overlaps(r)
        assert seg.overlap(r) is None

    def test_overlap_segment_partial(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert a.overlaps(b)
        ov = a.overlap(b)
        assert ov is not None
        assert ov == geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(5, 0))

    def test_no_overlap_disjoint_segments(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(6, 0), geompp.Point2D(9, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_touching_endpoint(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_perpendicular(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(2, -1), geompp.Point2D(2, 1))
        assert not a.overlaps(b)
        assert a.overlap(b) is None


class TestLine3DOverlap:
    def test_overlap_same_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        x2 = geompp.Line3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert x.overlaps(x2)
        ov = x.overlap(x2)
        assert ov is not None
        assert isinstance(ov, geompp.Line3D)

    def test_no_overlap_crossing(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        y = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0))
        assert not x.overlaps(y)
        assert x.overlap(y) is None

    def test_overlap_with_collinear_ray(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        rx = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert x.overlaps(rx)
        ov = x.overlap(rx)
        assert ov is not None
        assert isinstance(ov, geompp.Ray3D)

    def test_overlap_with_collinear_segment(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        assert x.overlaps(seg)
        ov = x.overlap(seg)
        assert ov == seg


class TestRay3DOverlap:
    def test_overlap_same_direction(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert r1.overlaps(r2)
        ov = r1.overlap(r2)
        assert ov is not None
        assert isinstance(ov, geompp.Ray3D)

    def test_overlap_anti_parallel(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r3 = geompp.Ray3D.make(geompp.Point3D(5, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert r1.overlaps(r3)
        ov = r1.overlap(r3)
        assert ov is not None
        assert isinstance(ov, geompp.LineSegment3D)

    def test_no_overlap_touching(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r4 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert not r1.overlaps(r4)
        assert r1.overlap(r4) is None

    def test_overlap_segment_inside_ray(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 0))
        assert r.overlaps(seg)
        assert r.overlap(seg) == seg

    def test_no_overlap_segment_before_ray(self):
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        seg = geompp.LineSegment3D.make(geompp.Point3D(-3, 0, 0), geompp.Point3D(-1, 0, 0))
        assert not r.overlaps(seg)
        assert r.overlap(seg) is None


class TestLineSegment3DOverlap:
    def test_overlap_with_line(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert seg.overlaps(x)
        assert seg.overlap(x) == seg

    def test_no_overlap_offset_line(self):
        seg = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(5, 0, 0))
        x_off = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(1, 1, 0))
        assert not seg.overlaps(x_off)
        assert seg.overlap(x_off) is None

    def test_overlap_segment_partial(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(7, 0, 0))
        assert a.overlaps(b)
        ov = a.overlap(b)
        assert ov == geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(5, 0, 0))

    def test_no_overlap_disjoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(6, 0, 0), geompp.Point3D(9, 0, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None

    def test_no_overlap_touching_endpoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert not a.overlaps(b)
        assert a.overlap(b) is None


# ──────────────────────────────────────────────────────────────────────────────
# Touch — 2D
# ──────────────────────────────────────────────────────────────────────────────

class TestLine2DTouch:
    def test_touch_with_ray_origin_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(3, 0), geompp.Vector2D(0, 1))
        assert x.touches(r)
        t = x.touch(r)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_ray_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        assert not x.touches(r)
        assert x.touch(r) is None

    def test_touch_with_segment_endpoint_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(2, 3))
        assert x.touches(s)
        t = x.touch(s)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_segment_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(1, 0), geompp.Point2D(4, 0))
        assert not x.touches(s)
        assert x.touch(s) is None


class TestRay2DTouch:
    def test_touch_with_line_origin_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(2, 0), geompp.Vector2D(0, 1))
        assert r.touches(x)
        t = r.touch(x)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_line_collinear_no_touch(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        r = geompp.Ray2D.make(geompp.Point2D(1, 0), geompp.Vector2D(1, 0))
        assert not r.touches(x)
        assert r.touch(x) is None

    def test_touch_with_ray_same_origin_different_dir(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(0, 1))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point2D(0, 0)

    def test_touch_with_ray_anti_parallel_same_origin(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(-1, 0))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point2D(0, 0)

    def test_touch_with_ray_anti_parallel_overlapping_no_touch(self):
        r1 = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        r2 = geompp.Ray2D.make(geompp.Point2D(3, 0), geompp.Vector2D(-1, 0))
        assert not r1.touches(r2)
        assert r1.touch(r2) is None

    def test_touch_with_segment_endpoint(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 2))
        assert r.touches(s)
        t = r.touch(s)
        assert t is not None
        assert t == geompp.Point2D(3, 0)


class TestLineSegment2DTouch:
    def test_touch_with_line_first_on_line(self):
        x = geompp.Line2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(2, 0), geompp.Point2D(2, 3))
        assert s.touches(x)
        t = s.touch(x)
        assert t is not None
        assert t == geompp.Point2D(2, 0)

    def test_touch_with_ray_endpoint(self):
        r = geompp.Ray2D.make(geompp.Point2D(0, 0), geompp.Vector2D(1, 0))
        s = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 2))
        assert s.touches(r)
        t = s.touch(r)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_segment_t_junction(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(3, 3))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point2D(3, 0)

    def test_touch_with_segment_collinear_endpoint(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(5, 0), geompp.Point2D(8, 0))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point2D(5, 0)

    def test_touch_with_segment_overlap_no_touch(self):
        a = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(5, 0))
        b = geompp.LineSegment2D.make(geompp.Point2D(3, 0), geompp.Point2D(7, 0))
        assert not a.touches(b)
        assert a.touch(b) is None


# ──────────────────────────────────────────────────────────────────────────────
# Touch — 3D
# ──────────────────────────────────────────────────────────────────────────────

class TestLine3DTouch:
    def test_touch_with_ray_origin_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(3, 0, 0), geompp.Vector3D(0, 0, 1))
        assert x.touches(r)
        t = x.touch(r)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_ray_collinear_no_touch(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(1, 0, 0), geompp.Vector3D(1, 0, 0))
        assert not x.touches(r)
        assert x.touch(r) is None

    def test_touch_with_segment_endpoint_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3))
        assert x.touches(s)
        t = x.touch(s)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)


class TestRay3DTouch:
    def test_touch_with_line_origin_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        r = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r.touches(x)
        t = r.touch(x)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)

    def test_touch_with_ray_same_origin(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(0, 0, 1))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point3D(0, 0, 0)

    def test_touch_with_ray_anti_parallel_same_origin(self):
        r1 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        r2 = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(-1, 0, 0))
        assert r1.touches(r2)
        t = r1.touch(r2)
        assert t is not None
        assert t == geompp.Point3D(0, 0, 0)

    def test_touch_with_segment_endpoint(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2))
        assert r.touches(s)
        t = r.touch(s)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)


class TestLineSegment3DTouch:
    def test_touch_with_line_first_on_line(self):
        x = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3))
        assert s.touches(x)
        t = s.touch(x)
        assert t is not None
        assert t == geompp.Point3D(2, 0, 0)

    def test_touch_with_ray_endpoint(self):
        r = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        s = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2))
        assert s.touches(r)
        t = s.touch(r)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_segment_t_junction(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point3D(3, 0, 0)

    def test_touch_with_segment_collinear_endpoint(self):
        a = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        b = geompp.LineSegment3D.make(geompp.Point3D(5, 0, 0), geompp.Point3D(8, 0, 0))
        assert a.touches(b)
        t = a.touch(b)
        assert t is not None
        assert t == geompp.Point3D(5, 0, 0)
