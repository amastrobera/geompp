cmake_minimum_required(VERSION 3.10)
project(geompp_tests)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Link the library to the test executable
target_link_libraries(${PROJECT_NAME} geompp)

include_directories(${PROJECT_NAME}/include geompp/include)


# Enable testing
enable_testing()

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Link GoogleTest to the test executable
target_link_libraries(${PROJECT_NAME} gtest gtest_main geompp)

# Add test cases
add_test(NAME ${PROJECT_NAME} COMMAND geompp_test)

# Print the summary of tests
include(CTest)