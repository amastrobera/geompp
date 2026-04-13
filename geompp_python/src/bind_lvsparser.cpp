#include "bind_helpers.hpp"

void bind_lvsparser(py::module_& m) {
    py::class_<geompp::LVSParser>(m, "LVSParser",
        "Parser for .lsv files containing WKT geometry entries.\n\n"
        "Usage::\n\n"
        "    parser = LVSParser.open('file.lsv')\n"
        "    while parser.has_next():\n"
        "        item = parser.next()  # returns a geometry object or None\n"
        "        if item is not None:\n"
        "            print(LVSParser.to_wkt(item))\n\n"
        "Also supports context manager syntax.")
        .def_static("open", [](const std::string& path) {
            return geompp::LVSParser::Open(path);
        }, "path"_a)
        .def("has_next",      &geompp::LVSParser::HasNext)
        .def("next", [](geompp::LVSParser& self) -> py::object {
            return opt_variant_to_py(self.Next());
        })
        .def("get_file_path", &geompp::LVSParser::GetFilePath)
        // to_wkt accepts any geometry object (or None) and calls its to_wkt()
        .def_static("to_wkt", [](const py::object& item) -> std::string {
            if (item.is_none()) return "GEOMETRYCOLLECTION EMPTY";
            return item.attr("to_wkt")().cast<std::string>();
        }, "item"_a,
           "Return the WKT string for a geometry object returned by next(), or 'GEOMETRYCOLLECTION EMPTY' for None.")
        .def("__enter__", [](geompp::LVSParser& self) -> geompp::LVSParser& { return self; },
             py::return_value_policy::reference)
        .def("__exit__", [](geompp::LVSParser&, py::object, py::object, py::object) {});
}
