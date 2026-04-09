#include "Precision.hpp"

namespace GeomPP {

int Precision::DecimalPrecision::get() {
    return geompp::DECIMAL_PRECISION;
}

void Precision::DecimalPrecision::set(int value) {
    geompp::DECIMAL_PRECISION = value;
}

double Precision::Epsilon::get() {
    return (double)geompp::DOUBLE_EPSILON;
}

}  // namespace GeomPP
