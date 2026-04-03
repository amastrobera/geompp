#pragma once

#include <cmath>
#include <thread>

namespace geompp {

#pragma region Precision

const int DP_THREE = 3;
const int DP_SIX = 6;
const int DP_NINE = 9;

extern thread_local int DECIMAL_PRECISION;

extern thread_local double DOUBLE_EPSILON;

#pragma endregion

}  // namespace geompp
