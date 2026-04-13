cmake_minimum_required(VERSION 3.10)

project(geompp VERSION 0.1.1)


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
FetchContent_MakeAvailable(glog)


# Add the library
add_library(${PROJECT_NAME}
    src/constants.cpp
    src/geompp_log.cpp
    src/utils.cpp
    src/lsv_parser.cpp

    src/point2d.cpp
    src/vector2d.cpp
    src/line2d.cpp
    src/ray2d.cpp
    src/line_segment2d.cpp
    src/polyline2d.cpp
    src/triangle2d.cpp
    src/polygon2d.cpp
    src/bbox2d.cpp

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
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC glog::glog)

# Required when geompp is linked into a shared library (e.g. Python extension .so)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
