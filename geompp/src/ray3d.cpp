#include "ray3d.hpp"

#include "constants.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "utils.hpp"

#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger 3D
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Ray3D Ray3D::Make(Point3D const& p0, Vector3D const& dir) {
  if (round(dir.Length()) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", DECIMAL_PRECISION));
  }
  return {p0, dir};
}

Ray3D::Ray3D(Point3D const& orig, Vector3D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

Ray3D& Ray3D::operator=(Ray3D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

bool Ray3D::IsAhead(Point3D const& point) const {
  return round(DIR.Dot(point - ORIGIN)) >= 0.0;
}

bool Ray3D::IsBehind(Point3D const& point) const {
  return round(DIR.Dot(point - ORIGIN)) < 0.0;
}

Line3D Ray3D::ToLine() const { return Line3D::Make(ORIGIN, DIR); }

double Ray3D::DistanceTo(Point3D const& point) const {
  return IsAhead(point) ? ToLine().DistanceTo(point)
                                           : ORIGIN.DistanceTo(point);
}

bool Ray3D::AlmostEquals(Ray3D const& other) const {
  return ORIGIN.AlmostEquals(other.ORIGIN) && DIR.AlmostEquals(other.DIR);
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Ray3D const& lhs, Ray3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Ray3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Ray3D::Contains(Point3D const& point) const {
  return ToLine().Contains(point) && IsAhead(point);
}

bool Ray3D::Intersects(Line3D const& line) const {
  return Intersection(line).has_value();
}

bool Ray3D::Intersects(Ray3D const& other) const {
  return Intersection(other).has_value();
}

bool Ray3D::Intersects(LineSegment3D const& segment) const {
  return segment.Intersects(*this);
}

Ray3D::ReturnSet Ray3D::Intersection(Line3D const& line) const {
  auto u = DIR;
  auto v = line.Direction();
  auto vp = v.Perp();
  auto w = (ORIGIN - line.First());

  if (round(u * vp) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = ORIGIN + t * u;
  if (!IsAhead(inter_p)) {
    return std::nullopt;
  }

  return inter_p;
}

Ray3D::ReturnSet Ray3D::Intersection(Ray3D const& other) const {
  auto u = DIR;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = other.DIR;
  auto vp = v.Perp();
  auto w = (ORIGIN - other.ORIGIN);

  // testing on this ray
  if (round(u * vp) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = ORIGIN + t * u;
  if (!IsAhead(inter_t)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round(v * up) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.ORIGIN + s * v;
  if (!other.IsAhead(inter_s)) {
    return std::nullopt;
  }

  return inter_t;
}

Ray3D::ReturnSet Ray3D::Intersection(LineSegment3D const& segment) const {
  return segment.Intersection(*this);
}

#pragma endregion

#pragma region Formatting

std::string Ray3D::ToWkt() const {
  return std::format("RAY ({} {} {}, {} {} {})", round(ORIGIN.x()),
                     round(ORIGIN.y()), round(ORIGIN.z()),
                     round(DIR.x()), round(DIR.y()),
                     round(DIR.z()));
}

Ray3D Ray3D::FromWkt(std::string const& wkt) {
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

    end_p1 = wkt.substr(end_gtype + 1).find(',');
    if (end_p1 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p1 = wkt.substr(end_gtype + 1, end_p1);

    auto nums_p1 = geompp::tokenize_to_doubles(s_nums_p1);
    if (nums_p1.size() != 3) {
      throw std::runtime_error("numbers p1");
    }

    end_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1).find(')');
    if (end_p2 == std::string::npos) {
      throw std::runtime_error("brakets");
    }
    std::string s_nums_p2 = wkt.substr(end_gtype + 1 + end_p1 + 1, end_p2);

    auto nums_p2 = geompp::tokenize_to_doubles(s_nums_p2);
    if (nums_p2.size() != 3) {
      throw std::runtime_error("numbers p2");
    }

    return Make(Point3D{nums_p1[0], nums_p1[1], nums_p1[2]}, Vector3D{nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger 3D
  }

  throw std::runtime_error("failed to parse WKT");
}

void Ray3D::ToFile(std::string const& path) const {
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
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger 3D
  }
}

Ray3D Ray3D::FromFile(std::string const& path) {
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
    std::cerr << "bad path " << path << std::endl;  // TODO: replace with logger 3D
  }

  throw std::runtime_error("failed to parse WKT");
}

#pragma endregion

}  // namespace geompp
