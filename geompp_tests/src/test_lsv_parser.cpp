#include "lsv_parser.hpp"

#include "constants.hpp"
#include "point2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <variant>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(LsvParser, FromFile) {
  std::string geom_file_path = (test_res_path / "lsv" / "initial_geometries.lsv").string();

  // expected values filling: creating the testw file
  std::vector<g::LVSParser::ReturnSet> geometry_list;
  ASSERT_NO_THROW({
    geometry_list.push_back(g::Point2D::FromWkt("POINT(5 2)"));
    geometry_list.push_back(g::LineSegment2D::FromWkt("LINESTRING(0 0, 1 1)"));

    geometry_list.push_back(g::Point3D::FromWkt("POINT(7 20 3)"));
    geometry_list.push_back(g::LineSegment3D::FromWkt("LINESTRING(-1 -2 0, 4 3 1)"));

    // write them in the file
    if (fs::exists(geom_file_path)) {
      if (!fs::remove(geom_file_path)) {
        throw std::runtime_error("failed to create file " + geom_file_path);
      }
    }

    // Open the file in write mode (truncates existing content)
    std::ofstream outfile(geom_file_path);

    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open test file " + geom_file_path);
    }

    // Write the text to the file
    for (auto geom : geometry_list) {
      ASSERT_TRUE(geom.has_value());

      auto geom_val = geom.value();
      if (std::holds_alternative<g::Point2D>(geom_val)) {
        outfile << std::get<g::Point2D>(geom_val).ToWkt() << "\n";

      } else if (std::holds_alternative<g::LineSegment2D>(geom_val)) {
        outfile << std::get<g::LineSegment2D>(geom_val).ToWkt() << "\n";

      } else if (std::holds_alternative<g::Point3D>(geom_val)) {
        outfile << std::get<g::Point3D>(geom_val).ToWkt() << "\n";

      } else if (std::holds_alternative<g::LineSegment3D>(geom_val)) {
        outfile << std::get<g::LineSegment3D>(geom_val).ToWkt() << "\n";
      }
    }

    outfile.close();
  });

  ASSERT_TRUE(fs::exists(geom_file_path));

  // ASSERT_NO_THROW({
  auto geom_parser = g::LVSParser::Open(geom_file_path);  // can throw
  while (geom_parser.HasNext()) {
    auto geom = geom_parser.Next();

    // ASSERT_TRUE(geom.has_value());
    if (geom.has_value()) {
      auto geom_val = geom.value();
      if (std::holds_alternative<g::Point2D>(geom_val)) {
        std::cout << "rendering " << std::get<g::Point2D>(geom_val).ToWkt() << std::endl;

      } else if (std::holds_alternative<g::LineSegment2D>(geom_val)) {
        std::cout << "rendering " << std::get<g::LineSegment2D>(geom_val).ToWkt() << std::endl;

      } else if (std::holds_alternative<g::Point3D>(geom_val)) {
        std::cout << "rendering " << std::get<g::Point3D>(geom_val).ToWkt() << std::endl;

      } else if (std::holds_alternative<g::LineSegment3D>(geom_val)) {
        std::cout << "rendering " << std::get<g::LineSegment3D>(geom_val).ToWkt() << std::endl;
      }
    }
  }
  //});
}

}  // namespace geompp_tests
