#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <transformations/transformations2d.hpp>
#include <transformations/transformations3d.hpp>
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
ref class Point3D;
ref class Vector3D;
ref class LineSegment3D;
ref class Polyline3D;
ref class Triangle3D;
ref class Polygon3D;
ref class Mesh3D;
ref class PolyMesh3D;

namespace Transformations {

// Static utility class -- affine transforms for GeomPP primitives, built on GeomPP.Maths
// (Matrix3/Matrix4). Translate/Rotate/Scale are the fast, single-Point2D/Point3D, no-matrix path;
// Transform(obj, matrix) is the general path covering every primitive from Point to PolyMesh -- see
// the native transformations2d.hpp/transformations3d.hpp file docs for the full design rationale.
public ref class Transform abstract sealed {
public:
    // -- 2D fast path (no matrix) --
    static Point2D^ Translate(Point2D^ point, Maths::Vector2^ offset);
    static Point2D^ Rotate(Point2D^ point, double angleRad);
    static Point2D^ Scale(Point2D^ point, double factor);
    static Point2D^ Scale(Point2D^ point, double sx, double sy);
    static Point2D^ Shear(Point2D^ point, double shx, double shy);
    static Point2D^ Reflect(Point2D^ point, Maths::Vector2^ normal);  // throws if normal is zero-length

    // -- 2D general path: Transform(obj, Matrix3) --
    static Point2D^        Apply(Point2D^ point, Maths::Matrix3^ matrix);
    static Vector2D^       Apply(Vector2D^ vector, Maths::Matrix3^ matrix);
    static LineSegment2D^  Apply(LineSegment2D^ segment, Maths::Matrix3^ matrix);
    static Polyline2D^     Apply(Polyline2D^ polyline, Maths::Matrix3^ matrix);
    static Triangle2D^     Apply(Triangle2D^ triangle, Maths::Matrix3^ matrix);
    static Polygon2D^      Apply(Polygon2D^ polygon, Maths::Matrix3^ matrix);
    static Mesh2D^         Apply(Mesh2D^ mesh, Maths::Matrix3^ matrix);
    static PolyMesh2D^     Apply(PolyMesh2D^ mesh, Maths::Matrix3^ matrix);

    // -- 3D fast path (no matrix) --
    static Point3D^ Translate(Point3D^ point, Maths::Vector3^ offset);
    static Point3D^ Rotate(Point3D^ point, double angleRad, Maths::Vector3^ axis);  // throws if axis is zero-length
    static Point3D^ Scale(Point3D^ point, double factor);
    static Point3D^ Scale(Point3D^ point, double sx, double sy, double sz);
    static Point3D^ Shear(Point3D^ point, double xy, double xz, double yx, double yz, double zx, double zy);
    static Point3D^ Reflect(Point3D^ point, Maths::Vector3^ normal);  // throws if normal is zero-length

    // -- 3D general path: Transform(obj, Matrix4) --
    static Point3D^        Apply(Point3D^ point, Maths::Matrix4^ matrix);
    static Vector3D^       Apply(Vector3D^ vector, Maths::Matrix4^ matrix);
    static LineSegment3D^  Apply(LineSegment3D^ segment, Maths::Matrix4^ matrix);
    static Polyline3D^     Apply(Polyline3D^ polyline, Maths::Matrix4^ matrix);
    static Triangle3D^     Apply(Triangle3D^ triangle, Maths::Matrix4^ matrix);
    static Polygon3D^      Apply(Polygon3D^ polygon, Maths::Matrix4^ matrix);
    static Mesh3D^         Apply(Mesh3D^ mesh, Maths::Matrix4^ matrix);
    static PolyMesh3D^     Apply(PolyMesh3D^ mesh, Maths::Matrix4^ matrix);
};

} }  // namespace GeomPP::Transformations
