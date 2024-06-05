#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "lsv_parser.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <filesystem>
#include <iostream>
#include <variant>
#include <vector>

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

namespace g = geompp;
namespace gv = geom_viewer;
namespace fs = std::filesystem;

namespace {

fs::path geoms_path;

std::vector<std::array<float, 3>> ParseShape(gv::LVSParser::ReturnSet const& shape) {
  std::vector<std::array<float, 3>> points;

  if (!shape.has_value()) {
    points.insert(points.end(), {0.0f, 0.0f, 0.0f});
  }

  if (std::holds_alternative<g::Point2D>(shape.value())) {
    auto p = std::get<g::Point2D>(shape.value());
    points.insert(points.end(), {(float)p.x(), (float)p.y(), 0.0f});

  } else if (std::holds_alternative<g::LineSegment2D>(shape.value())) {
    auto seg = std::get<g::LineSegment2D>(shape.value());
    points.insert(points.end(), {(float)seg.First().x(), (float)seg.First().y(), 0.0f});
    points.insert(points.end(), {(float)seg.Last().x(), (float)seg.Last().y(), 0.0f});

  } else {
    // can't really represent Lines or Rays here
    throw std::runtime_error("unhandled geometry ");
  }

  return points;
}

}  // namespace

int main(int argc, char** argv) {
  // init geoms file
  geoms_path = fs::absolute(fs::path(argv[0]).parent_path() / "res");

  //   // Parse geometries
  //   std::string geom_file_path = (geoms_path / "initial_geometries.lsv").string();
  //   auto geom_parser = gv::LVSParser::Open(geom_file_path);  // can throw
  //   std::vector<std::vector<std::array<float, 3>>> shapes_vertices;
  //   while (geom_parser.HasNext()) {
  //     auto geom = geom_parser.Next();
  //     shapes_vertices.push_back(ParseShape(geom));
  //   }

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  // Set OpenGL version to 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a windowed mode window and its OpenGL context
  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Line", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // make the vertices in one single array, with step = 3

  // Build and compile the shader program
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Set up vertex data and buffers
  float vertices[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the line
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    // Swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
