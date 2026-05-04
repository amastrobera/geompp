#include "polyline3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace geompp {

#pragma region Constructors

Polyline3D::Polyline3D(std::vector<Point3D>&& points, double length) : KNOTS{std::move(points)}, LENGTH(length) {}

Polyline3D Polyline3D::Make(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);

  if (unique_points.size() < 2) {
    throw std::runtime_error("cannot built polyline with less than 2 unique non-collinear consecutive points");
  }

  double length = 0;
  for (int i = 0; i < unique_points.size() - 1; ++i) {
    length += LineSegment3D::Make(unique_points[i], unique_points[1 + i]).Length();
  }

  return Polyline3D(std::move(unique_points), length);
}

Polyline3D& Polyline3D::operator=(Polyline3D const& other) {
  if (this != &other) {
    KNOTS = other.KNOTS;
    LENGTH = other.LENGTH;
  }
  return *this;
}

SegmentRange3D Polyline3D::ToSegments() const { return SegmentRange3D(KNOTS); }

bool Polyline3D::AlmostEquals(Polyline3D const& other, double epsilon) const {
  if (compare(LENGTH, other.LENGTH, epsilon) != 0) {
    return false;
  }

  if (KNOTS.size() != other.KNOTS.size()) {
    return false;
  }
  for (int i = 0; i < KNOTS.size(); ++i) {
    if (!KNOTS[i].AlmostEquals(other.KNOTS[i], epsilon)) {
      return false;
    }
  }
  return true;
}

#pragma endregion

#pragma region line operations

Point3D Polyline3D::ProjectOnto(Point3D const& point) const {
  double min_dist = std::numeric_limits<double>::max();
  Point3D result = KNOTS[0];

  for (auto const& seg : ToSegments()) {
    Point3D proj = seg.ProjectOnto(point);
    double dist = (point - proj).Length();
    if (compare(dist, min_dist) < 0) {
      min_dist = dist;
      result = proj;
    }
  }

  return result;
}

double Polyline3D::Location(Point3D const& point) const {
  if (!Contains(point)) {
    return std::numeric_limits<double>::infinity();
  }

  auto segs = ToSegments();

  double tot_len = 0;
  for (int i = 0; i < segs.size(); ++i) {
    auto seg = segs[i];
    if (seg.Contains(point)) {
      return (tot_len + seg.Location(point) * seg.Length()) / LENGTH;
    }
    tot_len += seg.Length();
  }
  return tot_len / LENGTH;
}

Point3D Polyline3D::Interpolate(double pct) const {
  if (compare(pct, 0) < 0 || compare(pct, 1.0) > 0) {
    throw std::invalid_argument("pct must be in [0, 1]");
  }

  double target = pct * LENGTH;
  double len_to_i = 0;
  double len_i = 0;
  for (int i = 0; i < KNOTS.size() - 1; ++i) {
    len_i = KNOTS[i].DistanceTo(KNOTS[i + 1]);

    if (compare(target, len_to_i + len_i) <= 0) {
      double pct_i = (target - len_to_i) / len_i;
      return KNOTS[i] + pct_i * (KNOTS[i + 1] - KNOTS[i]);
    }

    len_to_i += len_i;
  }

  return KNOTS[KNOTS.size() - 1];
}

double Polyline3D::DistanceTo(Point3D const& point) const {
  double min_dist = std::numeric_limits<double>::max();
  for (auto const& seg : ToSegments()) {
    double d = seg.DistanceTo(point);
    if (d < min_dist) {
      min_dist = d;
    }
  }
  return min_dist;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Polyline3D const& lhs, Polyline3D const& rhs) { return lhs.AlmostEquals(rhs); }

Point3D const& Polyline3D::operator[](size_t i) const {
  if (i >= Size()) {
    throw std::out_of_range("Index out of range");
  }
  return KNOTS[i];
}

std::ostream& operator<<(std::ostream& os, Polyline3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Polyline3D::Contains(Point3D const& point) const {
  for (auto const& seg : ToSegments()) {
    if (seg.Contains(point)) { return true; }
  }
  return false;
}

bool Polyline3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Polyline3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Polyline3D::Intersects(Polyline3D const& other) const { return Intersection(other).has_value(); }

bool Polyline3D::Intersects(LineSegment3D const& other) const { return Intersection(other).has_value(); }

Polyline3D::ReturnSet Polyline3D::Intersection(Line3D const& line) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = line.Intersection(seg);

    if (inter.has_value() && std::holds_alternative<Point3D>(*inter)) {
      intersections.push_back(std::get<Point3D>(*inter));
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

Polyline3D::ReturnSet Polyline3D::Intersection(Ray3D const& ray) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = ray.Intersection(seg);

    if (inter.has_value() && std::holds_alternative<Point3D>(*inter)) {
      intersections.push_back(std::get<Point3D>(*inter));
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

Polyline3D::ReturnSet Polyline3D::Intersection(LineSegment3D const& segment) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    auto inter = segment.Intersection(seg);

    if (inter.has_value() && std::holds_alternative<Point3D>(*inter)) {
      intersections.push_back(std::get<Point3D>(*inter));
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

Polyline3D::ReturnSet Polyline3D::Intersection(Polyline3D const& other) const {
  MultiPoint intersections;

  for (auto const& seg : ToSegments()) {
    for (auto const& other_seg : other.ToSegments()) {
      auto inter = seg.Intersection(other_seg);

      if (inter.has_value() && std::holds_alternative<Point3D>(*inter)) {
        intersections.push_back(std::get<Point3D>(*inter));
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

#pragma endregion

#pragma region Formatting

std::string Polyline3D::ToWkt() const {
  std::ostringstream buf;
  buf << "LINESTRING ";

  if (KNOTS.size() > 0) {
    buf << "(";
    for (int i = 0; i < KNOTS.size(); ++i) {
      buf << std::format("{} {} {}", round(KNOTS[i].x()), round(KNOTS[i].y()), round(KNOTS[i].z()));

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

Polyline3D Polyline3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINESTRING") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).rfind(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string mid_part = wkt.substr(end_gtype + 1, end_pn);

    std::vector<Point3D> pt_vec;
    for (std::string const& p_str : geompp::tokenize_string(mid_part, ',')) {
      std::string pt_trimmed = geompp::trim(p_str);
      auto nums = geompp::tokenize_to_doubles(pt_trimmed, ' ');
      if (nums.size() != 3) {
        throw std::runtime_error("numbers");
      }
      pt_vec.emplace_back(nums[0], nums[1], nums[2]);
    }

    return Make(pt_vec);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Polyline3D::ToFile(std::string const& path) const {
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

Polyline3D Polyline3D::FromFile(std::string const& path) {
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
    GEOMPP_LOG(ERROR) << "bad path " << path;
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
