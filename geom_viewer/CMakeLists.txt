cmake_minimum_required(VERSION 3.10)

project(geom_viewer)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")


find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
    include 
    ../geompp/include
    ${OPENGL_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIRS} 
    ${GLEW_INCLUDE_DIRS} 
    ${GLFW_INCLUDE_DIRS}
)


add_executable(${PROJECT_NAME} 
    src/lsv_parser.cpp
    main.cpp
)

target_link_libraries(${PROJECT_NAME} 
    ${OPENGL_LIBRARIES} 
    ${GLUT_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    glfw
    geompp
)

# copy resources test
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
