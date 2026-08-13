#include "TransformBuilder.hpp"

namespace GeomPP { namespace Transformations {

TransformBuilder::TransformBuilder() : _native(new geompp::transformations::TransformBuilder()) {}
TransformBuilder::~TransformBuilder() { delete _native; _native = nullptr; }
TransformBuilder::!TransformBuilder() { delete _native; _native = nullptr; }

TransformBuilder^ TransformBuilder::Translate(Maths::Vector3^ offset) {
    _native->Translate(*offset->_native);
    return this;
}

TransformBuilder^ TransformBuilder::Rotate(double angleRad, Maths::Vector3^ axis) {
    _native->Rotate(angleRad, *axis->_native);
    return this;
}

TransformBuilder^ TransformBuilder::Scale(double factor) {
    _native->Scale(factor);
    return this;
}

TransformBuilder^ TransformBuilder::Scale(double sx, double sy, double sz) {
    _native->Scale(sx, sy, sz);
    return this;
}

TransformBuilder^ TransformBuilder::Combine(Maths::Matrix4^ mat) {
    _native->Combine(*mat->_native);
    return this;
}

Maths::Matrix4^ TransformBuilder::Get() {
    return gcnew Maths::Matrix4(new geompp::maths::Matrix4(_native->Get()));
}

Maths::Matrix4^ TransformBuilder::Build() {
    return gcnew Maths::Matrix4(new geompp::maths::Matrix4(_native->Build()));
}

} }  // namespace GeomPP::Transformations
