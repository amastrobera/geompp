#include "polyline3d.hpp"

#include "calc_utils2d.hpp"
#include "calc_utils3d.hpp"
#include "line2d.hpp"
#include "line3d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

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

Point3D const& Polyline3D::operator[](std::size_t i) const {
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

    if (inter.has_value()) {
      intersections.push_back(*inter);
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

    if (inter.has_value()) {
      intersections.push_back(*inter);
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

    if (inter.has_value()) {
      intersections.push_back(*inter);
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

      if (inter.has_value()) {
        intersections.push_back(*inter);
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

bool Polyline3D::IsPlanar() const {
  return are_coplanar(KNOTS);
}

bool Polyline3D::IsSimple() const {
  if (!IsPlanar()) {
    throw std::logic_error("Polyline3D::IsSimple — polyline is not planar");
  }

  // project onto the best-fit plane and check for 2D self-intersections
  auto no_col = remove_collinear(KNOTS);
  Vector3D calc_normal = (no_col[1] - no_col[0]).Cross(no_col[2] - no_col[0]);
  Axis dax = calc_normal.DominantAxis();

  auto to2d = [dax](Point3D const& p) -> Point2D {
    if (dax == Axis::X) { return Point2D(p.y(), p.z()); }
    if (dax == Axis::Y) { return Point2D(p.z(), p.x()); }
    return Point2D(p.x(), p.y());
  };

  int n = static_cast<int>(KNOTS.size());
  std::vector<LineSegment2D> segs;
  segs.reserve(n - 1);
  for (int i = 0; i < n - 1; ++i) {
    segs.push_back(LineSegment2D::Make(to2d(KNOTS[i]), to2d(KNOTS[i + 1])));
  }

  return !detail::has_intersections_impl(segs);
}

bool Polyline3D::IsConvex() const {
  if (!IsPlanar()) {
    throw std::logic_error("Polyline3D::IsConvex — polyline is not planar");
  }

  int n = static_cast<int>(KNOTS.size());
  if (n < 3) {
    return false;
  }

  // project onto 2D using the dominant normal axis, then check all cross products have the same sign
  auto no_col = remove_collinear(KNOTS);
  if (no_col.size() < 3) {
    return false;
  }
  Vector3D calc_normal = (no_col[1] - no_col[0]).Cross(no_col[2] - no_col[0]);
  Axis dax = calc_normal.DominantAxis();

  auto to2d = [dax](Point3D const& p) -> Point2D {
    if (dax == Axis::X) { return Point2D(p.y(), p.z()); }
    if (dax == Axis::Y) { return Point2D(p.z(), p.x()); }
    return Point2D(p.x(), p.y());
  };

  bool seen_positive = false;
  bool seen_negative = false;
  for (int i = 0; i < n; ++i) {
    Point2D v0 = to2d(KNOTS[i]);
    Point2D v1 = to2d(KNOTS[(i + 1) % n]);
    Point2D v2 = to2d(KNOTS[(i + 2) % n]);
    Vector2D e1 = v1 - v0;
    Vector2D e2 = v2 - v1;
    double cross = e1.Cross(e2);
    auto ord = compare(cross, 0.0);
    if (ord == std::partial_ordering::equivalent) {
      continue;  // collinear edge — neutral
    }
    if (ord > 0) {
      seen_positive = true;
    } else {
      seen_negative = true;
    }
    if (seen_positive && seen_negative) {
      return false;
    }
  }
  return true;
}

Polyline3D Polyline3D::ConvexHull() const {
  if (static_cast<int>(KNOTS.size()) < 3) {
    throw std::runtime_error("Polyline3D::ConvexHull — fewer than 3 points");
  }

  if (!IsPlanar()) {
    throw std::logic_error("Polyline3D::ConvexHull — polyline is not planar");
  }

  auto hull_pts = convex_hull(KNOTS);
  return Make(hull_pts);
}

Polygon3D Polyline3D::ToPolygon() const {
  if (static_cast<int>(KNOTS.size()) < 3) {
    throw std::runtime_error("Polyline3D::ToPolygon — fewer than 3 vertices");
  }

  if (!IsPlanar()) {
    throw std::logic_error("Polyline3D::ToPolygon — polyline is not planar");
  }

  return Polygon3D::Make(KNOTS);
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
