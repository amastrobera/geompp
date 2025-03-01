#include "line3d.hpp"

//#include "line_segment3d.hpp"
//#include "ray3d.hpp"
#include "utils.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Line3D Line3D::Make(Point3D const& p0, Point3D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision), decimal_precision));
  }
  return {p0, p1};
}

Line3D Line3D::Make(Point3D const& p0, Vector3D const& dir, int decimal_precision) {
  if (round_to(dir.Length(), decimal_precision) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", decimal_precision));
  }
  return {p0, dir};
}

Line3D::Line3D(Point3D const& p0, Point3D const& p1) : P0(p0), P1(p1), DIR((p1 - p0).Normalize()) {}

Line3D::Line3D(Point3D const& orig, Vector3D const& dir) : P0(orig), DIR(dir.Normalize()), P1(orig + dir) {}

Line3D& Line3D::operator=(Line3D const& other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

bool Line3D::AlmostEquals(Line3D const& other, int decimal_precision) const {
  return P0.AlmostEquals(other.P0, decimal_precision) && P1.AlmostEquals(other.P1, decimal_precision);
}

double Line3D::DistanceTo(Point3D const& point, int decimal_precision) const {
  throw std::runtime_error("not implemented");
  // return round_to(std::abs(DIR.Cross(point - P0)), decimal_precision);
}

Point3D Line3D::ProjectOnto(Point3D const& point, int decimal_precision) const {
  return P0 + (point - P0).Dot(DIR) * DIR;
}

double Line3D::Location(Point3D const& point, int decimal_precision) const {
  return sign((point - P0).Dot(P1 - P0), decimal_precision) * (point - P0).Length();
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(Line3D const& lhs, Line3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, Line3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Geometrical Operations

bool Line3D::Contains(Point3D const& point, int decimal_precision) const {
  throw std::runtime_error("not implemented");
  // return round_to((point - P0).Cross(DIR), decimal_precision) == 0.0;
}

bool Line3D::Intersects(Line3D const& other, int decimal_precision) const {
  throw std::runtime_error("not implemented");

  // return round_to(DIR.Cross(other.DIR), decimal_precision) != 0.0;
}

// bool Line3D::Intersects(Ray3D const& ray, int decimal_precision) const {
//  return ray.Intersects(*this, decimal_precision);
//}
//
// bool Line3D::Intersects(LineSegment3D const& segment, int decimal_precision) const {
//  return segment.Intersects(*this, decimal_precision);
//}

Line3D::ReturnSet Line3D::Intersection(Line3D const& other, int decimal_precision) const {
  throw std::runtime_error("not implemented");
  // auto u = DIR;
  // auto v = other.DIR;
  // auto vp = v.Perp();
  // auto w = (P0 - other.P0);

  // if (round_to(u * vp, decimal_precision) == 0.0) {
  //  return std::nullopt;
  //}
  // double t = (-w * vp) / (u * vp);

  // return P0 + t * u;
}

// Line3D::ReturnSet Line3D::Intersection(Ray3D const& ray, int decimal_precision) const {
//  return ray.Intersection(*this, decimal_precision);
//}
//
// Line3D::ReturnSet Line3D::Intersection(LineSegment3D const& segment, int decimal_precision) const {
//  return segment.Intersection(*this, decimal_precision);
//}

#pragma endregion

#pragma region Formatting

std::string Line3D::ToWkt(int decimal_precision) const {
  return std::format("LINE ({} {} {}, {} {} {})", round_to(P0.x(), decimal_precision),
                     round_to(P0.y(), decimal_precision), round_to(P0.z(), decimal_precision),
                     round_to(P1.x(), decimal_precision), round_to(P1.y(), decimal_precision),
                     round_to(P1.z(), decimal_precision));
}

Line3D Line3D::FromWkt(std::string const& wkt) {
  try {
    std::size_t end_gtype, end_p1, end_p2;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "LINE") {
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

    return Make(Point3D{nums_p1[0], nums_p1[1], nums_p1[2]}, Point3D{nums_p2[0], nums_p2[1], nums_p2[2]});

  } catch (...) {
    std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  }

  throw std::runtime_error("failed to parse WKT");
}

void Line3D::ToFile(std::string const& path, int decimal_precision) const {
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

Line3D Line3D::FromFile(std::string const& path) {
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
