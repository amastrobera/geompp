#include "bind_helpers.hpp"

void bind_grid_cell3d(py::module_& m) {
    py::class_<geompp::GridCell3D>(m, "GridCell3D",
        "Axis-aligned spatial hash bucket: the integer grid cell a Point3D falls into at a given "
        "resolution. Used internally to weld near-duplicate mesh vertices (see Mesh3D/PolyMesh3D).")
        .def_static("from_point",
             [](const geompp::Point3D& p) { return geompp::GridCell3D::FromPoint(p); },
             "point"_a,
             "Buckets a point into its grid cell at the default resolution (tracks DECIMAL_PRECISION).")
        .def_static("from_point",
             [](const geompp::Point3D& p, double epsilon) { return geompp::GridCell3D::FromPoint(p, epsilon); },
             "point"_a, "epsilon"_a,
             "Buckets a point into its grid cell at the given resolution.")
        .def(py::init<const geompp::GridCell3D&>())
        .def_readonly("x", &geompp::GridCell3D::x)
        .def_readonly("y", &geompp::GridCell3D::y)
        .def_readonly("z", &geompp::GridCell3D::z)
        .def("__eq__", [](const geompp::GridCell3D& a, const geompp::GridCell3D& b) { return a == b; });
}
