#include "point2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {

Point2D::Point2D(double x, double y) : X(x), Y(y) {}

Point2D::Point2D(Point2D const& p) : X(p.X), Y(p.Y) {}

bool Point2D::AlmostEquals(Point2D const& other, int decimal_precision) const {
  return round_to(X - other.X, decimal_precision) == 0.0 && round_to(Y - other.Y, decimal_precision) == 0.0;
}

Vector2D Point2D::ToVector() { return {X, Y}; }

double Point2D::DistanceTo(Point2D const& other, int decimal_precision) const {
  return round_to((other - *this).Length(), decimal_precision);
}

Point2D& Point2D::operator=(Point2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

#pragma region Collection Operations

std::vector<Point2D> Point2D::remove_duplicates(std::vector<Point2D> const& points, int decimal_precision) {
  if (points.size() == 0) {
    return points;
  }

  std::unordered_set<int> duplicates;
  for (int i = 0; i < points.size() - 1; ++i) {
    if (duplicates.count(i)) {
      continue;
    }
    for (int j = i + 1; j < points.size(); ++j) {
      if (!points[i].AlmostEquals(points[j], decimal_precision)) {
        break;
      }
      duplicates.insert(j);
    }
  }

  std::vector<Point2D> unique_points;
  for (int i = 0; i < points.size(); ++i) {
    if (duplicates.count(i) == 0) {
      unique_points.push_back(points[i]);
    }
  }

  return unique_points;
}

std::vector<Point2D> Point2D::remove_collinear(std::vector<Point2D> const& points, int decimal_precision) {
  if (points.size() < 3) {
    return points;
  }

  std::unordered_set<int> duplicates;
  int i1 = 0;
  int i2 = i1 + 1;
  int i3 = i1 + 2;
  int max_iter = points.size();
  while (i1 < points.size() - 2 && i2 < points.size() - 1 && i3 < points.size() && max_iter > 0) {
    if (duplicates.count(i1)) {
      ++i1;
      ++i2;
      ++i3;
      continue;
    }

    auto u = (points[i2] - points[i1]);
    auto v = (points[i3] - points[i1]);

    if (round_to(u.Perp().Dot(v), decimal_precision) == 0) {  // test of collinearity
      if (round_to(u.Dot(v), decimal_precision) >=
          0) {  // same direction, pick the farthest point in the U-vector's direction
        if (round_to(points[i1].DistanceTo(points[i3]) - points[i1].DistanceTo(points[i2]), decimal_precision) >= 0) {
          duplicates.insert(i2);
          ++i2;
          ++i3;

        } else {
          duplicates.insert(i3);
          ++i3;
        }

      } else {  // not in the same direction, remove the point opposite to U-vector
        duplicates.insert(i3);
        ++i3;
      }

    } else {
      ++i1;  // increment loop
      i2 = i1 + 1;
      i3 = i1 + 2;
    }

    --max_iter;
  }

  std::vector<Point2D> unique_points;
  for (int i = 0; i < points.size(); ++i) {
    if (duplicates.count(i) == 0) {
      unique_points.push_back(points[i]);
    }
  }

  return unique_points;
}

Point2D Point2D::average(std::vector<Point2D> const& points) {
  int n = points.size();
  if (n == 0) {
    throw std::runtime_error("average of zero points");
  }
  double x = 0;
  double y = 0;
  for (auto const& p : points) {
    x += p.x();
    y += p.y();
  }

  return {x / n, y / n};
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Point2D const& lhs, Point2D const& rhs) { return lhs.AlmostEquals(rhs); }

Point2D operator+(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator-(Point2D const& lhs, Point2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }
Point2D operator-(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }

Point2D operator*(Point2D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a}; }
Point2D operator*(double a, Point2D const& rhs) { return rhs * a; }

#pragma endregion

#pragma region Formatting

std::string Point2D::ToWkt(int decimal_precision) const {
  return std::format("POINT ({} {})", round_to(X, decimal_precision), round_to(Y, decimal_precision));
}

Point2D Point2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_nums;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POINT") {
      throw std::runtime_error("geometry name");
    }

    end_nums = wkt.substr(end_gtype + 1).find(')');
    if (end_nums == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums = wkt.substr(end_gtype + 1, end_nums);

    auto nums = geompp::tokenize_to_doubles(s_nums);
    if (nums.size() != 2) {
      throw std::runtime_error("numbers");
    }

    return {nums[0], nums[1]};

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Point2D::ToFile(std::string const& path, int decimal_precision) const {
  try {
    std::string content = ToWkt(decimal_precision);

    // Open the file in write mode (truncates existing content)
    std::ofstream outfile(path);

    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open file");
    }

    // Write the text to the file
    outfile << content;

    outfile.close();

  } catch (...) {
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger lib
  }
}

Point2D Point2D::FromFile(std::string const& path) {
  try {
    std::string content;

    // Open the file in read mode
    std::ifstream in_file(path);

    if (!in_file.is_open()) {
      throw std::runtime_error("could not open file");
    }

    // Get the file size (optional, for efficiency)
    in_file.seekg(0, std::ios::end);
    std::streamsize fileSize = in_file.tellg();
    in_file.seekg(0, std::ios::beg);  // Reset the file pointer

    // Resize the string to the file size (optional, for efficiency)
    content.resize(static_cast<size_t>(fileSize));

    // Read the entire file into the string
    in_file.read(&content[0], fileSize);

    return FromWkt(content);

  } catch (...) {
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
