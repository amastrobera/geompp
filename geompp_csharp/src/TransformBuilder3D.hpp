#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <transformations/transform_builder3d.hpp>
#pragma managed(pop)

#include "MathsMatrix.hpp"
#include "MathsVector.hpp"

namespace GeomPP {

ref class Point3D;
ref class Vector3D;
ref class LineSegment3D;
ref class Polyline3D;
ref class Triangle3D;
ref class Polygon3D;
ref class Mesh3D;
ref class PolyMesh3D;

namespace Transformations {

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

    // Shorthand for Transform::Apply(shape, builder.Get()) -- doesn't consume the builder, so the same
    // chain can Apply() to several different shapes.
    Point3D^        Apply(Point3D^ point);
    Vector3D^       Apply(Vector3D^ vector);
    LineSegment3D^  Apply(LineSegment3D^ segment);
    Polyline3D^     Apply(Polyline3D^ polyline);
    Triangle3D^     Apply(Triangle3D^ triangle);
    Polygon3D^      Apply(Polygon3D^ polygon);
    Mesh3D^         Apply(Mesh3D^ mesh);
    PolyMesh3D^     Apply(PolyMesh3D^ mesh);

internal:
    geompp::transformations::TransformBuilder3D* _native;
};

} }  // namespace GeomPP::Transformations
