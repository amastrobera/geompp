#include "geompp_log.hpp"

#include <mutex>

namespace geompp {

inline namespace geometry {

void init_logging() {
  static std::once_flag flag;
  std::call_once(flag, []() {
    if (!google::IsGoogleLoggingInitialized()) {
      google::InitGoogleLogging("geompp");
      FLAGS_logtostderr = true;
    }
  });
}

}  // namespace geometry

}  // namespace geompp
