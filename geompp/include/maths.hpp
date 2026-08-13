#pragma once

/// @file maths.hpp
/// @brief Umbrella header for geompp::maths -- fixed-size linear algebra (Vector<T,N>/Matrix<T,Rows,Cols>
/// and their Vector2/3/4, Matrix2/3/4 aliases) plus the solve_gauss()/solve_cramer() system solvers.
/// Deliberately independent of geompp::geometry -- see maths/vector.hpp's class docs for why
/// Vector2/3/4 are distinct types from geometry::Vector2D/Vector3D. geompp::transformations builds on top
/// of this module.

#include "maths/matrix.hpp"
#include "maths/maths_concepts.hpp"
#include "maths/solvers.hpp"
#include "maths/vector.hpp"
