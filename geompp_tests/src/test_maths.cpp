#include "maths.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numbers>

namespace m = geompp::maths;

#pragma region Vector

TEST(MathsVectorTest, Construction_ComponentAccessMatchesCtorArgs) {
  m::Vector3 v(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(v.x(), 1.0);
  EXPECT_DOUBLE_EQ(v.y(), 2.0);
  EXPECT_DOUBLE_EQ(v.z(), 3.0);
  EXPECT_DOUBLE_EQ(v[0], 1.0);
  EXPECT_DOUBLE_EQ(v[2], 3.0);
}

TEST(MathsVectorTest, DefaultConstruction_IsZero) {
  m::Vector4 v;
  EXPECT_TRUE(v == m::Vector4::Zero());
}

TEST(MathsVectorTest, At_OutOfRange_Throws) {
  m::Vector2 v(1.0, 2.0);
  EXPECT_THROW(v.At(2), std::out_of_range);
}

TEST(MathsVectorTest, Arithmetic_AddSubScalarMulDiv) {
  m::Vector3 a(1.0, 2.0, 3.0);
  m::Vector3 b(4.0, 5.0, 6.0);
  EXPECT_TRUE((a + b) == m::Vector3(5.0, 7.0, 9.0));
  EXPECT_TRUE((b - a) == m::Vector3(3.0, 3.0, 3.0));
  EXPECT_TRUE((a * 2.0) == m::Vector3(2.0, 4.0, 6.0));
  EXPECT_TRUE((2.0 * a) == m::Vector3(2.0, 4.0, 6.0));
  EXPECT_TRUE((b / 2.0) == m::Vector3(2.0, 2.5, 3.0));
  EXPECT_TRUE((-a) == m::Vector3(-1.0, -2.0, -3.0));
}

TEST(MathsVectorTest, DivideByZero_Throws) {
  m::Vector2 v(1.0, 2.0);
  EXPECT_THROW(v / 0.0, std::invalid_argument);
}

TEST(MathsVectorTest, Dot_OrthogonalUnitVectors_IsZero) {
  m::Vector3 x(1.0, 0.0, 0.0);
  m::Vector3 y(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(x.Dot(y), 0.0);
  EXPECT_DOUBLE_EQ(x.Dot(x), 1.0);
}

TEST(MathsVectorTest, Cross_UnitXCrossUnitY_IsUnitZ) {
  m::Vector3 x(1.0, 0.0, 0.0);
  m::Vector3 y(0.0, 1.0, 0.0);
  EXPECT_TRUE(x.Cross(y) == m::Vector3(0.0, 0.0, 1.0));
}

TEST(MathsVectorTest, Length_KnownTriple_MatchesPythagoras) {
  m::Vector2 v(3.0, 4.0);
  EXPECT_DOUBLE_EQ(v.Length(), 5.0);
  EXPECT_DOUBLE_EQ(v.LengthSquared(), 25.0);
}

TEST(MathsVectorTest, Normalized_ScalesToUnitLength) {
  m::Vector2 v(3.0, 4.0);
  auto n = v.Normalized();
  EXPECT_NEAR(n.Length(), 1.0, 1e-12);
}

TEST(MathsVectorTest, Normalized_ZeroVector_Throws) {
  m::Vector3 v;
  EXPECT_THROW(v.Normalized(), std::invalid_argument);
}

TEST(MathsVectorTest, Transpose_ProducesOneByNRowMatrix) {
  m::Vector3 v(1.0, 2.0, 3.0);
  auto row = v.Transpose();
  EXPECT_EQ(row.RowCount(), 1u);
  EXPECT_EQ(row.ColCount(), 3u);
  EXPECT_DOUBLE_EQ(row(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(row(0, 2), 3.0);
}

#pragma endregion

#pragma region Matrix

TEST(MathsMatrixTest, Construction_RowMajorElementOrder) {
  m::Matrix2 mat(1.0, 2.0, 3.0, 4.0);
  EXPECT_DOUBLE_EQ(mat(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(mat(0, 1), 2.0);
  EXPECT_DOUBLE_EQ(mat(1, 0), 3.0);
  EXPECT_DOUBLE_EQ(mat(1, 1), 4.0);
}

TEST(MathsMatrixTest, Identity_DiagonalIsOne) {
  auto id = m::Matrix3::Identity();
  for (std::size_t r = 0; r < 3; ++r) {
    for (std::size_t c = 0; c < 3; ++c) {
      EXPECT_DOUBLE_EQ(id(r, c), r == c ? 1.0 : 0.0);
    }
  }
}

TEST(MathsMatrixTest, At_OutOfRange_Throws) {
  m::Matrix2 mat;
  EXPECT_THROW(mat.At(2, 0), std::out_of_range);
  EXPECT_THROW(mat.At(0, 5), std::out_of_range);
}

TEST(MathsMatrixTest, ScalarArithmetic) {
  m::Matrix2 mat(1.0, 2.0, 3.0, 4.0);
  EXPECT_TRUE((mat * 2.0) == m::Matrix2(2.0, 4.0, 6.0, 8.0));
  EXPECT_TRUE((mat + mat) == (mat * 2.0));
  EXPECT_TRUE((mat - mat) == m::Matrix2::Zero());
}

TEST(MathsMatrixTest, DivideByZero_Throws) {
  m::Matrix2 mat(1.0, 2.0, 3.0, 4.0);
  EXPECT_THROW(mat / 0.0, std::invalid_argument);
}

TEST(MathsMatrixTest, MatrixTimesMatrix_CompatibleSizes_ProducesCorrectProduct) {
  // 2x3 * 3x2 -> 2x2
  m::Matrix<double, 2, 3> a(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  m::Matrix<double, 3, 2> b(7.0, 8.0, 9.0, 10.0, 11.0, 12.0);
  auto c = a * b;
  static_assert(std::is_same_v<decltype(c), m::Matrix<double, 2, 2>>);
  EXPECT_DOUBLE_EQ(c(0, 0), 1 * 7 + 2 * 9 + 3 * 11);
  EXPECT_DOUBLE_EQ(c(0, 1), 1 * 8 + 2 * 10 + 3 * 12);
  EXPECT_DOUBLE_EQ(c(1, 0), 4 * 7 + 5 * 9 + 6 * 11);
  EXPECT_DOUBLE_EQ(c(1, 1), 4 * 8 + 5 * 10 + 6 * 12);
}

TEST(MathsMatrixTest, MatrixTimesIdentity_ReturnsSameMatrix) {
  m::Matrix3 mat(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0);
  EXPECT_TRUE((mat * m::Matrix3::Identity()) == mat);
}

TEST(MathsMatrixTest, MatrixTimesVector_TreatsVectorAsColumn) {
  m::Matrix3 mat(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0);
  m::Vector3 v(1.0, 1.0, 1.0);
  EXPECT_TRUE((mat * v) == m::Vector3(1.0, 2.0, 3.0));
}

TEST(MathsMatrixTest, Transpose_SwapsRowsAndColumns) {
  m::Matrix<double, 2, 3> a(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  auto t = a.Transpose();
  static_assert(std::is_same_v<decltype(t), m::Matrix<double, 3, 2>>);
  EXPECT_DOUBLE_EQ(t(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(t(1, 0), 2.0);
  EXPECT_DOUBLE_EQ(t(2, 1), 6.0);
}

TEST(MathsMatrixTest, Determinant_KnownMatrix2_MatchesHandComputation) {
  m::Matrix2 mat(1.0, 2.0, 3.0, 4.0);
  EXPECT_DOUBLE_EQ(mat.Determinant(), 1.0 * 4.0 - 2.0 * 3.0);
}

TEST(MathsMatrixTest, Determinant_Identity_IsOneForAnySize) {
  EXPECT_DOUBLE_EQ(m::Matrix2::Identity().Determinant(), 1.0);
  EXPECT_DOUBLE_EQ(m::Matrix3::Identity().Determinant(), 1.0);
  EXPECT_DOUBLE_EQ(m::Matrix4::Identity().Determinant(), 1.0);
}

TEST(MathsMatrixTest, Inverse_TimesOriginal_IsIdentity) {
  m::Matrix3 mat(2.0, 0.0, 1.0, 1.0, 3.0, 2.0, 1.0, 0.0, 4.0);
  auto inv = mat.Inverse();
  auto product = mat * inv;
  for (std::size_t r = 0; r < 3; ++r) {
    for (std::size_t c = 0; c < 3; ++c) {
      EXPECT_NEAR(product(r, c), r == c ? 1.0 : 0.0, 1e-9);
    }
  }
}

TEST(MathsMatrixTest, Inverse_SingularMatrix_Throws) {
  // Row 2 = 2 * Row 1 -> singular.
  m::Matrix2 mat(1.0, 2.0, 2.0, 4.0);
  EXPECT_THROW(mat.Inverse(), std::invalid_argument);
}

TEST(MathsMatrixTest, Translation_MovesHomogeneousPoint) {
  auto t = m::Matrix4::Translation(m::Vector3(10.0, 20.0, 30.0));
  m::Vector4 p(1.0, 2.0, 3.0, 1.0);  // homogeneous point
  auto moved = t * p;
  EXPECT_TRUE(moved == m::Vector4(11.0, 22.0, 33.0, 1.0));
}

TEST(MathsMatrixTest, Rotation_QuarterTurnAboutZ_RotatesXOntoY) {
  auto r = m::Matrix4::Rotation(std::numbers::pi / 2.0, m::Vector3(0.0, 0.0, 1.0));
  m::Vector4 p(1.0, 0.0, 0.0, 1.0);
  auto rotated = r * p;
  EXPECT_NEAR(rotated.x(), 0.0, 1e-9);
  EXPECT_NEAR(rotated.y(), 1.0, 1e-9);
  EXPECT_NEAR(rotated.z(), 0.0, 1e-9);
}

TEST(MathsMatrixTest, Rotation_ZeroLengthAxis_Throws) {
  EXPECT_THROW(m::Matrix4::Rotation(1.0, m::Vector3(0.0, 0.0, 0.0)), std::invalid_argument);
}

TEST(MathsMatrixTest, UniformScale_ScalesHomogeneousPoint) {
  auto s = m::Matrix4::Scale(2.0);
  m::Vector4 p(1.0, 2.0, 3.0, 1.0);
  EXPECT_TRUE((s * p) == m::Vector4(2.0, 4.0, 6.0, 1.0));
}

TEST(MathsMatrixTest, NonUniformScale_ScalesEachAxisIndependently) {
  auto s = m::Matrix4::Scale(2.0, 3.0, 4.0);
  m::Vector4 p(1.0, 1.0, 1.0, 1.0);
  EXPECT_TRUE((s * p) == m::Vector4(2.0, 3.0, 4.0, 1.0));
}

TEST(MathsMatrixTest, Matrix3_Translation_MovesHomogeneousPoint) {
  auto t = m::Matrix3::Translation(m::Vector2(10.0, 20.0));
  m::Vector3 p(1.0, 2.0, 1.0);  // homogeneous 2D point
  EXPECT_TRUE((t * p) == m::Vector3(11.0, 22.0, 1.0));
}

TEST(MathsMatrixTest, Matrix3_Rotation_QuarterTurn_RotatesXOntoY) {
  auto r = m::Matrix3::Rotation(std::numbers::pi / 2.0);
  m::Vector3 p(1.0, 0.0, 1.0);
  auto rotated = r * p;
  EXPECT_NEAR(rotated.x(), 0.0, 1e-9);
  EXPECT_NEAR(rotated.y(), 1.0, 1e-9);
}

TEST(MathsMatrixTest, Matrix3_UniformAndNonUniformScale) {
  auto s = m::Matrix3::Scale(2.0);
  EXPECT_TRUE((s * m::Vector3(1.0, 2.0, 1.0)) == m::Vector3(2.0, 4.0, 1.0));
  auto s2 = m::Matrix3::Scale(2.0, 3.0);
  EXPECT_TRUE((s2 * m::Vector3(1.0, 1.0, 1.0)) == m::Vector3(2.0, 3.0, 1.0));
}

TEST(MathsMatrixTest, Shear_OffsetsAxisByMultipleOfOther) {
  auto sh = m::Matrix4::Shear(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_TRUE(sh == m::Matrix4::Identity());

  // xy = 2: x' = x + 2*y, y and z unchanged.
  auto shxy = m::Matrix4::Shear(2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  m::Vector4 p(1.0, 3.0, 5.0, 1.0);
  auto sheared = shxy * p;
  EXPECT_DOUBLE_EQ(sheared.x(), 1.0 + 2.0 * 3.0);
  EXPECT_DOUBLE_EQ(sheared.y(), 3.0);
  EXPECT_DOUBLE_EQ(sheared.z(), 5.0);
}

TEST(MathsMatrixTest, Matrix3_Shear_OffsetsAxisByMultipleOfOther) {
  auto shxy = m::Matrix3::Shear(2.0, 0.0);
  m::Vector3 p(1.0, 3.0, 1.0);
  auto sheared = shxy * p;
  EXPECT_DOUBLE_EQ(sheared.x(), 1.0 + 2.0 * 3.0);
  EXPECT_DOUBLE_EQ(sheared.y(), 3.0);
}

TEST(MathsMatrixTest, Reflection_AboutXAxisNormal_FlipsY) {
  // Reflecting across the line through the origin with normal (0, 1) (the X axis) flips Y, keeps X.
  auto r = m::Matrix4::Reflection(m::Vector3(0.0, 1.0, 0.0));
  m::Vector4 p(3.0, 4.0, 5.0, 1.0);
  auto reflected = r * p;
  EXPECT_NEAR(reflected.x(), 3.0, 1e-9);
  EXPECT_NEAR(reflected.y(), -4.0, 1e-9);
  EXPECT_NEAR(reflected.z(), 5.0, 1e-9);
}

TEST(MathsMatrixTest, Reflection_AppliedTwice_IsIdentity) {
  auto r = m::Matrix4::Reflection(m::Vector3(1.0, 2.0, 3.0));
  auto r2 = r * r;
  for (std::size_t row = 0; row < 4; ++row) {
    for (std::size_t col = 0; col < 4; ++col) {
      EXPECT_NEAR(r2(row, col), row == col ? 1.0 : 0.0, 1e-9);
    }
  }
}

TEST(MathsMatrixTest, Reflection_ZeroLengthNormal_Throws) {
  EXPECT_THROW(m::Matrix4::Reflection(m::Vector3(0.0, 0.0, 0.0)), std::invalid_argument);
}

TEST(MathsMatrixTest, Matrix3_Reflection_AboutXAxisNormal_FlipsY) {
  auto r = m::Matrix3::Reflection(m::Vector2(0.0, 1.0));
  m::Vector3 p(3.0, 4.0, 1.0);
  auto reflected = r * p;
  EXPECT_NEAR(reflected.x(), 3.0, 1e-9);
  EXPECT_NEAR(reflected.y(), -4.0, 1e-9);
}

TEST(MathsMatrixTest, Matrix3_Reflection_AppliedTwice_IsIdentity) {
  auto r = m::Matrix3::Reflection(m::Vector2(1.0, 2.0));
  auto r2 = r * r;
  for (std::size_t row = 0; row < 3; ++row) {
    for (std::size_t col = 0; col < 3; ++col) {
      EXPECT_NEAR(r2(row, col), row == col ? 1.0 : 0.0, 1e-9);
    }
  }
}

TEST(MathsMatrixTest, Matrix3_Reflection_ZeroLengthNormal_Throws) {
  EXPECT_THROW(m::Matrix3::Reflection(m::Vector2(0.0, 0.0)), std::invalid_argument);
}

#pragma endregion

#pragma region Solvers

TEST(MathsSolversTest, SolveGauss_KnownSystem_MatchesHandSolution) {
  // x + y + z = 6, 2y + 5z = -4, 2x + 5y - z = 27  ->  x=5, y=3, z=-2 (classic textbook example)
  m::Matrix3 a(1.0, 1.0, 1.0, 0.0, 2.0, 5.0, 2.0, 5.0, -1.0);
  m::Vector3 b(6.0, -4.0, 27.0);
  auto x = m::solve_gauss(a, b);
  EXPECT_NEAR(x.x(), 5.0, 1e-9);
  EXPECT_NEAR(x.y(), 3.0, 1e-9);
  EXPECT_NEAR(x.z(), -2.0, 1e-9);
}

TEST(MathsSolversTest, SolveCramer_KnownSystem_MatchesHandSolution) {
  m::Matrix3 a(1.0, 1.0, 1.0, 0.0, 2.0, 5.0, 2.0, 5.0, -1.0);
  m::Vector3 b(6.0, -4.0, 27.0);
  auto x = m::solve_cramer(a, b);
  EXPECT_NEAR(x.x(), 5.0, 1e-9);
  EXPECT_NEAR(x.y(), 3.0, 1e-9);
  EXPECT_NEAR(x.z(), -2.0, 1e-9);
}

TEST(MathsSolversTest, GaussAndCramer_AgreeOnRandomishSystem) {
  m::Matrix2 a(4.0, 3.0, 6.0, 3.0);
  m::Vector2 b(1.0, 2.0);
  auto xg = m::solve_gauss(a, b);
  auto xc = m::solve_cramer(a, b);
  EXPECT_NEAR(xg.x(), xc.x(), 1e-9);
  EXPECT_NEAR(xg.y(), xc.y(), 1e-9);
}

TEST(MathsSolversTest, SolveGauss_SingularSystem_Throws) {
  m::Matrix2 a(1.0, 2.0, 2.0, 4.0);
  m::Vector2 b(1.0, 2.0);
  EXPECT_THROW(m::solve_gauss(a, b), std::invalid_argument);
}

TEST(MathsSolversTest, SolveCramer_SingularSystem_Throws) {
  m::Matrix2 a(1.0, 2.0, 2.0, 4.0);
  m::Vector2 b(1.0, 2.0);
  EXPECT_THROW(m::solve_cramer(a, b), std::invalid_argument);
}

#pragma endregion
