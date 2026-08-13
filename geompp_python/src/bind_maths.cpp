#include "bind_helpers.hpp"

#include <maths.hpp>

// Registers geompp::maths: Vector2/3/4, Matrix2/3/4 (concrete double-valued aliases -- the underlying
// Vector<T,N>/Matrix<T,Rows,Cols> templates themselves aren't bound, same reasoning as Point2D/Point3D
// never exposing a generic Point<T> template to Python), and solve_gauss/solve_cramer overloaded per
// size. Registered as geompp.maths.Vector2/etc, a Python submodule mirroring the C++ geompp::maths
// namespace -- unlike geompp::geometry (an *inline* C++ namespace with no Python equivalent, see
// visual_doc_and_sample_code.md's Modules & Namespaces section), geompp::maths is a genuinely separate,
// non-inline namespace, so a real Python submodule is the faithful mirror here.
namespace {

template <std::size_t N>
void bind_vector(py::module_& m, char const* name) {
    using V = geompp::maths::Vector<double, N>;
    auto cls = py::class_<V>(m, name, "Fixed-size numeric vector -- see geompp::maths::Vector<T,N> (C++).");

    // Component-list constructor: Vector2(1, 2), Vector3(1, 2, 3), Vector4(1, 2, 3, 4).
    if constexpr (N == 2) {
        cls.def(py::init<double, double>(), "x"_a, "y"_a);
    } else if constexpr (N == 3) {
        cls.def(py::init<double, double, double>(), "x"_a, "y"_a, "z"_a);
    } else if constexpr (N == 4) {
        cls.def(py::init<double, double, double, double>(), "x"_a, "y"_a, "z"_a, "w"_a);
    }
    cls.def(py::init<>());

    cls.def("__getitem__", [](V const& v, std::size_t i) { return v.At(i); }, "i"_a)
       .def("__setitem__", [](V& v, std::size_t i, double value) { v.At(i) = value; }, "i"_a, "value"_a)
       .def("__len__", [](V const&) { return N; })
       .def_property_readonly("x", [](V const& v) { return v.x(); })
       .def_property_readonly("y", [](V const& v) { return v.y(); });
    if constexpr (N >= 3) {
        cls.def_property_readonly("z", [](V const& v) { return v.z(); });
    }
    if constexpr (N >= 4) {
        cls.def_property_readonly("w", [](V const& v) { return v.w(); });
    }

    cls.def("dot", &V::Dot, "other"_a)
       .def("length", &V::Length)
       .def("length_squared", &V::LengthSquared)
       .def("normalized", &V::Normalized,
            "Returns a unit-length copy. Raises ValueError if this vector is zero-length.")
       .def_static("zero", &V::Zero)
       .def("__eq__", [](V const& a, V const& b) { return a == b; }, "other"_a)
       .def("__neg__", [](V const& v) { return -v; })
       .def("__add__", [](V const& a, V const& b) { return a + b; }, "other"_a)
       .def("__sub__", [](V const& a, V const& b) { return a - b; }, "other"_a)
       .def("__mul__", [](V const& v, double s) { return v * s; }, "scalar"_a)
       .def("__rmul__", [](V const& v, double s) { return v * s; }, "scalar"_a)
       .def("__truediv__", [](V const& v, double s) { return v / s; }, "scalar"_a)
       .def("__repr__", &V::ToString)
       .def("__str__", &V::ToString);

    if constexpr (N == 3) {
        cls.def("cross", &V::Cross, "other"_a,
                "3D cross product (Vector3 only).");
    }
}

template <std::size_t N>
void bind_matrix(py::module_& m, char const* name) {
    using M = geompp::maths::Matrix<double, N, N>;
    using V = geompp::maths::Vector<double, N>;
    auto cls = py::class_<M>(m, name,
        "Fixed-size square numeric matrix -- see geompp::maths::Matrix<T,Rows,Cols> (C++). Only the "
        "square N x N aliases (Matrix2/3/4) are bound; the generic Rows != Cols template stays C++-only, "
        "same convention as detail:: internals.");

    cls.def(py::init([name](py::sequence elems) {
             if (py::len(elems) != N * N) {
                 throw py::value_error(std::string(name) + "() expects " + std::to_string(N * N) +
                                       " row-major elements");
             }
             M mat;
             for (std::size_t i = 0; i < N * N; ++i) {
                 mat.data()[i] = elems[i].cast<double>();
             }
             return mat;
         }),
         "elements"_a, "Row-major flat list of N*N elements.")
       .def(py::init<>());

    cls.def("__call__", [](M const& m2, std::size_t r, std::size_t c) { return m2.At(r, c); }, "row"_a, "col"_a)
       .def("set", [](M& m2, std::size_t r, std::size_t c, double v) { m2.At(r, c) = v; }, "row"_a, "col"_a, "value"_a)
       .def_static("zero", &M::Zero)
       .def_static("identity", &M::Identity)
       .def("transpose", &M::Transpose)
       .def("determinant", &M::Determinant)
       .def("inverse", &M::Inverse, "Raises ValueError if this matrix is singular.")
       .def("__eq__", [](M const& a, M const& b) { return a == b; }, "other"_a)
       .def("__neg__", [](M const& m2) { return -m2; })
       .def("__add__", [](M const& a, M const& b) { return a + b; }, "other"_a)
       .def("__sub__", [](M const& a, M const& b) { return a - b; }, "other"_a)
       .def("__mul__", [](M const& a, double s) { return a * s; }, "scalar"_a)
       .def("__rmul__", [](M const& a, double s) { return a * s; }, "scalar"_a)
       .def("__truediv__", [](M const& a, double s) { return a / s; }, "scalar"_a)
       .def("__matmul__", [](M const& a, M const& b) -> M { return a * b; }, "other"_a)
       .def("__matmul__", [](M const& a, V const& v) -> V { return a * v; }, "vector"_a)
       .def("__repr__", &M::ToString)
       .def("__str__", &M::ToString);

    if constexpr (N == 4) {
        using V3 = geompp::maths::Vector<double, 3>;
        cls.def_static("translation", [](V3 const& offset) { return M::Translation(offset); }, "offset"_a,
                       "4x4 homogeneous translation matrix.")
           .def_static("rotation",
                       [](double angle_rad, V3 const& axis) { return M::Rotation(angle_rad, axis); },
                       "angle_rad"_a, "axis"_a,
                       "4x4 homogeneous rotation matrix (axis-angle, Rodrigues' formula). Raises "
                       "ValueError if axis is zero-length.")
           .def_static("scale", [](double f) { return M::Scale(f); }, "factor"_a,
                       "4x4 homogeneous uniform scale matrix.")
           .def_static("scale", [](double sx, double sy, double sz) { return M::Scale(sx, sy, sz); },
                       "sx"_a, "sy"_a, "sz"_a, "4x4 homogeneous non-uniform scale matrix.")
           .def_static("shear",
                       [](double xy, double xz, double yx, double yz, double zx, double zy) {
                           return M::Shear(xy, xz, yx, yz, zx, zy);
                       },
                       "xy"_a, "xz"_a, "yx"_a, "yz"_a, "zx"_a, "zy"_a,
                       "4x4 homogeneous shear matrix -- each axis offset by a multiple of the other "
                       "two (xy shears X by Y, zy shears Z by Y, ...).")
           .def_static("reflection", [](V3 const& normal) { return M::Reflection(normal); }, "normal"_a,
                       "4x4 homogeneous reflection matrix across the plane through the origin whose "
                       "normal is `normal` (Householder reflection). Raises ValueError if normal is "
                       "zero-length.");
    }
    if constexpr (N == 3) {
        using V2 = geompp::maths::Vector<double, 2>;
        cls.def_static("translation", [](V2 const& offset) { return M::Translation(offset); }, "offset"_a,
                       "3x3 homogeneous translation matrix (2D) -- geompp.transformations' Matrix3 "
                       "counterpart of Matrix4.translation().")
           .def_static("rotation", [](double angle_rad) { return M::Rotation(angle_rad); }, "angle_rad"_a,
                       "3x3 homogeneous rotation matrix (2D, about the origin).")
           .def_static("scale", [](double f) { return M::Scale(f); }, "factor"_a,
                       "3x3 homogeneous uniform scale matrix (2D).")
           .def_static("scale", [](double sx, double sy) { return M::Scale(sx, sy); },
                       "sx"_a, "sy"_a, "3x3 homogeneous non-uniform scale matrix (2D).")
           .def_static("shear", [](double shx, double shy) { return M::Shear(shx, shy); }, "shx"_a, "shy"_a,
                       "3x3 homogeneous shear matrix (2D) -- shx shears X by Y, shy shears Y by X.")
           .def_static("reflection", [](V2 const& normal) { return M::Reflection(normal); }, "normal"_a,
                       "3x3 homogeneous reflection matrix across the line through the origin whose "
                       "normal is `normal` (2D, Householder reflection). Raises ValueError if normal "
                       "is zero-length.");
    }
}

}  // namespace

void bind_maths(py::module_& m) {
    auto maths = m.def_submodule("maths", "geompp::maths -- fixed-size linear algebra (Vector2/3/4, "
                                          "Matrix2/3/4) backing geompp.transformations.");

    bind_vector<2>(maths, "Vector2");
    bind_vector<3>(maths, "Vector3");
    bind_vector<4>(maths, "Vector4");

    bind_matrix<2>(maths, "Matrix2");
    bind_matrix<3>(maths, "Matrix3");
    bind_matrix<4>(maths, "Matrix4");

    maths.def("solve_gauss",
              [](geompp::maths::Matrix2 const& a, geompp::maths::Vector2 const& b) { return geompp::maths::solve_gauss(a, b); },
              "a"_a, "b"_a, "Solves a*x = b via Gauss-Jordan elimination. Raises ValueError if a is singular.")
         .def("solve_gauss",
              [](geompp::maths::Matrix3 const& a, geompp::maths::Vector3 const& b) { return geompp::maths::solve_gauss(a, b); },
              "a"_a, "b"_a)
         .def("solve_gauss",
              [](geompp::maths::Matrix4 const& a, geompp::maths::Vector4 const& b) { return geompp::maths::solve_gauss(a, b); },
              "a"_a, "b"_a)
         .def("solve_cramer",
              [](geompp::maths::Matrix2 const& a, geompp::maths::Vector2 const& b) { return geompp::maths::solve_cramer(a, b); },
              "a"_a, "b"_a, "Solves a*x = b via Cramer's rule. Raises ValueError if a is singular.")
         .def("solve_cramer",
              [](geompp::maths::Matrix3 const& a, geompp::maths::Vector3 const& b) { return geompp::maths::solve_cramer(a, b); },
              "a"_a, "b"_a)
         .def("solve_cramer",
              [](geompp::maths::Matrix4 const& a, geompp::maths::Vector4 const& b) { return geompp::maths::solve_cramer(a, b); },
              "a"_a, "b"_a);
}
