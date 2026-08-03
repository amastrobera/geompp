"""
geometry queries binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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

class TestTangentsTo:
    def test_2d_convex_square_point(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4)])
        t = geompp.tangents_to(square, geompp.Point2D(10, -2))
        assert approx(t.left.last.x, 0) and approx(t.left.last.y, 0)
        assert approx(t.right.last.x, 4) and approx(t.right.last.y, 4)

    def test_2d_non_convex_dart_point_reduces_to_hull(self):
        dart = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4)])
        assert not dart.is_convex()
        t = geompp.tangents_to(dart, geompp.Point2D(-6, 2))
        assert approx(t.left.last.x, 0) and approx(t.left.last.y, 4)
        assert approx(t.right.last.x, 0) and approx(t.right.last.y, 0)

    def test_2d_convex_squares_polygon(self):
        square_a = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4)])
        square_b = geompp.Polygon2D.make([
            geompp.Point2D(10, 1), geompp.Point2D(14, 1),
            geompp.Point2D(14, 5), geompp.Point2D(10, 5)])
        t = geompp.tangents_to(square_a, square_b)
        assert approx(t.left.first.x, 0) and approx(t.left.first.y, 4)
        assert approx(t.left.last.x, 10) and approx(t.left.last.y, 5)
        assert approx(t.right.first.x, 4) and approx(t.right.first.y, 0)
        assert approx(t.right.last.x, 14) and approx(t.right.last.y, 1)

    def test_2d_non_convex_darts_polygon_reduces_both_to_hull(self):
        dart_a = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4)])
        dart_b = geompp.Polygon2D.make([
            geompp.Point2D(10, 1), geompp.Point2D(14, 1), geompp.Point2D(14, 5),
            geompp.Point2D(12, 2), geompp.Point2D(10, 5)])
        t = geompp.tangents_to(dart_a, dart_b)
        assert approx(t.left.first.x, 0) and approx(t.left.first.y, 4)
        assert approx(t.left.last.x, 10) and approx(t.left.last.y, 5)
        assert approx(t.right.first.x, 4) and approx(t.right.first.y, 0)
        assert approx(t.right.last.x, 14) and approx(t.right.last.y, 1)

    def test_3d_coplanar_point_on_xy_plane(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        t = geompp.tangents_to(square, geompp.Point3D(10, -2, 0))
        assert approx(t.left.last.x, 0) and approx(t.left.last.y, 0) and approx(t.left.last.z, 0)
        assert approx(t.right.last.x, 4) and approx(t.right.last.y, 4) and approx(t.right.last.z, 0)

    def test_3d_coplanar_point_on_tilted_plane(self):
        tilted = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 4),
            geompp.Point3D(4, 4, 4), geompp.Point3D(0, 4, 0)])
        t = geompp.tangents_to(tilted, geompp.Point3D(10, -2, 10))
        assert approx(t.left.last.x, 0) and approx(t.left.last.y, 0) and approx(t.left.last.z, 0)
        assert approx(t.right.last.x, 4) and approx(t.right.last.y, 4) and approx(t.right.last.z, 4)

    def test_3d_coplanar_squares_polygon(self):
        tilted_a = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 4),
            geompp.Point3D(4, 4, 4), geompp.Point3D(0, 4, 0)])
        tilted_b = geompp.Polygon3D.make([
            geompp.Point3D(10, 1, 10), geompp.Point3D(14, 1, 14),
            geompp.Point3D(14, 5, 14), geompp.Point3D(10, 5, 10)])
        t = geompp.tangents_to(tilted_a, tilted_b)
        assert approx(t.left.first.x, 0) and approx(t.left.first.y, 4) and approx(t.left.first.z, 0)
        assert approx(t.left.last.x, 10) and approx(t.left.last.y, 5) and approx(t.left.last.z, 10)
        assert approx(t.right.first.x, 4) and approx(t.right.first.y, 0) and approx(t.right.first.z, 4)
        assert approx(t.right.last.x, 14) and approx(t.right.last.y, 1) and approx(t.right.last.z, 14)

    def test_3d_point_not_coplanar_raises(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        with pytest.raises(Exception):
            geompp.tangents_to(square, geompp.Point3D(10, -2, 1))

    def test_3d_polygons_not_coplanar_raises(self):
        square_a = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)])
        square_b = geompp.Polygon3D.make([
            geompp.Point3D(10, 1, 1), geompp.Point3D(14, 1, 1),
            geompp.Point3D(14, 5, 1), geompp.Point3D(10, 5, 1)])
        with pytest.raises(Exception):
            geompp.tangents_to(square_a, square_b)

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
