#include "bind_helpers.hpp"

void bind_wktparser(py::module_& m) {
    py::class_<geompp::WktParser>(m, "WktParser",
        "Parser for .lsv files containing WKT geometry entries.\n\n"
        "Usage::\n\n"
        "    parser = WktParser.open('file.lsv')\n"
        "    while parser.has_next():\n"
        "        item = parser.next()  # returns a geometry object or None\n\n"
        "Also supports context manager syntax.")
        .def_static("open", [](const std::string& path) {
            return geompp::WktParser::Open(path);
        }, "path"_a)
        .def("has_next",      &geompp::WktParser::HasNext)
        .def("next", [](geompp::WktParser& self) -> py::object {
            return opt_variant_to_py(self.Next());
        })
        .def("get_file_path", &geompp::WktParser::GetFilePath)
        .def_static("from_wkt", [](const std::string& wkt) -> py::object {
            return opt_variant_to_py(geompp::WktParser::FromWkt(wkt));
        }, "wkt"_a)
        .def_static("get", [](const std::string& wkt) -> py::object {
            return opt_variant_to_py(geompp::WktParser::FromWkt(wkt));
        }, "wkt"_a)
        .def_static("to_wkt", [](const py::object& item) -> std::string {
            geompp::WktParser::ReturnSet rs;
            if (item.is_none()) {
                rs = std::nullopt;
            } else if (py::isinstance<geompp::Vector2D>(item)) {
                rs = item.cast<geompp::Vector2D>();
            } else if (py::isinstance<geompp::Point2D>(item)) {
                rs = item.cast<geompp::Point2D>();
            } else if (py::isinstance<geompp::Line2D>(item)) {
                rs = item.cast<geompp::Line2D>();
            } else if (py::isinstance<geompp::Ray2D>(item)) {
                rs = item.cast<geompp::Ray2D>();
            } else if (py::isinstance<geompp::LineSegment2D>(item)) {
                rs = item.cast<geompp::LineSegment2D>();
            } else if (py::isinstance<geompp::Polyline2D>(item)) {
                rs = item.cast<geompp::Polyline2D>();
            } else if (py::isinstance<geompp::Triangle2D>(item)) {
                rs = item.cast<geompp::Triangle2D>();
            } else if (py::isinstance<geompp::Polygon2D>(item)) {
                rs = item.cast<geompp::Polygon2D>();
            } else if (py::isinstance<geompp::GeometryCollection2D>(item)) {
                rs = item.cast<geompp::GeometryCollection2D>();
            } else if (py::isinstance<geompp::Vector3D>(item)) {
                rs = item.cast<geompp::Vector3D>();
            } else if (py::isinstance<geompp::Point3D>(item)) {
                rs = item.cast<geompp::Point3D>();
            } else if (py::isinstance<geompp::Line3D>(item)) {
                rs = item.cast<geompp::Line3D>();
            } else if (py::isinstance<geompp::Ray3D>(item)) {
                rs = item.cast<geompp::Ray3D>();
            } else if (py::isinstance<geompp::LineSegment3D>(item)) {
                rs = item.cast<geompp::LineSegment3D>();
            } else if (py::isinstance<geompp::Polyline3D>(item)) {
                rs = item.cast<geompp::Polyline3D>();
            } else if (py::isinstance<geompp::Triangle3D>(item)) {
                rs = item.cast<geompp::Triangle3D>();
            } else if (py::isinstance<geompp::Polygon3D>(item)) {
                rs = item.cast<geompp::Polygon3D>();
            } else if (py::isinstance<geompp::GeometryCollection3D>(item)) {
                rs = item.cast<geompp::GeometryCollection3D>();
            } else {
                throw py::type_error("unsupported geometry type for WktParser.to_wkt");
            }
            return geompp::WktParser::ToWkt(rs);
        }, "shape"_a)
        .def("__enter__", [](geompp::WktParser& self) -> geompp::WktParser& { return self; },
             py::return_value_policy::reference)
        .def("__exit__", [](geompp::WktParser&, py::object, py::object, py::object) {});
}
