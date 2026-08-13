"""
geompp.maths binding tests (Vector2/3/4, Matrix2/3/4, solve_gauss/solve_cramer).
"""

import math
import pytest
import geompp
from geompp import maths

from ._helpers import approx


class TestMathsVector:
    def test_construction_and_component_access(self):
        v = maths.Vector3(1, 2, 3)
        assert v.x == 1 and v.y == 2 and v.z == 3
        assert v[0] == 1 and v[2] == 3

    def test_len_matches_dimension(self):
        assert len(maths.Vector2(1, 2)) == 2
        assert len(maths.Vector3(1, 2, 3)) == 3
        assert len(maths.Vector4(1, 2, 3, 4)) == 4

    def test_arithmetic(self):
        a = maths.Vector3(1, 2, 3)
        b = maths.Vector3(4, 5, 6)
        assert a + b == maths.Vector3(5, 7, 9)
        assert b - a == maths.Vector3(3, 3, 3)
        assert a * 2 == maths.Vector3(2, 4, 6)
        assert 2 * a == maths.Vector3(2, 4, 6)
        assert b / 2 == maths.Vector3(2, 2.5, 3)
        assert -a == maths.Vector3(-1, -2, -3)

    def test_dot_and_cross(self):
        x = maths.Vector3(1, 0, 0)
        y = maths.Vector3(0, 1, 0)
        assert x.dot(y) == 0
        assert x.cross(y) == maths.Vector3(0, 0, 1)

    def test_length_and_normalized(self):
        v = maths.Vector2(3, 4)
        assert approx(v.length(), 5.0)
        assert approx(v.length_squared(), 25.0)
        assert approx(v.normalized().length(), 1.0)

    def test_normalized_zero_vector_raises(self):
        with pytest.raises(ValueError):
            maths.Vector3(0, 0, 0).normalized()

    def test_getitem_out_of_range_raises(self):
        with pytest.raises(IndexError):
            maths.Vector2(1, 2)[5]


class TestMathsMatrix:
    def test_construction_row_major(self):
        mat = maths.Matrix2([1, 2, 3, 4])
        assert mat(0, 0) == 1 and mat(0, 1) == 2
        assert mat(1, 0) == 3 and mat(1, 1) == 4

    def test_wrong_element_count_raises(self):
        with pytest.raises(ValueError):
            maths.Matrix2([1, 2, 3])

    def test_identity_and_zero(self):
        ident = maths.Matrix3.identity()
        for r in range(3):
            for c in range(3):
                assert ident(r, c) == (1.0 if r == c else 0.0)
        zero = maths.Matrix3.zero()
        assert zero + ident == ident

    def test_matmul_matrix_and_vector(self):
        a = maths.Matrix3([1, 0, 0, 0, 2, 0, 0, 0, 3])
        v = maths.Vector3(1, 1, 1)
        assert a @ v == maths.Vector3(1, 2, 3)
        assert a @ maths.Matrix3.identity() == a

    def test_transpose(self):
        mat = maths.Matrix2([1, 2, 3, 4])
        t = mat.transpose()
        assert t(0, 1) == mat(1, 0)
        assert t(1, 0) == mat(0, 1)

    def test_determinant(self):
        mat = maths.Matrix2([1, 2, 3, 4])
        assert approx(mat.determinant(), 1 * 4 - 2 * 3)

    def test_inverse_round_trip(self):
        mat = maths.Matrix3([2, 0, 1, 1, 3, 2, 1, 0, 4])
        product = mat @ mat.inverse()
        for r in range(3):
            for c in range(3):
                assert approx(product(r, c), 1.0 if r == c else 0.0, 1e-9)

    def test_inverse_singular_raises(self):
        with pytest.raises(ValueError):
            maths.Matrix2([1, 2, 2, 4]).inverse()

    def test_translation(self):
        t = maths.Matrix4.translation(maths.Vector3(10, 20, 30))
        moved = t @ maths.Vector4(1, 2, 3, 1)
        assert moved == maths.Vector4(11, 22, 33, 1)

    def test_rotation_quarter_turn_about_z(self):
        r = maths.Matrix4.rotation(math.pi / 2, maths.Vector3(0, 0, 1))
        rotated = r @ maths.Vector4(1, 0, 0, 1)
        assert approx(rotated.x, 0.0, 1e-9)
        assert approx(rotated.y, 1.0, 1e-9)

    def test_rotation_zero_axis_raises(self):
        with pytest.raises(ValueError):
            maths.Matrix4.rotation(1.0, maths.Vector3(0, 0, 0))

    def test_uniform_and_nonuniform_scale(self):
        s = maths.Matrix4.scale(2.0)
        assert (s @ maths.Vector4(1, 2, 3, 1)) == maths.Vector4(2, 4, 6, 1)
        s2 = maths.Matrix4.scale(2.0, 3.0, 4.0)
        assert (s2 @ maths.Vector4(1, 1, 1, 1)) == maths.Vector4(2, 3, 4, 1)

    def test_matrix3_translation(self):
        t = maths.Matrix3.translation(maths.Vector2(10, 20))
        assert (t @ maths.Vector3(1, 2, 1)) == maths.Vector3(11, 22, 1)

    def test_matrix3_rotation_quarter_turn(self):
        r = maths.Matrix3.rotation(math.pi / 2)
        rotated = r @ maths.Vector3(1, 0, 1)
        assert approx(rotated.x, 0.0, 1e-9)
        assert approx(rotated.y, 1.0, 1e-9)

    def test_matrix3_uniform_and_nonuniform_scale(self):
        s = maths.Matrix3.scale(2.0)
        assert (s @ maths.Vector3(1, 2, 1)) == maths.Vector3(2, 4, 1)
        s2 = maths.Matrix3.scale(2.0, 3.0)
        assert (s2 @ maths.Vector3(1, 1, 1)) == maths.Vector3(2, 3, 1)


class TestMathsSolvers:
    def test_solve_gauss_known_system(self):
        a = maths.Matrix3([1, 1, 1, 0, 2, 5, 2, 5, -1])
        b = maths.Vector3(6, -4, 27)
        x = maths.solve_gauss(a, b)
        assert approx(x.x, 5.0, 1e-9)
        assert approx(x.y, 3.0, 1e-9)
        assert approx(x.z, -2.0, 1e-9)

    def test_solve_cramer_matches_gauss(self):
        a = maths.Matrix3([1, 1, 1, 0, 2, 5, 2, 5, -1])
        b = maths.Vector3(6, -4, 27)
        xg = maths.solve_gauss(a, b)
        xc = maths.solve_cramer(a, b)
        assert approx(xg.x, xc.x, 1e-9)
        assert approx(xg.y, xc.y, 1e-9)
        assert approx(xg.z, xc.z, 1e-9)

    def test_solve_gauss_singular_raises(self):
        a = maths.Matrix2([1, 2, 2, 4])
        b = maths.Vector2(1, 2)
        with pytest.raises(ValueError):
            maths.solve_gauss(a, b)

    def test_solve_cramer_singular_raises(self):
        a = maths.Matrix2([1, 2, 2, 4])
        b = maths.Vector2(1, 2)
        with pytest.raises(ValueError):
            maths.solve_cramer(a, b)


def test_maths_submodule_importable_both_ways():
    import geompp.maths as m2
    assert m2 is maths
