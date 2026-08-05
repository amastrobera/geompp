"""
plane binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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
