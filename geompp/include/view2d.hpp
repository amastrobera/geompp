#pragma once

#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <utility>

namespace geompp {

enum class ProjectionType { XY, YZ, ZX, Custom };

class View2D {
 public:
  static View2D XY();
  static View2D YZ();
  static View2D ZX();
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

  ProjectionType type() const;

 private:
  View2D(ProjectionType type, Point3D origin, Vector3D axis_u, Vector3D axis_v);

  ProjectionType TYPE;
  Point3D ORIGIN;
  Vector3D AXIS_U, AXIS_V;
};

#pragma region Inlined Functions

inline View2D::View2D(ProjectionType type, Point3D origin, Vector3D axis_u, Vector3D axis_v)
    : TYPE(type), ORIGIN(origin), AXIS_U(axis_u), AXIS_V(axis_v) {}

inline View2D View2D::XY() { return View2D(ProjectionType::XY, {0, 0, 0}, {1, 0, 0}, {0, 1, 0}); }

inline View2D View2D::YZ() { return View2D(ProjectionType::YZ, {0, 0, 0}, {0, 1, 0}, {0, 0, 1}); }

inline View2D View2D::ZX() { return View2D(ProjectionType::ZX, {0, 0, 0}, {0, 0, 1}, {1, 0, 0}); }

inline View2D View2D::OnPlane(Plane ref_plane) {
  return View2D(ProjectionType::Custom, ref_plane.origin(), ref_plane.axis_u(), ref_plane.axis_v());
}

inline ProjectionType View2D::type() const { return TYPE; }

inline double View2D::x(Point2D const& p) const { return p.x(); }
inline double View2D::y(Point2D const& p) const { return p.y(); }
inline std::pair<double, double> View2D::xy(Point2D const& p) const { return {p.x(), p.y()}; }

#pragma endregion

}  // namespace geompp
