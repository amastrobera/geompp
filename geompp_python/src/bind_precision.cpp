#include "bind_helpers.hpp"

void bind_precision(py::module_& m) {
    m.attr("DP_THREE") = geompp::DP_THREE;
    m.attr("DP_SIX")   = geompp::DP_SIX;
    m.attr("DP_NINE")  = geompp::DP_NINE;

    m.def("set_decimal_precision",
          [](int dp) { geompp::DECIMAL_PRECISION = dp; }, "dp"_a,
          "Set thread-local decimal precision (DP_THREE=3, DP_SIX=6, DP_NINE=9).");

    m.def("get_decimal_precision",
          []() { return geompp::DECIMAL_PRECISION; },
          "Get current thread-local decimal precision.");
}
