#include "bind_helpers.hpp"

void bind_geometry_collection3d(py::module_& m) {
    py::class_<geompp::GeometryCollection3D>(m, "GeometryCollection3D",
        "Heterogeneous collection of 3D geometries.")
        .def(py::init<>())
        .def(py::init<const geompp::GeometryCollection3D&>())
        .def("size", &geompp::GeometryCollection3D::Size)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Point3D& p) { gc.Add(p); },
             "point"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Line3D& l) { gc.Add(l); },
             "line"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::LineSegment3D& ls) { gc.Add(ls); },
             "line_segment"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Ray3D& r) { gc.Add(r); },
             "ray"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Polyline3D& pl) { gc.Add(pl); },
             "polyline"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Triangle3D& t) { gc.Add(t); },
             "triangle"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::Polygon3D& p) { gc.Add(p); },
             "polygon"_a)
        .def("add",
             [](geompp::GeometryCollection3D& gc, const geompp::GeometryCollection3D& c) { gc.Add(c); },
             "collection"_a)
        .def("get",
             [](const geompp::GeometryCollection3D& gc, std::size_t i) -> py::object {
                 return std::visit([](const auto& v) -> py::object { return py::cast(v); }, gc.Get(i));
             },
             "index"_a)
        .def("__len__", &geompp::GeometryCollection3D::Size)
        .def("__getitem__",
             [](const geompp::GeometryCollection3D& gc, int i) -> py::object {
                 if (i < 0) i += static_cast<int>(gc.Size());
                 if (i < 0 || i >= static_cast<int>(gc.Size()))
                     throw py::index_error("index out of range");
                 return std::visit([](const auto& v) -> py::object { return py::cast(v); },
                                   gc.Get(static_cast<std::size_t>(i)));
             },
             "i"_a)
        BIND_ALMOST_EQUALS(GeometryCollection3D)
        BIND_SERIALIZATION(GeometryCollection3D)
        .def("__eq__",
             [](const geompp::GeometryCollection3D& a, const geompp::GeometryCollection3D& b) {
                 return a == b;
             });
}
