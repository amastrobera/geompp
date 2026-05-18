#pragma once

#include "constants.hpp"
#include "point3d.hpp"

#include <optional>

namespace geompp {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc);

std::optional<Point3D> intersection_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                                 Point3D const& L2_P1, double& sc, double& tc);

}  // namespace geompp
