cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(geompp_proj)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/geompp/include)

add_subdirectory(geompp)
add_subdirectory(geompp_tests)
add_subdirectory(geompp_sample)

# ─── Python bindings (opt-in) ────────────────────────────────────────────────
# Enable with:  cmake -DBUILD_PYTHON=ON ...
# Requires Python 3.8+ dev headers and pip install pybind11
option(BUILD_PYTHON "Build Python bindings for geompp" OFF)

if(BUILD_PYTHON)
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)
    add_subdirectory(geompp_python)
endif()

#add_subdirectory(geom_viewer)
