#pragma once

#include <cmath>
#include <thread>

namespace geompp {

#pragma region Precision

const int DP_THREE = 3;
const int DP_SIX = 6;
const int DP_NINE = 9;

#ifdef GEOMPP_NO_THREAD_LOCAL
extern int DECIMAL_PRECISION;
#else
extern thread_local int DECIMAL_PRECISION;
#endif

struct DynamicEpsilon {
  operator double() const { return pow(10.0, -DECIMAL_PRECISION); }
};

#ifdef GEOMPP_NO_THREAD_LOCAL
extern DynamicEpsilon DOUBLE_EPSILON;
#else
extern thread_local DynamicEpsilon DOUBLE_EPSILON;
#endif

#pragma endregion

enum class GeometryType {
  Point,
  Line,
  Ray,
  LineSegment,
  Polyline,
  Triangle,
  Polygon,
  GeometryCollection
};  // update as needed

enum class PolylineDecimationStrategy { RadialDistance, RamerDouglasPeucker, VisvalingamWhyatt };

}  // namespace geompp
