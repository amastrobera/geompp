#include "bind_helpers.hpp"

void bind_geometry_collection2d(py::module_& m) {
    py::class_<geompp::GeometryCollection2D>(m, "GeometryCollection2D",
        "Heterogeneous collection of 2D geometries.")
        .def(py::init<>())
        .def(py::init<const geompp::GeometryCollection2D&>())
        .def("size", &geompp::GeometryCollection2D::Size)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Point2D& p) { gc.Add(p); },
             "point"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Line2D& l) { gc.Add(l); },
             "line"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::LineSegment2D& ls) { gc.Add(ls); },
             "line_segment"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Ray2D& r) { gc.Add(r); },
             "ray"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Polyline2D& pl) { gc.Add(pl); },
             "polyline"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Triangle2D& t) { gc.Add(t); },
             "triangle"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::Polygon2D& p) { gc.Add(p); },
             "polygon"_a)
        .def("add",
             [](geompp::GeometryCollection2D& gc, const geompp::GeometryCollection2D& c) { gc.Add(c); },
             "collection"_a)
        .def("get",
             [](const geompp::GeometryCollection2D& gc, std::size_t i) -> py::object {
                 return std::visit([](const auto& v) -> py::object { return py::cast(v); }, gc.Get(i));
             },
             "index"_a)
        .def("__len__", &geompp::GeometryCollection2D::Size)
        .def("__getitem__",
             [](const geompp::GeometryCollection2D& gc, int i) -> py::object {
                 if (i < 0) {
                     i += static_cast<int>(gc.Size());
                 }
                 if (i < 0 || i >= static_cast<int>(gc.Size())) {
                     throw py::index_error("index out of range");
                 }
                 return std::visit([](const auto& v) -> py::object { return py::cast(v); },
                                   gc.Get(static_cast<std::size_t>(i)));
             },
             "i"_a)
        BIND_ALMOST_EQUALS(GeometryCollection2D)
        BIND_SERIALIZATION(GeometryCollection2D)
        .def("__eq__",
             [](const geompp::GeometryCollection2D& a, const geompp::GeometryCollection2D& b) {
                 return a == b;
             });
}
