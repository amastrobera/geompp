"""
triangle3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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
