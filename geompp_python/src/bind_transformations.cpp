#include "bind_helpers.hpp"

#include <transformations.hpp>

// Registers geompp::transformations as a real Python submodule (geompp.transformations), same reasoning
// as geompp.maths (bind_maths.cpp): a genuinely separate, non-inline C++ namespace, unlike
// geompp::geometry's *inline* one which has no Python equivalent to mirror.
void bind_transformations(py::module_& m) {
    auto tf = m.def_submodule("transformations",
        "geompp::transformations -- affine transforms for geompp primitives, built on geompp.maths "
        "(Matrix3/Matrix4). translate()/rotate()/scale()/shear()/reflect() are the fast, single-Point, "
        "no-matrix path; transform(obj, matrix) is the general path covering every primitive from Point "
        "to PolyMesh.");

    // ── TransformBuilder3D ─────────────────────────────────────────────────────
    py::class_<geompp::transformations::TransformBuilder3D>(tf, "TransformBuilder3D",
        "Fluent composer for a single 4x4 homogeneous affine transform. Each translate()/rotate()/"
        "scale()/combine() call PRE-multiplies the new operation onto the accumulated matrix, so "
        "chained calls apply in the order they're written: builder.translate(t).rotate(r) moves a "
        "point by t first, then rotates the result by r. See TransformBuilder2D for the Matrix3-backed "
        "2D counterpart.")
        .def(py::init<>())
        .def("translate",
             [](geompp::transformations::TransformBuilder3D& b, geompp::maths::Vector3 const& offset)
                 -> geompp::transformations::TransformBuilder3D& { return b.Translate(offset); },
             "offset"_a, py::return_value_policy::reference_internal)
        .def("rotate",
             [](geompp::transformations::TransformBuilder3D& b, double angle_rad, geompp::maths::Vector3 const& axis)
                 -> geompp::transformations::TransformBuilder3D& { return b.Rotate(angle_rad, axis); },
             "angle_rad"_a, "axis"_a, py::return_value_policy::reference_internal,
             "Raises ValueError if axis is zero-length.")
        .def("scale",
             [](geompp::transformations::TransformBuilder3D& b, double factor)
                 -> geompp::transformations::TransformBuilder3D& { return b.Scale(factor); },
             "factor"_a, py::return_value_policy::reference_internal)
        .def("scale",
             [](geompp::transformations::TransformBuilder3D& b, double sx, double sy, double sz)
                 -> geompp::transformations::TransformBuilder3D& { return b.Scale(sx, sy, sz); },
             "sx"_a, "sy"_a, "sz"_a, py::return_value_policy::reference_internal)
        .def("shear",
             [](geompp::transformations::TransformBuilder3D& b, double xy, double xz, double yx, double yz,
                double zx, double zy) -> geompp::transformations::TransformBuilder3D& {
                 return b.Shear(xy, xz, yx, yz, zx, zy);
             },
             "xy"_a, "xz"_a, "yx"_a, "yz"_a, "zx"_a, "zy"_a, py::return_value_policy::reference_internal)
        .def("reflect",
             [](geompp::transformations::TransformBuilder3D& b, geompp::maths::Vector3 const& normal)
                 -> geompp::transformations::TransformBuilder3D& { return b.Reflect(normal); },
             "normal"_a, py::return_value_policy::reference_internal,
             "Raises ValueError if normal is zero-length.")
        .def("combine",
             [](geompp::transformations::TransformBuilder3D& b, geompp::maths::Matrix4 const& mat)
                 -> geompp::transformations::TransformBuilder3D& { return b.Combine(mat); },
             "matrix"_a, py::return_value_policy::reference_internal)
        .def("get", &geompp::transformations::TransformBuilder3D::Get)
        .def("build", &geompp::transformations::TransformBuilder3D::Build);

    // ── TransformBuilder2D ─────────────────────────────────────────────────────
    py::class_<geompp::transformations::TransformBuilder2D>(tf, "TransformBuilder2D",
        "Fluent composer for a single 3x3 homogeneous affine transform. 2D counterpart of "
        "TransformBuilder3D -- same call-order composition semantics; rotate() takes a plain angle (no "
        "axis) and shear() takes 2 terms rather than 6.")
        .def(py::init<>())
        .def("translate",
             [](geompp::transformations::TransformBuilder2D& b, geompp::maths::Vector2 const& offset)
                 -> geompp::transformations::TransformBuilder2D& { return b.Translate(offset); },
             "offset"_a, py::return_value_policy::reference_internal)
        .def("rotate",
             [](geompp::transformations::TransformBuilder2D& b, double angle_rad)
                 -> geompp::transformations::TransformBuilder2D& { return b.Rotate(angle_rad); },
             "angle_rad"_a, py::return_value_policy::reference_internal)
        .def("scale",
             [](geompp::transformations::TransformBuilder2D& b, double factor)
                 -> geompp::transformations::TransformBuilder2D& { return b.Scale(factor); },
             "factor"_a, py::return_value_policy::reference_internal)
        .def("scale",
             [](geompp::transformations::TransformBuilder2D& b, double sx, double sy)
                 -> geompp::transformations::TransformBuilder2D& { return b.Scale(sx, sy); },
             "sx"_a, "sy"_a, py::return_value_policy::reference_internal)
        .def("shear",
             [](geompp::transformations::TransformBuilder2D& b, double shx, double shy)
                 -> geompp::transformations::TransformBuilder2D& { return b.Shear(shx, shy); },
             "shx"_a, "shy"_a, py::return_value_policy::reference_internal)
        .def("reflect",
             [](geompp::transformations::TransformBuilder2D& b, geompp::maths::Vector2 const& normal)
                 -> geompp::transformations::TransformBuilder2D& { return b.Reflect(normal); },
             "normal"_a, py::return_value_policy::reference_internal,
             "Raises ValueError if normal is zero-length.")
        .def("combine",
             [](geompp::transformations::TransformBuilder2D& b, geompp::maths::Matrix3 const& mat)
                 -> geompp::transformations::TransformBuilder2D& { return b.Combine(mat); },
             "matrix"_a, py::return_value_policy::reference_internal)
        .def("get", &geompp::transformations::TransformBuilder2D::Get)
        .def("build", &geompp::transformations::TransformBuilder2D::Build);

    // ── 2D fast-path (Point2D translate/rotate/scale, no matrix) ─────────────────
    tf.def("translate", [](geompp::Point2D const& p, geompp::maths::Vector2 const& offset) { return geompp::transformations::translate(p, offset); },
           "point"_a, "offset"_a, "Translates a Point2D by a plain (x, y) offset -- direct arithmetic, no matrix.");
    tf.def("rotate", [](geompp::Point2D const& p, double angle_rad) { return geompp::transformations::rotate(p, angle_rad); },
           "point"_a, "angle_rad"_a, "Rotates a Point2D about the origin -- direct arithmetic, no matrix.");
    tf.def("scale", [](geompp::Point2D const& p, double factor) { return geompp::transformations::scale(p, factor); },
           "point"_a, "factor"_a, "Uniformly scales a Point2D about the origin.");
    tf.def("scale", [](geompp::Point2D const& p, double sx, double sy) { return geompp::transformations::scale(p, sx, sy); },
           "point"_a, "sx"_a, "sy"_a, "Non-uniformly scales a Point2D about the origin.");
    tf.def("shear", [](geompp::Point2D const& p, double shx, double shy) { return geompp::transformations::shear(p, shx, shy); },
           "point"_a, "shx"_a, "shy"_a, "Shears a Point2D -- direct arithmetic, no matrix.");
    tf.def("reflect", [](geompp::Point2D const& p, geompp::maths::Vector2 const& normal) { return geompp::transformations::reflect(p, normal); },
           "point"_a, "normal"_a, "Reflects a Point2D across the line through the origin whose normal is `normal`. "
           "Raises ValueError if normal is zero-length.");

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
    tf.def("shear", [](geompp::Point3D const& p, double xy, double xz, double yx, double yz, double zx, double zy) {
               return geompp::transformations::shear(p, xy, xz, yx, yz, zx, zy);
           },
           "point"_a, "xy"_a, "xz"_a, "yx"_a, "yz"_a, "zx"_a, "zy"_a,
           "Shears a Point3D -- each axis offset by a multiple of the other two, direct arithmetic, no matrix.");
    tf.def("reflect", [](geompp::Point3D const& p, geompp::maths::Vector3 const& normal) { return geompp::transformations::reflect(p, normal); },
           "point"_a, "normal"_a, "Reflects a Point3D across the plane through the origin whose normal is `normal`. "
           "Raises ValueError if normal is zero-length.");

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
