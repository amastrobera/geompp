#include "constants.hpp"

namespace geompp {

#ifdef GEOMPP_NO_THREAD_LOCAL
int DECIMAL_PRECISION = DP_THREE;
DynamicEpsilon DOUBLE_EPSILON;
#else
thread_local int DECIMAL_PRECISION = DP_THREE;
thread_local DynamicEpsilon DOUBLE_EPSILON;
#endif

}  // namespace geompp
