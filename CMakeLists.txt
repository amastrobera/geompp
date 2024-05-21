cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(geompp_proj)

include_directories(${CMAKE_SOURCE_DIR}/geompp/include)

add_subdirectory(geompp)

add_subdirectory(geompp_tests)