"""
triangle2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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

    def test_distance_to(self):
        # right triangle: legs on the axes (length 4 each), hypotenuse x + y = 4
        t = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 4))

        # interior, on an edge, on a vertex, on the hypotenuse -- all zero
        assert approx(t.distance_to(geompp.Point2D(1, 1)), 0.0)
        assert approx(t.distance_to(geompp.Point2D(2, 0)), 0.0)
        assert approx(t.distance_to(geompp.Point2D(0, 0)), 0.0)
        assert approx(t.distance_to(geompp.Point2D(2, 2)), 0.0)

        # outside, perpendicular foot lands within a leg
        assert approx(t.distance_to(geompp.Point2D(2, -3)), 3.0)
        assert approx(t.distance_to(geompp.Point2D(-3, 1)), 3.0)

        # outside, perpendicular foot lands within the hypotenuse
        assert approx(t.distance_to(geompp.Point2D(4, 4)), 2 * 2.0 ** 0.5)

        # outside, perpendicular foot falls off every edge -- nearest point is a vertex
        assert approx(t.distance_to(geompp.Point2D(-1, -1)), 2.0 ** 0.5)
        assert approx(t.distance_to(geompp.Point2D(5, -1)), 2.0 ** 0.5)

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

    def test_intersection_with_ray(self, tri):
        # ray from inside pointing right exits through the hypotenuse -> a Point2D
        r = geompp.Ray2D.make(geompp.Point2D(0.5, 0.5), geompp.Vector2D.basis_x())
        assert tri.intersects(r)
        hit = tri.intersection(r)
        assert isinstance(hit, geompp.Point2D)

        r_away = geompp.Ray2D.make(geompp.Point2D(-3, 0), -geompp.Vector2D.basis_x())
        assert not tri.intersects(r_away)
        assert tri.intersection(r_away) is None

    def test_intersection_with_segment(self, tri):
        s = geompp.LineSegment2D.make(geompp.Point2D(-3, 1), geompp.Point2D(3, 1))
        assert tri.intersects(s)
        hit = tri.intersection(s)
        assert isinstance(hit, geompp.LineSegment2D)

        s_outside = geompp.LineSegment2D.make(geompp.Point2D(-3, 1), geompp.Point2D(-1, 1))
        assert not tri.intersects(s_outside)
        assert tri.intersection(s_outside) is None

    def test_intersects_and_intersection_with_triangle(self, tri):
        overlapping = geompp.Triangle2D.make(
            geompp.Point2D(1, 1), geompp.Point2D(5, 1), geompp.Point2D(1, 4))
        assert tri.intersects_triangle(overlapping)
        hit = tri.intersection(overlapping)
        assert isinstance(hit, geompp.Triangle2D)
        assert hit.area() > 0

        disjoint = geompp.Triangle2D.make(
            geompp.Point2D(100, 100), geompp.Point2D(104, 100), geompp.Point2D(100, 104))
        assert not tri.intersects_triangle(disjoint)
        assert tri.intersection(disjoint) is None

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
