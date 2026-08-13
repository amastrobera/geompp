#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <transformations/transform_builder.hpp>
#pragma managed(pop)

#include "MathsMatrix.hpp"
#include "MathsVector.hpp"

namespace GeomPP { namespace Transformations {

// Fluent composer for a single 4x4 homogeneous affine transform -- see the native
// geompp::transformations::TransformBuilder docs (transform_builder.hpp) for the pre-multiply /
// call-order semantics: Translate(t).Rotate(r) moves a point by t first, then rotates the result by r.
public ref class TransformBuilder sealed {
public:
    TransformBuilder();
    ~TransformBuilder();
    !TransformBuilder();

    TransformBuilder^ Translate(Maths::Vector3^ offset);
    TransformBuilder^ Rotate(double angleRad, Maths::Vector3^ axis);  // throws if axis is zero-length
    TransformBuilder^ Scale(double factor);
    TransformBuilder^ Scale(double sx, double sy, double sz);
    TransformBuilder^ Shear(double xy, double xz, double yx, double yz, double zx, double zy);
    TransformBuilder^ Reflect(Maths::Vector3^ normal);  // throws if normal is zero-length
    TransformBuilder^ Combine(Maths::Matrix4^ mat);

    Maths::Matrix4^ Get();
    Maths::Matrix4^ Build();

internal:
    geompp::transformations::TransformBuilder* _native;
};

} }  // namespace GeomPP::Transformations
