#include "plane.hpp"

#include "line3d.hpp"
//#include "line_segment3d.hpp"
#include "point3d.hpp"
//#include "polygon3d.hpp"
//#include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <tuple>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Plane, Constructor) {}

TEST(Plane, IntersectionWLine) {}

}  // namespace geompp_tests