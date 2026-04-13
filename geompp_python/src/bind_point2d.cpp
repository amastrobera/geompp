#include "bind_helpers.hpp"

void bind_point2d(py::module_& m) {
    py::class_<geompp::Point2D>(m, "Point2D", "2D point (x, y).")
        .def(py::init<double, double>(),        "x"_a, "y"_a)
        .def(py::init<const geompp::Point2D&>())
        .def_property_readonly("x", &geompp::Point2D::x)
        .def_property_readonly("y", &geompp::Point2D::y)
        .def("to_vector",    &geompp::Point2D::ToVector)
        .def("distance_to",  &geompp::Point2D::DistanceTo, "other"_a)
        BIND_ALMOST_EQUALS(Point2D)
        BIND_SERIALIZATION(Point2D)
        .def_static("zero",  &geompp::Point2D::Zero)
        .def("__eq__",   [](const geompp::Point2D& a, const geompp::Point2D& b) { return a == b; })
        .def("__add__",  [](const geompp::Point2D& p, const geompp::Vector2D& v) { return p + v; }, "v"_a)
        .def("__sub__",  [](const geompp::Point2D& a, const geompp::Point2D& b)  -> geompp::Vector2D { return a - b; }, "other"_a)
        .def("__sub__",  [](const geompp::Point2D& p, const geompp::Vector2D& v) -> geompp::Point2D  { return p - v; }, "v"_a)
        .def("__mul__",  [](const geompp::Point2D& p, double a) { return p * a; }, "scalar"_a)
        .def("__rmul__", [](const geompp::Point2D& p, double a) { return a * p; }, "scalar"_a);
}
