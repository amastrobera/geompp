#include "TransformBuilder2D.hpp"

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

} }  // namespace GeomPP::Transformations
