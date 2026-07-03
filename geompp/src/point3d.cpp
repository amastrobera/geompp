#include "point3d.hpp"

#include "calc_utils3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <unordered_set>

namespace geompp {

bool Point3D::AlmostEquals(Point3D const& other, double epsilon) const {
  return compare(X, other.X, epsilon) == 0 && compare(Y, other.Y, epsilon) == 0 && compare(Z, other.Z, epsilon) == 0;
}

Vector3D Point3D::ToVector() const { return {X, Y, Z}; }

double Point3D::DistanceTo(Point3D const& other) const { return round((other - *this).Length()); }

Point3D& Point3D::operator=(Point3D const& other) {
  if (this != &other) {
    X = other.X;
    Y = other.Y;
    Z = other.Z;
  }
  return *this;
}

#pragma region Collection Operations

bool are_collinear(Point3D const& p1, Point3D const& p2, Point3D const& p3) {
  return compare((p2 - p1).Cross(p3 - p1).Length(), 0) == 0;
}

std::vector<Point3D> remove_consecutive_duplicates(std::vector<Point3D> const& points) {
  if (points.size() < 2) {
    return points;
  }

  std::vector<Point3D> unique_points{points.front()};
  for (int i = 1; i < points.size(); ++i) {
    if (!unique_points.back().AlmostEquals(points[i])) {
      unique_points.push_back(points[i]);
    }
  }

  return unique_points;
}

std::vector<Point3D> remove_duplicates(std::vector<Point3D> const& points) {
  if (points.size() == 0) {
    return points;
  }

  std::unordered_set<int> duplicates;
  for (int i = 0; i < points.size() - 1; ++i) {
    if (duplicates.count(i)) {
      continue;
    }
    for (int j = i + 1; j < points.size(); ++j) {
      if (!points[i].AlmostEquals(points[j])) {
        break;
      }
      duplicates.insert(j);
    }
  }

  std::vector<Point3D> unique_points;
  for (int i = 0; i < points.size(); ++i) {
    if (duplicates.count(i) == 0) {
      unique_points.push_back(points[i]);
    }
  }

  return unique_points;
}

std::vector<Point3D> remove_collinear(std::vector<Point3D> const& points) {
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

    if (are_collinear(points[i1], points[i2], points[i3])) {  // test of collinearity
      auto u = (points[i2] - points[i1]);
      auto v = (points[i3] - points[i1]);
      if (compare(u.Dot(v), 0) >= 0) {  // same direction, pick the farthest point in the U-vector's direction
        if (compare(points[i1].DistanceTo(points[i3]), points[i1].DistanceTo(points[i2])) >= 0) {
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

  std::vector<Point3D> unique_points;
  for (int i = 0; i < points.size(); ++i) {
    if (duplicates.count(i) == 0) {
      unique_points.push_back(points[i]);
    }
  }

  return unique_points;
}

Point3D linear_combination(std::vector<Point3D> const& points, std::vector<double> const& weights) {
  int n = points.size();
  if (n == 0) {
    throw std::runtime_error("average of zero points");
  }
  if (weights.size() != n) {
    throw std::runtime_error("weights and points' vectors have different sizes");
  }
  double x = 0;
  double y = 0;
  double z = 0;
  for (int i = 0; i < n; ++i) {
    x += points[i].x() * weights[i];
    y += points[i].y() * weights[i];
    z += points[i].z() * weights[i];
  }

  return {x, y, z};
}

Point3D average(std::vector<Point3D> const& points) {
  int n = points.size();
  if (n == 0) {
    throw std::runtime_error("average of zero points");
  }

  std::vector<double> weights(points.size());
  for (int i = 0; i < n; ++i) {
    weights[i] = 1.0 / n;
  }

  return linear_combination(points, weights);
}

std::vector<Point3D> convex_hull(std::vector<Point3D> const& points, std::optional<Vector3D> normal) {
  if (points.size() <= 3) {
    return points;
  }

  // dispatch: if a normal is provided, assume coplanar; otherwise use PCA to approximate the plane
  auto cv_indices = normal.has_value() ? detail::convex_hull_indices(points, normal.value()) : detail::convex_hull_indices(points);

  std::vector<Point3D> cv;
  cv.reserve(cv_indices.size());
  for (std::size_t i : cv_indices) {
    cv.emplace_back(points[i]);
  }

  return cv;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Point3D const& lhs, Point3D const& rhs) { return lhs.AlmostEquals(rhs); }

Point3D operator+(Point3D const& lhs, Vector3D const& rhs) {
  return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}
Point3D& operator+=(Point3D& lhs, Vector3D const& rhs) {
  lhs = lhs + rhs;
  return lhs;
}

Vector3D operator-(Point3D const& lhs, Point3D const& rhs) {
  return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}
Point3D operator-(Point3D const& lhs, Vector3D const& rhs) {
  return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

Point3D operator*(Point3D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a, lhs.z() * a}; }
Point3D operator*(double a, Point3D const& rhs) { return rhs * a; }

Point3D operator/(Point3D const& lhs, double a) {
  if (compare(a, 0) == 0) {
    throw std::runtime_error("division by zero");
  }
  return {lhs.x() / a, lhs.y() / a, lhs.z() / a};
}

std::ostream& operator<<(std::ostream& os, Point3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Formatting

std::string Point3D::ToWkt() const { return std::format("POINT ({} {} {})", round(X), round(Y), round(Z)); }

Point3D Point3D::FromWkt(std::string const& wkt) {
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
    if (nums.size() != 3) {
      throw std::runtime_error("numbers");
    }

    return {nums[0], nums[1], nums[2]};

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Point3D::ToFile(std::string const& path) const {
  try {
    std::string content = ToWkt();

    // Open the file in write mode (truncates existing content)
    std::ofstream outfile(path);

    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open file");
    }

    // Write the text to the file
    outfile << content;

    outfile.close();

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }
}

Point3D Point3D::FromFile(std::string const& path) {
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
    content.resize(static_cast<std::size_t>(fileSize));

    // Read the entire file into the string
    in_file.read(&content[0], fileSize);

    return FromWkt(content);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
