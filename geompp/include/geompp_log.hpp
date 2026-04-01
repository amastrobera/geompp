#pragma once

#include <glog/logging.h>

namespace geompp {

void init_logging();

}  // namespace geompp

#define GEOMPP_LOG(level) (geompp::init_logging(), LOG(level))
