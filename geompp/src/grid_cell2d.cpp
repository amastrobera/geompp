#include "grid_cell2d.hpp"

#include "point2d.hpp"
#include "utils.hpp"

namespace geompp {

GridCell2D GridCell2D::FromPoint(Point2D const& p, double epsilon) {
  return {static_cast<std::int64_t>(std::floor(p.x() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.y() / epsilon))};
}

bool GridCell2D::operator==(GridCell2D const& other) const { return x == other.x && y == other.y; }

std::size_t GridCell2DHash::operator()(GridCell2D const& cell) const noexcept {
  // Standard hash combination (e.g., boost::hash_combine style)
  std::size_t h1 = std::hash<std::int64_t>{}(cell.x);
  std::size_t h2 = std::hash<std::int64_t>{}(cell.y);
  return h1 ^ (h2 << 1);
}

}  // namespace geompp
