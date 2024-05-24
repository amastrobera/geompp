#pragma once

#include <cmath>
#include <variant>

namespace geompp {


#pragma region Fwd Declarations
class Point2D;
#pragma endregion


#pragma region Precision

const int DP_THREE = 3;
const int DP_SIX = 6;
const int DP_NINE = 9;


#pragma endregion


#pragma region ReturnTypes

using Shape2D = std::variant<Point2D>;

#pragma endregion






}