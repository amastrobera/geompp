#include "polygon3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Polygon3D Polygon3D::Make(std::vector<Point3D> const& points) {
  auto unique_points = remove_duplicates(points);

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_coplanar(unique_points)) {
    throw std::runtime_error("cannot create polygon with non-coplanar points");
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }
  auto outer_plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);
  return {unique_points, outer_plane};
}

Polygon3D Polygon3D::Make(std::vector<Point3D> const& points, std::vector<std::vector<Point3D>> const& holes) {
  auto unique_points =
      remove_collinear(remove_duplicates_from_sorted_list(points));  // remove duplicates and collinear points

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format(
        "cannot create polygon with less than 3 unique points; points are too close with {} decimals precision",
        DECIMAL_PRECISION));
  }

  if (!are_coplanar(unique_points)) {
    throw std::runtime_error("cannot create polygon with non-coplanar points");
  }

  if (!are_ccw(unique_points)) {
    throw std::runtime_error("cannot create polygon with points in anti clock-wise order");
  }

  auto outer_plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);

  std::vector<std::vector<Point3D>> unique_holes_points;
  for (auto const& hole : holes) {
    auto unique_hole_points = remove_collinear(remove_duplicates_from_sorted_list(hole));

    if (unique_hole_points.size() < 3) {
      throw std::runtime_error(std::format(
          "cannot create hole with less than 3 unique points; points are too close with {} decimals precision",
          DECIMAL_PRECISION));
    }

    if (!are_coplanar(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes non-coplanar points");
    }

    if (!are_cw(unique_hole_points)) {
      throw std::runtime_error("cannot create polygon holes in anti-clock-wise order");
    }

    // verify that holes are on the same plane as the outer loop
    for (int i = 0; i < unique_hole_points.size(); ++i) {
      if (!outer_plane.Contains(unique_hole_points[i])) {
        throw std::runtime_error("cannot create polygon holes that are not on the same plane as the outer loop");
      }
    }

    unique_holes_points.push_back(unique_hole_points);
  }

  return {unique_points, outer_plane, unique_holes_points};
}

Polygon3D::Polygon3D(std::vector<Point3D> const& points, Plane const& plane) : VERTICES(points), PLANE(plane) {}

Polygon3D::Polygon3D(std::vector<Point3D> const& points, Plane const& plane,
                     std::vector<std::vector<Point3D>> const& holes)
    : VERTICES(points), PLANE(plane), HOLES(holes) {}

Polygon3D& Polygon3D::operator=(Polygon3D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
  }
  return *this;
}

bool Polygon3D::AlmostEquals(Polygon3D const& other, double epsilon) const {
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

Point3D Polygon3D::Centroid() const {
  Point3D cs = centroid(VERTICES, PLANE);
  double sa = signed_area(VERTICES, PLANE);

  if (HOLES.empty()) {
    return cs;
  }

  // weighted average: c = Σ(aᵢ·cᵢ) / Σ(aᵢ)  — hole areas are negative (CW) so they subtract
  double total_sa = sa;
  double wx = sa * cs.x();
  double wy = sa * cs.y();
  double wz = sa * cs.z();

  for (auto const& hole : HOLES) {
    double sa_h = signed_area(hole, PLANE);
    Point3D c_h = centroid(hole, PLANE);
    total_sa += sa_h;
    wx += sa_h * c_h.x();
    wy += sa_h * c_h.y();
    wz += sa_h * c_h.z();
  }

  return Point3D(wx / total_sa, wy / total_sa, wz / total_sa);
}

double Polygon3D::Area() const {
  double area = signed_area(VERTICES, PLANE);  // guaranteed to be positive by construction
  for (auto const& hole : HOLES) {
    area += signed_area(hole, PLANE);  // guaranteed to be negative by construction, so we add it
  }
  return area;
}

double Polygon3D::Perimeter() const {
  double perimeter = 0;
  int n = VERTICES.size();
  for (int i = 0; i < n; ++i) {
    perimeter += VERTICES[i].DistanceTo(VERTICES[(i + 1) % n]);
  }
  return perimeter;
}

double Polygon3D::DistanceTo(Point3D const& point) const { throw std::runtime_error("not implemented"); }

double Polygon3D::Location(Point3D const& point) const { throw std::runtime_error("not implemented"); }

Point3D Polygon3D::Interpolate(double pct) const { throw std::runtime_error("not implemented"); }

#pragma region Operator Overloading

bool operator==(Polygon3D const& lhs, Polygon3D const& rhs) { return lhs.AlmostEquals(rhs); }

Point3D const& Polygon3D::operator[](int i) const {
  if (i >= Size()) {
    throw std::out_of_range("Index out of range");
  }
  return VERTICES[i];
}

std::ostream& operator<<(std::ostream& os, Polygon3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Polygon3D::Contains(Point3D const& point) const { throw std::runtime_error("not implemented"); }

bool Polygon3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Polygon3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Polygon3D::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

Polygon3D::ReturnSet Polygon3D::Intersection(Line3D const& line) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(Ray3D const& ray) const { throw std::runtime_error("not implemented"); }

Polygon3D::ReturnSet Polygon3D::Intersection(LineSegment3D const& other) const {
  throw std::runtime_error("not implemented");
}

#pragma endregion

#pragma region Formatting

std::string Polygon3D::ToWkt() const {
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
      buf << std::format("{} {} {}, ", round(VERTICES[i].x()), round(VERTICES[i].y()), round(VERTICES[i].z()));
    }
    buf << std::format("{} {} {}", round(VERTICES[0].x()), round(VERTICES[0].y()), round(VERTICES[0].z()));
    buf << ")";
  }

  // inner loops
  {
    for (auto const& hole : HOLES) {
      buf << ", (";
      std::size_t n = hole.size();
      for (int i = 0; i < n; ++i) {
        buf << std::format("{} {} {}, ", round(hole[i].x()), round(hole[i].y()), round(hole[i].z()));
      }
      buf << std::format("{} {} {}", round(hole[0].x()), round(hole[0].y()), round(hole[0].z()));
      buf << ")";
    }
  }

  buf << ")";

  return buf.str();
}

Polygon3D Polygon3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "POLYGON") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).rfind(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string polygon_loops_wkt = wkt.substr(end_gtype + 1, end_pn);

    // find outer loop (first loop)
    std::vector<Point3D> points;
    std::size_t start_outer_loop, end_outer_loop;
    {
      start_outer_loop = end_gtype + polygon_loops_wkt.find('(');
      if (start_outer_loop == std::string::npos) {
        throw std::runtime_error("brakets (outer)");
      }

      end_outer_loop = end_gtype + polygon_loops_wkt.find(')');
      if (end_pn == std::string::npos) {
        throw std::runtime_error("brakets (outer/end)");
      }

      std::string outer_loop_str = wkt.substr(start_outer_loop + 1, end_outer_loop - start_outer_loop - 1);

      for (std::string const& wkt_str : geompp::tokenize_string(outer_loop_str, ',')) {
        std::string wkt_trimmed = geompp::trim(wkt_str);
        auto nums = geompp::tokenize_to_doubles(wkt_trimmed);
        if (nums.size() != 3) {
          throw std::runtime_error("numbers");
        }
        points.emplace_back(nums[0], nums[1], nums[2]);
      }
    }

    // find holes (other loops)
    std::vector<std::vector<Point3D>> holes;
    std::size_t start_inner_loop =
        polygon_loops_wkt.substr(end_outer_loop + 1).find(',');  // find the comma separator of loops
    std::size_t end_inner_loop;
    while (start_inner_loop != std::string::npos) {
      start_inner_loop = start_inner_loop + polygon_loops_wkt.substr(end_outer_loop + 1).find('(');
      if (start_inner_loop == std::string::npos) {
        throw std::runtime_error("brakets (inner)");
      }

      end_inner_loop = start_inner_loop + polygon_loops_wkt.substr(end_outer_loop + 1).find(')');
      if (end_inner_loop == std::string::npos) {
        throw std::runtime_error("brakets (inner/end)");
      }

      std::vector<Point3D> hole;
      std::string inner_loop_str = wkt.substr(start_inner_loop + 1, end_inner_loop - start_inner_loop - 1);
      for (std::string const& wkt_str : geompp::tokenize_string(inner_loop_str, ',')) {
        std::string wkt_trimmed = geompp::trim(wkt_str);
        auto nums = geompp::tokenize_to_doubles(wkt_trimmed);
        if (nums.size() != 3) {
          throw std::runtime_error("numbers");
        }
        hole.emplace_back(nums[0], nums[1], nums[2]);
      }
      holes.push_back(hole);

      // reset to next comma separator of loops
      start_inner_loop =
          end_inner_loop + polygon_loops_wkt.substr(end_inner_loop + 1).find(',');  // find the comma separator of loops
    }

    return Make(points, holes);

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

  throw std::runtime_error("failed to parse WKT");
}

void Polygon3D::ToFile(std::string const& path) const {
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

Polygon3D Polygon3D::FromFile(std::string const& path) {
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
