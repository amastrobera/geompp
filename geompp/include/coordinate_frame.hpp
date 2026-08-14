#pragma once

#include "vector3d.hpp"

namespace geompp {

inline namespace geometry {

struct CoordinateFrame {
  Vector3D X;  // primary axis   — direction of largest variance (longest spread)
  Vector3D Y;  // secondary axis — direction of second largest variance
  Vector3D Z;  // normal         — direction of least variance (perpendicular to best-fit plane)
};

}  // namespace geometry

}  // namespace geompp
