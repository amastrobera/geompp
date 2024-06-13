#include "polyline2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
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

namespace geompp {

#pragma region Constructors

Polyline2D::Polyline2D(std::vector<Point2D>&& points) : KNOTS{std::move(points)} {}

Polyline2D Polyline2D::Make(std::vector<Point2D> const& points, int decimal_precision) {
  auto unique_points =
      Point2D::remove_collinear(Point2D::remove_duplicates(points, decimal_precision), decimal_precision);

  if (unique_points.size() < 2) {
    throw std::runtime_error("cannot built polyline with less than 2 unique non-collinear consecutive points");
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

double Polyline2D::Location(Point2D const& point, int decimal_precision) const {
  auto segs = ToSegments();

  // check if the point is in the middle of the polyline
  double tot_len = 0;
  for (int i = 0; i < segs.size(); ++i) {
    if (segs[i].Contains(point, decimal_precision)) {
      tot_len += segs[i].Location(point, decimal_precision);
      return tot_len;
    }
    tot_len += segs[i].Length();
  }
  // at this point tot_len == Lenght(), no need to call that loop again

  // check if the point is behind the polyline (on the first "line")
  if (round_to((segs[0].Last() - segs[0].First()).Perp().Dot(point - segs[0].First()), decimal_precision) ==
      0) {  // collinearity check
    return sign((point - segs[0].First()).Dot(segs[0].Last() - segs[0].First()), decimal_precision) *
           segs[0].First().DistanceTo(point) / tot_len;
  }

  // check if the point is is beyond the polyline (on the last "line")
  int n = segs.size();
  if (round_to((segs[n - 1].Last() - segs[n - 1].First()).Perp().Dot(point - segs[n - 1].First()),
               decimal_precision) == 0) {  // collinearity check
    return (tot_len + segs[n - 1].Last().DistanceTo(point)) / tot_len;
  }

  // the point is not located along the polyline or its first/last "line"
  return std::numeric_limits<double>::infinity();
}

Point2D Polyline2D::Interpolate(double pct) const {
  // the point is behind the polyline
  if (round_to(pct, DP_NINE) < 0.0) {
    return KNOTS[0];
  }

  // the point is beyond the polyline
  if (round_to(pct, DP_NINE) > 1.0) {
    return KNOTS[KNOTS.size() - 1];
  }

  // pct is within [0, 1]
  double len_to_i = 0;
  double len_i = 0;
  for (int i = 0; i < KNOTS.size() - 1; ++i) {
    len_i = KNOTS[i].DistanceTo(KNOTS[i + 1], DP_NINE);

    if (round_to(pct - (len_to_i + len_i), DP_NINE) <= 0) {
      double pct_i = pct - len_to_i;
      return KNOTS[i] + pct_i * (KNOTS[i + 1] - KNOTS[i]);
    }

    len_to_i += len_i;
  }

  return KNOTS[KNOTS.size() - 1];
}

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

bool Polyline2D::Intersects(Line2D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}

bool Polyline2D::Intersects(Ray2D const& ray, int decimal_precision) const {
  return Intersection(ray, decimal_precision).has_value();
}

bool Polyline2D::Intersects(Polyline2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

bool Polyline2D::Intersects(LineSegment2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

Polyline2D::ReturnSet Polyline2D::Intersection(Line2D const& line, int decimal_precision) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = line.Intersection(seg, decimal_precision);

    if (inter.has_value() && std::holds_alternative<Point2D>(*inter)) {
      intersections.push_back(std::get<Point2D>(*inter));
    }
  }

  if (intersections.size() == 0) {
    return std::nullopt;
  }

  if (intersections.size() == 1) {
    return intersections[0];
  }

  return intersections;
}

Polyline2D::ReturnSet Polyline2D::Intersection(Ray2D const& ray, int decimal_precision) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = ray.Intersection(seg, decimal_precision);

    if (inter.has_value() && std::holds_alternative<Point2D>(*inter)) {
      intersections.push_back(std::get<Point2D>(*inter));
    }
  }

  if (intersections.size() == 0) {
    return std::nullopt;
  }

  if (intersections.size() == 1) {
    return intersections[0];
  }

  return intersections;
}

Polyline2D::ReturnSet Polyline2D::Intersection(LineSegment2D const& segment, int decimal_precision) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = segment.Intersection(seg, decimal_precision);

    if (inter.has_value() && std::holds_alternative<Point2D>(*inter)) {
      intersections.push_back(std::get<Point2D>(*inter));
    }
  }

  if (intersections.size() == 0) {
    return std::nullopt;
  }

  if (intersections.size() == 1) {
    return intersections[0];
  }

  return intersections;
}

Polyline2D::ReturnSet Polyline2D::Intersection(Polyline2D const& other, int decimal_precision) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    for (auto const& other_seg : other.ToSegments()) {
      auto inter = seg.Intersection(other_seg, decimal_precision);

      if (inter.has_value() && std::holds_alternative<Point2D>(*inter)) {
        intersections.push_back(std::get<Point2D>(*inter));
      }
    }
  }

  if (intersections.size() == 0) {
    return std::nullopt;
  }

  if (intersections.size() == 1) {
    return intersections[0];
  }

  return intersections;
}

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
