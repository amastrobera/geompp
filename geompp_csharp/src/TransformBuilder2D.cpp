#include "TransformBuilder2D.hpp"

#include "Point2D.hpp"
#include "Vector2D.hpp"
#include "LineSegment2D.hpp"
#include "Polyline2D.hpp"
#include "Triangle2D.hpp"
#include "Polygon2D.hpp"
#include "Mesh2D.hpp"
#include "PolyMesh2D.hpp"

namespace GeomPP { namespace Transformations {

TransformBuilder2D::TransformBuilder2D() : _native(new geompp::transformations::TransformBuilder2D()) {}
TransformBuilder2D::~TransformBuilder2D() { delete _native; _native = nullptr; }
TransformBuilder2D::!TransformBuilder2D() { delete _native; _native = nullptr; }

TransformBuilder2D^ TransformBuilder2D::Translate(Maths::Vector2^ offset) {
    _native->Translate(*offset->_native);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Rotate(double angleRad) {
    _native->Rotate(angleRad);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Scale(double factor) {
    _native->Scale(factor);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Scale(double sx, double sy) {
    _native->Scale(sx, sy);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Shear(double shx, double shy) {
    _native->Shear(shx, shy);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Reflect(Maths::Vector2^ normal) {
    _native->Reflect(*normal->_native);
    return this;
}

TransformBuilder2D^ TransformBuilder2D::Combine(Maths::Matrix3^ mat) {
    _native->Combine(*mat->_native);
    return this;
}

Maths::Matrix3^ TransformBuilder2D::Get() {
    return gcnew Maths::Matrix3(new geompp::maths::Matrix3(_native->Get()));
}

Maths::Matrix3^ TransformBuilder2D::Build() {
    return gcnew Maths::Matrix3(new geompp::maths::Matrix3(_native->Build()));
}

Point2D^ TransformBuilder2D::Apply(Point2D^ point) {
    return gcnew Point2D(new geompp::Point2D(_native->Apply(*point->_native)));
}
Vector2D^ TransformBuilder2D::Apply(Vector2D^ vector) {
    return gcnew Vector2D(new geompp::Vector2D(_native->Apply(*vector->_native)));
}
LineSegment2D^ TransformBuilder2D::Apply(LineSegment2D^ segment) {
    return gcnew LineSegment2D(new geompp::LineSegment2D(_native->Apply(*segment->_native)));
}
Polyline2D^ TransformBuilder2D::Apply(Polyline2D^ polyline) {
    return gcnew Polyline2D(new geompp::Polyline2D(_native->Apply(*polyline->_native)));
}
Triangle2D^ TransformBuilder2D::Apply(Triangle2D^ triangle) {
    return gcnew Triangle2D(new geompp::Triangle2D(_native->Apply(*triangle->_native)));
}
Polygon2D^ TransformBuilder2D::Apply(Polygon2D^ polygon) {
    return gcnew Polygon2D(new geompp::Polygon2D(_native->Apply(*polygon->_native)));
}
Mesh2D^ TransformBuilder2D::Apply(Mesh2D^ mesh) {
    return gcnew Mesh2D(new geompp::Mesh2D(_native->Apply(*mesh->_native)));
}
PolyMesh2D^ TransformBuilder2D::Apply(PolyMesh2D^ mesh) {
    return gcnew PolyMesh2D(new geompp::PolyMesh2D(_native->Apply(*mesh->_native)));
}

} }  // namespace GeomPP::Transformations
