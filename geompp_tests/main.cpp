#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace geompp_tests {

fs::path test_res_path;

}  // namespace geompp_tests

// Run all the tests that were declared with TEST()
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  // init static variables
  geompp_tests::test_res_path = fs::absolute(fs::path(argv[0]).parent_path() / "res");

  std::cout << "test_res_path initialized to: " << geompp_tests::test_res_path << std::endl;

  return RUN_ALL_TESTS();
}
