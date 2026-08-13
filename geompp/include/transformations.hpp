#pragma once

/// @file transformations.hpp
/// @brief Umbrella header for geompp::transformations -- affine transforms for geompp::geometry's
/// primitives, built on geompp::maths (Matrix3/Matrix4). See transformations2d.hpp/transformations3d.hpp
/// for the translate()/rotate()/scale() (fast, single Point, no matrix) vs. transform(primitive, matrix)
/// (general, every primitive from Point to PolyMesh) design, and transform_builder.hpp for
/// TransformBuilder, the fluent Matrix4 composer.

#include "transformations/transform_builder.hpp"
#include "transformations/transformations2d.hpp"
#include "transformations/transformations3d.hpp"
