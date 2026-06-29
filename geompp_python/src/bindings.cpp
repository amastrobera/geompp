#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_precision(py::module_& m);
void bind_point2d(py::module_& m);
void bind_point3d(py::module_& m);
void bind_vector2d(py::module_& m);
void bind_vector3d(py::module_& m);
void bind_line_segment2d(py::module_& m);
void bind_line_segment3d(py::module_& m);
void bind_line2d(py::module_& m);
void bind_line3d(py::module_& m);
void bind_ray2d(py::module_& m);
void bind_ray3d(py::module_& m);
void bind_polygon2d(py::module_& m);
void bind_polygon3d(py::module_& m);
void bind_polyline2d(py::module_& m);
void bind_polyline3d(py::module_& m);
void bind_triangle2d(py::module_& m);
void bind_triangle3d(py::module_& m);
void bind_bbox2d(py::module_& m);
void bind_bbox3d(py::module_& m);
void bind_bball2d(py::module_& m);
void bind_bball3d(py::module_& m);
void bind_plane(py::module_& m);
void bind_wktparser(py::module_& m);
void bind_free_functions(py::module_& m);
void bind_geometry_collection2d(py::module_& m);
void bind_geometry_collection3d(py::module_& m);

PYBIND11_MODULE(_geompp, m) {
    m.doc() = "geompp: C++ 2D/3D geometry library — Python bindings";

    bind_precision(m);
    bind_point2d(m);
    bind_point3d(m);
    bind_vector2d(m);
    bind_vector3d(m);
    bind_line_segment2d(m);
    bind_line_segment3d(m);
    bind_line2d(m);
    bind_line3d(m);
    bind_ray2d(m);
    bind_ray3d(m);
    bind_polygon2d(m);
    bind_polygon3d(m);
    bind_polyline2d(m);
    bind_polyline3d(m);
    bind_triangle2d(m);
    bind_triangle3d(m);
    bind_bbox2d(m);
    bind_bbox3d(m);
    bind_bball2d(m);
    bind_bball3d(m);
    bind_plane(m);
    bind_wktparser(m);
    bind_free_functions(m);
    bind_geometry_collection2d(m);
    bind_geometry_collection3d(m);
}
