"""
precision binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPrecision:
    def test_constants_exist(self):
        assert geompp.DP_THREE == 3
        assert geompp.DP_SIX   == 6
        assert geompp.DP_NINE  == 9

    def test_set_and_get(self):
        geompp.set_decimal_precision(geompp.DP_THREE)
        assert geompp.get_decimal_precision() == 3
        geompp.set_decimal_precision(geompp.DP_SIX)
        assert geompp.get_decimal_precision() == 6
