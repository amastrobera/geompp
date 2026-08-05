#include "bind_helpers.hpp"

void bind_polymesh2d(py::module_& m) {
    py::class_<geompp::PolyMesh2D>(m, "PolyMesh2D",
        "A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer. "
        "No adjacency structure is stored to find a face's neighbors. Each facet has no holes.")
        .def_static("from_polygons",
             [](const std::vector<geompp::Polygon2D>& polygons) { return geompp::PolyMesh2D::FromPolygons(polygons); },
             "polygons"_a,
             "Builds a mesh from a set of hole-free polygons, welding vertices that land in the same "
             "spatial grid cell into a single shared vertex. Raises if any polygon has holes.")
        .def(py::init<const geompp::PolyMesh2D&>())
        .def("size", &geompp::PolyMesh2D::Size, "The number of facets.")
        .def("area", &geompp::PolyMesh2D::Area, "Sum of each input polygon's own area, independent of welding.")
        .def("triangulate", &geompp::PolyMesh2D::Triangulate,
             "strategy"_a = geompp::TriangulationParams::Strategy::EarClippingBestFit,
             "Returns a Mesh2D of triangles instead of n-gons — every facet is triangulated "
             "independently (already simple/CCW/collinear-free by construction, so no re-validation) "
             "and combined into one mesh.")
        .def("__len__", &geompp::PolyMesh2D::Size)
        .def("__getitem__", [](const geompp::PolyMesh2D& mesh, int i) -> geompp::Polygon2D {
            if (i < 0) {
                i += static_cast<int>(mesh.Size());
            }
            if (i < 0 || i >= static_cast<int>(mesh.Size())) {
                throw py::index_error("index out of range");
            }
            return mesh[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::PolyMesh2D& mesh) {
            py::list faces;
            for (std::size_t i = 0; i < mesh.Size(); ++i) {
                faces.append(mesh[static_cast<int>(i)]);
            }
            return faces.attr("__iter__")();
        });
}
