#include "bind_helpers.hpp"

void bind_connected_mesh3d(py::module_& m) {
    using Edge = geompp::detail::TriangleCompactNeighborRef::TriangleEdge;
    using FaceView3D = geompp::ConnectedMesh3D::FaceView3D;

    // Edge is already registered as geompp.TriangleEdge by bind_connected_mesh2d() — shared between
    // ConnectedMesh2D and ConnectedMesh3D, so it must not be registered again here.

    // FaceView3D holds a raw pointer back to its owning ConnectedMesh3D, so every FaceView3D handed
    // to Python (from __getitem__/__iter__/neighbor()) is kept alive together with its source object
    // via keep_alive<0, 1>: as long as the returned face lives, argument 1 (the mesh, or the face
    // neighbor() was called on) is kept from being garbage-collected too.
    py::class_<FaceView3D>(m, "FaceView3D",
        "A lightweight, chainable view onto one facet of a ConnectedMesh3D: its geometry, plus "
        "topology navigation across its 3 edges via neighbor()/neighbor_entry_edge(). Must not "
        "outlive the ConnectedMesh3D it was obtained from.")
        .def("id", &FaceView3D::ID, "This facet's index into the owning ConnectedMesh3D.")
        .def("geometry", &FaceView3D::Geometry, "This facet rebuilt as a Triangle3D.")
        .def("neighbor", &FaceView3D::Neighbor, "edge"_a, py::keep_alive<0, 1>(),
             "The facet across the given edge, or None if that edge is a boundary edge (no twin).")
        .def("neighbor_entry_edge", &FaceView3D::NeighborEntryEdge, "edge"_a,
             "Which edge of neighbor(edge) was entered through, or TriangleEdge.INVALID at a boundary.");

    py::class_<geompp::ConnectedMesh3D>(m, "ConnectedMesh3D",
        "A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. "
        "Per-facet edge adjacency is precomputed internally and exposed via FaceView3D.neighbor()/"
        "neighbor_entry_edge() on the faces returned by indexing/iteration.")
        .def_static("from_triangles",
             [](const std::vector<geompp::Triangle3D>& triangles) { return geompp::ConnectedMesh3D::FromTriangles(triangles); },
             "triangles"_a,
             "Builds a mesh from a set of triangles, welding vertices that land in the same spatial "
             "grid cell into a single shared vertex.")
        .def(py::init<const geompp::ConnectedMesh3D&>())
        .def("size", &geompp::ConnectedMesh3D::Size, "The number of facets.")
        .def("area", &geompp::ConnectedMesh3D::Area, "Sum of each input triangle's own area, independent of welding.")
        .def("__len__", &geompp::ConnectedMesh3D::Size)
        .def("__getitem__", [](const geompp::ConnectedMesh3D& mesh, int i) -> FaceView3D {
            if (i < 0) {
                i += static_cast<int>(mesh.Size());
            }
            if (i < 0 || i >= static_cast<int>(mesh.Size())) {
                throw py::index_error("index out of range");
            }
            return mesh[i];
        }, "i"_a, py::keep_alive<0, 1>())
        // See ConnectedMesh2D.__iter__ for why this eagerly materializes into a plain py::list with
        // per-element keep_alive_impl rather than a keep_alive-tagged lazy generator.
        .def("__iter__", [](py::object self) {
            const auto& mesh = self.cast<const geompp::ConnectedMesh3D&>();
            py::list faces;
            for (std::size_t i = 0; i < mesh.Size(); ++i) {
                py::object face = py::cast(mesh[i]);
                py::detail::keep_alive_impl(face, self);
                faces.append(face);
            }
            return faces.attr("__iter__")();
        });
}
