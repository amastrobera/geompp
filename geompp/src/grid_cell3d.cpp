#include "grid_cell3d.hpp"

#include "point3d.hpp"
#include "utils.hpp"

namespace geompp {

GridCell3D GridCell3D::FromPoint(Point3D const& p, double epsilon) {
  return {static_cast<std::int64_t>(std::floor(p.x() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.y() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.z() / epsilon))};
}

bool GridCell3D::operator==(GridCell3D const& other) const { return x == other.x && y == other.y && z == other.z; }

std::size_t GridCell3DHash::operator()(GridCell3D const& cell) const noexcept {
  // Standard hash combination (e.g., boost::hash_combine style)
  std::size_t h1 = std::hash<std::int64_t>{}(cell.x);
  std::size_t h2 = std::hash<std::int64_t>{}(cell.y);
  std::size_t h3 = std::hash<std::int64_t>{}(cell.z);
  return h1 ^ (h2 << 1) ^ (h3 << 2);
}

}  // namespace geompp
