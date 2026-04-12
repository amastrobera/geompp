cmake_minimum_required(VERSION 3.15)
project(geompp_python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GEOMPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../geompp")

# ─── pybind11 ────────────────────────────────────────────────────────────────
# Prefer an installed copy; fall back to FetchContent.
find_package(pybind11 2.11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    message(STATUS "pybind11 not found, fetching via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        URL      https://github.com/pybind/pybind11/archive/refs/tags/v2.13.6.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ─── geompp sources (only needed in standalone / pip-install mode) ────────────
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

# ─── Binding sources (one file per class) ─────────────────────────────────────
set(BINDING_SOURCES
    src/bindings.cpp
    src/bind_precision.cpp
    src/bind_point2d.cpp
    src/bind_point3d.cpp
    src/bind_vector2d.cpp
    src/bind_vector3d.cpp
    src/bind_line_segment2d.cpp
    src/bind_line_segment3d.cpp
    src/bind_line2d.cpp
    src/bind_line3d.cpp
    src/bind_ray2d.cpp
    src/bind_ray3d.cpp
    src/bind_polygon2d.cpp
    src/bind_polygon3d.cpp
    src/bind_polyline2d.cpp
    src/bind_polyline3d.cpp
    src/bind_triangle2d.cpp
    src/bind_triangle3d.cpp
    src/bind_bbox2d.cpp
    src/bind_bbox3d.cpp
    src/bind_plane.cpp
    src/bind_lvsparser.cpp
    src/bind_free_functions.cpp
)

# ─── Python extension module ─────────────────────────────────────────────────
# Two modes:
#
#  • Parent-CMake mode  (-DBUILD_PYTHON=ON from root):
#      The `geompp` static-library target already exists. Link against it so
#      we don't recompile 23 source files and glog a second time.
#
#  • Standalone mode  (pip install ./geompp_python):
#      No parent target. Compile geompp sources directly into the extension
#      and fetch glog ourselves.
#
if(TARGET geompp)
    # ── parent mode ──────────────────────────────────────────────────────────
    message(STATUS "geompp_python: linking against existing `geompp` target")

    pybind11_add_module(_geompp NO_EXTRAS ${BINDING_SOURCES})
    target_include_directories(_geompp PRIVATE ${GEOMPP_ROOT}/include src)
    # glog is transitively pulled in through geompp's PUBLIC link deps
    target_link_libraries(_geompp PRIVATE geompp)

else()
    # ── standalone mode ───────────────────────────────────────────────────────
    message(STATUS "geompp_python: standalone build — compiling geompp sources into extension")

    if(NOT TARGET glog::glog)
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
    endif()

    pybind11_add_module(_geompp NO_EXTRAS ${BINDING_SOURCES} ${GEOMPP_SOURCES})
    target_include_directories(_geompp PRIVATE ${GEOMPP_ROOT}/include src)
    target_link_libraries(_geompp PRIVATE glog::glog)

endif()

# Windows: suppress min/max macros from <windows.h>
if(WIN32)
    target_compile_definitions(_geompp PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# ─── Post-build: copy extension + run smoke tests ────────────────────────────
# Copy first; then run pytest if it is importable in Python_EXECUTABLE.
# A test failure makes `cmake --build` exit non-zero.
execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import pytest"
    RESULT_VARIABLE _pytest_found
    OUTPUT_QUIET ERROR_QUIET
)

if(_pytest_found EQUAL 0)
    add_custom_command(TARGET _geompp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:_geompp>
            "${CMAKE_CURRENT_SOURCE_DIR}/geompp/$<TARGET_FILE_NAME:_geompp>"
        COMMAND ${Python_EXECUTABLE} -m pytest tests/ -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python smoke tests"
    )
else()
    message(STATUS "pytest not found in ${Python_EXECUTABLE} — smoke tests skipped (pip install pytest)")
    add_custom_command(TARGET _geompp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:_geompp>
            "${CMAKE_CURRENT_SOURCE_DIR}/geompp/$<TARGET_FILE_NAME:_geompp>"
        COMMENT "Copying _geompp into geompp_python/geompp/"
    )
endif()

# ─── Install ─────────────────────────────────────────────────────────────────
# pip/scikit-build-core: installs _geompp.so|pyd into the `geompp` package dir.
# Root-CMake: installs alongside the rest of the project if `cmake --install` is run.
install(TARGETS _geompp DESTINATION geompp)
