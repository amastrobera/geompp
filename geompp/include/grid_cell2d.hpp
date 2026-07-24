#pragma once

#include "constants.hpp"

namespace geompp {

class Point2D;

struct GridCell2D {
  std::int64_t x, y;

  // Convert Point3D to GridCell2D
  static GridCell2D FromPoint(Point2D const& p, double epsilon = DOUBLE_EPSILON);

  bool operator==(GridCell2D const& other) const;
};

// Custom Hash for GridCell2D
struct GridCell2DHash {
  std::size_t operator()(GridCell2D const& cell) const noexcept;
};

}  // namespace geompp
