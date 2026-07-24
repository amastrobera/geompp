#pragma once

#include "constants.hpp"

namespace geompp {

class Point3D;

struct GridCell3D {
  std::int64_t x, y, z;

  // Convert Point3D to GridCell3D
  static GridCell3D FromPoint(Point3D const& p, double epsilon = DOUBLE_EPSILON);

  bool operator==(GridCell3D const& other) const;
};

// Custom Hash for GridCell3D
struct GridCell3DHash {
  std::size_t operator()(GridCell3D const& cell) const noexcept;
};

}  // namespace geompp
