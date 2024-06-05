#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "lsv_parser.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
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

std::vector<float> ParseShape(g::Point2D p) { return {(float)p.x(), (float)p.y(), 0.0f}; }

std::vector<float> ParseShape(g::LineSegment2D seg) {
  return {(float)seg.First().x(), (float)seg.First().y(), 0.0f, (float)seg.Last().x(), (float)seg.Last().y(), 0.0f};
}

}  // namespace

int main(int argc, char** argv) {
  // init GL application
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Line and Points", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // make a multi-shape vertex array

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

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // init geoms file
  geoms_path = fs::absolute(fs::path(argv[0]).parent_path() / "res");

  // Parse geometries
  std::string geom_file_path = (geoms_path / "initial_geometries.lsv").string();
  auto geom_parser = gv::LVSParser::Open(geom_file_path);  // can throw

  // will this stupid hack work ?
  auto normalize_to_viewport = [](float value, float min, float max) -> float {
    return 2.0f * (value - min) / (max - min) - 1.0f;
  };

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }

    // Clear the screen before rendering each frame
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // make vertices according to the shape
    while (geom_parser.HasNext()) {
      auto geom = geom_parser.Next();

      if (geom.has_value()) {
        if (std::holds_alternative<g::Point2D>(geom.value())) {
          auto point = std::get<g::Point2D>(geom.value());
          std::cout << "rendering " << point.ToWkt() << std::endl;

          auto geom_vertices = ParseShape(point);
          std::vector<float> vertices;
          vertices.resize(geom_vertices.size());
          std::transform(geom_vertices.begin(), geom_vertices.end(), vertices.begin(),
                         [&](float v) { return normalize_to_viewport(v, -10.0f, 10.0f); });

          glBindBuffer(GL_ARRAY_BUFFER, VBO);
          glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

          glDrawArrays(GL_POINTS, 0, 1);

        } else if (std::holds_alternative<g::LineSegment2D>(geom.value())) {
          auto seg = std::get<g::LineSegment2D>(geom.value());
          std::cout << "rendering " << seg.ToWkt() << std::endl;

          auto geom_vertices = ParseShape(seg);
          std::vector<float> vertices;
          vertices.resize(geom_vertices.size());
          std::transform(geom_vertices.begin(), geom_vertices.end(), vertices.begin(),
                         [&](float v) { return normalize_to_viewport(v, -10.0f, 10.0f); });

          glBindBuffer(GL_ARRAY_BUFFER, VBO);
          glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

          glDrawArrays(GL_LINES, 0, 2);

        } else {
          std::cerr << "unsupported geom in rendering" << std::endl;
        }
      }
    }

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
