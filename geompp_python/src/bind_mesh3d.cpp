#include "bind_helpers.hpp"

void bind_mesh3d(py::module_& m) {
    py::class_<geompp::Mesh3D>(m, "Mesh3D",
        "A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. "
        "No adjacency structure is stored to find a face's neighbors.")
        .def_static("from_triangles",
             [](const std::vector<geompp::Triangle3D>& triangles) { return geompp::Mesh3D::FromTriangles(triangles); },
             "triangles"_a,
             "Builds a mesh from a set of triangles, welding vertices that land in the same spatial "
             "grid cell into a single shared vertex.")
        .def(py::init<const geompp::Mesh3D&>())
        .def("size", &geompp::Mesh3D::Size, "The number of facets.")
        .def("area", &geompp::Mesh3D::Area, "Sum of each input triangle's own area, independent of welding.")
        .def("__len__", &geompp::Mesh3D::Size)
        .def("__getitem__", [](const geompp::Mesh3D& mesh, int i) -> geompp::Triangle3D {
            if (i < 0) {
                i += static_cast<int>(mesh.Size());
            }
            if (i < 0 || i >= static_cast<int>(mesh.Size())) {
                throw py::index_error("index out of range");
            }
            return mesh[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Mesh3D& mesh) {
            py::list faces;
            for (std::size_t i = 0; i < mesh.Size(); ++i) {
                faces.append(mesh[static_cast<int>(i)]);
            }
            return faces.attr("__iter__")();
        });
}
