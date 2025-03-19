#include "triangle2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <limits>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace geompp {

namespace {

static bool within_axis_boundary(double s, double t) {
  return (round(s) >= 0.0 && round(s - 1.0) <= 0.0) &&
         (round(t) >= 0.0 && round(t - 1.0) <= 0.0 && round(s + t - 1.0) <= 0.0);  // including borders
}

}  // namespace

#pragma region Constructors

Triangle2D Triangle2D::Make(Point2D const& p0, Point2D const& p1, Point2D const& p2) {
  auto unique_points = remove_duplicates({p0, p1, p2});

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format("points {}, {}, {} are too close with {} decimals precision",
                                         DECIMAL_PRECISION, p0.ToWkt(), p1.ToWkt(), p2.ToWkt()));
  }
  return {p0, p1, p2};
}

Triangle2D::Triangle2D(Point2D const& p0, Point2D const& p1, Point2D const& p2) : P0(p0), P1(p1), P2(p2) {}

Triangle2D& Triangle2D::operator=(Triangle2D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

bool Triangle2D::AlmostEquals(Triangle2D const& other) const {
  return P0.AlmostEquals(other.P0) && P1.AlmostEquals(other.P1) && P2.AlmostEquals(other.P2);
}

Point2D Triangle2D::Centroid() const { return average({P0, P1, P2}); }

Polygon2D Triangle2D::ToPolygon() const { return Polygon2D::Make({P0, P1, P2}); }

double Triangle2D::SignedArea() const { return ((P1 - P0).Cross(P2 - P0)) / 2.0; }

double Triangle2D::Area() const { return std::abs(SignedArea()); }

double Triangle2D::Perimeter() const { return (P1 - P0).Length() + (P2 - P1).Length() + (P0 - P2).Length(); }

double Triangle2D::DistanceTo(Point2D const& point) const {
  if (Contains(point)) {
    return 0;
  }
  return std::min(
      std::min(LineSegment2D::Make(P0, P1).DistanceTo(point), LineSegment2D::Make(P1, P2).DistanceTo(point)),
      LineSegment2D::Make(P2, P0).DistanceTo(point));
}

std::tuple<Vector2D, Vector2D> Triangle2D::ToAxis() const { return {P1 - P0, P2 - P0}; }

std::optional<Point2D> Triangle2D::Interpolate(double s, double t) const {
  if (!within_axis_boundary(s, t)) {
    std::cerr << "(s, t) = (" << s << ", " << t << ") are not within boundaries [0, 1]"
              << std::endl;  // TODO: log warning
    return std::nullopt;
  }

  return linear_combination({P0, P1, P2}, {1 - s - t, s, t});
}

// #pragma endregion

#pragma region Operator Overloading

bool operator==(Triangle2D const& lhs, Triangle2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Triangle2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

// #pragma region Geometrical Operations

std::tuple<double, double> Triangle2D::Location(Point2D const& point) const {
  auto u = (P1 - P0);
  auto v = (P2 - P0);
  auto w = (point - P0);

  auto up = u.Perp();
  auto vp = v.Perp();

  double s = w.Dot(vp) / u.Dot(vp);
  double t = w.Dot(up) / v.Dot(up);

  return {s, t};
}

bool Triangle2D::Contains(Point2D const& point) const {
  auto loc = Location(point);

  return within_axis_boundary(std::get<0>(loc), std::get<1>(loc));
}

bool Triangle2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

// bool LineSegment2D::Intersects(Ray2D const& ray) const {
//   return Intersection(ray).has_value();
// }

// bool LineSegment2D::Intersects(LineSegment2D const& other) const {
//   return Intersection(other).has_value();
// }

Triangle2D::ReturnSet Triangle2D::Intersection(Line2D const& line) const {
  auto intersections =
      std::vector<LineSegment2D>{LineSegment2D::Make(P0, P1), LineSegment2D::Make(P1, P2),
                                 LineSegment2D::Make(P2, P0)} |
      std::views::transform([&](LineSegment2D const& seg) { return seg.Intersection(line); }) |
      std::views::filter([](LineSegment2D::ReturnSet const& res) {
        return res.has_value() && std::holds_alternative<Point2D>(*res);
      }) |
      std::views::transform([](LineSegment2D::ReturnSet const& res) { return std::get<Point2D>(*res); });

  std::vector<Point2D> intersection_points(intersections.begin(), intersections.end());

  if (intersection_points.empty()) {
    return std::nullopt;
  }

  intersection_points = remove_duplicates(intersection_points);

  if (intersection_points.size() == 1) {
    return intersection_points[0];
  }

  // TODO: this makes a stupid error "unknown file: error: SEH exception with code 0xc00000fd thrown in the test body."
  // std::ranges::sort(intersection_points, [&](Point2D const& a, Point2D const& b) {
  //  return round(line.Location(a) - line.Location(b),
  //                  decimal_precision) < 0.0;
  //});

  return LineSegment2D::Make(intersection_points[0], intersection_points[1]);
}

// LineSegment2D::ReturnSet LineSegment2D::Intersection(Ray2D const& ray) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = ray.Direction();
//   auto vp = v.Perp();
//   auto w = (P0 - ray.Origin());

//   // testing on this ray
//   if (round(u * vp) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round(v * up) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = ray.Origin() + s * v;
//   if (!ray.IsAhead(inter_s)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// LineSegment2D::ReturnSet LineSegment2D::Intersection(LineSegment2D const& other) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = (other.P1 - other.P0);
//   auto vp = v.Perp();
//   auto w = (P0 - other.P0);

//   // testing on this ray
//   if (round(u * vp) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round(v * up) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = other.P0 + s * v;
//   if (!other.Contains(inter_s)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// #pragma endregion

// #pragma region Formatting

std::string Triangle2D::ToWkt() const {
  // clang-format off
  return std::format("TRIANGLE ({} {}, {} {}, {} {})", 
                     round(P0.x()), round(P0.y()), 
                     round(P1.x()), round(P1.y()),
                     round(P2.x()), round(P2.y())
                     );
  // clang-format on
}

Triangle2D Triangle2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pi, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "TRIANGLE") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).find(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string mid_part = wkt.substr(end_gtype + 1, wkt.size() - (end_gtype + 1 + 1));

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

    if (pt_vec.size() != 3) {
      throw std::runtime_error("initialized with n != 3 points");
    }

    return Make(pt_vec[0], pt_vec[1], pt_vec[2]);

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Triangle2D::ToFile(std::string const& path) const {
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
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger lib
  }
}

Triangle2D Triangle2D::FromFile(std::string const& path) {
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

// #pragma endregion

}  // namespace geompp
