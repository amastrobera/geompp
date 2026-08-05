#include "FaceView3D.hpp"
#include "Triangle3D.hpp"
#include "ConnectedMesh3D.hpp"

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
