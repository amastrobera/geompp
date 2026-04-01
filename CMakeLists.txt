cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")



project(geompp_proj)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/geompp/include)

add_subdirectory(geompp)

add_subdirectory(geompp_tests)

add_subdirectory(geompp_sample)

#add_subdirectory(geom_viewer)