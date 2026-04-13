#include "bind_helpers.hpp"

void bind_vector2d(py::module_& m) {
    py::class_<geompp::Vector2D>(m, "Vector2D", "2D vector (x, y).")
        .def(py::init<double, double>(),          "x"_a, "y"_a)
        .def(py::init<const geompp::Vector2D&>())
        .def_property_readonly("x", &geompp::Vector2D::x)
        .def_property_readonly("y", &geompp::Vector2D::y)
        .def("to_point",  &geompp::Vector2D::ToPoint)
        .def("length",    &geompp::Vector2D::Length)
        .def("dot",       &geompp::Vector2D::Dot,       "v"_a)
        .def("cross",     &geompp::Vector2D::Cross,     "v"_a)
        .def("perp",      &geompp::Vector2D::Perp)
        .def("normalize", &geompp::Vector2D::Normalize)
        BIND_ALMOST_EQUALS(Vector2D)
        BIND_SERIALIZATION(Vector2D)
        .def_static("basis_x", &geompp::Vector2D::BasisX)
        .def_static("basis_y", &geompp::Vector2D::BasisY)
        .def("__eq__",      [](const geompp::Vector2D& a, const geompp::Vector2D& b) { return a == b; })
        .def("__neg__",     [](const geompp::Vector2D& v) { return -v; })
        .def("__add__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> geompp::Vector2D { return a + b; }, "v"_a)
        .def("__add__",     [](const geompp::Vector2D& v, const geompp::Point2D& p)   -> geompp::Point2D  { return v + p; }, "p"_a)
        .def("__sub__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> geompp::Vector2D { return a - b; }, "v"_a)
        .def("__mul__",     [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return v * a; }, "scalar"_a)
        .def("__mul__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> double           { return a * b; }, "v"_a)
        .def("__rmul__",    [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return a * v; }, "scalar"_a)
        .def("__truediv__", [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return v / a; }, "scalar"_a);
}
