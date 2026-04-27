#include "geometry_collection3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"
#include "wkt_parser.hpp"

#include "geompp_log.hpp"

#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace geompp {

bool GeometryCollection3D::AlmostEquals(GeometryCollection3D const& other, double epsilon) const {
  if (GEOMETRIES.size() != other.GEOMETRIES.size()) {
    return false;
  }

  for (std::size_t i = 0; i < GEOMETRIES.size(); ++i) {
    auto const& entry1 = GEOMETRIES[i];
    auto const& entry2 = other.GEOMETRIES[i];
    if (entry1.Type != entry2.Type) {
      return false;
    }

    switch (entry1.Type) {
      case GeometryType::Point: {
        if (!POINTS[entry1.Index].AlmostEquals(other.POINTS[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::Line: {
        if (!LINES[entry1.Index].AlmostEquals(other.LINES[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::LineSegment: {
        if (!LINE_SEGMENTS[entry1.Index].AlmostEquals(other.LINE_SEGMENTS[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::Ray: {
        if (!RAYS[entry1.Index].AlmostEquals(other.RAYS[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::Polyline: {
        if (!POLYLINES[entry1.Index].AlmostEquals(other.POLYLINES[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::Triangle: {
        if (!TRIANGLES[entry1.Index].AlmostEquals(other.TRIANGLES[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::Polygon: {
        if (!POLYGONS[entry1.Index].AlmostEquals(other.POLYGONS[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      case GeometryType::GeometryCollection: {
        if (!GEOMETRY_COLLECTIONS[entry1.Index].AlmostEquals(other.GEOMETRY_COLLECTIONS[entry2.Index], epsilon)) {
          return false;
        }
        break;
      }
      default:
        throw std::runtime_error("invalid geometry type");
    }
  }
  return true;
}

#pragma region Operator Overloading

GeometryCollection3D& GeometryCollection3D::operator=(GeometryCollection3D const& other) {
  if (this != &other) {
    GEOMETRIES = other.GEOMETRIES;
    POINTS = other.POINTS;
    LINES = other.LINES;
    LINE_SEGMENTS = other.LINE_SEGMENTS;
    RAYS = other.RAYS;
    POLYLINES = other.POLYLINES;
    TRIANGLES = other.TRIANGLES;
    POLYGONS = other.POLYGONS;
    GEOMETRY_COLLECTIONS = other.GEOMETRY_COLLECTIONS;
  }
  return *this;
}

bool operator==(GeometryCollection3D const& lhs, GeometryCollection3D const& rhs) { return lhs.AlmostEquals(rhs); }

std::ostream& operator<<(std::ostream& os, GeometryCollection3D const& g) {
  os << g.ToWkt();
  return os;
}

#pragma endregion

#pragma region Getters And Setters

void GeometryCollection3D::Add(Point3D const& point) {
  GEOMETRIES.push_back({GeometryType::Point, POINTS.size()});
  POINTS.push_back(point);
}

void GeometryCollection3D::Add(Line3D const& line) {
  GEOMETRIES.push_back({GeometryType::Line, LINES.size()});
  LINES.push_back(line);
}

void GeometryCollection3D::Add(LineSegment3D const& line_segment) {
  GEOMETRIES.push_back({GeometryType::LineSegment, LINE_SEGMENTS.size()});
  LINE_SEGMENTS.push_back(line_segment);
}

void GeometryCollection3D::Add(Ray3D const& ray) {
  GEOMETRIES.push_back({GeometryType::Ray, RAYS.size()});
  RAYS.push_back(ray);
}

void GeometryCollection3D::Add(Polyline3D const& polyline) {
  GEOMETRIES.push_back({GeometryType::Polyline, POLYLINES.size()});
  POLYLINES.push_back(polyline);
}

void GeometryCollection3D::Add(Triangle3D const& triangle) {
  GEOMETRIES.push_back({GeometryType::Triangle, TRIANGLES.size()});
  TRIANGLES.push_back(triangle);
}

void GeometryCollection3D::Add(Polygon3D const& polygon) {
  GEOMETRIES.push_back({GeometryType::Polygon, POLYGONS.size()});
  POLYGONS.push_back(polygon);
}

void GeometryCollection3D::Add(GeometryCollection3D const& polygon) {
  GEOMETRIES.push_back({GeometryType::GeometryCollection, GEOMETRY_COLLECTIONS.size()});
  GEOMETRY_COLLECTIONS.push_back(polygon);
}

Shape3D GeometryCollection3D::Get(std::size_t index) const {
  if (index >= GEOMETRIES.size()) {
    throw std::out_of_range("index out of range");
  }

  auto const& entry = GEOMETRIES[index];
  switch (entry.Type) {
    case GeometryType::Point:
      return POINTS[entry.Index];
    case GeometryType::Line:
      return LINES[entry.Index];
    case GeometryType::LineSegment:
      return LINE_SEGMENTS[entry.Index];
    case GeometryType::Ray:
      return RAYS[entry.Index];
    case GeometryType::Polyline:
      return POLYLINES[entry.Index];
    case GeometryType::Triangle:
      return TRIANGLES[entry.Index];
    case GeometryType::Polygon:
      return POLYGONS[entry.Index];
    case GeometryType::GeometryCollection:
      return GEOMETRY_COLLECTIONS[entry.Index];
    default:
      throw std::runtime_error("invalid geometry type");
  }
}

#pragma endregion

#pragma region Formatting

std::string GeometryCollection3D::ToWkt() const {
  std::ostringstream buf;
  buf << "GEOMETRYCOLLECTION ";
  int num_geoms = GEOMETRIES.size();
  if (!num_geoms) {
    buf << "EMPTY";
    return buf.str();
  }

  buf << "(";
  for (int i = 0; i < num_geoms; ++i) {
    auto const& entry = GEOMETRIES[i];
    switch (entry.Type) {
      case GeometryType::Point: {
        buf << POINTS[entry.Index].ToWkt();
        break;
      }
      case GeometryType::Line: {
        buf << LINES[entry.Index].ToWkt();
        break;
      }
      case GeometryType::LineSegment: {
        buf << LINE_SEGMENTS[entry.Index].ToWkt();
        break;
      }
      case GeometryType::Ray: {
        buf << RAYS[entry.Index].ToWkt();
        break;
      }
      case GeometryType::Polyline: {
        buf << POLYLINES[entry.Index].ToWkt();
        break;
      }
      case GeometryType::Triangle: {
        buf << TRIANGLES[entry.Index].ToWkt();
        break;
      }
      case GeometryType::Polygon: {
        buf << POLYGONS[entry.Index].ToWkt();
        break;
      }
      case GeometryType::GeometryCollection: {
        buf << GEOMETRY_COLLECTIONS[entry.Index].ToWkt();
        break;
      }
      default:
        throw std::runtime_error("invalid geometry type");
    }

    if (i < num_geoms - 1) {
      buf << ", ";
    }
  }
  buf << ")";

  return buf.str();
}

GeometryCollection3D GeometryCollection3D::FromWkt(std::string const& wkt) {
  try {
    GeometryCollection3D geoms;

    std::size_t end_gtype, end_pn;

    end_gtype = wkt.find('(');
    if (end_gtype == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string g_type = geompp::to_upper(geompp::trim(wkt.substr(0, end_gtype)));
    if (g_type != "GEOMETRYCOLLECTION") {
      throw std::runtime_error("geometry name");
    }

    end_pn = wkt.substr(end_gtype + 1).rfind(')');
    if (end_pn == std::string::npos) {
      throw std::runtime_error("brakets");
    }

    std::string mid_part = wkt.substr(end_gtype + 1, end_pn);

    for (std::string const& wkt_str : geompp::tokenize_string(mid_part, ',')) {
      std::string wkt_trimmed = geompp::trim(wkt_str);

      auto shape = WktParser::FromWkt(wkt_trimmed);

      if (!shape.has_value()) {
        throw std::runtime_error("bad format of str " + wkt_trimmed);
      }

      std::visit(
          [&geoms](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Point3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, Line3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, LineSegment3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, Ray3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, Polyline3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, Triangle3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, Polygon3D>) {
              geoms.Add(arg);
            } else if constexpr (std::is_same_v<T, GeometryCollection3D>) {
              geoms.Add(arg);
            } else {
              throw std::runtime_error("unsupported geometry type " + std::string(typeid(T).name()));
            }
          },
          shape.value());
    }

    return geoms;

  } catch (std::exception const& e) {
    GEOMPP_LOG(ERROR) << e.what();
  }

  throw std::runtime_error("failed to parse WKT");
}

void GeometryCollection3D::ToFile(std::string const& path) const {
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

GeometryCollection3D GeometryCollection3D::FromFile(std::string const& path) {
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
