#pragma once

#include <glog/logging.h>

namespace geompp {

inline namespace geometry {

void init_logging();

}  // namespace geometry

}  // namespace geompp

#define GEOMPP_LOG(level) (geompp::init_logging(), LOG(level))
