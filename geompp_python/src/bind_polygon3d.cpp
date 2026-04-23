#include "bind_helpers.hpp"

void bind_polygon3d(py::module_& m) {
    py::class_<geompp::Polygon3D>(m, "Polygon3D",
        "3D polygon (ordered vertex list).")
        .def_static("make", &geompp::Polygon3D::Make, "points"_a)
        .def(py::init<const geompp::Polygon3D&>())
        .def("size",        &geompp::Polygon3D::Size)
        .def("centroid",    &geompp::Polygon3D::Centroid)
        .def("signed_area", &geompp::Polygon3D::SignedArea)
        .def("area",        &geompp::Polygon3D::Area)
        .def("perimeter",   &geompp::Polygon3D::Perimeter)
        .def("distance_to", &geompp::Polygon3D::DistanceTo,  "point"_a)
        .def("location",    &geompp::Polygon3D::Location,    "point"_a)
        .def("interpolate", &geompp::Polygon3D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polygon3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Polygon3D)
        BIND_SERIALIZATION(Polygon3D)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::Line3D& l) { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::Ray3D& r) { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::LineSegment3D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::Line3D& l) { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::Ray3D& r) { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::LineSegment3D& s) { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
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
