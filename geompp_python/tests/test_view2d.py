"""
view2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


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
