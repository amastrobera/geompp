#include "polygon2d.hpp"

#include "bbox2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace geompp {

namespace {

bool is_left(Point2D const& v1, Point2D const& v2, Point2D const& p) {
  return compare((v2.x() - v1.x()) * (p.y() - v1.y()) - (v2.y() - v1.y()) * (p.x() - v1.x()), 0) > 0;
}

bool is_right(Point2D const& v1, Point2D const& v2, Point2D const& p) {
  return compare((v2.x() - v1.x()) * (p.y() - v1.y()) - (v2.y() - v1.y()) * (p.x() - v1.x()), 0) < 0;
}

int wn_count(std::vector<Point2D> const& vertices, Point2D const& p) {
  int wn = 0;

  int i2 = -1;
  int n = vertices.size();
  for (int i1 = 0; i1 < n; ++i1) {
    i2 = (i1 + 1) % n;

    auto const& v1 = vertices[i1];
    auto const& v2 = vertices[i2];

    if (compare(v1.y(), p.y()) <= 0) {   // edge from v(i) to v(i+1) is starts below p
      if (compare(v2.y(), p.y()) > 0) {  //     ... and ends above p (upward crossing)
        if (is_left(v1, v2, p)) {        //     p left of the edge
          ++wn;                          // valid up-intersect
        }
      }

    } else {                              // edge from v(i) to v(i+1) is starts above p
      if (compare(v2.y(), p.y()) <= 0) {  //     ... and ends below p (downward crossing)
        if (is_right(v1, v2, p)) {        //     p is right of the edge
          --wn;                           // valid down-intersect
        }
      }
    }
  }

  return wn;
}

}  // namespace

#pragma region Constructors

Polygon2D Polygon2D::Make(std::vector<Point2D> const& points) {
  auto unique_points = remove_collinear(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points  are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  double perimeter = 0;
  int n0 = unique_points.size();
  for (int i = 0; i < n0; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % n0]);
  }

  return {unique_points, perimeter};
}

Polygon2D Polygon2D::Make(std::vector<Point2D> const& points, std::vector<std::vector<Point2D>> const& holes) {
  auto unique_points =
      remove_collinear(remove_consecutive_duplicates(points));  // remove duplicates and collinear points

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  std::vector<std::vector<Point2D>> unique_holes_points;
  for (auto const& hole : holes) {
    auto unique_hole_points = remove_collinear(remove_consecutive_duplicates(hole));

    if (unique_hole_points.size() < 3) {
      throw std::runtime_error(std::format(
          "cannot create hole with less than 3 unique points; points are too close with {} decimals precision",
          DECIMAL_PRECISION));
    }

    if (!are_cw(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes in anti-clock-wise order");
    }

    unique_holes_points.push_back(unique_hole_points);
  }

  double perimeter = 0;
  int nh = unique_points.size();
  for (int i = 0; i < nh; ++i) {
    perimeter += unique_points[i].DistanceTo(unique_points[(i + 1) % nh]);
  }

  return {unique_points, perimeter, unique_holes_points};
}

Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter) : VERTICES(points), PERIMETER(perimeter) {}

Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter,
                     std::vector<std::vector<Point2D>> const& holes)
    : VERTICES(points), HOLES(holes), PERIMETER(perimeter) {}

Polygon2D& Polygon2D::operator=(Polygon2D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
    HOLES = other.HOLES;
    PERIMETER = other.PERIMETER;
  }
  return *this;
}

bool Polygon2D::AlmostEquals(Polygon2D const& other, double epsilon) const {
  // size comparison of loops
  if (Size() != other.Size() || HOLES.size() != other.HOLES.size()) {
    return false;
  }
  for (size_t i = 0; i < HOLES.size(); ++i) {
    if (HOLES[i].size() != other.HOLES[i].size()) {
      return false;
    }
  }

  // outer loop vertices comparison
  for (size_t i = 0; i < VERTICES.size(); ++i) {
    if (!VERTICES[i].AlmostEquals(other[i], epsilon)) {
      return false;
    }
  }

  // inner loops vertices comparison
  for (size_t i = 0; i < HOLES.size(); ++i) {
    for (size_t j = 0; j < HOLES[i].size(); ++j) {
      if (!HOLES[i][j].AlmostEquals(other.HOLES[i][j], epsilon)) {
        return false;
      }
    }
  }
  return true;
}

SegmentRange2D Polygon2D::ToSegments() const { return SegmentRange2D(VERTICES, true); }

Point2D Polygon2D::Centroid() const {
  Point2D cs = centroid(VERTICES);
  double sa = signed_area(VERTICES);

  if (HOLES.empty()) {
    return cs;
  }

  // weighted average: c = Σ(aᵢ·cᵢ) / Σ(aᵢ)  — hole areas are negative (CW) so they subtract
  double total_sa = sa;
  double wx = sa * cs.x();
  double wy = sa * cs.y();

  for (auto const& hole : HOLES) {
    double sa_h = signed_area(hole);
    Point2D c_h = centroid(hole);
    total_sa += sa_h;
    wx += sa_h * c_h.x();
    wy += sa_h * c_h.y();
  }

  return Point2D(wx / total_sa, wy / total_sa);
}

double Polygon2D::Area() const {
  // outer loop
  double area = signed_area(VERTICES);  // this is guaranteed to be positive by the constructor

  // remove the areas of holes (inner loops)
  for (auto const& hole : HOLES) {
    area += signed_area(
        hole);  // holes are checked to be CW (guaranteed by constructor), therefore this area WILL be negative
  }

  return area;
}

double Polygon2D::DistanceTo(Point2D const& point) const { throw std::runtime_error("not implemented"); }

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs) { return lhs.AlmostEquals(rhs); }

Point2D const& Polygon2D::operator[](int i) const {
  if (i >= Size()) {
    throw std::out_of_range("Index out of range");
  }
  return VERTICES[i];
}

std::ostream& operator<<(std::ostream& os, Polygon2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Polygon2D::IsOnBoundary(Point2D const& point) const {
  int n = VERTICES.size();
  for (int i = 0; i < n; ++i) {
    if (LineSegment2D::Make(VERTICES[i], VERTICES[(i + 1) % n]).Contains(point)) {
      return true;
    }
  }
  for (auto const& hole : HOLES) {
    int nh = hole.size();
    for (int i = 0; i < nh; ++i) {
      if (LineSegment2D::Make(hole[i], hole[(i + 1) % nh]).Contains(point)) {
        return true;
      }
    }
  }
  return false;
}

// winding number method — boundary-inclusive (matches Triangle behaviour)
bool Polygon2D::Contains(Point2D const& point) const {
  // quick rejection (outside of bounding box)
  if (!BBox2D(*this).Contains(point)) {
    return false;
  }

  // containment on boundaries (slow?)
  if (IsOnBoundary(point)) {
    return true;
  }

  // strict containment: winding number method
  int wn = 0;
  wn += wn_count(VERTICES, point);
  for (auto const& hole : HOLES) {
    wn += wn_count(hole, point);
  }
  return wn != 0;  // wn == 0 if the point is outside
}

bool Polygon2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool Polygon2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon2D::Intersects(LineSegment2D const& segment) const { return Intersection(segment).has_value(); }

Polygon2D::ReturnSet Polygon2D::Intersection(Line2D const& line) const { throw std::runtime_error("not implemented"); }

Polygon2D::ReturnSet Polygon2D::Intersection(Ray2D const& ray) const { throw std::runtime_error("not implemented"); }

Polygon2D::ReturnSet Polygon2D::Intersection(LineSegment2D const& other) const {
  throw std::runtime_error("not implemented");
}

#pragma endregion

#pragma region Formatting

std::string Polygon2D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYGON ";
  int num_verts = VERTICES.size();
  if (!num_verts) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "(";

  // outer loop
  {
    buf << "(";
    for (int i = 0; i < num_verts; ++i) {
      buf << std::format("{} {}, ", round(VERTICES[i].x()), round(VERTICES[i].y()));
    }
    buf << std::format("{} {}", round(VERTICES[0].x()), round(VERTICES[0].y()));
    buf << ")";
  }

  // inner loops
  {
    for (auto const& hole : HOLES) {
      buf << ", (";
      std::size_t n = hole.size();
      for (int i = 0; i < n; ++i) {
        buf << std::format("{} {}, ", round(hole[i].x()), round(hole[i].y()));
      }
      buf << std::format("{} {}", round(hole[0].x()), round(hole[0].y()));
      buf << ")";
    }
  }

  buf << ")";

  return buf.str();
  ;
}

Polygon2D Polygon2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POLYGON") {
      throw std::runtime_error("geometry name");
    }

    std::size_t outer_close = wkt.rfind(')');
    if (outer_close == std::string::npos) {
      throw std::runtime_error("brakets (outer close)");
    }

    // Content between outermost parens: e.g. "(x y, ...), (hx hy, ...)"
    std::string content = wkt.substr(end_gtype + 1, outer_close - end_gtype - 1);

    // Parse each ring by scanning for '(' ... ')' pairs
    std::vector<std::vector<Point2D>> rings;
    std::size_t pos = 0;
    while (pos < content.size()) {
      std::size_t ring_open = content.find('(', pos);
      if (ring_open == std::string::npos) {
        break;
      }
      std::size_t ring_close = content.find(')', ring_open);
      if (ring_close == std::string::npos) {
        throw std::runtime_error("brakets (ring close)");
      }

      std::string ring_str = content.substr(ring_open + 1, ring_close - ring_open - 1);
      std::vector<Point2D> ring;
      for (std::string const& tok : geompp::tokenize_string(ring_str, ',')) {
        std::string trimmed = geompp::trim(tok);
        auto nums = geompp::tokenize_to_doubles(trimmed);
        if (nums.size() != 2) {
          throw std::runtime_error("numbers");
        }
        ring.emplace_back(nums[0], nums[1]);
      }
      // WKT rings close by repeating the first point — drop it before passing to Make
      if (ring.size() > 1 && ring.back().AlmostEquals(ring.front())) {
        ring.pop_back();
      }
      rings.push_back(ring);
      pos = ring_close + 1;
    }

    if (rings.empty()) {
      throw std::runtime_error("no rings");
    }

    std::vector<Point2D> outer_ring = rings[0];
    std::vector<std::vector<Point2D>> holes(rings.begin() + 1, rings.end());
    return Make(outer_ring, holes);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

  throw std::runtime_error("failed to parse WKT");
}

void Polygon2D::ToFile(std::string const& path) const {
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

Polygon2D Polygon2D::FromFile(std::string const& path) {
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
