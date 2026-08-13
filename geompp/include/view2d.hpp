#pragma once

#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <utility>

namespace geompp {

inline namespace geometry {

enum class ProjectionType { XY, YZ, ZX, Custom };

class View2D {
 public:
  static View2D XY(double z_offset = 0.0);
  static View2D YZ(double x_offset = 0.0);
  static View2D ZX(double y_offset = 0.0);
  static View2D OnPlane(Plane ref_plane);
  View2D(View2D const&) = default;
  View2D(View2D&&) = default;
  ~View2D() = default;

  double x(Point2D const& p) const;
  double y(Point2D const& p) const;
  std::pair<double, double> xy(Point2D const& p) const;

  double x(Point3D const& p) const;
  double y(Point3D const& p) const;
  std::pair<double, double> xy(Point3D const& p) const;

  /// @brief Reconstructs the 3D point that a (x, y) pair in this view's 2D space corresponds to —
  /// the inverse of x()/y()/xy() on Point3D.
  /// @warning Only exact when this View2D truly represents the target plane: OnPlane()/Custom always
  /// qualify; XY(z)/YZ(x)/ZX(y) qualify only when the source plane really is parallel to that world
  /// plane at that offset. A View2D built as a cheap dominant-axis *approximation* of a tilted plane
  /// (e.g. the bare XY()/YZ()/ZX() picked in Polygon3D::IsSimple()) is NOT safe to unproject through
  /// this — its dropped coordinate isn't constant, so this would silently return the wrong point.
  Point3D xyz(Point2D const& p) const;
  Point3D xyz(double x, double y) const;

  ProjectionType type() const;

 private:
  View2D(ProjectionType type, Point3D origin, Vector3D axis_u, Vector3D axis_v, double plane_offset = 0.0);

  ProjectionType TYPE;
  Point3D ORIGIN;
  Vector3D AXIS_U, AXIS_V;
  double PLANE_OFFSET;  // value of the dropped world coordinate for XY/YZ/ZX; unused for Custom (ORIGIN
                        // already carries the real embedding there)
};

#pragma region Inlined Functions

inline View2D::View2D(ProjectionType type, Point3D origin, Vector3D axis_u, Vector3D axis_v, double plane_offset)
    : TYPE(type), ORIGIN(origin), AXIS_U(axis_u), AXIS_V(axis_v), PLANE_OFFSET(plane_offset) {}

inline View2D View2D::XY(double z_offset) {
  return View2D(ProjectionType::XY, {0, 0, z_offset}, {1, 0, 0}, {0, 1, 0}, z_offset);
}

inline View2D View2D::YZ(double x_offset) {
  return View2D(ProjectionType::YZ, {x_offset, 0, 0}, {0, 1, 0}, {0, 0, 1}, x_offset);
}

inline View2D View2D::ZX(double y_offset) {
  return View2D(ProjectionType::ZX, {0, y_offset, 0}, {0, 0, 1}, {1, 0, 0}, y_offset);
}

inline View2D View2D::OnPlane(Plane ref_plane) {
  return View2D(ProjectionType::Custom, ref_plane.origin(), ref_plane.axis_u(), ref_plane.axis_v());
}

inline ProjectionType View2D::type() const { return TYPE; }

inline double View2D::x(Point2D const& p) const { return p.x(); }
inline double View2D::y(Point2D const& p) const { return p.y(); }
inline std::pair<double, double> View2D::xy(Point2D const& p) const { return {p.x(), p.y()}; }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
