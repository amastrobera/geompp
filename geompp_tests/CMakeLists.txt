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
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
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

# Include directories for library and GoogleTest
include_directories(${CMAKE_SOURCE_DIR}/../geompp/include)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add the executable for tests
add_executable(${PROJECT_NAME}
    src/test_point2d.cpp
    src/test_point3d.cpp
    src/test_vector2d.cpp
    src/test_vector3d.cpp
    src/test_line2d.cpp
    src/test_line3d.cpp
    src/test_ray2d.cpp
    src/test_ray3d.cpp
    src/test_line_segment2d.cpp
    src/test_line_segment3d.cpp
    src/test_polyline2d.cpp
    src/test_triangle2d.cpp

    src/test_triangle3d.cpp
    src/test_plane.cpp
    src/test_bbox2d.cpp
    src/test_bbox3d.cpp
    src/test_bball2d.cpp
    src/test_bball3d.cpp
    src/test_brect2d.cpp
    src/test_bprism3d.cpp
    src/test_polygon2d.cpp
    src/test_polygon3d.cpp

    src/test_wkt_parser.cpp
    src/test_polyline3d.cpp
    src/test_utils.cpp
    src/test_geometry_collection2d.cpp
    src/test_geometry_collection3d.cpp
    src/test_calc_utils3d.cpp
    src/test_calc_utils2d.cpp
    src/test_view2d.cpp

    main.cpp
)

#target_include_directories(${PROJECT_NAME} include geompp/include)


# Link the library to the test executable
target_link_libraries(${PROJECT_NAME} gtest gtest_main geompp) # pthread

# Copy resources next to the executable after build.
# $<TARGET_FILE_DIR> resolves correctly for both Ninja (flat) and VS generator (Release/ subdir)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)

# Add test cases
# WORKING_DIRECTORY ensures the test runs from the executable's directory,
# where the res/ folder is copied, so argv[0]-based path resolution works on all platforms.
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# Print the summary of tests
include(CTest)

# Ensure C++20 standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)