cmake_minimum_required(VERSION 3.15)
project(geompp_python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── pybind11 ────────────────────────────────────────────────────────────────
find_package(pybind11 2.11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    message(STATUS "pybind11 not found in system, fetching via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        v2.13.6
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ─── glog (required by geompp) ───────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    glog
    URL https://github.com/google/glog/archive/refs/tags/v0.7.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(WITH_GFLAGS   OFF CACHE BOOL "" FORCE)
set(WITH_GTEST    OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glog)

# ─── geompp C++ sources ──────────────────────────────────────────────────────
# Compiled directly into the extension to avoid CMake target-name conflicts.
set(GEOMPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../geompp")

set(GEOMPP_SOURCES
    ${GEOMPP_ROOT}/src/constants.cpp
    ${GEOMPP_ROOT}/src/geompp_log.cpp
    ${GEOMPP_ROOT}/src/utils.cpp
    ${GEOMPP_ROOT}/src/lsv_parser.cpp
    ${GEOMPP_ROOT}/src/point2d.cpp
    ${GEOMPP_ROOT}/src/vector2d.cpp
    ${GEOMPP_ROOT}/src/line2d.cpp
    ${GEOMPP_ROOT}/src/ray2d.cpp
    ${GEOMPP_ROOT}/src/line_segment2d.cpp
    ${GEOMPP_ROOT}/src/polyline2d.cpp
    ${GEOMPP_ROOT}/src/triangle2d.cpp
    ${GEOMPP_ROOT}/src/polygon2d.cpp
    ${GEOMPP_ROOT}/src/bbox2d.cpp
    ${GEOMPP_ROOT}/src/point3d.cpp
    ${GEOMPP_ROOT}/src/vector3d.cpp
    ${GEOMPP_ROOT}/src/line3d.cpp
    ${GEOMPP_ROOT}/src/line_segment3d.cpp
    ${GEOMPP_ROOT}/src/polyline3d.cpp
    ${GEOMPP_ROOT}/src/ray3d.cpp
    ${GEOMPP_ROOT}/src/plane.cpp
    ${GEOMPP_ROOT}/src/triangle3d.cpp
    ${GEOMPP_ROOT}/src/polygon3d.cpp
    ${GEOMPP_ROOT}/src/bbox3d.cpp
)

# ─── Python extension module ─────────────────────────────────────────────────
pybind11_add_module(_geompp
    src/bindings.cpp
    ${GEOMPP_SOURCES}
)

target_include_directories(_geompp PRIVATE ${GEOMPP_ROOT}/include)

target_link_libraries(_geompp PRIVATE glog::glog)

# Windows: disable min/max macros from windows.h
if(WIN32)
    target_compile_definitions(_geompp PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# ─── Install into the geompp Python package ──────────────────────────────────
install(TARGETS _geompp DESTINATION geompp)
