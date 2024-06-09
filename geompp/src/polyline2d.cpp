#include "polyline2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <limits>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>

namespace geompp {

#pragma region Constructors

Polyline2D::Polyline2D(std::vector<Point2D>&& points) : KNOTS{std::move(points)} {}

Polyline2D Polyline2D::Make(std::vector<Point2D> const& points, int decimal_precision) {
  if (points.size() < 2) {
    throw std::runtime_error("cannot built polyline with less than 2 points");
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

  if (duplicates.size() == points.size() - 1) {
    throw std::runtime_error("cannot built polyline with less than 2 unique consecutive points");
  }

  std::vector<Point2D> unique_points;
  for (int i = 0; i < points.size(); ++i) {
    if (duplicates.count(i) == 0) {
      unique_points.push_back(points[i]);
    }
  }

  return Polyline2D(std::move(unique_points));
}

Polyline2D& Polyline2D::operator=(Polyline2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

std::vector<LineSegment2D> Polyline2D::ToSegments() const {
  std::vector<LineSegment2D> segs;

  for (int i = 0; i < KNOTS.size() - 1; ++i) {
    segs.push_back(LineSegment2D::Make(KNOTS[i], KNOTS[i + 1]));
  }

  return segs;
}

double Polyline2D::Length() const {
  auto iterable_range = ToSegments() | std::ranges::views::transform([](LineSegment2D const& s) { return s.Length(); });
  return std::accumulate(iterable_range.begin(), iterable_range.end(), 0);
}

bool Polyline2D::AlmostEquals(Polyline2D const& other, int decimal_precision) const {
  if (KNOTS.size() != other.KNOTS.size()) {
    return false;
  }
  for (int i = 0; i < KNOTS.size(); ++i) {
    if (!KNOTS[i].AlmostEquals(other.KNOTS[i], decimal_precision)) {
      return false;
    }
  }
  return true;
}

// double Polyline2D::Location(Point2D const& point, int decimal_precision) const {
//   if (!ToLine().Contains(point, decimal_precision)) {
//     return std::numeric_limits<double>::infinity();
//   }
//   return sign((point - P0).Dot(P1 - P0), decimal_precision) * (point - P0).Length() / Length();
// }

// Point2D Polyline2D::Interpolate(double pct) const { return P0 + pct * (P1 - P0); }

double Polyline2D::DistanceTo(Point2D const& point, int decimal_precision) const {
  std::vector<double> distances;
  for (auto const& s : ToSegments()) {
    distances.push_back(s.DistanceTo(point, decimal_precision));
  }
  return *std::min_element(distances.begin(), distances.end());
  // std::vector<double> iterable_range =
  //     ToSegments() | std::ranges::views::transform([&point, decimal_precision](LineSegment2D const& s) {
  //       return s.DistanceTo(point, decimal_precision);
  //     });
  // return std::min_element(iterable_range.begin(), iterable_range.end());
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Polyline2D const& lhs, Polyline2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Geometrical Operations

bool Polyline2D::Contains(Point2D const& point, int decimal_precision) const {
  for (auto const& s : ToSegments()) {
    if (s.Contains(point, decimal_precision)) {
      return true;
    }
  }
  return false;
  // return std::ranges::any_of(ToSegments() |
  //                            std::ranges::views::transform([&point, decimal_precision](LineSegment2D const& s) {
  //                              return s.Contains(point, decimal_precision);
  //                            }));
}

// bool Polyline2D::Intersects(Line2D const& line, int decimal_precision) const {
//   return Intersection(line, decimal_precision).has_value();
// }

// bool Polyline2D::Intersects(Ray2D const& ray, int decimal_precision) const {
//   return Intersection(ray, decimal_precision).has_value();
// }

// bool Polyline2D::Intersects(LineSegment2D const& other, int decimal_precision) const {
//   return Intersection(other, decimal_precision).has_value();
// }

// Polyline2D::ReturnSet Polyline2D::Intersection(Line2D const& line, int decimal_precision) const {
//   auto u = P1 - P0;
//   auto v = line.Direction();
//   auto vp = v.Perp();
//   auto w = (P0 - line.First());

//   if (round_to(u * vp, decimal_precision) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);

//   // verify that the intersection is ahead of the ray
//   auto inter_p = P0 + t * u;
//   if (!Contains(inter_p, decimal_precision)) {
//     return std::nullopt;
//   }

//   return inter_p;
// }

// Polyline2D::ReturnSet Polyline2D::Intersection(Ray2D const& ray, int decimal_precision) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = ray.Direction();
//   auto vp = v.Perp();
//   auto w = (P0 - ray.Origin());

//   // testing on this ray
//   if (round_to(u * vp, decimal_precision) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t, decimal_precision)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round_to(v * up, decimal_precision) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = ray.Origin() + s * v;
//   if (!ray.IsAhead(inter_s, decimal_precision)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// Polyline2D::ReturnSet Polyline2D::Intersection(LineSegment2D const& other, int decimal_precision) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = (other.P1 - other.P0);
//   auto vp = v.Perp();
//   auto w = (P0 - other.P0);

//   // testing on this ray
//   if (round_to(u * vp, decimal_precision) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t, decimal_precision)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round_to(v * up, decimal_precision) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = other.P0 + s * v;
//   if (!other.Contains(inter_s, decimal_precision)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// #pragma endregion

#pragma region Formatting

std::string Polyline2D::ToWkt(int decimal_precision) const {
  std::ostringstream buf;
  buf << "LINESTRING ";

  if (KNOTS.size() > 0) {
    buf << "(";
    for (int i = 0; i < KNOTS.size(); ++i) {
      buf << std::format("{} {}", round_to(KNOTS[i].x(), decimal_precision), round_to(KNOTS[i].y(), decimal_precision));

      if (i < KNOTS.size() - 1) {
        buf << ", ";
      }
    }
    buf << ")";

  } else {
    buf << "EMPTY";
  }

  return buf.str();
}

Polyline2D Polyline2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pi, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINESTRING") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).find(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string mid_part = wkt.substr(end_gtype + 1, wkt.size() - (end_gtype + 1 + 1));

    std::cout << "mid_part=" << mid_part << std::endl;

    std::vector<Point2D> pt_vec;
    int decimal_precision = 0;
    int num_dec = 0;
    std::string pt_trimmed;
    for (std::string const& p_str : geompp::tokenize_string(mid_part, ',')) {
      pt_trimmed = geompp::trim(p_str);

      auto nums = geompp::tokenize_to_doubles(pt_trimmed, ' ');
      if (nums.size() != 2) {
        throw std::runtime_error("numbers");
      }

      num_dec = count_decimal_places(nums[0]);
      if (num_dec > decimal_precision) {
        decimal_precision = num_dec;
      }
      num_dec = count_decimal_places(nums[1]);
      if (num_dec > decimal_precision) {
        decimal_precision = num_dec;
      }

      pt_vec.push_back({nums[0], nums[1]});
    }

    return Make(pt_vec, decimal_precision);

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Polyline2D::ToFile(std::string const& path, int decimal_precision) const {
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

Polyline2D Polyline2D::FromFile(std::string const& path) {
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
