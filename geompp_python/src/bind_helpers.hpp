#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "bbox2d.hpp"
#include "bbox3d.hpp"
#include "bball2d.hpp"
#include "bball3d.hpp"
#include "brect2d.hpp"
#include "bprism3d.hpp"
#include "constants.hpp"
#include "geometry_collection2d.hpp"
#include "geometry_collection3d.hpp"
#include "line2d.hpp"
#include "line3d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "wkt_parser.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "polyline2d.hpp"
#include "polyline3d.hpp"
#include "ray2d.hpp"
#include "ray3d.hpp"
#include "triangle2d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

// ─────────────────────────────────────────────────────────────────────────────
// Helper: convert std::optional<std::variant<Ts...>> to py::object.
//
// pybind11 3.x variant_caster fails for class-typed variants, so we use
// std::visit + py::cast explicitly instead of relying on the auto type caster.
// ─────────────────────────────────────────────────────────────────────────────
template <typename Variant>
inline py::object opt_variant_to_py(const std::optional<Variant>& opt) {
    if (!opt.has_value()) return py::none();
    return std::visit([](const auto& v) -> py::object {
        return py::cast(v);
    }, opt.value());
}

// ─────────────────────────────────────────────────────────────────────────────
// Macro: almost_equals() with and without epsilon
// ─────────────────────────────────────────────────────────────────────────────
#define BIND_ALMOST_EQUALS(T)                                                        \
    .def("almost_equals",                                                            \
         [](const geompp::T& self, const geompp::T& other) {                        \
             return self.AlmostEquals(other);                                        \
         }, "other"_a)                                                               \
    .def("almost_equals",                                                            \
         [](const geompp::T& self, const geompp::T& other, double eps) {            \
             return self.AlmostEquals(other, eps);                                   \
         }, "other"_a, "epsilon"_a)

// ─────────────────────────────────────────────────────────────────────────────
// Macro: WKT serialization + file I/O
// ─────────────────────────────────────────────────────────────────────────────
#define BIND_SERIALIZATION(T)                                        \
    .def("to_wkt",            &geompp::T::ToWkt)                     \
    .def_static("from_wkt",   &geompp::T::FromWkt,   "wkt"_a)       \
    .def("to_file",           &geompp::T::ToFile,    "path"_a)       \
    .def_static("from_file",  &geompp::T::FromFile,  "path"_a)       \
    .def("__repr__",          &geompp::T::ToWkt)                     \
    .def("__str__",           &geompp::T::ToWkt)
