using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class PrecisionTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Precision ─────────────────────────────────────────────────────────────────
    Console.WriteLine("Precision");

    Test("DefaultPrecision_IsThree", () => Eq(3, Precision.DecimalPrecision, 0));

    Test("SetPrecision_RoundTrips", () => {
      int orig = Precision.DecimalPrecision;
      Precision.DecimalPrecision = Precision.DP_SIX;
      Eq(6, Precision.DecimalPrecision, 0);
      Precision.DecimalPrecision = orig;
    });
  }
}

}  // namespace GeomPPTests
