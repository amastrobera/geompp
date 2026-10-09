#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <transformations/transform_builder2d.hpp>
#pragma managed(pop)

#include "MathsMatrix.hpp"
#include "MathsVector.hpp"

namespace GeomPP {

ref class Point2D;
ref class Vector2D;
ref class LineSegment2D;
ref class Polyline2D;
ref class Triangle2D;
ref class Polygon2D;
ref class Mesh2D;
ref class PolyMesh2D;

namespace Transformations {

// Fluent composer for a single 3x3 homogeneous affine transform -- 2D counterpart of
// TransformBuilder3D (same pre-multiply / call-order semantics, see its docs); Rotate() takes a
// plain angle (no axis) and Shear() takes 2 terms rather than 6.
public ref class TransformBuilder2D sealed {
public:
    TransformBuilder2D();
    ~TransformBuilder2D();
    !TransformBuilder2D();

    TransformBuilder2D^ Translate(Maths::Vector2^ offset);
    TransformBuilder2D^ Rotate(double angleRad);
    TransformBuilder2D^ Scale(double factor);
    TransformBuilder2D^ Scale(double sx, double sy);
    TransformBuilder2D^ Shear(double shx, double shy);
    TransformBuilder2D^ Reflect(Maths::Vector2^ normal);  // throws if normal is zero-length
    TransformBuilder2D^ Combine(Maths::Matrix3^ mat);

    Maths::Matrix3^ Get();
    Maths::Matrix3^ Build();

    // Shorthand for Transform::Apply(shape, builder.Get()) -- doesn't consume the builder, so the same
    // chain can Apply() to several different shapes.
    Point2D^        Apply(Point2D^ point);
    Vector2D^       Apply(Vector2D^ vector);
    LineSegment2D^  Apply(LineSegment2D^ segment);
    Polyline2D^     Apply(Polyline2D^ polyline);
    Triangle2D^     Apply(Triangle2D^ triangle);
    Polygon2D^      Apply(Polygon2D^ polygon);
    Mesh2D^         Apply(Mesh2D^ mesh);
    PolyMesh2D^     Apply(PolyMesh2D^ mesh);

internal:
    geompp::transformations::TransformBuilder2D* _native;
};

} }  // namespace GeomPP::Transformations
