#include "line_segment2d.hpp"

#include "line2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment2D LineSegment2D::Make(Point2D const& p0, Point2D const& p1) {
  if (p0.AlmostEquals(p1)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", p0.ToWkt(),
                                         p1.ToWkt(), DECIMAL_PRECISION));
  }
  return {p0, p1};
}

LineSegment2D::LineSegment2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1) {}

LineSegment2D& LineSegment2D::operator=(LineSegment2D const& other) {
  if (this != &other) {
    P0 = other.P0;
    P1 = other.P1;
  }
  return *this;
}

double LineSegment2D::Length() const { return (P1 - P0).Length(); }

bool LineSegment2D::AlmostEquals(LineSegment2D const& other, double epsilon) const {
  return (P0.AlmostEquals(other.P0, epsilon) && P1.AlmostEquals(other.P1, epsilon))  // forward
         ||                                                                          //
         (P0.AlmostEquals(other.P1, epsilon) && P1.AlmostEquals(other.P0, epsilon))  // backward
      ;  // no heading check (needed for edges in a mesh though)
}

Line2D LineSegment2D::ToLine() const { return Line2D::Make(P0, P1); }

double LineSegment2D::Location(Point2D const& point) const {
  if (!ToLine().Contains(point)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length() / Length();
}

Point2D LineSegment2D::Interpolate(double pct) const {
  // the point is behind the polyline
  if (compare(pct, 0) < 0) {
    return P0;
  }

  // the point is beyond the polyline
  if (compare(pct, 1.0) > 0) {
    return P1;
  }

  return P0 + pct * (P1 - P0);
}

double LineSegment2D::DistanceTo(Point2D const& point) const {
  auto line_eqv = ToLine();
  auto proj = line_eqv.ProjectOnto(point);
  double loc = Location(proj);

  if (compare(loc, 0) < 0) {
    return P0.DistanceTo(point);

  } else if (compare(loc, 1.0) > 0) {
    return P1.DistanceTo(point);
  }

  return line_eqv.DistanceTo(point);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, LineSegment2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment2D::Contains(Point2D const& point) const {
  double t = Location(point);
  return compare(t, 0) >= 0 && compare(t, 1.0) <= 0;
}

bool LineSegment2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool LineSegment2D::Intersects(Ray2D const& ray) const { return Intersection(ray).has_value(); }

bool LineSegment2D::Intersects(LineSegment2D const& other) const { return Intersection(other).has_value(); }

LineSegment2D::ReturnSet LineSegment2D::Intersection(Line2D const& line) const {
  double sc, tc;
  auto Pc = ToLine().Intersection(line, sc, tc);

  // respecting constraints: sc should be in the range [0, 1]
  if (!Pc.has_value() || !is_in_range(sc, 0, 1)) {
    return std::nullopt;
  }

  return Pc;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(Ray2D const& ray) const {
  double sc, tc;
  auto Pc = ToLine().Intersection(ray.ToLine(), sc, tc);

  // respecting constraints: sc should be in the range [0, 1] while tc should be in non negative
  if (!Pc.has_value() || !is_in_range(sc, 0, 1) || !is_greater_or_equal(tc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(LineSegment2D const& other) const {
  double sc, tc;
  auto Pc = ToLine().Intersection(other.ToLine(), sc, tc);

  // respecting constraints: sc and tc should be in the range [0, 1]
  if (!Pc.has_value() || !is_in_range(sc, 0, 1) || !is_in_range(tc, 0, 1)) {
    return std::nullopt;
  }

  return Pc;
}

#pragma endregion

#pragma region Formatting

std::string LineSegment2D::ToWkt() const {
  return std::format("LINESTRING ({} {}, {} {})", round(P0.x()), round(P0.y()), round(P1.x()), round(P1.y()));
}

LineSegment2D LineSegment2D::FromWkt(std::string const& wkt) {
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
    if (nums_p1.size() != 2) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2 - 1);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 2) {
      throw std::runtime_error("numbers p2");
    }

    return Make({nums_p1[0], nums_p1[1]}, {nums_p2[0], nums_p2[1]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void LineSegment2D::ToFile(std::string const& path) const {
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

LineSegment2D LineSegment2D::FromFile(std::string const& path) {
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
