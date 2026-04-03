#include "line_segment3d.hpp"

#include "line3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment3D LineSegment3D::Make(Point3D const& p0, Point3D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", DECIMAL_PRECISION,
                                         p0.ToWkt(), p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

LineSegment3D::LineSegment3D(Point3D const& p0, Point3D const& p1) : P0(p0), P1(p1) {}

LineSegment3D& LineSegment3D::operator=(LineSegment3D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
  }
  return *this;
}

double LineSegment3D::Length() const { return (P1 - P0).Length(); }

bool LineSegment3D::AlmostEquals(LineSegment3D const& other, int decimal_precision) const {
  return P0.AlmostEquals(other.P0, decimal_precision) && P1.AlmostEquals(other.P1, decimal_precision);
}

Line3D LineSegment3D::ToLine() const { return Line3D::Make(P0, P1); }

double LineSegment3D::Location(Point3D const& point) const {
  if (!ToLine().Contains(point)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length() / Length();
}

Point3D LineSegment3D::Interpolate(double pct) const {
  // the point is behind the polyline
  if (round(pct) < 0.0) {
    return P0;
  }

  // the point is beyond the polyline
  if (round(pct) > 1.0) {
    return P1;
  }

  return P0 + pct * (P1 - P0);
}

double LineSegment3D::DistanceTo(Point3D const& point) const {
  auto line_eqv = ToLine();
  auto proj = line_eqv.ProjectOnto(point);
  double loc = Location(proj);

  if (round(loc) < 0.0) {
    return P0.DistanceTo(point);

  } else if (round(loc - 1.0) > 0.0) {
    return P1.DistanceTo(point);
  }

  return line_eqv.DistanceTo(point);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment3D const& lhs, LineSegment3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, LineSegment3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment3D::Contains(Point3D const& point) const {
  double t = Location(point);
  return round(t) >= 0 && round(t - 1) <= 0;
}

bool LineSegment3D::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool LineSegment3D::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool LineSegment3D::Intersects(LineSegment3D const& other) const { return Intersection(other).has_value(); }

LineSegment3D::ReturnSet LineSegment3D::Intersection(Line3D const& line) const {
  auto u = P1 - P0;
  auto v = line.Direction();

  if (u.IsParallel(v)) {
    return std::nullopt;
  }

  auto vp = v.Perp();
  auto w = (P0 - line.First());
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = P0 + t * u;
  if (!Contains(inter_p)) {
    return std::nullopt;
  }

  return inter_p;
}

LineSegment3D::ReturnSet LineSegment3D::Intersection(Ray3D const& ray) const {
  auto u = P1 - P0;
  auto v = ray.Direction();

  // testing on this ray
  if (u.IsParallel(v)) {
    return std::nullopt;
  }

  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto vp = v.Perp();
  auto w = (P0 - ray.Origin());

  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (v.IsParallel(up)) {
    return std::nullopt;
  }

  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = ray.Origin() + s * v;
  if (!ray.IsAhead(inter_s)) {
    return std::nullopt;
  }

  return inter_t;
}

LineSegment3D::ReturnSet LineSegment3D::Intersection(LineSegment3D const& other) const {
  auto u = P1 - P0;
  auto v = (other.P1 - other.P0);

  // testing on this ray
  if (u.IsParallel(v)) {
    return std::nullopt;
  }

  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto vp = v.Perp();
  auto w = (P0 - other.P0);

  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (v.IsParallel(up)) {
    return std::nullopt;
  }

  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.P0 + s * v;
  if (!other.Contains(inter_s)) {
    return std::nullopt;
  }

  return inter_t;
}

#pragma endregion

#pragma region Formatting

std::string LineSegment3D::ToWkt() const {
  return std::format("LINESTRING ({} {} {}, {} {} {})", round(P0.x()), round(P0.y()), round(P0.z()), round(P1.x()),
                     round(P1.y()), round(P1.z()));
}

LineSegment3D LineSegment3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINESTRING") {
      throw std::runtime_error("geometry name");
    }
    end_p1 = wkt.substr(end_gtype).find(',');
    if (end_p1 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1 - 1);

    auto nums_p1 = geompp::tokenize_to_doubles(s_nums_p1);
    if (nums_p1.size() != 3) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2 - 1);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 3) {
      throw std::runtime_error("numbers p2");
    }

    return Make({nums_p1[0], nums_p1[1], nums_p1[2]}, {nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void LineSegment3D::ToFile(std::string const& path) const {
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

LineSegment3D LineSegment3D::FromFile(std::string const& path) {
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
