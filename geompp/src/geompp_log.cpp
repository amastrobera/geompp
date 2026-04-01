#include "geompp_log.hpp"

#include <mutex>

namespace geompp {

void init_logging() {
  static std::once_flag flag;
  std::call_once(flag, []() {
    if (!google::IsGoogleLoggingInitialized()) {
      google::InitGoogleLogging("geompp");
      FLAGS_logtostderr = true;
    }
  });
}

}  // namespace geompp
