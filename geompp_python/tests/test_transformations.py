"""
geompp.transformations binding tests (translate/rotate/scale fast path, transform(obj, matrix)
general path, TransformBuilder).
"""

import math
import pytest
import geompp
from geompp import maths, transformations as tf

from ._helpers import approx


class TestTransformations2DFastPath:
    def test_translate_point_adds_offset(self):
        p = tf.translate(geompp.Point2D(1, 2), maths.Vector2(10, 20))
        assert approx(p.x, 11.0)
        assert approx(p.y, 22.0)

    def test_rotate_point_quarter_turn(self):
        p = tf.rotate(geompp.Point2D(1, 0), math.pi / 2)
        assert approx(p.x, 0.0, 1e-9)
        assert approx(p.y, 1.0, 1e-9)

    def test_scale_point_uniform_and_nonuniform(self):
        uniform = tf.scale(geompp.Point2D(2, 3), 2.0)
        assert approx(uniform.x, 4.0)
        assert approx(uniform.y, 6.0)
        non_uniform = tf.scale(geompp.Point2D(2, 3), 2.0, 5.0)
        assert approx(non_uniform.x, 4.0)
        assert approx(non_uniform.y, 15.0)

    def test_shear_point_offsets_axis_by_multiple_of_other(self):
        p = tf.shear(geompp.Point2D(1, 3), 2.0, 0.0)
        assert approx(p.x, 1.0 + 2.0 * 3.0)
        assert approx(p.y, 3.0)

    def test_reflect_point_about_x_axis_normal_flips_y(self):
        p = tf.reflect(geompp.Point2D(3, 4), maths.Vector2(0, 1))
        assert approx(p.x, 3.0, 1e-9)
        assert approx(p.y, -4.0, 1e-9)

    def test_reflect_point_zero_length_normal_raises(self):
        with pytest.raises(ValueError):
            tf.reflect(geompp.Point2D(1, 1), maths.Vector2(0, 0))


class TestTransformations2DGeneralPath:
    def test_transform_vector_ignores_translation(self):
        m = maths.Matrix3.translation(maths.Vector2(100, 100))
        v = tf.transform(geompp.Vector2D(1, 0), m)
        assert approx(v.x, 1.0)
        assert approx(v.y, 0.0)

    def test_transform_line_segment_preserves_length(self):
        seg = geompp.LineSegment2D.make(geompp.Point2D(0, 0), geompp.Point2D(1, 0))
        m = maths.Matrix3.rotation(0.6) @ maths.Matrix3.translation(maths.Vector2(3, -2))
        moved = tf.transform(seg, m)
        assert approx(moved.length(), seg.length())

    def test_transform_triangle_preserves_area_under_rigid_transform(self):
        tri = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3))
        m = maths.Matrix3.rotation(0.7) @ maths.Matrix3.translation(maths.Vector2(2, 2))
        moved = tf.transform(tri, m)
        assert approx(moved.area(), tri.area())

    def test_transform_triangle_scale_multiplies_area_by_square(self):
        tri = geompp.Triangle2D.make(geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3))
        moved = tf.transform(tri, maths.Matrix3.scale(2.0))
        assert approx(moved.area(), tri.area() * 4.0)

    def test_transform_polygon_with_holes_preserves_area(self):
        poly = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(10, 0), geompp.Point2D(10, 10), geompp.Point2D(0, 10)],
            [[geompp.Point2D(4, 4), geompp.Point2D(4, 6), geompp.Point2D(6, 6), geompp.Point2D(6, 4)]],
        )
        m = maths.Matrix3.rotation(0.4) @ maths.Matrix3.translation(maths.Vector2(-5, 8))
        moved = tf.transform(poly, m)
        assert moved.has_holes()
        assert approx(moved.area(), poly.area())

    def test_transform_mesh_preserves_total_area(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        mesh = geompp.Mesh2D.from_triangles([
            geompp.Triangle2D.make(p0.perimeter()[0], p0.perimeter()[1], p0.perimeter()[2]),
            geompp.Triangle2D.make(p0.perimeter()[0], p0.perimeter()[2], p0.perimeter()[3]),
        ])
        m = maths.Matrix3.rotation(1.2) @ maths.Matrix3.translation(maths.Vector2(1, 1))
        moved = tf.transform(mesh, m)
        assert moved.size() == mesh.size()
        assert approx(moved.area(), mesh.area())

    def test_transform_polymesh_preserves_total_area(self):
        p0 = geompp.Polygon2D.make([geompp.Point2D(0, 0), geompp.Point2D(1, 0), geompp.Point2D(1, 1), geompp.Point2D(0, 1)])
        p1 = geompp.Polygon2D.make([geompp.Point2D(1, 0), geompp.Point2D(2, 0), geompp.Point2D(2, 1), geompp.Point2D(1, 1)])
        mesh = geompp.PolyMesh2D.from_polygons([p0, p1])
        m = maths.Matrix3.rotation(0.3) @ maths.Matrix3.translation(maths.Vector2(-1, 2))
        moved = tf.transform(mesh, m)
        assert moved.size() == mesh.size()
        assert approx(moved.area(), mesh.area())


class TestTransformations3DFastPath:
    def test_translate_point_adds_offset(self):
        p = tf.translate(geompp.Point3D(1, 2, 3), maths.Vector3(10, 20, 30))
        assert approx(p.x, 11.0)
        assert approx(p.y, 22.0)
        assert approx(p.z, 33.0)

    def test_rotate_point_quarter_turn_about_z(self):
        p = tf.rotate(geompp.Point3D(1, 0, 0), math.pi / 2, maths.Vector3(0, 0, 1))
        assert approx(p.x, 0.0, 1e-9)
        assert approx(p.y, 1.0, 1e-9)

    def test_rotate_point_zero_axis_raises(self):
        with pytest.raises(ValueError):
            tf.rotate(geompp.Point3D(1, 0, 0), 1.0, maths.Vector3(0, 0, 0))

    def test_scale_point_uniform_and_nonuniform(self):
        uniform = tf.scale(geompp.Point3D(2, 3, 4), 2.0)
        assert approx(uniform.x, 4.0)
        assert approx(uniform.z, 8.0)
        non_uniform = tf.scale(geompp.Point3D(2, 3, 4), 2.0, 5.0, 0.5)
        assert approx(non_uniform.y, 15.0)

    def test_shear_point_offsets_axis_by_multiple_of_other(self):
        p = tf.shear(geompp.Point3D(1, 3, 5), 2.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        assert approx(p.x, 1.0 + 2.0 * 3.0)
        assert approx(p.y, 3.0)
        assert approx(p.z, 5.0)

    def test_reflect_point_about_x_axis_normal_flips_y(self):
        p = tf.reflect(geompp.Point3D(3, 4, 5), maths.Vector3(0, 1, 0))
        assert approx(p.x, 3.0, 1e-9)
        assert approx(p.y, -4.0, 1e-9)
        assert approx(p.z, 5.0, 1e-9)

    def test_reflect_point_zero_length_normal_raises(self):
        with pytest.raises(ValueError):
            tf.reflect(geompp.Point3D(1, 1, 1), maths.Vector3(0, 0, 0))


class TestTransformations3DGeneralPath:
    def test_transform_vector_ignores_translation(self):
        m = maths.Matrix4.translation(maths.Vector3(100, 100, 100))
        v = tf.transform(geompp.Vector3D(1, 0, 0), m)
        assert approx(v.x, 1.0)
        assert approx(v.y, 0.0)
        assert approx(v.z, 0.0)

    def test_transform_triangle_preserves_area_under_rigid_transform(self):
        tri = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 3, 0))
        m = maths.Matrix4.rotation(0.5, maths.Vector3(1, 1, 1)) @ maths.Matrix4.translation(maths.Vector3(1, 2, 3))
        moved = tf.transform(tri, m)
        assert approx(moved.area(), tri.area())

    def test_transform_mesh_preserves_total_area(self):
        a = geompp.Triangle3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(0, 1, 0))
        b = geompp.Triangle3D.make(geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0))
        mesh = geompp.Mesh3D.from_triangles([a, b])
        m = maths.Matrix4.rotation(1.1, maths.Vector3(0, 1, 0)) @ maths.Matrix4.translation(maths.Vector3(1, 1, 1))
        moved = tf.transform(mesh, m)
        assert moved.size() == mesh.size()
        assert approx(moved.area(), mesh.area())

    def test_transform_polymesh_preserves_total_area(self):
        p0 = geompp.Polygon3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0)])
        p1 = geompp.Polygon3D.make([geompp.Point3D(1, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(2, 1, 0), geompp.Point3D(1, 1, 0)])
        mesh = geompp.PolyMesh3D.from_polygons([p0, p1])
        m = maths.Matrix4.rotation(0.2, maths.Vector3(1, 0, 0)) @ maths.Matrix4.translation(maths.Vector3(-1, 2, 0))
        moved = tf.transform(mesh, m)
        assert moved.size() == mesh.size()
        assert approx(moved.area(), mesh.area())


class TestTransformBuilder:
    def test_default_constructed_is_identity(self):
        builder = tf.TransformBuilder()
        assert builder.get() == maths.Matrix4.identity()

    def test_chained_ops_apply_in_call_order(self):
        # translate then rotate: (1,0,0) -> translate(+5,0,0) -> (6,0,0) -> rotate 90deg -> (0,6,0)
        builder = tf.TransformBuilder()
        builder.translate(maths.Vector3(5, 0, 0)).rotate(math.pi / 2, maths.Vector3(0, 0, 1))
        p = tf.transform(geompp.Point3D(1, 0, 0), builder.get())
        assert approx(p.x, 0.0, 1e-9)
        assert approx(p.y, 6.0, 1e-9)

    def test_reversed_chain_order_produces_different_result(self):
        builder = tf.TransformBuilder()
        builder.rotate(math.pi / 2, maths.Vector3(0, 0, 1)).translate(maths.Vector3(5, 0, 0))
        p = tf.transform(geompp.Point3D(1, 0, 0), builder.get())
        assert approx(p.x, 5.0, 1e-9)
        assert approx(p.y, 1.0, 1e-9)

    def test_combine_applies_arbitrary_matrix(self):
        builder = tf.TransformBuilder()
        builder.combine(maths.Matrix4.translation(maths.Vector3(1, 2, 3)))
        p = tf.transform(geompp.Point3D(0, 0, 0), builder.get())
        assert p == geompp.Point3D(1, 2, 3)

    def test_shear_offsets_axis_by_multiple_of_other(self):
        builder = tf.TransformBuilder()
        builder.shear(2.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        p = tf.transform(geompp.Point3D(1, 3, 5), builder.get())
        assert approx(p.x, 1.0 + 2.0 * 3.0, 1e-9)
        assert approx(p.y, 3.0, 1e-9)
        assert approx(p.z, 5.0, 1e-9)

    def test_reflect_about_x_axis_normal_flips_y(self):
        builder = tf.TransformBuilder()
        builder.reflect(maths.Vector3(0, 1, 0))
        p = tf.transform(geompp.Point3D(3, 4, 5), builder.get())
        assert approx(p.x, 3.0, 1e-9)
        assert approx(p.y, -4.0, 1e-9)
        assert approx(p.z, 5.0, 1e-9)

    def test_reflect_zero_length_normal_raises(self):
        builder = tf.TransformBuilder()
        with pytest.raises(ValueError):
            builder.reflect(maths.Vector3(0, 0, 0))

    def test_build_returns_independent_snapshot(self):
        builder = tf.TransformBuilder()
        builder.translate(maths.Vector3(1, 0, 0))
        snapshot = builder.build()
        builder.translate(maths.Vector3(0, 1, 0))
        assert snapshot == maths.Matrix4.translation(maths.Vector3(1, 0, 0))


def test_transformations_submodule_importable_both_ways():
    import geompp.transformations as tf2
    assert tf2 is tf
