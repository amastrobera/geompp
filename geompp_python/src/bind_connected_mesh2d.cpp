#include "bind_helpers.hpp"

void bind_connected_mesh2d(py::module_& m) {
    using Edge = geompp::detail::TriangleCompactNeighborRef::TriangleEdge;
    using FaceView2D = geompp::ConnectedMesh2D::FaceView2D;

    py::enum_<Edge>(m, "TriangleEdge",
        "Which of a triangular facet's 3 edges to cross via FaceView2D.neighbor()/neighbor_entry_edge(). "
        "INVALID is only ever returned (never passed in) — it marks a boundary edge with no twin.")
        .value("INVALID", Edge::INVALID)
        .value("FIRST", Edge::FIRST)
        .value("SECOND", Edge::SECOND)
        .value("THIRD", Edge::THIRD)
        .export_values();

    // FaceView2D holds a raw pointer back to its owning ConnectedMesh2D, so every FaceView2D handed
    // to Python (from __getitem__/__iter__/neighbor()) is kept alive together with its source object
    // via keep_alive<0, 1>: as long as the returned face lives, argument 1 (the mesh, or the face
    // neighbor() was called on) is kept from being garbage-collected too.
    py::class_<FaceView2D>(m, "FaceView2D",
        "A lightweight, chainable view onto one facet of a ConnectedMesh2D: its geometry, plus "
        "topology navigation across its 3 edges via neighbor()/neighbor_entry_edge(). Must not "
        "outlive the ConnectedMesh2D it was obtained from.")
        .def("id", &FaceView2D::ID, "This facet's index into the owning ConnectedMesh2D.")
        .def("geometry", &FaceView2D::Geometry, "This facet rebuilt as a Triangle2D.")
        .def("neighbor", &FaceView2D::Neighbor, "edge"_a, py::keep_alive<0, 1>(),
             "The facet across the given edge, or None if that edge is a boundary edge (no twin).")
        .def("neighbor_entry_edge", &FaceView2D::NeighborEntryEdge, "edge"_a,
             "Which edge of neighbor(edge) was entered through, or TriangleEdge.INVALID at a boundary.");

    py::class_<geompp::ConnectedMesh2D>(m, "ConnectedMesh2D",
        "A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. "
        "Per-facet edge adjacency is precomputed internally and exposed via FaceView2D.neighbor()/"
        "neighbor_entry_edge() on the faces returned by indexing/iteration.")
        .def_static("from_triangles",
             [](const std::vector<geompp::Triangle2D>& triangles) { return geompp::ConnectedMesh2D::FromTriangles(triangles); },
             "triangles"_a,
             "Builds a mesh from a set of triangles, welding vertices that land in the same spatial "
             "grid cell into a single shared vertex.")
        .def(py::init<const geompp::ConnectedMesh2D&>())
        .def("size", &geompp::ConnectedMesh2D::Size, "The number of facets.")
        .def("area", &geompp::ConnectedMesh2D::Area, "Sum of each input triangle's own area, independent of welding.")
        .def("__len__", &geompp::ConnectedMesh2D::Size)
        .def("__getitem__", [](const geompp::ConnectedMesh2D& mesh, int i) -> FaceView2D {
            if (i < 0) {
                i += static_cast<int>(mesh.Size());
            }
            if (i < 0 || i >= static_cast<int>(mesh.Size())) {
                throw py::index_error("index out of range");
            }
            return mesh[i];
        }, "i"_a, py::keep_alive<0, 1>())
        // Materializes eagerly into a plain py::list (not a lazy generator): a tag-based
        // keep_alive<0, 1>() here would tie the mesh's lifetime to the returned list_iterator, which
        // CPython's list_iterator doesn't support (it isn't weak-referenceable) — so instead each
        // yielded FaceView2D is individually tied to `self` via the same mechanism keep_alive uses
        // internally, exactly mirroring what __getitem__ does per element.
        .def("__iter__", [](py::object self) {
            const auto& mesh = self.cast<const geompp::ConnectedMesh2D&>();
            py::list faces;
            for (std::size_t i = 0; i < mesh.Size(); ++i) {
                py::object face = py::cast(mesh[i]);
                py::detail::keep_alive_impl(face, self);
                faces.append(face);
            }
            return faces.attr("__iter__")();
        })
        .def("polygonize", &geompp::ConnectedMesh2D::Polygonize, "params"_a = geompp::PolygonizationParams{},
             "Merges coplanar, edge-adjacent facets into polygons, per params.strategy. Returns a "
             "PolyMesh2D.");
}
