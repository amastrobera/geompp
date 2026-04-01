cmake_minimum_required(VERSION 3.10)
project(geompp_sample)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")


##################
## Build Sample ##
##################


# Include directories for library and GoogleTest
include_directories(${CMAKE_SOURCE_DIR}/../geompp/include)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add the executable for tests
add_executable(${PROJECT_NAME}
    sample.cpp
)

# Link the library to the test executable
target_link_libraries(${PROJECT_NAME} geompp) # pthread

# Ensure C++20 standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)