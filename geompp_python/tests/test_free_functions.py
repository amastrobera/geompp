"""
free functions binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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

    def test_lerp_2d(self):
        p0, p1 = geompp.Point2D(0, 0), geompp.Point2D(10, 20)
        assert approx(geompp.lerp(p0, p1, 0.0).x, 0.0)
        assert approx(geompp.lerp(p0, p1, 1.0).x, 10.0)
        mid = geompp.lerp(p0, p1, 0.5)
        assert approx(mid.x, 5.0) and approx(mid.y, 10.0)
        # not clamped: t outside [0, 1] extrapolates
        far = geompp.lerp(p0, p1, 2.0)
        assert approx(far.x, 20.0) and approx(far.y, 40.0)

    def test_lerp_3d(self):
        p0, p1 = geompp.Point3D(0, 0, 0), geompp.Point3D(10, 20, 30)
        mid = geompp.lerp(p0, p1, 0.5)
        assert approx(mid.x, 5.0) and approx(mid.y, 10.0) and approx(mid.z, 15.0)

    def test_clip_2d_overlapping_squares(self):
        clipper = [geompp.Point2D(0.5, 0.5), geompp.Point2D(1.5, 0.5),
                   geompp.Point2D(1.5, 1.5), geompp.Point2D(0.5, 1.5)]
        subject = [geompp.Point2D(0, 0), geompp.Point2D(1, 0),
                   geompp.Point2D(1, 1), geompp.Point2D(0, 1)]
        rings = geompp.clip(clipper, subject)
        assert len(rings) == 1
        poly = geompp.Polygon2D.make(rings[0])
        assert approx(poly.area(), 0.25)

    def test_clip_2d_disjoint_returns_empty(self):
        clipper = [geompp.Point2D(5, 5), geompp.Point2D(6, 5), geompp.Point2D(6, 6), geompp.Point2D(5, 6)]
        subject = [geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)]
        assert geompp.clip(clipper, subject) == []

    def test_clip_3d_coplanar_overlapping_squares(self):
        clipper = [geompp.Point3D(0.5, 0.5, 0), geompp.Point3D(1.5, 0.5, 0),
                   geompp.Point3D(1.5, 1.5, 0), geompp.Point3D(0.5, 1.5, 0)]
        subject = [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
                   geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)]
        rings = geompp.clip(clipper, subject)
        assert len(rings) == 1
        poly = geompp.Polygon3D.make(rings[0])
        assert approx(poly.area(), 0.25)

    def test_clip_3d_non_coplanar_raises(self):
        clipper = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
                   geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1)]
        subject = [geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
                   geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)]
        with pytest.raises(Exception):
            geompp.clip(clipper, subject)

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
