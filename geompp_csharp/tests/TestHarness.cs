namespace GeomPPTests {

// Tiny homegrown test harness (not xUnit/NUnit) -- see CLAUDE.md: GeomPPTests.csproj is an Exe-type
// console harness, run via `dotnet run`, not `dotnet test`. Each topic file (Polygon2DTests,
// Point2DTests, ...) gets a `TestHarness` instance and calls its Test/Eq/IsTrue/etc. methods.
public class TestHarness {
  public int Passed { get; private set; } = 0;
  public int Failed { get; private set; } = 0;

  public void Test(string name, Action body) {
    try {
      body();
      Console.WriteLine($"  PASS  {name}");
      Passed++;
    } catch (Exception e) {
      Console.WriteLine($"  FAIL  {name}");
      Console.WriteLine($"        {e.Message}");
      Failed++;
    }
  }

  public void Eq(double expected, double actual, int decimals = 3) {
    double tol = 0.5 * Math.Pow(10, -decimals);
    if (Math.Abs(expected - actual) >= tol)
      throw new Exception($"expected {expected}, got {actual} (tolerance {tol})");
  }

  public void IsTrue(bool value, string msg = "expected true") {
    if (!value) throw new Exception(msg);
  }

  public void IsFalse(bool value, string msg = "expected false") {
    if (value) throw new Exception(msg);
  }

  public void NotNull(object? value, string msg = "expected non-null") {
    if (value is null) throw new Exception(msg);
  }

  public void IsNull(object? value, string msg = "expected null") {
    if (value is not null) throw new Exception(msg);
  }

  public void PrintSummary() {
    Console.WriteLine($"\n{Passed} passed, {Failed} failed out of {Passed + Failed} tests.");
  }
}

}  // namespace GeomPPTests
