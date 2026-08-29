#include "bind_helpers.hpp"

// Registers PolygonizationParams and its Strategy enum. Called early from bindings.cpp (before
// bind_mesh2d/bind_mesh3d/bind_connected_mesh2d/bind_connected_mesh3d and bind_free_functions), for
// the same reason bind_triangulation_params is: pybind11 resolves a method's default *argument value*
// (e.g. Mesh2D.polygonize's params=PolygonizationParams()) at bind time, not call time.
void bind_polygonization_params(py::module_& m) {
    py::enum_<geompp::PolygonizationParams::Strategy>(m, "PolygonizationStrategy",
        "Which polygonization algorithm to run, and under what constraint — see PolygonizationParams.")
        .value("PlanarBoundaryExtraction", geompp::PolygonizationParams::Strategy::PlanarBoundaryExtraction,
               "Finds the external boundary of a set of triangles in O(N), returning 1+ planar polygon "
               "(not guaranteed convex).")
        .value("PlanarQuads", geompp::PolygonizationParams::Strategy::PlanarQuads,
               "Pairs 2 adjacent coplanar triangles into 1 planar quad, in O(N); planar yet not "
               "necessarily convex.")
        .value("HertelMehlhorn", geompp::PolygonizationParams::Strategy::HertelMehlhorn,
               "Merges as many triangles as possible into polygons in O(N); polygons are planar and "
               "convex. Default.")
        .export_values();

    py::class_<geompp::PolygonizationParams>(m, "PolygonizationParams",
        "Bundles the polygonization strategy for polygonize() / Mesh2D.polygonize() / "
        "Mesh3D.polygonize() / ConnectedMesh2D.polygonize() / ConnectedMesh3D.polygonize(), plus how the "
        "final PolyMesh2D/3D.from_polygons() call inside those should handle a mesh-conformity violation "
        "(see AdjacencyConformity). Defaults to HertelMehlhorn and Assert -- polygonize_impl's own "
        "seam-collapse logic already keeps every strategy's output provably conformant by construction, so "
        "Assert is a safety net for unexpected input, not something normally expected to fire.")
        .def(py::init([](geompp::PolygonizationParams::Strategy strategy,
                          geompp::AdjacencyConformity conformity) {
                 geompp::PolygonizationParams p;
                 p.strategy = strategy;
                 p.conformity = conformity;
                 return p;
             }),
             "strategy"_a = geompp::PolygonizationParams::Strategy::HertelMehlhorn,
             "conformity"_a = geompp::AdjacencyConformity::Assert)
        .def_readwrite("strategy", &geompp::PolygonizationParams::strategy)
        .def_readwrite("conformity", &geompp::PolygonizationParams::conformity);
}
