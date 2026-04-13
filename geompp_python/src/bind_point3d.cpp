#include "bind_helpers.hpp"

void bind_point3d(py::module_& m) {
    py::class_<geompp::Point3D>(m, "Point3D", "3D point (x, y, z).")
        .def(py::init<double, double, double>(), "x"_a, "y"_a, "z"_a)
        .def(py::init<const geompp::Point3D&>())
        .def_property_readonly("x", &geompp::Point3D::x)
        .def_property_readonly("y", &geompp::Point3D::y)
        .def_property_readonly("z", &geompp::Point3D::z)
        .def("to_vector",    &geompp::Point3D::ToVector)
        .def("distance_to",  &geompp::Point3D::DistanceTo, "other"_a)
        BIND_ALMOST_EQUALS(Point3D)
        BIND_SERIALIZATION(Point3D)
        .def_static("zero",  &geompp::Point3D::Zero)
        .def("__eq__",   [](const geompp::Point3D& a, const geompp::Point3D& b) { return a == b; })
        .def("__add__",  [](const geompp::Point3D& p, const geompp::Vector3D& v) { return p + v; }, "v"_a)
        .def("__sub__",  [](const geompp::Point3D& a, const geompp::Point3D& b)  -> geompp::Vector3D { return a - b; }, "other"_a)
        .def("__sub__",  [](const geompp::Point3D& p, const geompp::Vector3D& v) -> geompp::Point3D  { return p - v; }, "v"_a)
        .def("__mul__",  [](const geompp::Point3D& p, double a) { return p * a; }, "scalar"_a)
        .def("__rmul__", [](const geompp::Point3D& p, double a) { return a * p; }, "scalar"_a);
}
