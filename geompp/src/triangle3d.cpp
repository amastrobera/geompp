#include "triangle3d.hpp"

#include "bbox3d.hpp"
#include "line2d.hpp"
#include "line3d.hpp"
#include "line_segment2d.hpp"
#include "line_segment3d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "ray3d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace geompp {

namespace {

static bool within_axis_boundary(double s, double t) {
  return (compare(s, 0) >= 0 && compare(s, 1.0) <= 0) &&
         (compare(t, 0) >= 0 && compare(t, 1.0) <= 0 && compare(s + t, 1.0) <= 0);  // including borders
}

}  // namespace

#pragma region Constructors

Triangle3D Triangle3D::Make(Point3D const& p0, Point3D const& p1, Point3D const& p2) {
  auto unique_points = remove_duplicates({p0, p1, p2});

  if (unique_points.size() < 3) {
    throw std::runtime_error(std::format("points {}, {}, {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), p2.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1, p2};
}

Triangle3D& Triangle3D::operator=(Triangle3D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
    P2 = other.P2;
  }
  return *this;
}

bool Triangle3D::AlmostEquals(Triangle3D const& other, double epsilon) const {
  return P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon) && P2.AlmostEquals(other.P2, epsilon);
}

#pragma endregion

#pragma region Dimentions

Point3D Triangle3D::Centroid() const { return average({P0, P1, P2}); }

Polygon3D Triangle3D::ToPolygon() const { return Polygon3D::Make({P0, P1, P2}); }

Plane Triangle3D::ToPlane() const { return Plane::From3Points(P0, P1, P2); }

Vector3D Triangle3D::Normal() const { return AreaVector().Normalize(); }

Vector3D Triangle3D::AreaVector() const { return (P1 - P0).Cross(P2 - P0) / 2.0; }

double Triangle3D::SignedArea(Vector3D const& ref_normal) const { return ref_normal.Dot(AreaVector()); }

double Triangle3D::Area() const { return AreaVector().Length(); }

double Triangle3D::Perimeter() const { return (P1 - P0).Length() + (P2 - P1).Length() + (P0 - P2).Length(); }

bool Triangle3D::IsCCW(Vector3D const& ref_normal) const { return SignedArea(ref_normal) > 0; }

#pragma endregion

#pragma region line operations

double Triangle3D::DistanceTo(Point3D const& point) const { throw std::runtime_error("not implemented"); }

std::tuple<Vector3D, Vector3D> Triangle3D::ToAxis() const { return {P1 - P0, P2 - P0}; }

std::optional<Point3D> Triangle3D::Interpolate(double s, double t) const {
  if (!within_axis_boundary(s, t)) {
    GEOMPP_LOG(ERROR) << "(s, t) = (" << s << ", " << t << ") are not within boundaries [0, 1]";
    return std::nullopt;
  }

  return linear_combination({P0, P1, P2}, {1 - s - t, s, t});
}

std::optional<std::tuple<double, double>> Triangle3D::Location(Point3D const& point) const {
  if (!ToPlane().Contains(point)) {
    return std::nullopt;
  }

  auto n = ToPlane().normal();
  auto u = P1 - P0;
  auto v = P2 - P0;
  auto w = point - P0;
  auto u_perp = n.Cross(u);
  auto v_perp = n.Cross(v);
  double s = w.Dot(v_perp) / u.Dot(v_perp);  // guaranteed non zero (triangle ctor)
  double t = w.Dot(u_perp) / v.Dot(u_perp);  // guaranteed non zero (triangle ctor)

  if (!within_axis_boundary(s, t)) {
    return std::nullopt;
  }
  return std::make_tuple(s, t);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Triangle3D const& lhs, Triangle3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Triangle3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Triangle3D::Contains(Point3D const& point) const {
  if (!BBox3D(*this).Contains(point)) {
    return false;
  }
  return Location(point).has_value();
}

bool Triangle3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Triangle3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Triangle3D::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

bool Triangle3D::Intersects(Triangle3D const& other) const { return Intersection(other).has_value(); }

bool Triangle3D::Intersects(Plane const& plane) const { return plane.Intersects(*this); }

Triangle3D::ReturnSet Triangle3D::Intersection(Line3D const& line) const {
  // intersection with the plane
  auto t_plane = ToPlane();

  auto plane_intersection = t_plane.Intersection(line);
  if (!(plane_intersection.has_value() && std::holds_alternative<Point3D>(*plane_intersection))) {
    return std::nullopt;
  }
  // the point of intersection on the plane
  auto PI = std::get<Point3D>(*plane_intersection);

  // ... does it belong to the triangle ?
  // we could project and compute 2D but there is a quicker 3D direct approach
  auto U = P1 - P0;
  auto V = P2 - P0;
  auto W = PI - P0;

  double U2 = U.Dot(U);
  double V2 = V.Dot(V);
  double UV = U.Dot(V);
  double WU = W.Dot(U);
  double WV = W.Dot(V);

  double D = UV * UV - U2 * V2;  // can never be zero since triangle is not degenerate (ctor guarantees unique points)

  double sc = (UV * WV - V2 * WU) / D;
  double tc = (UV * WU - U2 * WV) / D;

  // test if the baricentric coordinates are within the triangle's axis range
  if (!within_axis_boundary(sc, tc)) {
    return std::nullopt;
  }

  return PI;
}

Triangle3D::ReturnSet Triangle3D::Intersection(Ray3D const& ray) const {
  auto line_intersection = Intersection(ray.ToLine());
  if (!(line_intersection.has_value() && std::holds_alternative<Point3D>(*line_intersection))) {
    return std::nullopt;
  }

  auto PI = std::get<Point3D>(*line_intersection);

  if (!ray.IsAhead(PI)) {
    return std::nullopt;
  }

  return PI;
}

Triangle3D::ReturnSet Triangle3D::Intersection(LineSegment3D const& segment) const {
  auto line_intersection = Intersection(segment.ToLine());
  if (!(line_intersection.has_value() && std::holds_alternative<Point3D>(*line_intersection))) {
    return std::nullopt;
  }

  auto PI = std::get<Point3D>(*line_intersection);

  if (!segment.Contains(PI)) {
    return std::nullopt;
  }

  return PI;
}

Triangle3D::ReturnSet Triangle3D::Intersection(Plane const& plane) const {
  auto t_plane = ToPlane();
  auto plane_intersection = t_plane.Intersection(plane);

  if (!(plane_intersection.has_value() && std::holds_alternative<Line3D>(*plane_intersection))) {
    return std::nullopt;
  }

  auto plane_intersection_line = std::get<Line3D>(*plane_intersection);

  // evaluate in 2D
  auto line2d = Line2D::Make(t_plane.ProjectInto(plane_intersection_line.First()),
                             t_plane.ProjectInto(plane_intersection_line.Last()));

  auto triangle2d = Triangle2D::Make(t_plane.ProjectInto(P0), t_plane.ProjectInto(P1), t_plane.ProjectInto(P2));

  auto intersection_2d = triangle2d.Intersection(line2d);  // this can be either a point or a line segment - if not null

  if (!intersection_2d.has_value()) {
    return std::nullopt;
  }

  if (std::holds_alternative<Point2D>(*intersection_2d)) {
    auto intersection_point_2d = std::get<Point2D>(*intersection_2d);
    return t_plane.Evaluate(intersection_point_2d);
  }

  if (std::holds_alternative<LineSegment2D>(*intersection_2d)) {
    auto intersection_segment_2d = std::get<LineSegment2D>(*intersection_2d);
    return LineSegment3D::Make(t_plane.Evaluate(intersection_segment_2d.First()),
                               t_plane.Evaluate(intersection_segment_2d.Last()));
  }

  throw std::runtime_error("unexpected type of intersection result");
}

Triangle3D::ReturnSet Triangle3D::Intersection(Triangle3D const& other) const {
  auto t_plane = ToPlane();
  auto other_plane = other.ToPlane();

  // intersection of this triangle to other plane exists ?
  auto t_plane_intersection = Intersection(other_plane);
  if (!(t_plane_intersection.has_value() && std::holds_alternative<LineSegment3D>(*t_plane_intersection))) {
    return std::nullopt;
  }

  // intersection of other triangle to this plane exists ?
  auto other_plane_intersection = other.Intersection(t_plane);
  if (!(other_plane_intersection.has_value() && std::holds_alternative<LineSegment3D>(*other_plane_intersection))) {
    return std::nullopt;
  }

  // verify that they overlap and compute the overlapping segment (if any)
  auto seg = std::get<LineSegment3D>(*t_plane_intersection);
  auto other_seg = std::get<LineSegment3D>(*other_plane_intersection);

  auto s1 = seg.First();
  auto s2 = seg.Last();
  auto o1 = other_seg.First();
  auto o2 = other_seg.Last();

  bool o1_in = seg.Contains(o1);
  bool o2_in = seg.Contains(o2);
  bool s1_in = other_seg.Contains(s1);
  bool s2_in = other_seg.Contains(s2);

  // case 1: overlap or full containment of one segment to another
  //         s1 *-------------* s2
  //     o1 *-----------------------* o2
  if (s1_in && s2_in) {
    return seg;
  }
  //     s1 *-----------------------* s2
  //         o1 *-------------* o2
  if (o1_in && o2_in) {
    return other_seg;
  }

  // case 2 partial overlap
  //                s1 *-------------* s2
  //     o1 *-----------------* o2
  if (s1_in && o2_in && !s2_in && !o1_in) {
    return LineSegment3D::Make(s1, o2);
  }
  //    s1 *-------------* s2
  //            o1 *-----------------* o2
  if (!s1_in && !o2_in && o1_in && s2_in) {
    return LineSegment3D::Make(o1, s2);
  }

  throw std::runtime_error("unexpected type of intersection result");
}

#pragma endregion

#pragma region Formatting

std::string Triangle3D::ToWkt() const {
  // clang-format off
  return std::format("TRIANGLE ({} {} {}, {} {} {}, {} {} {})", 
                     round(P0.x()), round(P0.y()), round(P0.z()),
                     round(P1.x()), round(P1.y()), round(P1.z()),
                     round(P2.x()), round(P2.y()), round(P2.z())
                     );
  // clang-format on
}

Triangle3D Triangle3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "TRIANGLE") {
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

    if (pt_vec.size() != 3) {
      throw std::runtime_error("initialized with n != 3 points");
    }

    return Make(pt_vec[0], pt_vec[1], pt_vec[2]);

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Triangle3D::ToFile(std::string const& path) const {
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

Triangle3D Triangle3D::FromFile(std::string const& path) {
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
