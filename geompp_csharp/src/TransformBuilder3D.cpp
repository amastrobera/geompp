#include "TransformBuilder3D.hpp"

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

} }  // namespace GeomPP::Transformations
