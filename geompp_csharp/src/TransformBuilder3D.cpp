#include "TransformBuilder3D.hpp"

#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "LineSegment3D.hpp"
#include "Polyline3D.hpp"
#include "Triangle3D.hpp"
#include "Polygon3D.hpp"
#include "Mesh3D.hpp"
#include "PolyMesh3D.hpp"

namespace GeomPP { namespace Transformations {

TransformBuilder3D::TransformBuilder3D() : _native(new geompp::transformations::TransformBuilder3D()) {}
TransformBuilder3D::~TransformBuilder3D() { delete _native; _native = nullptr; }
TransformBuilder3D::!TransformBuilder3D() { delete _native; _native = nullptr; }

TransformBuilder3D^ TransformBuilder3D::Translate(Maths::Vector3^ offset) {
    _native->Translate(*offset->_native);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Rotate(double angleRad, Maths::Vector3^ axis) {
    _native->Rotate(angleRad, *axis->_native);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Scale(double factor) {
    _native->Scale(factor);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Scale(double sx, double sy, double sz) {
    _native->Scale(sx, sy, sz);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Shear(double xy, double xz, double yx, double yz, double zx, double zy) {
    _native->Shear(xy, xz, yx, yz, zx, zy);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Reflect(Maths::Vector3^ normal) {
    _native->Reflect(*normal->_native);
    return this;
}

TransformBuilder3D^ TransformBuilder3D::Combine(Maths::Matrix4^ mat) {
    _native->Combine(*mat->_native);
    return this;
}

Maths::Matrix4^ TransformBuilder3D::Get() {
    return gcnew Maths::Matrix4(new geompp::maths::Matrix4(_native->Get()));
}

Maths::Matrix4^ TransformBuilder3D::Build() {
    return gcnew Maths::Matrix4(new geompp::maths::Matrix4(_native->Build()));
}

Point3D^ TransformBuilder3D::Apply(Point3D^ point) {
    return gcnew Point3D(new geompp::Point3D(_native->Apply(*point->_native)));
}
Vector3D^ TransformBuilder3D::Apply(Vector3D^ vector) {
    return gcnew Vector3D(new geompp::Vector3D(_native->Apply(*vector->_native)));
}
LineSegment3D^ TransformBuilder3D::Apply(LineSegment3D^ segment) {
    return gcnew LineSegment3D(new geompp::LineSegment3D(_native->Apply(*segment->_native)));
}
Polyline3D^ TransformBuilder3D::Apply(Polyline3D^ polyline) {
    return gcnew Polyline3D(new geompp::Polyline3D(_native->Apply(*polyline->_native)));
}
Triangle3D^ TransformBuilder3D::Apply(Triangle3D^ triangle) {
    return gcnew Triangle3D(new geompp::Triangle3D(_native->Apply(*triangle->_native)));
}
Polygon3D^ TransformBuilder3D::Apply(Polygon3D^ polygon) {
    return gcnew Polygon3D(new geompp::Polygon3D(_native->Apply(*polygon->_native)));
}
Mesh3D^ TransformBuilder3D::Apply(Mesh3D^ mesh) {
    return gcnew Mesh3D(new geompp::Mesh3D(_native->Apply(*mesh->_native)));
}
PolyMesh3D^ TransformBuilder3D::Apply(PolyMesh3D^ mesh) {
    return gcnew PolyMesh3D(new geompp::PolyMesh3D(_native->Apply(*mesh->_native)));
}

} }  // namespace GeomPP::Transformations
