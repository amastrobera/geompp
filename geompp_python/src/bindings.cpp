#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "bbox2d.hpp"
#include "bbox3d.hpp"
#include "constants.hpp"
#include "line2d.hpp"
#include "line3d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "lsv_parser.hpp"
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
py::object opt_variant_to_py(const std::optional<Variant>& opt) {
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


PYBIND11_MODULE(_geompp, m) {
    m.doc() = "geompp: C++ 2D/3D geometry library — Python bindings";

    // ─────────────────────────────────────────────────────────────────────────
    // Precision
    // ─────────────────────────────────────────────────────────────────────────
    m.attr("DP_THREE") = geompp::DP_THREE;
    m.attr("DP_SIX")   = geompp::DP_SIX;
    m.attr("DP_NINE")  = geompp::DP_NINE;

    m.def("set_decimal_precision",
          [](int dp) { geompp::DECIMAL_PRECISION = dp; }, "dp"_a,
          "Set thread-local decimal precision (DP_THREE=3, DP_SIX=6, DP_NINE=9).");

    m.def("get_decimal_precision",
          []() { return geompp::DECIMAL_PRECISION; },
          "Get current thread-local decimal precision.");


    // ─────────────────────────────────────────────────────────────────────────
    // Point2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Point2D>(m, "Point2D", "2D point (x, y).")
        .def(py::init<double, double>(),        "x"_a, "y"_a)
        .def(py::init<const geompp::Point2D&>())
        .def_property_readonly("x", &geompp::Point2D::x)
        .def_property_readonly("y", &geompp::Point2D::y)
        .def("to_vector",    &geompp::Point2D::ToVector)
        .def("distance_to",  &geompp::Point2D::DistanceTo, "other"_a)
        BIND_ALMOST_EQUALS(Point2D)
        BIND_SERIALIZATION(Point2D)
        .def_static("zero",  &geompp::Point2D::Zero)
        .def("__eq__",   [](const geompp::Point2D& a, const geompp::Point2D& b) { return a == b; })
        .def("__add__",  [](const geompp::Point2D& p, const geompp::Vector2D& v) { return p + v; }, "v"_a)
        .def("__sub__",  [](const geompp::Point2D& a, const geompp::Point2D& b)  -> geompp::Vector2D { return a - b; }, "other"_a)
        .def("__sub__",  [](const geompp::Point2D& p, const geompp::Vector2D& v) -> geompp::Point2D  { return p - v; }, "v"_a)
        .def("__mul__",  [](const geompp::Point2D& p, double a) { return p * a; }, "scalar"_a)
        .def("__rmul__", [](const geompp::Point2D& p, double a) { return a * p; }, "scalar"_a);

    // ─────────────────────────────────────────────────────────────────────────
    // Point3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Point3D>(m, "Point3D", "3D point (x, y, z).")
        .def(py::init<double, double, double>(), "x"_a, "y"_a, "z"_a)
        .def(py::init<const geompp::Point3D&>())
        .def_property_readonly("x", &geompp::Point3D::x)
        .def_property_readonly("y", &geompp::Point3D::y)
        .def_property_readonly("z", &geompp::Point3D::z)
        .def("to_vector",    &geompp::Point3D::ToVector)
        .def("distance_to",  &geompp::Point3D::DistanceTo, "other"_a)
        BIND_ALMOST_EQUALS(Point3D)
        BIND_SERIALIZATION(Point3D)
        .def_static("zero",  &geompp::Point3D::Zero)
        .def("__eq__",   [](const geompp::Point3D& a, const geompp::Point3D& b) { return a == b; })
        .def("__add__",  [](const geompp::Point3D& p, const geompp::Vector3D& v) { return p + v; }, "v"_a)
        .def("__sub__",  [](const geompp::Point3D& a, const geompp::Point3D& b)  -> geompp::Vector3D { return a - b; }, "other"_a)
        .def("__sub__",  [](const geompp::Point3D& p, const geompp::Vector3D& v) -> geompp::Point3D  { return p - v; }, "v"_a)
        .def("__mul__",  [](const geompp::Point3D& p, double a) { return p * a; }, "scalar"_a)
        .def("__rmul__", [](const geompp::Point3D& p, double a) { return a * p; }, "scalar"_a);

    // ─────────────────────────────────────────────────────────────────────────
    // Vector2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Vector2D>(m, "Vector2D", "2D vector (x, y).")
        .def(py::init<double, double>(),          "x"_a, "y"_a)
        .def(py::init<const geompp::Vector2D&>())
        .def_property_readonly("x", &geompp::Vector2D::x)
        .def_property_readonly("y", &geompp::Vector2D::y)
        .def("to_point",  &geompp::Vector2D::ToPoint)
        .def("length",    &geompp::Vector2D::Length)
        .def("dot",       &geompp::Vector2D::Dot,       "v"_a)
        .def("cross",     &geompp::Vector2D::Cross,     "v"_a)
        .def("perp",      &geompp::Vector2D::Perp)
        .def("normalize", &geompp::Vector2D::Normalize)
        BIND_ALMOST_EQUALS(Vector2D)
        BIND_SERIALIZATION(Vector2D)
        .def_static("basis_x", &geompp::Vector2D::BasisX)
        .def_static("basis_y", &geompp::Vector2D::BasisY)
        .def("__eq__",      [](const geompp::Vector2D& a, const geompp::Vector2D& b) { return a == b; })
        .def("__neg__",     [](const geompp::Vector2D& v) { return -v; })
        .def("__add__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> geompp::Vector2D { return a + b; }, "v"_a)
        .def("__add__",     [](const geompp::Vector2D& v, const geompp::Point2D& p)   -> geompp::Point2D  { return v + p; }, "p"_a)
        .def("__sub__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> geompp::Vector2D { return a - b; }, "v"_a)
        .def("__mul__",     [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return v * a; }, "scalar"_a)
        .def("__mul__",     [](const geompp::Vector2D& a, const geompp::Vector2D& b)  -> double           { return a * b; }, "v"_a)
        .def("__rmul__",    [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return a * v; }, "scalar"_a)
        .def("__truediv__", [](const geompp::Vector2D& v, double a)                   -> geompp::Vector2D { return v / a; }, "scalar"_a);

    // ─────────────────────────────────────────────────────────────────────────
    // Vector3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Vector3D>(m, "Vector3D", "3D vector (x, y, z).")
        .def(py::init<double, double, double>(),  "x"_a, "y"_a, "z"_a)
        .def(py::init<const geompp::Vector3D&>())
        .def_property_readonly("x", &geompp::Vector3D::x)
        .def_property_readonly("y", &geompp::Vector3D::y)
        .def_property_readonly("z", &geompp::Vector3D::z)
        .def("to_point",    &geompp::Vector3D::ToPoint)
        .def("length",      &geompp::Vector3D::Length)
        .def("dot",         &geompp::Vector3D::Dot,         "other"_a)
        .def("cross",       &geompp::Vector3D::Cross,       "other"_a)
        .def("perp",        &geompp::Vector3D::Perp)
        .def("normalize",   &geompp::Vector3D::Normalize)
        .def("is_parallel", &geompp::Vector3D::IsParallel, "other"_a)
        BIND_ALMOST_EQUALS(Vector3D)
        BIND_SERIALIZATION(Vector3D)
        .def_static("basis_x", &geompp::Vector3D::BasisX)
        .def_static("basis_y", &geompp::Vector3D::BasisY)
        .def_static("basis_z", &geompp::Vector3D::BasisZ)
        .def("__eq__",      [](const geompp::Vector3D& a, const geompp::Vector3D& b) { return a == b; })
        .def("__neg__",     [](const geompp::Vector3D& v) { return -v; })
        .def("__add__",     [](const geompp::Vector3D& a, const geompp::Vector3D& b)  -> geompp::Vector3D { return a + b; }, "v"_a)
        .def("__add__",     [](const geompp::Vector3D& v, const geompp::Point3D& p)   -> geompp::Point3D  { return v + p; }, "p"_a)
        .def("__sub__",     [](const geompp::Vector3D& a, const geompp::Vector3D& b)  -> geompp::Vector3D { return a - b; }, "v"_a)
        .def("__mul__",     [](const geompp::Vector3D& v, double a)                   -> geompp::Vector3D { return v * a; }, "scalar"_a)
        .def("__mul__",     [](const geompp::Vector3D& a, const geompp::Vector3D& b)  -> double           { return a * b; }, "v"_a)
        .def("__rmul__",    [](const geompp::Vector3D& v, double a)                   -> geompp::Vector3D { return a * v; }, "scalar"_a)
        .def("__truediv__", [](const geompp::Vector3D& v, double a)                   -> geompp::Vector3D { return v / a; }, "scalar"_a);

    // ─────────────────────────────────────────────────────────────────────────
    // LineSegment2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::LineSegment2D>(m, "LineSegment2D",
        "2D finite segment between two points.")
        .def_static("make", &geompp::LineSegment2D::Make, "p0"_a, "p1"_a)
        .def(py::init<const geompp::LineSegment2D&>())
        .def_property_readonly("first", &geompp::LineSegment2D::First)
        .def_property_readonly("last",  &geompp::LineSegment2D::Last)
        .def("to_line",     &geompp::LineSegment2D::ToLine)
        .def("length",      &geompp::LineSegment2D::Length)
        .def("distance_to", &geompp::LineSegment2D::DistanceTo,  "point"_a)
        .def("location",    &geompp::LineSegment2D::Location,    "point"_a)
        .def("interpolate", &geompp::LineSegment2D::Interpolate, "pct"_a)
        .def("contains",    &geompp::LineSegment2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(LineSegment2D)
        BIND_SERIALIZATION(LineSegment2D)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return s.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return s.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return s.Intersects(o); }, "segment"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return opt_variant_to_py(s.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return opt_variant_to_py(s.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return opt_variant_to_py(s.Intersection(o)); }, "segment"_a)
        .def("__eq__", [](const geompp::LineSegment2D& a, const geompp::LineSegment2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // LineSegment3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::LineSegment3D>(m, "LineSegment3D",
        "3D finite segment between two points.")
        .def_static("make", &geompp::LineSegment3D::Make, "p0"_a, "p1"_a)
        .def(py::init<const geompp::LineSegment3D&>())
        .def_property_readonly("first", &geompp::LineSegment3D::First)
        .def_property_readonly("last",  &geompp::LineSegment3D::Last)
        .def("to_line",     &geompp::LineSegment3D::ToLine)
        .def("length",      &geompp::LineSegment3D::Length)
        .def("distance_to", &geompp::LineSegment3D::DistanceTo,  "point"_a)
        .def("location",    &geompp::LineSegment3D::Location,    "point"_a)
        .def("interpolate", &geompp::LineSegment3D::Interpolate, "pct"_a)
        .def("contains",    &geompp::LineSegment3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(LineSegment3D)
        BIND_SERIALIZATION(LineSegment3D)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l)        { return s.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r)         { return s.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) { return s.Intersects(o); }, "segment"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l)        { return opt_variant_to_py(s.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r)         { return opt_variant_to_py(s.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) { return opt_variant_to_py(s.Intersection(o)); }, "segment"_a)
        .def("__eq__", [](const geompp::LineSegment3D& a, const geompp::LineSegment3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Line2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Line2D>(m, "Line2D",
        "Infinite 2D line (origin + unit direction).")
        .def_static("make",
             py::overload_cast<const geompp::Point2D&, const geompp::Point2D&>(&geompp::Line2D::Make),
             "p0"_a, "p1"_a)
        .def_static("make",
             py::overload_cast<const geompp::Point2D&, const geompp::Vector2D&>(&geompp::Line2D::Make),
             "origin"_a, "direction"_a)
        .def(py::init<const geompp::Line2D&>())
        .def_property_readonly("first",     &geompp::Line2D::First)
        .def_property_readonly("last",      &geompp::Line2D::Last)
        .def_property_readonly("origin",    &geompp::Line2D::Origin)
        .def_property_readonly("direction", &geompp::Line2D::Direction)
        .def("distance_to",  &geompp::Line2D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::Line2D::ProjectOnto, "point"_a)
        .def("location",     &geompp::Line2D::Location,    "point"_a)
        .def("contains",     &geompp::Line2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Line2D)
        BIND_SERIALIZATION(Line2D)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::Line2D& o)        { return l.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::Ray2D& r)         { return l.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::LineSegment2D& s) { return l.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::Line2D& o)        { return opt_variant_to_py(l.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::Ray2D& r)         { return opt_variant_to_py(l.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::LineSegment2D& s) { return opt_variant_to_py(l.Intersection(s)); }, "segment"_a)
        // Returns (Optional[Point2D], sc, tc) where sc/tc are parametric t-values
        .def("intersection_with_params",
             [](const geompp::Line2D& l, const geompp::Line2D& other) {
                 double sc = 0.0, tc = 0.0;
                 auto result = l.Intersection(other, sc, tc);
                 return py::make_tuple(opt_variant_to_py(result), sc, tc);
             }, "other"_a,
             "Returns (point_or_None, sc, tc). sc/tc are the parametric positions on each line.")
        .def("__eq__", [](const geompp::Line2D& a, const geompp::Line2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Line3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Line3D>(m, "Line3D",
        "Infinite 3D line (origin + unit direction).")
        .def_static("make",
             py::overload_cast<const geompp::Point3D&, const geompp::Point3D&>(&geompp::Line3D::Make),
             "p0"_a, "p1"_a)
        .def_static("make",
             py::overload_cast<const geompp::Point3D&, const geompp::Vector3D&>(&geompp::Line3D::Make),
             "origin"_a, "direction"_a)
        .def(py::init<const geompp::Line3D&>())
        .def_property_readonly("first",     &geompp::Line3D::First)
        .def_property_readonly("last",      &geompp::Line3D::Last)
        .def_property_readonly("origin",    &geompp::Line3D::Origin)
        .def_property_readonly("direction", &geompp::Line3D::Direction)
        .def("distance_to",  &geompp::Line3D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::Line3D::ProjectOnto, "point"_a)
        .def("location",     &geompp::Line3D::Location,    "point"_a)
        .def("contains",     &geompp::Line3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Line3D)
        BIND_SERIALIZATION(Line3D)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::Line3D& o)        { return l.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::Ray3D& r)         { return l.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) { return l.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::Line3D& o)        { return opt_variant_to_py(l.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::Ray3D& r)         { return opt_variant_to_py(l.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) { return opt_variant_to_py(l.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Line3D& a, const geompp::Line3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Ray2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Ray2D>(m, "Ray2D",
        "2D ray (semi-infinite line from origin in direction).")
        .def_static("make", &geompp::Ray2D::Make, "origin"_a, "direction"_a)
        .def(py::init<const geompp::Ray2D&>())
        .def_property_readonly("origin",    &geompp::Ray2D::Origin)
        .def_property_readonly("direction", &geompp::Ray2D::Direction)
        .def("is_ahead",    &geompp::Ray2D::IsAhead,    "point"_a)
        .def("is_behind",   &geompp::Ray2D::IsBehind,   "point"_a)
        .def("to_line",     &geompp::Ray2D::ToLine)
        .def("distance_to", &geompp::Ray2D::DistanceTo, "point"_a)
        .def("contains",    &geompp::Ray2D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Ray2D)
        BIND_SERIALIZATION(Ray2D)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::Line2D& l)        { return r.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::Ray2D& o)         { return r.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::LineSegment2D& s) { return r.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::Line2D& l)        { return opt_variant_to_py(r.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::Ray2D& o)         { return opt_variant_to_py(r.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::LineSegment2D& s) { return opt_variant_to_py(r.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Ray2D& a, const geompp::Ray2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Ray3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Ray3D>(m, "Ray3D",
        "3D ray (semi-infinite line from origin in direction).")
        .def_static("make", &geompp::Ray3D::Make, "origin"_a, "direction"_a)
        .def(py::init<const geompp::Ray3D&>())
        .def_property_readonly("origin",    &geompp::Ray3D::Origin)
        .def_property_readonly("direction", &geompp::Ray3D::Direction)
        .def("is_ahead",    &geompp::Ray3D::IsAhead,    "point"_a)
        .def("is_behind",   &geompp::Ray3D::IsBehind,   "point"_a)
        .def("to_line",     &geompp::Ray3D::ToLine)
        .def("distance_to", &geompp::Ray3D::DistanceTo, "point"_a)
        .def("contains",    &geompp::Ray3D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Ray3D)
        BIND_SERIALIZATION(Ray3D)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::Line3D& l)        { return r.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::Ray3D& o)         { return r.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::LineSegment3D& s) { return r.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::Line3D& l)        { return opt_variant_to_py(r.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::Ray3D& o)         { return opt_variant_to_py(r.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::LineSegment3D& s) { return opt_variant_to_py(r.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Ray3D& a, const geompp::Ray3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Polygon2D
    // ─────────────────────────────────────────────────────────────────────────
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

    // ─────────────────────────────────────────────────────────────────────────
    // Polygon3D
    // ─────────────────────────────────────────────────────────────────────────
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

    // ─────────────────────────────────────────────────────────────────────────
    // Polyline2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Polyline2D>(m, "Polyline2D",
        "2D polyline (open chain of line segments).")
        .def_static("make", &geompp::Polyline2D::Make, "points"_a)
        .def(py::init<const geompp::Polyline2D&>())
        .def("size",        &geompp::Polyline2D::Size)
        .def("to_segments", &geompp::Polyline2D::ToSegments)
        .def("length",      &geompp::Polyline2D::Length)
        .def("distance_to", &geompp::Polyline2D::DistanceTo,  "point"_a)
        .def("location",    &geompp::Polyline2D::Location,    "point"_a)
        .def("interpolate", &geompp::Polyline2D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polyline2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Polyline2D)
        BIND_SERIALIZATION(Polyline2D)
        .def("__len__",     &geompp::Polyline2D::Size)
        .def("__getitem__", [](const geompp::Polyline2D& p, int i) -> geompp::Point2D {
            int n = p.Size();
            if (i < 0) i += n;
            if (i < 0 || i >= n) throw py::index_error("index out of range");
            return p[static_cast<size_t>(i)];
        }, "i"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    { return p.Intersects(o); }, "other"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    { return opt_variant_to_py(p.Intersection(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline2D& a, const geompp::Polyline2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Polyline3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Polyline3D>(m, "Polyline3D",
        "3D polyline (open chain of line segments).")
        .def_static("make", &geompp::Polyline3D::Make, "points"_a)
        .def(py::init<const geompp::Polyline3D&>())
        .def("size",        &geompp::Polyline3D::Size)
        .def("to_segments", &geompp::Polyline3D::ToSegments)
        .def("length",      &geompp::Polyline3D::Length)
        .def("distance_to", &geompp::Polyline3D::DistanceTo,  "point"_a)
        .def("location",    &geompp::Polyline3D::Location,    "point"_a)
        .def("interpolate", &geompp::Polyline3D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polyline3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Polyline3D)
        BIND_SERIALIZATION(Polyline3D)
        .def("__len__",     &geompp::Polyline3D::Size)
        .def("__getitem__", [](const geompp::Polyline3D& p, int i) -> geompp::Point3D {
            int n = p.Size();
            if (i < 0) i += n;
            if (i < 0 || i >= n) throw py::index_error("index out of range");
            return p[static_cast<size_t>(i)];
        }, "i"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Line3D& l)        { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Ray3D& r)         { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::LineSegment3D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Polyline3D& o)    { return p.Intersects(o); }, "other"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Line3D& l)        { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Ray3D& r)         { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::LineSegment3D& s) { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Polyline3D& o)    { return opt_variant_to_py(p.Intersection(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline3D& a, const geompp::Polyline3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Triangle2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Triangle2D>(m, "Triangle2D",
        "2D triangle defined by three vertices.")
        .def_static("make", &geompp::Triangle2D::Make, "p0"_a, "p1"_a, "p2"_a)
        .def(py::init<const geompp::Triangle2D&>())
        // pybind11 3.x doesn't auto-cast std::tuple<RegisteredType...>, so we wrap explicitly
        .def_property_readonly("vertices", [](const geompp::Triangle2D& t) {
            auto [p0, p1, p2] = t.Vertices();
            return py::make_tuple(py::cast(p0), py::cast(p1), py::cast(p2));
        })
        .def("centroid",    &geompp::Triangle2D::Centroid)
        .def("to_polygon",  &geompp::Triangle2D::ToPolygon)
        .def("signed_area", &geompp::Triangle2D::SignedArea)
        .def("area",        &geompp::Triangle2D::Area)
        .def("perimeter",   &geompp::Triangle2D::Perimeter)
        .def("distance_to", &geompp::Triangle2D::DistanceTo, "point"_a)
        .def("to_axis", [](const geompp::Triangle2D& t) {
            auto [u, v] = t.ToAxis();
            return py::make_tuple(py::cast(u), py::cast(v));
        })
        .def("location", [](const geompp::Triangle2D& t, const geompp::Point2D& p) {
            auto [s, tt] = t.Location(p);
            return py::make_tuple(s, tt);
        }, "point"_a)
        // interpolate() → std::optional<Point2D> — single type, pybind11 handles fine
        .def("interpolate", [](const geompp::Triangle2D& t, double s, double tt) -> py::object {
            auto r = t.Interpolate(s, tt);
            if (!r.has_value()) return py::none();
            return py::cast(r.value());
        }, "s"_a, "t"_a)
        .def("contains",    &geompp::Triangle2D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Triangle2D)
        BIND_SERIALIZATION(Triangle2D)
        .def("intersects",
             [](const geompp::Triangle2D& t, const geompp::Line2D& l) { return t.Intersects(l); }, "line"_a)
        .def("intersection",
             [](const geompp::Triangle2D& t, const geompp::Line2D& l) { return opt_variant_to_py(t.Intersection(l)); }, "line"_a)
        .def("__eq__", [](const geompp::Triangle2D& a, const geompp::Triangle2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Triangle3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Triangle3D>(m, "Triangle3D",
        "3D triangle defined by three vertices.")
        .def_static("make", &geompp::Triangle3D::Make, "p0"_a, "p1"_a, "p2"_a)
        .def(py::init<const geompp::Triangle3D&>())
        .def_property_readonly("vertices", [](const geompp::Triangle3D& t) {
            auto [p0, p1, p2] = t.Vertices();
            return py::make_tuple(py::cast(p0), py::cast(p1), py::cast(p2));
        })
        .def("centroid",    &geompp::Triangle3D::Centroid)
        .def("signed_area", &geompp::Triangle3D::SignedArea)
        .def("area",        &geompp::Triangle3D::Area)
        .def("perimeter",   &geompp::Triangle3D::Perimeter)
        .def("distance_to", &geompp::Triangle3D::DistanceTo, "point"_a)
        .def("to_axis", [](const geompp::Triangle3D& t) {
            auto [u, v] = t.ToAxis();
            return py::make_tuple(py::cast(u), py::cast(v));
        })
        .def("location", [](const geompp::Triangle3D& t, const geompp::Point3D& p) {
            auto [s, tt] = t.Location(p);
            return py::make_tuple(s, tt);
        }, "point"_a)
        .def("interpolate", [](const geompp::Triangle3D& t, double s, double tt) -> py::object {
            auto r = t.Interpolate(s, tt);
            if (!r.has_value()) return py::none();
            return py::cast(r.value());
        }, "s"_a, "t"_a)
        .def("contains",    &geompp::Triangle3D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Triangle3D)
        BIND_SERIALIZATION(Triangle3D)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) { return t.Intersects(l); }, "line"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) { return opt_variant_to_py(t.Intersection(l)); }, "line"_a)
        .def("__eq__", [](const geompp::Triangle3D& a, const geompp::Triangle3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // BBox2D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::BBox2D>(m, "BBox2D",
        "2D axis-aligned bounding box.")
        .def(py::init<const geompp::Point2D&,     const geompp::Point2D&>(),     "min"_a, "max"_a)
        .def(py::init<const geompp::LineSegment2D&>(), "segment"_a)
        .def(py::init<const geompp::Polyline2D&>(),    "polyline"_a)
        .def(py::init<const geompp::Polygon2D&>(),     "polygon"_a)
        .def(py::init<const geompp::Triangle2D&>(),    "triangle"_a)
        .def(py::init<const geompp::BBox2D&>())
        .def_property_readonly("min", &geompp::BBox2D::min)
        .def_property_readonly("max", &geompp::BBox2D::max)
        .def("contains",     &geompp::BBox2D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBox2D)
        .def("__eq__", [](const geompp::BBox2D& a, const geompp::BBox2D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // BBox3D
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::BBox3D>(m, "BBox3D",
        "3D axis-aligned bounding box.")
        .def(py::init<const geompp::Point3D&, const geompp::Point3D&>(), "min"_a, "max"_a)
        .def(py::init<const geompp::BBox3D&>())
        .def_property_readonly("min", &geompp::BBox3D::min)
        .def_property_readonly("max", &geompp::BBox3D::max)
        .def("contains",     &geompp::BBox3D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBox3D)
        .def("__eq__", [](const geompp::BBox3D& a, const geompp::BBox3D& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // Plane
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::Plane>(m, "Plane",
        "3D plane (origin + normal + two orthonormal axes).")
        .def_static("from_3_points",          &geompp::Plane::From3Points,         "p1"_a, "p2"_a, "p3"_a)
        .def_static("from_origin_and_axes",   &geompp::Plane::FromOriginAndAxes,   "origin"_a, "u"_a, "v"_a)
        .def_static("from_origin_and_normal", &geompp::Plane::FromOriginAndNormal, "origin"_a, "normal"_a)
        .def_static("xy", &geompp::Plane::XY)
        .def_static("yz", &geompp::Plane::YZ)
        .def_static("zx", &geompp::Plane::ZX)
        .def(py::init<const geompp::Plane&>())
        .def_property_readonly("origin", &geompp::Plane::origin)
        .def_property_readonly("normal", &geompp::Plane::normal)
        .def_property_readonly("axis_u", &geompp::Plane::axis_u)
        .def_property_readonly("axis_v", &geompp::Plane::axis_v)
        .def("signed_distance_to", &geompp::Plane::SignedDistanceTo, "point"_a)
        .def("distance_to",        &geompp::Plane::DistanceTo,       "point"_a)
        .def("project_onto",       &geompp::Plane::ProjectOnto,      "point"_a)
        .def("project_into",       &geompp::Plane::ProjectInto,      "point"_a)
        .def("evaluate",           &geompp::Plane::Evaluate,         "p2d"_a)
        .def("contains",           &geompp::Plane::Contains,         "point"_a)
        BIND_ALMOST_EQUALS(Plane)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::Line3D& l) { return pl.Intersects(l); }, "line"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::Line3D& l) { return opt_variant_to_py(pl.Intersection(l)); }, "line"_a)
        .def("__eq__", [](const geompp::Plane& a, const geompp::Plane& b) { return a == b; });

    // ─────────────────────────────────────────────────────────────────────────
    // LVSParser
    // ─────────────────────────────────────────────────────────────────────────
    py::class_<geompp::LVSParser>(m, "LVSParser",
        "Parser for .lsv files containing WKT geometry entries.\n\n"
        "Usage::\n\n"
        "    parser = LVSParser.open('file.lsv')\n"
        "    while parser.has_next():\n"
        "        item = parser.next()  # returns a geometry object or None\n"
        "        if item is not None:\n"
        "            print(LVSParser.to_wkt(item))\n\n"
        "Also supports context manager syntax.")
        .def_static("open", [](const std::string& path) {
            return geompp::LVSParser::Open(path);
        }, "path"_a)
        .def("has_next",      &geompp::LVSParser::HasNext)
        .def("next", [](geompp::LVSParser& self) -> py::object {
            return opt_variant_to_py(self.Next());
        })
        .def("get_file_path", &geompp::LVSParser::GetFilePath)
        // to_wkt accepts any geometry object (or None) and calls its to_wkt()
        .def_static("to_wkt", [](const py::object& item) -> std::string {
            if (item.is_none()) return "GEOMETRYCOLLECTION EMPTY";
            return item.attr("to_wkt")().cast<std::string>();
        }, "item"_a,
           "Return the WKT string for a geometry object returned by next(), or 'GEOMETRYCOLLECTION EMPTY' for None.")
        .def("__enter__", [](geompp::LVSParser& self) -> geompp::LVSParser& { return self; },
             py::return_value_policy::reference)
        .def("__exit__", [](geompp::LVSParser&, py::object, py::object, py::object) {});

    // ─────────────────────────────────────────────────────────────────────────
    // Free functions — collection operations (overloaded for 2D and 3D)
    // ─────────────────────────────────────────────────────────────────────────
    m.def("are_collinear",
          [](const geompp::Point2D& p1, const geompp::Point2D& p2, const geompp::Point2D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 2D points are collinear.");

    m.def("are_collinear",
          [](const geompp::Point3D& p1, const geompp::Point3D& p2, const geompp::Point3D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 3D points are collinear.");

    m.def("remove_duplicates_from_sorted_list",
          [](const std::vector<geompp::Point2D>& pts) {
              return geompp::remove_duplicates_from_sorted_list(pts);
          }, "points"_a, "Remove consecutive duplicate 2D points.");

    m.def("remove_duplicates_from_sorted_list",
          [](const std::vector<geompp::Point3D>& pts) {
              return geompp::remove_duplicates_from_sorted_list(pts);
          }, "points"_a, "Remove consecutive duplicate 3D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 2D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 3D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 2D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 3D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point2D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 2D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point3D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 3D points.");

    m.def("average",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 2D points.");

    m.def("average",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 3D points.");
}
