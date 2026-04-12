#include "bind_helpers.hpp"

void bind_polygon2d(py::module_& m) {
    py::class_<geompp::Polygon2D>(m, "Polygon2D",
        "2D polygon (ordered vertex list, open/closed by convention).")
        .def_static("make", &geompp::Polygon2D::Make, "points"_a)
        .def(py::init<const geompp::Polygon2D&>())
        .def("size", &geompp::Polygon2D::Size)
        BIND_ALMOST_EQUALS(Polygon2D)
        BIND_SERIALIZATION(Polygon2D)
        .def("__len__",     &geompp::Polygon2D::Size)
        .def("__getitem__", [](const geompp::Polygon2D& p, int i) -> geompp::Point2D {
            if (i < 0) i += static_cast<int>(p.Size());
            if (i < 0 || i >= static_cast<int>(p.Size()))
                throw py::index_error("index out of range");
            return p[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Polygon2D& p) {
            py::list pts;
            for (std::size_t i = 0; i < p.Size(); ++i)
                pts.append(p[static_cast<int>(i)]);
            return pts.attr("__iter__")();
        })
        .def("__eq__", [](const geompp::Polygon2D& a, const geompp::Polygon2D& b) { return a == b; });
}
