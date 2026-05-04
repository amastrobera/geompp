#include "ray2d.hpp"

#include "constants.hpp"
#include "geompp_log.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Ray2D Ray2D::Make(Point2D const& p0, Vector2D const& dir) {
  if (compare(dir.Length(), 0) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Ray2D::Ray2D(Point2D const& orig, Vector2D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

Ray2D& Ray2D::operator=(Ray2D const& other) {
  if (this != &other) {
    ORIGIN = other.ORIGIN;
    DIR = other.DIR;
  }
  return *this;
}

bool Ray2D::IsAhead(Point2D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) >= 0; }

bool Ray2D::IsBehind(Point2D const& point) const { return compare(DIR.Dot(point - ORIGIN), 0) < 0; }

Line2D Ray2D::ToLine() const { return Line2D::Make(ORIGIN, DIR); }

bool Ray2D::AlmostEquals(Ray2D const& other, double epsilon) const {
  return ORIGIN.AlmostEquals(other.ORIGIN, epsilon) && DIR.AlmostEquals(other.DIR, epsilon);
}

#pragma endregion

#pragma region line operations

Point2D Ray2D::ProjectOnto(Point2D const& point) const {
  double t = (point - ORIGIN).Dot(DIR);
  if (compare(t, 0) <= 0) {
    return ORIGIN;
  }
  return ORIGIN + t * DIR;
}

double Ray2D::DistanceTo(Point2D const& point) const { return (point - ProjectOnto(point)).Length(); }

#pragma endregion

#pragma region Operator Overloading

bool operator==(Ray2D const& lhs, Ray2D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Ray2D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Ray2D::Contains(Point2D const& point) const { return ToLine().Contains(point) && IsAhead(point); }

bool Ray2D::Intersects(Line2D const& line) const { return Intersection(line).has_value(); }

bool Ray2D::Intersects(Ray2D const& other) const { return Intersection(other).has_value(); }

bool Ray2D::Intersects(LineSegment2D const& segment) const { return segment.Intersects(*this); }

Ray2D::ReturnSet Ray2D::Intersection(Line2D const& line) const {
  double sc, tc;
  auto Pc = ToLine().Intersection(line, sc, tc);

  // respecting constraints: sc should be positive
  if (!Pc.has_value() || !is_greater_or_equal(sc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

Ray2D::ReturnSet Ray2D::Intersection(Ray2D const& other) const {
  double sc, tc;
  auto Pc = ToLine().Intersection(other.ToLine(), sc, tc);

  // respecting constraints: sc and tc should be positive
  if (!Pc.has_value() || !is_greater_or_equal(sc, 0) || !is_greater_or_equal(tc, 0)) {
    return std::nullopt;
  }

  return Pc;
}

Ray2D::ReturnSet Ray2D::Intersection(LineSegment2D const& segment) const { return segment.Intersection(*this); }

#pragma endregion

#pragma region Formatting

std::string Ray2D::ToWkt() const {
  return std::format("RAY ({} {}, {} {})", round(ORIGIN.x()), round(ORIGIN.y()), round(DIR.x()), round(DIR.y()));
}

Ray2D Ray2D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "RAY") {
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

    return Make(Point2D{nums_p1[0], nums_p1[1]}, Vector2D{nums_p2[0], nums_p2[1]});

  } catch (...) {
    GEOMPP_LOG(ERROR) << "bad format of str " << wkt;
  }

  throw std::runtime_error("failed to parse WKT");
}

void Ray2D::ToFile(std::string const& path) const {
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

Ray2D Ray2D::FromFile(std::string const& path) {
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
