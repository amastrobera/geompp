#include "bind_helpers.hpp"
#include "view2d.hpp"

void bind_view2d(py::module_& m) {
    py::enum_<geompp::ProjectionType>(m, "ProjectionType")
        .value("XY",     geompp::ProjectionType::XY)
        .value("YZ",     geompp::ProjectionType::YZ)
        .value("ZX",     geompp::ProjectionType::ZX)
        .value("Custom", geompp::ProjectionType::Custom)
        .export_values();

    py::class_<geompp::View2D>(m, "View2D",
        "Projects 3D points onto a 2D coordinate system via x() / y() getters.\n"
        "Particularly useful when streaming large containers of Point3D into 2D algorithms:\n"
        "call view.x(p) and view.y(p) instead of constructing a whole Point2D per element.")
        .def_static("xy",       &geompp::View2D::XY,
                    "Axis-aligned projection: x→x, y→y (drops z).")
        .def_static("yz",       &geompp::View2D::YZ,
                    "Axis-aligned projection: y→x, z→y (drops x).")
        .def_static("zx",       &geompp::View2D::ZX,
                    "Axis-aligned projection: z→x, x→y (drops y).")
        .def_static("on_plane", &geompp::View2D::OnPlane, "plane"_a,
                    "Custom projection onto a Plane: (p-origin)·axis_u → x, (p-origin)·axis_v → y.")
        .def(py::init<const geompp::View2D&>())
        .def_property_readonly("type", &geompp::View2D::type)
        .def("x",
             [](const geompp::View2D& v, const geompp::Point2D& p) { return v.x(p); }, "point"_a,
             "Return the x-coordinate of a 2D point (identity pass-through).")
        .def("x",
             [](const geompp::View2D& v, const geompp::Point3D& p) { return v.x(p); }, "point"_a,
             "Project a 3D point and return the x-coordinate in this view.")
        .def("y",
             [](const geompp::View2D& v, const geompp::Point2D& p) { return v.y(p); }, "point"_a,
             "Return the y-coordinate of a 2D point (identity pass-through).")
        .def("y",
             [](const geompp::View2D& v, const geompp::Point3D& p) { return v.y(p); }, "point"_a,
             "Project a 3D point and return the y-coordinate in this view.");
}
