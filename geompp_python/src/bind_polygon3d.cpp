#include "bind_helpers.hpp"

void bind_polygon3d(py::module_& m) {
    py::class_<geompp::Polygon3D>(m, "Polygon3D",
        "3D polygon (ordered vertex list).")
        .def_static("make", &geompp::Polygon3D::Make, "points"_a)
        .def(py::init<const geompp::Polygon3D&>())
        .def("size", &geompp::Polygon3D::Size)
        BIND_ALMOST_EQUALS(Polygon3D)
        BIND_SERIALIZATION(Polygon3D)
        .def("__len__",     &geompp::Polygon3D::Size)
        .def("__getitem__", [](const geompp::Polygon3D& p, int i) -> geompp::Point3D {
            if (i < 0) i += static_cast<int>(p.Size());
            if (i < 0 || i >= static_cast<int>(p.Size()))
                throw py::index_error("index out of range");
            return p[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Polygon3D& p) {
            py::list pts;
            for (std::size_t i = 0; i < p.Size(); ++i)
                pts.append(p[static_cast<int>(i)]);
            return pts.attr("__iter__")();
        })
        .def("__eq__", [](const geompp::Polygon3D& a, const geompp::Polygon3D& b) { return a == b; });
}
