cmake_minimum_required(VERSION 3.10)
project(geompp_tests)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")



###################
## Prepare GTEST ##
###################

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()


#################
## Build Tests ##
#################

#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

# copy resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/temp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Include directories for library and GoogleTest
include_directories(${CMAKE_SOURCE_DIR}/../geompp/include)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add the executable for tests
add_executable(${PROJECT_NAME}
    src/test_point2d.cpp
    src/test_vector2d.cpp
    src/test_line2d.cpp
    src/test_ray2d.cpp
    src/test_line_segment2d.cpp
    src/test_polyline2d.cpp
    main.cpp
)

#target_include_directories(${PROJECT_NAME} include geompp/include)


# Link the library to the test executable
target_link_libraries(${PROJECT_NAME} gtest gtest_main geompp) # pthread

# Add test cases
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Print the summary of tests
include(CTest)

# Ensure C++20 standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)