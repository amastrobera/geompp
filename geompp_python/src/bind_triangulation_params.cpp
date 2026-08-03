#include "bind_helpers.hpp"

// Registers TriangulationParams and its nested enums. Called early from bindings.cpp (before
// bind_polygon2d / bind_polygon3d / bind_polymesh2d / bind_polymesh3d) because pybind11 resolves a
// method's default *argument value* (e.g. Polygon2D.triangulate's strategy=EarClipping) at bind time,
// not call time — unlike a plain parameter/return type, which only needs its type registered by the
// time Python code actually calls the function. Registering these types late (e.g. inside
// bind_free_functions, which used to hold them) breaks every other binding that defaults to one of
// them, with an "into a Python object (type not registered yet?)" ImportError at import time.
void bind_triangulation_params(py::module_& m) {
    py::enum_<geompp::TriangulationParams::Strategy>(m, "TriangulationStrategy",
        "Which triangulation algorithm to run — see TriangulationParams.")
        .value("EarClipping", geompp::TriangulationParams::Strategy::EarClipping,
               "O(n^2) worst case, but simple and robust for small polygons.")
        .value("MonotonePolygon", geompp::TriangulationParams::Strategy::MonotonePolygon,
               "O(n log n) worst case; requires a monotone polygon (or a decomposition into monotone "
               "pieces). Not yet implemented.")
        .value("Delaunay", geompp::TriangulationParams::Strategy::Delaunay,
               "O(n log n) worst case; maximizes the minimum angle across all triangles (avoids skinny "
               "slivers). Not yet implemented.")
        .export_values();

    py::enum_<geompp::TriangulationParams::Simplicity>(m, "TriangulationSimplicity",
        "How triangulate() handles a possibly self-intersecting input ring.")
        .value("Guaranteed", geompp::TriangulationParams::Simplicity::Guaranteed,
               "No check is carried out (runs at your own risk).")
        .value("Assert", geompp::TriangulationParams::Simplicity::Assert,
               "Raises if the input isn't simple.")
        .value("Enforce", geompp::TriangulationParams::Simplicity::Enforce,
               "Decomposes non-simple input into simple pieces (via simplify_rings) before triangulating.")
        .export_values();

    py::enum_<geompp::TriangulationParams::Winding>(m, "TriangulationWinding",
        "How triangulate() handles input that may not be wound counter-clockwise (CCW).")
        .value("Guaranteed", geompp::TriangulationParams::Winding::Guaranteed,
               "No check is carried out (runs at your own risk).")
        .value("Assert", geompp::TriangulationParams::Winding::Assert, "Raises if the input isn't CCW.")
        .value("Enforce", geompp::TriangulationParams::Winding::Enforce,
               "Reverses the input if it's CW, before triangulating.")
        .export_values();

    py::enum_<geompp::TriangulationParams::Collinearity>(m, "TriangulationCollinearity",
        "How triangulate() handles collinear points (a duplicate consecutive point is just the "
        "degenerate case of three collinear points, so this covers both).")
        .value("Guaranteed", geompp::TriangulationParams::Collinearity::Guaranteed,
               "No check is carried out (runs at your own risk).")
        .value("Assert", geompp::TriangulationParams::Collinearity::Assert,
               "Raises if the input has collinear (or duplicate) points.")
        .value("Enforce", geompp::TriangulationParams::Collinearity::Enforce,
               "Removes collinear/duplicate points before triangulating.")
        .export_values();

    py::class_<geompp::TriangulationParams>(m, "TriangulationParams",
        "Bundles the triangulation strategy and how to handle non-simple / non-CCW / collinear input for "
        "triangulate() / Polygon2D.triangulate() / Polygon3D.triangulate() / PolyMesh2D.triangulate() / "
        "PolyMesh3D.triangulate(). Defaults match triangulate()'s own defaults: EarClipping, and Enforce "
        "for all three input-quality checks.")
        .def(py::init([](geompp::TriangulationParams::Strategy strategy,
                          geompp::TriangulationParams::Simplicity simplicity,
                          geompp::TriangulationParams::Winding ccw_winding,
                          geompp::TriangulationParams::Collinearity collinearity) {
                 geompp::TriangulationParams p;
                 p.strategy = strategy;
                 p.simplicity = simplicity;
                 p.ccw_winding = ccw_winding;
                 p.collinearity = collinearity;
                 return p;
             }),
             "strategy"_a = geompp::TriangulationParams::Strategy::EarClipping,
             "simplicity"_a = geompp::TriangulationParams::Simplicity::Enforce,
             "ccw_winding"_a = geompp::TriangulationParams::Winding::Enforce,
             "collinearity"_a = geompp::TriangulationParams::Collinearity::Enforce)
        .def_readwrite("strategy", &geompp::TriangulationParams::strategy)
        .def_readwrite("simplicity", &geompp::TriangulationParams::simplicity)
        .def_readwrite("ccw_winding", &geompp::TriangulationParams::ccw_winding)
        .def_readwrite("collinearity", &geompp::TriangulationParams::collinearity);
}
