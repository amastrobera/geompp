#include "FaceView3D.hpp"
#include "Triangle3D.hpp"
#include "ConnectedMesh3D.hpp"

namespace GeomPP {

namespace {
using NativeEdge = geompp::detail::TriangleCompactNeighborRef::TriangleEdge;

// Managed and native TriangleEdge enums are numbered independently, so the mapping is explicit
// rather than a numeric cast (native is 0-indexed to match std::array; managed is not).
NativeEdge ToNative(TriangleEdge edge) {
    switch (edge) {
        case TriangleEdge::First: return NativeEdge::FIRST;
        case TriangleEdge::Second: return NativeEdge::SECOND;
        case TriangleEdge::Third: return NativeEdge::THIRD;
        default: return NativeEdge::INVALID;
    }
}

TriangleEdge ToManaged(NativeEdge edge) {
    switch (edge) {
        case NativeEdge::FIRST: return TriangleEdge::First;
        case NativeEdge::SECOND: return TriangleEdge::Second;
        case NativeEdge::THIRD: return TriangleEdge::Third;
        default: return TriangleEdge::Invalid;
    }
}
}  // namespace

// ── Lifecycle ────────────────────────────────────────────────────────────────

FaceView3D::FaceView3D(geompp::ConnectedMesh3D::FaceView3D* native, ConnectedMesh3D^ parentMesh)
    : _native(native), _parentMesh(parentMesh) {}

FaceView3D::~FaceView3D() {
    delete _native;
    _native = nullptr;
}

FaceView3D::!FaceView3D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

int FaceView3D::Id() {
    return static_cast<int>(_native->ID());
}

Triangle3D^ FaceView3D::Geometry() {
    return gcnew Triangle3D(new geompp::Triangle3D(_native->Geometry()));
}

FaceView3D^ FaceView3D::Neighbor(TriangleEdge edge) {
    auto next = _native->Neighbor(ToNative(edge));
    if (!next.has_value()) {
        return nullptr;
    }
    return gcnew FaceView3D(new geompp::ConnectedMesh3D::FaceView3D(next.value()), _parentMesh);
}

TriangleEdge FaceView3D::NeighborEntryEdge(TriangleEdge edge) {
    return ToManaged(_native->NeighborEntryEdge(ToNative(edge)));
}

System::String^ FaceView3D::ToString() {
    return gcnew System::String(("FaceView3D[id=" + std::to_string(_native->ID()) + "]").c_str());
}

}  // namespace GeomPP
