#include "triangle2d.hpp"

#include "line2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Triangle2D Triangle2D::Make(Point2D const& p0, Point2D const& p1, Point2D const& p2, int decimal_precision) {
  auto unique_points = Point2D::remove_duplicates({p0, p1, p2}, decimal_precision);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format("points {}, {}, {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision),
                                         p2.ToWkt(decimal_precision), decimal_precision));
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

// double LineSegment2D::Length() const { return (P1 - P0).Length(); }

bool Triangle2D::AlmostEquals(Triangle2D const& other, int decimal_precision) const {
  return P0.AlmostEquals(other.P0, decimal_precision) && P1.AlmostEquals(other.P1, decimal_precision) &&
         P2.AlmostEquals(other.P2, decimal_precision);
}

// Line2D LineSegment2D::ToLine(int decimal_precision) const { return Line2D::Make(P0, P1, decimal_precision); }

// double LineSegment2D::Location(Point2D const& point, int decimal_precision) const {
//   if (!ToLine().Contains(point, decimal_precision)) {
//     return std::numeric_limits<double>::infinity();
//   }
//   return sign((point - P0).Dot(P1 - P0), decimal_precision) * (point - P0).Length() / Length();
// }

// Point2D LineSegment2D::Interpolate(double pct) const {
//   // the point is behind the polyline
//   if (round_to(pct, DP_NINE) < 0.0) {
//     return P0;
//   }

//   // the point is beyond the polyline
//   if (round_to(pct, DP_NINE) > 1.0) {
//     return P1;
//   }

//   return P0 + pct * (P1 - P0);
// }

// double LineSegment2D::DistanceTo(Point2D const& point, int decimal_precision) const {
//   auto line_eqv = ToLine(decimal_precision);
//   auto proj = line_eqv.ProjectOnto(point, decimal_precision);
//   double loc = Location(proj, decimal_precision);
//   if (round_to(loc, decimal_precision) < 0) {
//     return P0.DistanceTo(point, decimal_precision);

//   } else if (round_to(loc, decimal_precision) > 1) {
//     return P1.DistanceTo(point, decimal_precision);
//   }

//   return line_eqv.DistanceTo(point, decimal_precision);
// }

// #pragma endregion

// #pragma region Operator Overloading

// bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs) { return lhs.AlmostEquals(rhs); }

// #pragma endregion

// #pragma region Geometrical Operations

// bool LineSegment2D::Contains(Point2D const& point, int decimal_precision) const {
//   double t = Location(point, decimal_precision);
//   return round_to(t, decimal_precision) >= 0 && round_to(t - 1, decimal_precision) <= 0;
// }

// bool LineSegment2D::Intersects(Line2D const& line, int decimal_precision) const {
//   return Intersection(line, decimal_precision).has_value();
// }

// bool LineSegment2D::Intersects(Ray2D const& ray, int decimal_precision) const {
//   return Intersection(ray, decimal_precision).has_value();
// }

// bool LineSegment2D::Intersects(LineSegment2D const& other, int decimal_precision) const {
//   return Intersection(other, decimal_precision).has_value();
// }

// LineSegment2D::ReturnSet LineSegment2D::Intersection(Line2D const& line, int decimal_precision) const {
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

// LineSegment2D::ReturnSet LineSegment2D::Intersection(Ray2D const& ray, int decimal_precision) const {
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

// LineSegment2D::ReturnSet LineSegment2D::Intersection(LineSegment2D const& other, int decimal_precision) const {
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

// #pragma region Formatting

std::string Triangle2D::ToWkt(int decimal_precision) const {
  return std::format("TRIANGLE ({} {}, {} {})", round_to(P0.x(), decimal_precision),
                     round_to(P0.y(), decimal_precision), round_to(P1.x(), decimal_precision),
                     round_to(P1.y(), decimal_precision));
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

    return Make(pt_vec[0], pt_vec[1], pt_vec[2], decimal_precision);

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Triangle2D::ToFile(std::string const& path, int decimal_precision) const {
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
