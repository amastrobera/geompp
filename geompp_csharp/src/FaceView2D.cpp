#include "FaceView2D.hpp"
#include "Triangle2D.hpp"
#include "ConnectedMesh2D.hpp"

namespace GeomPP {

namespace {
using NativeEdge = geompp::detail::TriangleCompactNeighborRef::TriangleEdge;

NativeEdge ToNative(TriangleEdge edge) {
    return static_cast<NativeEdge>(static_cast<int>(edge));
}

TriangleEdge ToManaged(NativeEdge edge) {
    return static_cast<TriangleEdge>(static_cast<int>(edge));
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
