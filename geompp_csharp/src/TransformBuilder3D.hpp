#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <transformations/transform_builder3d.hpp>
#pragma managed(pop)

#include "MathsMatrix.hpp"
#include "MathsVector.hpp"

namespace GeomPP { namespace Transformations {

// Fluent composer for a single 4x4 homogeneous affine transform -- see the native
// geompp::transformations::TransformBuilder3D docs (transform_builder3d.hpp) for the pre-multiply /
// call-order semantics: Translate(t).Rotate(r) moves a point by t first, then rotates the result by r.
// See TransformBuilder2D for the Matrix3-backed 2D counterpart.
public ref class TransformBuilder3D sealed {
public:
    TransformBuilder3D();
    ~TransformBuilder3D();
    !TransformBuilder3D();

    TransformBuilder3D^ Translate(Maths::Vector3^ offset);
    TransformBuilder3D^ Rotate(double angleRad, Maths::Vector3^ axis);  // throws if axis is zero-length
    TransformBuilder3D^ Scale(double factor);
    TransformBuilder3D^ Scale(double sx, double sy, double sz);
    TransformBuilder3D^ Shear(double xy, double xz, double yx, double yz, double zx, double zy);
    TransformBuilder3D^ Reflect(Maths::Vector3^ normal);  // throws if normal is zero-length
    TransformBuilder3D^ Combine(Maths::Matrix4^ mat);

    Maths::Matrix4^ Get();
    Maths::Matrix4^ Build();

internal:
    geompp::transformations::TransformBuilder3D* _native;
};

} }  // namespace GeomPP::Transformations
