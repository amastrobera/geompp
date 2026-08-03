import pytest
import geompp


@pytest.fixture(autouse=True)
def _reset_decimal_precision():
    # DECIMAL_PRECISION is a thread-local global (see CLAUDE.md); a handful of tests call
    # set_decimal_precision() without resetting it afterward. Splitting the old single-file suite
    # into many files (collected alphabetically, not in the old file's sequential order) exposed the
    # resulting cross-test leakage. Reset before AND after every test, mirroring the C++ suite's own
    # SetUp()/TearDown() convention (g::DECIMAL_PRECISION = g::DP_THREE).
    geompp.set_decimal_precision(geompp.DP_THREE)
    yield
    geompp.set_decimal_precision(geompp.DP_THREE)
