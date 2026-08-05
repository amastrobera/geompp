#include "bind_helpers.hpp"

void bind_grid_cell2d(py::module_& m) {
    py::class_<geompp::GridCell2D>(m, "GridCell2D",
        "Axis-aligned spatial hash bucket: the integer grid cell a Point2D falls into at a given "
        "resolution. Used internally to weld near-duplicate mesh vertices (see Mesh2D/PolyMesh2D).")
        .def_static("from_point",
             [](const geompp::Point2D& p) { return geompp::GridCell2D::FromPoint(p); },
             "point"_a,
             "Buckets a point into its grid cell at the default resolution (tracks DECIMAL_PRECISION).")
        .def_static("from_point",
             [](const geompp::Point2D& p, double epsilon) { return geompp::GridCell2D::FromPoint(p, epsilon); },
             "point"_a, "epsilon"_a,
             "Buckets a point into its grid cell at the given resolution.")
        .def(py::init<const geompp::GridCell2D&>())
        .def_readonly("x", &geompp::GridCell2D::x)
        .def_readonly("y", &geompp::GridCell2D::y)
        .def("__eq__", [](const geompp::GridCell2D& a, const geompp::GridCell2D& b) { return a == b; });
}
