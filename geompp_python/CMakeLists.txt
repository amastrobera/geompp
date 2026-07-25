cmake_minimum_required(VERSION 3.15)
project(geompp_python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/bind_bball2d.cpp
    src/bind_bball3d.cpp
    src/bind_brect2d.cpp
    src/bind_bprism3d.cpp
    src/bind_plane.cpp
    src/bind_wktparser.cpp
    src/bind_free_functions.cpp
    src/bind_geometry_collection2d.cpp
    src/bind_geometry_collection3d.cpp
    src/bind_view2d.cpp
    src/bind_grid_cell2d.cpp
    src/bind_grid_cell3d.cpp
    src/bind_mesh2d.cpp
    src/bind_mesh3d.cpp
    src/bind_polymesh2d.cpp
    src/bind_polymesh3d.cpp
)

# ─── Python extension module ─────────────────────────────────────────────────
# Three build modes:
#
#  • Parent-CMake mode  (-DBUILD_PYTHON=ON from monorepo root):
#      The `geompp` static-library target already exists. Link against it so
#      we don't recompile sources and glog a second time.
#
#  • Monorepo standalone  (pip install ./geompp_python or pip install -e .):
#      No parent `geompp` target, but the sibling ../geompp/ directory is
#      present. Compile those sources directly into the extension.
#
#  • sdist standalone  (pip install geompp from PyPI tarball):
#      No parent target and no ../geompp/ sibling (the sdist doesn't bundle
#      the core C++ library). Fetch the matching version from GitHub and
#      compile it directly into the extension. Requires network access.
#
if(TARGET geompp)
    # ── parent mode ──────────────────────────────────────────────────────────
    set(GEOMPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../geompp")
    message(STATUS "geompp_python: linking against existing `geompp` target")

    pybind11_add_module(_geompp NO_EXTRAS ${BINDING_SOURCES})
    target_include_directories(_geompp PRIVATE ${GEOMPP_ROOT}/include src)
    # glog is transitively pulled in through geompp's PUBLIC link deps
    target_link_libraries(_geompp PRIVATE geompp)

else()
    # ── standalone mode ───────────────────────────────────────────────────────
    message(STATUS "geompp_python: standalone build — compiling geompp sources into extension")

    # ── Resolve geompp core location ─────────────────────────────────────────
    # Try the sibling directory first (works for a monorepo checkout).
    # If it is absent (sdist build), fetch the core sources from GitHub.
    set(GEOMPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../geompp")
    if(NOT EXISTS "${GEOMPP_ROOT}/src/constants.cpp")
        message(STATUS "geompp_python: geompp core not found at ${GEOMPP_ROOT}")
        message(STATUS "geompp_python: fetching geompp core from GitHub (sdist build)...")

        # Pin the git tag to match the Python package version.
        # Override with -DGEOMPP_CORE_GIT_TAG=<tag> if you need a specific release.
        set(GEOMPP_CORE_GIT_TAG "master" CACHE STRING
            "geompp core git tag/branch to fetch when building from the sdist")

        include(FetchContent)
        FetchContent_Declare(
            geompp_core
            GIT_REPOSITORY https://github.com/amastrobera/geompp.git
            GIT_TAG        ${GEOMPP_CORE_GIT_TAG}
            GIT_SHALLOW    TRUE
        )
        FetchContent_GetProperties(geompp_core)
        if(NOT geompp_core_POPULATED)
            FetchContent_Populate(geompp_core)
        endif()
        # The monorepo lays out geompp/geompp/{include,src}, so the core is one
        # level down from the repo root.
        set(GEOMPP_ROOT "${geompp_core_SOURCE_DIR}/geompp")
        message(STATUS "geompp_python: using fetched geompp core at ${GEOMPP_ROOT}")
    endif()

    # ── geompp sources ────────────────────────────────────────────────────────
    set(GEOMPP_SOURCES
        ${GEOMPP_ROOT}/src/constants.cpp
        ${GEOMPP_ROOT}/src/geompp_log.cpp
        ${GEOMPP_ROOT}/src/utils.cpp
        ${GEOMPP_ROOT}/src/wkt_parser.cpp
        ${GEOMPP_ROOT}/src/point2d.cpp
        ${GEOMPP_ROOT}/src/vector2d.cpp
        ${GEOMPP_ROOT}/src/line2d.cpp
        ${GEOMPP_ROOT}/src/ray2d.cpp
        ${GEOMPP_ROOT}/src/line_segment2d.cpp
        ${GEOMPP_ROOT}/src/polyline2d.cpp
        ${GEOMPP_ROOT}/src/triangle2d.cpp
        ${GEOMPP_ROOT}/src/polygon2d.cpp
        ${GEOMPP_ROOT}/src/bbox2d.cpp
        ${GEOMPP_ROOT}/src/bball2d.cpp
        ${GEOMPP_ROOT}/src/brect2d.cpp
        ${GEOMPP_ROOT}/src/calc_utils2d.cpp
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
        ${GEOMPP_ROOT}/src/bball3d.cpp
        ${GEOMPP_ROOT}/src/bprism3d.cpp
        ${GEOMPP_ROOT}/src/geometry_collection2d.cpp
        ${GEOMPP_ROOT}/src/geometry_collection3d.cpp
        ${GEOMPP_ROOT}/src/calc_utils3d.cpp
        ${GEOMPP_ROOT}/src/view2d.cpp
    )

    # ── glog ─────────────────────────────────────────────────────────────────
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

# Link-Time Optimization (Release builds only)
include(CheckIPOSupported)
check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_err)
if(_ipo_supported)
    set_property(TARGET _geompp PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(STATUS "geompp_python: LTO not supported: ${_ipo_err}")
endif()

# ─── Post-build: copy extension into package directory ───────────────────────
add_custom_command(TARGET _geompp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:_geompp>
        "${CMAKE_CURRENT_SOURCE_DIR}/geompp/$<TARGET_FILE_NAME:_geompp>"
    COMMENT "Copying _geompp into geompp_python/geompp/"
)

# ─── Install ─────────────────────────────────────────────────────────────────
# pip/scikit-build-core: installs _geompp.so|pyd into the `geompp` package dir.
# Root-CMake: installs alongside the rest of the project if `cmake --install` is run.
install(TARGETS _geompp DESTINATION geompp)
