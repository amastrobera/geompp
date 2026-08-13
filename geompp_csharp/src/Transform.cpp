#include "Transform.hpp"

#include "Point2D.hpp"
#include "Vector2D.hpp"
#include "LineSegment2D.hpp"
#include "Polyline2D.hpp"
#include "Triangle2D.hpp"
#include "Polygon2D.hpp"
#include "Mesh2D.hpp"
#include "PolyMesh2D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "LineSegment3D.hpp"
#include "Polyline3D.hpp"
#include "Triangle3D.hpp"
#include "Polygon3D.hpp"
#include "Mesh3D.hpp"
#include "PolyMesh3D.hpp"

namespace GeomPP { namespace Transformations {

// -- 2D fast path --

Point2D^ Transform::Translate(Point2D^ point, Maths::Vector2^ offset) {
    return gcnew Point2D(new geompp::Point2D(geompp::transformations::translate(*point->_native, *offset->_native)));
}
Point2D^ Transform::Rotate(Point2D^ point, double angleRad) {
    return gcnew Point2D(new geompp::Point2D(geompp::transformations::rotate(*point->_native, angleRad)));
}
Point2D^ Transform::Scale(Point2D^ point, double factor) {
    return gcnew Point2D(new geompp::Point2D(geompp::transformations::scale(*point->_native, factor)));
}
Point2D^ Transform::Scale(Point2D^ point, double sx, double sy) {
    return gcnew Point2D(new geompp::Point2D(geompp::transformations::scale(*point->_native, sx, sy)));
}

// -- 2D general path --

Point2D^ Transform::Apply(Point2D^ point, Maths::Matrix3^ matrix) {
    return gcnew Point2D(new geompp::Point2D(geompp::transformations::transform(*point->_native, *matrix->_native)));
}
Vector2D^ Transform::Apply(Vector2D^ vector, Maths::Matrix3^ matrix) {
    return gcnew Vector2D(new geompp::Vector2D(geompp::transformations::transform(*vector->_native, *matrix->_native)));
}
LineSegment2D^ Transform::Apply(LineSegment2D^ segment, Maths::Matrix3^ matrix) {
    return gcnew LineSegment2D(new geompp::LineSegment2D(geompp::transformations::transform(*segment->_native, *matrix->_native)));
}
Polyline2D^ Transform::Apply(Polyline2D^ polyline, Maths::Matrix3^ matrix) {
    return gcnew Polyline2D(new geompp::Polyline2D(geompp::transformations::transform(*polyline->_native, *matrix->_native)));
}
Triangle2D^ Transform::Apply(Triangle2D^ triangle, Maths::Matrix3^ matrix) {
    return gcnew Triangle2D(new geompp::Triangle2D(geompp::transformations::transform(*triangle->_native, *matrix->_native)));
}
Polygon2D^ Transform::Apply(Polygon2D^ polygon, Maths::Matrix3^ matrix) {
    return gcnew Polygon2D(new geompp::Polygon2D(geompp::transformations::transform(*polygon->_native, *matrix->_native)));
}
Mesh2D^ Transform::Apply(Mesh2D^ mesh, Maths::Matrix3^ matrix) {
    return gcnew Mesh2D(new geompp::Mesh2D(geompp::transformations::transform(*mesh->_native, *matrix->_native)));
}
PolyMesh2D^ Transform::Apply(PolyMesh2D^ mesh, Maths::Matrix3^ matrix) {
    return gcnew PolyMesh2D(new geompp::PolyMesh2D(geompp::transformations::transform(*mesh->_native, *matrix->_native)));
}

// -- 3D fast path --

Point3D^ Transform::Translate(Point3D^ point, Maths::Vector3^ offset) {
    return gcnew Point3D(new geompp::Point3D(geompp::transformations::translate(*point->_native, *offset->_native)));
}
Point3D^ Transform::Rotate(Point3D^ point, double angleRad, Maths::Vector3^ axis) {
    return gcnew Point3D(new geompp::Point3D(geompp::transformations::rotate(*point->_native, angleRad, *axis->_native)));
}
Point3D^ Transform::Scale(Point3D^ point, double factor) {
    return gcnew Point3D(new geompp::Point3D(geompp::transformations::scale(*point->_native, factor)));
}
Point3D^ Transform::Scale(Point3D^ point, double sx, double sy, double sz) {
    return gcnew Point3D(new geompp::Point3D(geompp::transformations::scale(*point->_native, sx, sy, sz)));
}

// -- 3D general path --

Point3D^ Transform::Apply(Point3D^ point, Maths::Matrix4^ matrix) {
    return gcnew Point3D(new geompp::Point3D(geompp::transformations::transform(*point->_native, *matrix->_native)));
}
Vector3D^ Transform::Apply(Vector3D^ vector, Maths::Matrix4^ matrix) {
    return gcnew Vector3D(new geompp::Vector3D(geompp::transformations::transform(*vector->_native, *matrix->_native)));
}
LineSegment3D^ Transform::Apply(LineSegment3D^ segment, Maths::Matrix4^ matrix) {
    return gcnew LineSegment3D(new geompp::LineSegment3D(geompp::transformations::transform(*segment->_native, *matrix->_native)));
}
Polyline3D^ Transform::Apply(Polyline3D^ polyline, Maths::Matrix4^ matrix) {
    return gcnew Polyline3D(new geompp::Polyline3D(geompp::transformations::transform(*polyline->_native, *matrix->_native)));
}
Triangle3D^ Transform::Apply(Triangle3D^ triangle, Maths::Matrix4^ matrix) {
    return gcnew Triangle3D(new geompp::Triangle3D(geompp::transformations::transform(*triangle->_native, *matrix->_native)));
}
Polygon3D^ Transform::Apply(Polygon3D^ polygon, Maths::Matrix4^ matrix) {
    return gcnew Polygon3D(new geompp::Polygon3D(geompp::transformations::transform(*polygon->_native, *matrix->_native)));
}
Mesh3D^ Transform::Apply(Mesh3D^ mesh, Maths::Matrix4^ matrix) {
    return gcnew Mesh3D(new geompp::Mesh3D(geompp::transformations::transform(*mesh->_native, *matrix->_native)));
}
PolyMesh3D^ Transform::Apply(PolyMesh3D^ mesh, Maths::Matrix4^ matrix) {
    return gcnew PolyMesh3D(new geompp::PolyMesh3D(geompp::transformations::transform(*mesh->_native, *matrix->_native)));
}

} }  // namespace GeomPP::Transformations
