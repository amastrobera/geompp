#include "bind_helpers.hpp"

#include <transformations.hpp>

// Registers geompp::transformations as a real Python submodule (geompp.transformations), same reasoning
// as geompp.maths (bind_maths.cpp): a genuinely separate, non-inline C++ namespace, unlike
// geompp::geometry's *inline* one which has no Python equivalent to mirror.
void bind_transformations(py::module_& m) {
    auto tf = m.def_submodule("transformations",
        "geompp::transformations -- affine transforms for geompp primitives, built on geompp.maths "
        "(Matrix3/Matrix4). translate()/rotate()/scale() are the fast, single-Point, no-matrix path; "
        "transform(obj, matrix) is the general path covering every primitive from Point to PolyMesh.");

    // ── TransformBuilder ───────────────────────────────────────────────────────
    py::class_<geompp::transformations::TransformBuilder>(tf, "TransformBuilder",
        "Fluent composer for a single 4x4 homogeneous affine transform. Each translate()/rotate()/"
        "scale()/combine() call PRE-multiplies the new operation onto the accumulated matrix, so "
        "chained calls apply in the order they're written: builder.translate(t).rotate(r) moves a "
        "point by t first, then rotates the result by r.")
        .def(py::init<>())
        .def("translate",
             [](geompp::transformations::TransformBuilder& b, geompp::maths::Vector3 const& offset)
                 -> geompp::transformations::TransformBuilder& { return b.Translate(offset); },
             "offset"_a, py::return_value_policy::reference_internal)
        .def("rotate",
             [](geompp::transformations::TransformBuilder& b, double angle_rad, geompp::maths::Vector3 const& axis)
                 -> geompp::transformations::TransformBuilder& { return b.Rotate(angle_rad, axis); },
             "angle_rad"_a, "axis"_a, py::return_value_policy::reference_internal,
             "Raises ValueError if axis is zero-length.")
        .def("scale",
             [](geompp::transformations::TransformBuilder& b, double factor)
                 -> geompp::transformations::TransformBuilder& { return b.Scale(factor); },
             "factor"_a, py::return_value_policy::reference_internal)
        .def("scale",
             [](geompp::transformations::TransformBuilder& b, double sx, double sy, double sz)
                 -> geompp::transformations::TransformBuilder& { return b.Scale(sx, sy, sz); },
             "sx"_a, "sy"_a, "sz"_a, py::return_value_policy::reference_internal)
        .def("combine",
             [](geompp::transformations::TransformBuilder& b, geompp::maths::Matrix4 const& mat)
                 -> geompp::transformations::TransformBuilder& { return b.Combine(mat); },
             "matrix"_a, py::return_value_policy::reference_internal)
        .def("get", &geompp::transformations::TransformBuilder::Get)
        .def("build", &geompp::transformations::TransformBuilder::Build);

    // ── 2D fast-path (Point2D translate/rotate/scale, no matrix) ─────────────────
    tf.def("translate", [](geompp::Point2D const& p, geompp::maths::Vector2 const& offset) { return geompp::transformations::translate(p, offset); },
           "point"_a, "offset"_a, "Translates a Point2D by a plain (x, y) offset -- direct arithmetic, no matrix.");
    tf.def("rotate", [](geompp::Point2D const& p, double angle_rad) { return geompp::transformations::rotate(p, angle_rad); },
           "point"_a, "angle_rad"_a, "Rotates a Point2D about the origin -- direct arithmetic, no matrix.");
    tf.def("scale", [](geompp::Point2D const& p, double factor) { return geompp::transformations::scale(p, factor); },
           "point"_a, "factor"_a, "Uniformly scales a Point2D about the origin.");
    tf.def("scale", [](geompp::Point2D const& p, double sx, double sy) { return geompp::transformations::scale(p, sx, sy); },
           "point"_a, "sx"_a, "sy"_a, "Non-uniformly scales a Point2D about the origin.");

    // ── 2D general path: transform(obj, Matrix3) ──────────────────────────────────
    tf.def("transform", [](geompp::Point2D const& p, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(p, m); }, "point"_a, "matrix"_a);
    tf.def("transform", [](geompp::Vector2D const& v, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(v, m); }, "vector"_a, "matrix"_a,
           "Applies rotation/scale only (a Vector2D has no position to translate).");
    tf.def("transform", [](geompp::LineSegment2D const& s, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(s, m); }, "segment"_a, "matrix"_a);
    tf.def("transform", [](geompp::Polyline2D const& p, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(p, m); }, "polyline"_a, "matrix"_a);
    tf.def("transform", [](geompp::Triangle2D const& t, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(t, m); }, "triangle"_a, "matrix"_a);
    tf.def("transform", [](geompp::Polygon2D const& p, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(p, m); }, "polygon"_a, "matrix"_a);
    tf.def("transform", [](geompp::Mesh2D const& mesh, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(mesh, m); }, "mesh"_a, "matrix"_a);
    tf.def("transform", [](geompp::PolyMesh2D const& mesh, geompp::maths::Matrix3 const& m) { return geompp::transformations::transform(mesh, m); }, "mesh"_a, "matrix"_a);

    // ── 3D fast-path (Point3D translate/rotate/scale, no matrix) ─────────────────
    tf.def("translate", [](geompp::Point3D const& p, geompp::maths::Vector3 const& offset) { return geompp::transformations::translate(p, offset); },
           "point"_a, "offset"_a, "Translates a Point3D by a plain (x, y, z) offset -- direct arithmetic, no matrix.");
    tf.def("rotate", [](geompp::Point3D const& p, double angle_rad, geompp::maths::Vector3 const& axis) { return geompp::transformations::rotate(p, angle_rad, axis); },
           "point"_a, "angle_rad"_a, "axis"_a, "Rotates a Point3D about the origin around axis (Rodrigues' formula). Raises ValueError if axis is zero-length.");
    tf.def("scale", [](geompp::Point3D const& p, double factor) { return geompp::transformations::scale(p, factor); },
           "point"_a, "factor"_a, "Uniformly scales a Point3D about the origin.");
    tf.def("scale", [](geompp::Point3D const& p, double sx, double sy, double sz) { return geompp::transformations::scale(p, sx, sy, sz); },
           "point"_a, "sx"_a, "sy"_a, "sz"_a, "Non-uniformly scales a Point3D about the origin.");

    // ── 3D general path: transform(obj, Matrix4) ──────────────────────────────────
    tf.def("transform", [](geompp::Point3D const& p, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(p, m); }, "point"_a, "matrix"_a);
    tf.def("transform", [](geompp::Vector3D const& v, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(v, m); }, "vector"_a, "matrix"_a,
           "Applies rotation/scale only (a Vector3D has no position to translate).");
    tf.def("transform", [](geompp::LineSegment3D const& s, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(s, m); }, "segment"_a, "matrix"_a);
    tf.def("transform", [](geompp::Polyline3D const& p, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(p, m); }, "polyline"_a, "matrix"_a);
    tf.def("transform", [](geompp::Triangle3D const& t, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(t, m); }, "triangle"_a, "matrix"_a);
    tf.def("transform", [](geompp::Polygon3D const& p, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(p, m); }, "polygon"_a, "matrix"_a);
    tf.def("transform", [](geompp::Mesh3D const& mesh, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(mesh, m); }, "mesh"_a, "matrix"_a);
    tf.def("transform", [](geompp::PolyMesh3D const& mesh, geompp::maths::Matrix4 const& m) { return geompp::transformations::transform(mesh, m); }, "mesh"_a, "matrix"_a);
}
