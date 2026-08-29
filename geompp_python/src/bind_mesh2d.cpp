#include "bind_helpers.hpp"

void bind_mesh2d(py::module_& m) {
    py::class_<geompp::Mesh2D>(m, "Mesh2D",
        "A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. "
        "No adjacency structure is stored to find a face's neighbors.")
        .def_static("from_triangles",
             [](const std::vector<geompp::Triangle2D>& triangles, geompp::AdjacencyConformity conformity) {
                 return geompp::Mesh2D::FromTriangles(triangles, conformity);
             },
             "triangles"_a, "conformity"_a = geompp::AdjacencyConformity::Assert,
             "Builds a mesh from a set of triangles, welding vertices that land in the same spatial "
             "grid cell into a single shared vertex. conformity (see AdjacencyConformity) decides how "
             "a mesh-conformity violation (a T-junction or non-manifold edge) is handled: Assert "
             "(default) raises, Enforce auto-repairs every T-junction via fix_adjacency(), Guaranteed "
             "skips the check.")
        .def(py::init<const geompp::Mesh2D&>())
        .def("size", &geompp::Mesh2D::Size, "The number of facets.")
        .def("area", &geompp::Mesh2D::Area, "Sum of each input triangle's own area, independent of welding.")
        .def("__len__", &geompp::Mesh2D::Size)
        .def("__getitem__", [](const geompp::Mesh2D& mesh, int i) -> geompp::Triangle2D {
            if (i < 0) {
                i += static_cast<int>(mesh.Size());
            }
            if (i < 0 || i >= static_cast<int>(mesh.Size())) {
                throw py::index_error("index out of range");
            }
            return mesh[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Mesh2D& mesh) {
            py::list faces;
            for (std::size_t i = 0; i < mesh.Size(); ++i) {
                faces.append(mesh[static_cast<int>(i)]);
            }
            return faces.attr("__iter__")();
        })
        .def("connect", &geompp::Mesh2D::Connect,
             "Same facets/vertices, plus precomputed per-facet edge adjacency (see ConnectedMesh2D).")
        .def("polygonize", &geompp::Mesh2D::Polygonize, "params"_a = geompp::PolygonizationParams{},
             "Merges coplanar, edge-adjacent facets into polygons, per params.strategy. Returns a "
             "PolyMesh2D.");
}
