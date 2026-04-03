#include "constants.hpp"

namespace geompp {

thread_local int DECIMAL_PRECISION = DP_THREE;

thread_local double DOUBLE_EPSILON = pow(10, -DECIMAL_PRECISION);

}  // namespace geompp
