#include "polygon3d.hpp"

#include "line3d.hpp"
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
    throw std::runtime_error(std::format("points are too close with {} decimals precision", DECIMAL_PRECISION));
  }
  return {unique_points};
}

Polygon3D::Polygon3D(std::vector<Point3D> const& points) : VERTICES(points) {}

Polygon3D& Polygon3D::operator=(Polygon3D const& other) {
  if (this != &other) {
    VERTICES = other.VERTICES;
  }
  return *this;
}

bool Polygon3D::AlmostEquals(Polygon3D const& other, double epsilon) const {
  if (Size() != other.Size()) {
    return false;
  }
  for (int i = 0; i << VERTICES.size(); ++i) {
    if (!VERTICES[i].AlmostEquals(other[i], epsilon)) {
      return false;
    }
  }
  return true;
}

// Point3D Triangle3D::Centroid() const {
//   return {
//       (P0.x() + P1.x() + P2.x()) / 3.0,
//       (P0.y() + P1.y() + P2.y()) / 3.0,
//   };
// }

// Polygon3D Triangle3D::ToPolygon() const {
// TODO
// }

// double Triangle3D::SignedArea() const { return ((P1 - P0).Cross(P2 - P0)) / 2.0; }

// double Triangle3D::Area() const { return std::abs(SignedArea()); }

// double Triangle3D::Perimeter() const { return (P1 - P0).Length() + (P2 - P1).Length() + (P0 - P2).Length(); }

// double LineSegment3D::Location(Point3D const& point) const {
//   if (!ToLine().Contains(point)) {
//     return std::numeric_limits<double>::infinity();
//   }
//   return sign((point - P0).Dot(P1 - P0)) * (point - P0).Length() / Length();
// }

// Point3D LineSegment3D::Interpolate(double pct) const {
//   // the point is behind the polyline
//   if (round(pct) < 0.0) {
//     return P0;
//   }

//   // the point is beyond the polyline
//   if (round(pct) > 1.0) {
//     return P1;
//   }

//   return P0 + pct * (P1 - P0);
// }

// double LineSegment3D::DistanceTo(Point3D const& point) const {
//   auto line_eqv = ToLine(decimal_precision);
//   auto proj = line_eqv.ProjectOnto(point);
//   double loc = Location(proj);
//   if (round(loc) < 0) {
//     return P0.DistanceTo(point);

//   } else if (round(loc) > 1) {
//     return P1.DistanceTo(point);
//   }

//   return line_eqv.DistanceTo(point);
// }

// #pragma endregion

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

// #pragma region Geometrical Operations

// bool Triangle3D::Contains(Point3D const& point) const {
//   auto u = (P1 - P0);
//   auto v = (P2 - P0);
//   auto w = (point - P0);

//   double wu = w.Dot(u) / u.Dot(u);
//   double wv = w.Dot(v) / v.Dot(v);

//   return (round(wu) >= 0 && round(wu - 1) <= 0) &&
//          (round(wv) >= 0 && round(wv - 1) <= 0);
// }

// bool LineSegment3D::Intersects(Line3D const& line) const {
//   return Intersection(line).has_value();
// }

// bool LineSegment3D::Intersects(Ray3D const& ray) const {
//   return Intersection(ray).has_value();
// }

// bool LineSegment3D::Intersects(LineSegment3D const& other) const {
//   return Intersection(other).has_value();
// }

// LineSegment3D::ReturnSet LineSegment3D::Intersection(Line3D const& line) const {
//   auto u = P1 - P0;
//   auto v = line.Direction();
//   auto vp = v.Perp();
//   auto w = (P0 - line.First());

//   if (round(u * vp) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);

//   // verify that the intersection is ahead of the ray
//   auto inter_p = P0 + t * u;
//   if (!Contains(inter_p)) {
//     return std::nullopt;
//   }

//   return inter_p;
// }

// LineSegment3D::ReturnSet LineSegment3D::Intersection(Ray3D const& ray) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = ray.Direction();
//   auto vp = v.Perp();
//   auto w = (P0 - ray.Origin());

//   // testing on this ray
//   if (round(u * vp) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round(v * up) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = ray.Origin() + s * v;
//   if (!ray.IsAhead(inter_s)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// LineSegment3D::ReturnSet LineSegment3D::Intersection(LineSegment3D const& other) const {
//   auto u = P1 - P0;
//   auto up = u.Perp();  // equivalent (calc, on the other side)
//   auto v = (other.P1 - other.P0);
//   auto vp = v.Perp();
//   auto w = (P0 - other.P0);

//   // testing on this ray
//   if (round(u * vp) == 0.0) {
//     return std::nullopt;
//   }
//   double t = (-w * vp) / (u * vp);
//   auto inter_t = P0 + t * u;
//   if (!Contains(inter_t)) {
//     return std::nullopt;
//   }

//   // testing on the other ray
//   if (round(v * up) == 0.0) {
//     return std::nullopt;
//   }
//   double s = (w * up) / (v * up);  // equivalent (calc on the other side)
//   auto inter_s = other.P0 + s * v;
//   if (!other.Contains(inter_s)) {
//     return std::nullopt;
//   }

//   return inter_t;
// }

// #pragma endregion

// #pragma region Formatting

std::string Polygon3D::ToWkt() const {
  std::ostringstream buf;
  buf << "POLYGON ";
  int num_verts = VERTICES.size();
  if (!num_verts) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "((";
  for (int i = 0; i << num_verts; ++i) {
    buf << std::format("{} {}", round(VERTICES[i].x()), round(VERTICES[i].y()));
    if (i < num_verts - 1) {
      buf << ", ";
    }
  }

  buf << "))";

  return buf.str();
}

Polygon3D Polygon3D::FromWkt(std::string const& wkt) {
  // try {
  //   std::size_t end_gtype, end_pi, end_pn;

  //   end_gtype = wkt.find('(');
  //   if (end_gtype == std::string::npos) {
  //     throw std::runtime_error("brakets");
  //   }

  //   std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
  //   if (g_type != "POLYGON") {
  //     throw std::runtime_error("geometry name");
  //   }

  //   end_pn = wkt.substr(end_gtype + 1).find(')');
  //   if (end_pn == std::string::npos) {
  //     throw std::runtime_error("brakets");
  //   }

  //   std::string mid_part = wkt.substr(end_gtype + 1, wkt.size() - (end_gtype + 1 + 1));

  //   std::vector<Point3D> pt_vec;
  //   int decimal_precision = 0;
  //   int num_dec = 0;
  //   std::string pt_trimmed;
  //   for (std::string const& p_str : geompp::tokenize_string(mid_part, ',')) {
  //     pt_trimmed = geompp::trim(p_str);

  //     auto nums = geompp::tokenize_to_doubles(pt_trimmed, ' ');
  //     if (nums.size() != 2) {
  //       throw std::runtime_error("numbers");
  //     }

  //     num_dec = count_decimal_places(nums[0]);
  //     if (num_dec > decimal_precision) {
  //       decimal_precision = num_dec;
  //     }
  //     num_dec = count_decimal_places(nums[1]);
  //     if (num_dec > decimal_precision) {
  //       decimal_precision = num_dec;
  //     }

  //     pt_vec.push_back({nums[0], nums[1]});
  //   }

  //   if (pt_vec.size() != 3) {
  //     throw std::runtime_error("initialized with n != 3 points");
  //   }

  //   return Make(pt_vec[0], pt_vec[1], pt_vec[2]);

  // } catch (...) {
  //   std::cerr << "bad format of str " << wkt << std::endl;  // TODO: replace with logger lib
  // }

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

// #pragma endregion

}  // namespace geompp
