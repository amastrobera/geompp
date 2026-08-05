"""
convex hull binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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
