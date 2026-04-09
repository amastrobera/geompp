#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "constants.hpp"
#pragma managed(pop)

namespace GeomPP {

public ref class Precision {
public:
    static property int DecimalPrecision {
        int  get();
        void set(int value);
    }

    static property double Epsilon {
        double get();
    }

    static const int DP_THREE = 3;
    static const int DP_SIX   = 6;
    static const int DP_NINE  = 9;
};

}  // namespace GeomPP
