cmake_minimum_required(VERSION 3.10)

project(geompp VERSION 0.8.0)


# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Include directories
include_directories(include)

##################
## Prepare GLOG ##
##################

include(FetchContent)
FetchContent_Declare(
  glog
  URL https://github.com/google/glog/archive/refs/tags/v0.7.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(WITH_GFLAGS     OFF CACHE BOOL "" FORCE)
set(WITH_GTEST      OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Build static library
set(BUILD_TESTING   OFF CACHE BOOL "" FORCE) # Prevent glog from registering its own tests
FetchContent_MakeAvailable(glog)


# Add the library
add_library(${PROJECT_NAME}
    include/concepts.hpp
    include/coordinate_frame.hpp
    include/segment_iterator2d.hpp
    include/segment_iterator3d.hpp
    src/constants.cpp
    src/geompp_log.cpp
    src/utils.cpp
    src/wkt_parser.cpp

    src/point2d.cpp
    src/vector2d.cpp
    src/line2d.cpp
    src/ray2d.cpp
    src/line_segment2d.cpp
    src/polyline2d.cpp
    src/triangle2d.cpp
    src/polygon2d.cpp
    src/bbox2d.cpp
    src/bball2d.cpp
    src/brect2d.cpp
    src/calc_utils2d.cpp
    src/grid_cell2d.cpp
    src/mesh2d.cpp
    src/polymesh2d.cpp

    src/point3d.cpp
    src/vector3d.cpp
    src/line3d.cpp
    src/line_segment3d.cpp
    src/polyline3d.cpp
    src/ray3d.cpp
    src/plane.cpp
    src/triangle3d.cpp
    src/polygon3d.cpp
    src/bbox3d.cpp
    src/bball3d.cpp
    src/bprism3d.cpp
    src/geometry_collection2d.cpp
    src/geometry_collection3d.cpp
    src/calc_utils3d.cpp
    src/view2d.cpp
    src/grid_cell3d.cpp
    src/mesh3d.cpp
    src/polymesh3d.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC glog::glog)

# Required when geompp is linked into a shared library (e.g. Python extension .so)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Avoid PDB write collisions when building with /MP (parallel compilation)
if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /FS)
endif()

# Link-Time Optimization (Release builds only — Debug LTO adds cost with no benefit)
include(CheckIPOSupported)
check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_err)
if(_ipo_supported)
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
  message(STATUS "geompp: LTO not supported: ${_ipo_err}")
endif()
