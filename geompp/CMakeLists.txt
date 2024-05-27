cmake_minimum_required(VERSION 3.10)

project(geompp)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Include directories
include_directories(include)


# Add the library
add_library(${PROJECT_NAME}
    src/point2d.cpp
    src/vector2d.cpp
    src/line2d.cpp
    src/ray2d.cpp
    src/utils.cpp
    src/line_segment2d.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
