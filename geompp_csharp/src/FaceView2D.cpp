#include "FaceView2D.hpp"
#include "Triangle2D.hpp"
#include "ConnectedMesh2D.hpp"

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

FaceView2D::FaceView2D(geompp::ConnectedMesh2D::FaceView2D* native, ConnectedMesh2D^ parentMesh)
    : _native(native), _parentMesh(parentMesh) {}

FaceView2D::~FaceView2D() {
    delete _native;
    _native = nullptr;
}

FaceView2D::!FaceView2D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

int FaceView2D::Id() {
    return static_cast<int>(_native->ID());
}

Triangle2D^ FaceView2D::Geometry() {
    return gcnew Triangle2D(new geompp::Triangle2D(_native->Geometry()));
}

FaceView2D^ FaceView2D::Neighbor(TriangleEdge edge) {
    auto next = _native->Neighbor(ToNative(edge));
    if (!next.has_value()) {
        return nullptr;
    }
    return gcnew FaceView2D(new geompp::ConnectedMesh2D::FaceView2D(next.value()), _parentMesh);
}

TriangleEdge FaceView2D::NeighborEntryEdge(TriangleEdge edge) {
    return ToManaged(_native->NeighborEntryEdge(ToNative(edge)));
}

System::String^ FaceView2D::ToString() {
    return gcnew System::String(("FaceView2D[id=" + std::to_string(_native->ID()) + "]").c_str());
}

}  // namespace GeomPP
