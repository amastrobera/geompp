# Test Coverage Report — geompp

> **Run tests with:** `ctest` from the build directory, or your IDE's GTest runner.
> **Coverage tool:** GTest does not include a built-in line-coverage tool. For LCOV/gcov output, build with `-DCMAKE_CXX_FLAGS="--coverage"` and run `lcov` + `genhtml`.

---

## Summary

| Metric | Value |
|--------|-------|
| Total classes | 20 |
| Classes with any tests | 10 |
| Classes with no tests | 10 |
| Estimated total functions/methods | ~290 |
| Estimated untested | ~175 |
| Estimated coverage | ~40% |

### Classes with zero coverage
`Point3D`, `Vector3D`, `Line3D`, `Ray3D`, `LineSegment3D`, `Polygon2D`, `Polygon3D`, `Polyline3D`, `BBox2D`, `BBox3D`

### Classes with partial coverage
`Triangle3D` (constructor/Wkt/ToFile only; 8 tests commented out), `Plane` (2 empty stubs), `Line2D` (missing ProjectOnto, Location, Ray/Segment/Triangle intersection), `Vector2D` (missing Normalize, BasisX/Y, unary -), `Point2D` (missing ToVector, linear_combination, AlmostEquals)

---

## Untested Functions (ranked by priority)

---

### 1. `Point3D` — `geompp/src/point3d.cpp` / `geompp/include/point3d.hpp`

**Description:** 3D point class — foundation of all 3D geometry in the library. Every 3D shape (Line3D, Ray3D, Triangle3D, Plane, etc.) is built on top of Point3D.

**Why it matters:** Highest fan-in of any 3D type. Bugs here cascade into every 3D class. The `feature/geom_3d` branch is actively adding 3D geometry, making this the most critical gap right now.

**Test plan:**
- [ ] `Constructor` — `Point3D(x, y, z)`, default constructor, copy constructor
- [ ] `Equality` — `operator==` for equal and unequal points; verify `AlmostEquals` rounds correctly per `DECIMAL_PRECISION`
- [ ] `Getters` — `x()`, `y()`, `z()` return correct values
- [ ] `ToVector / ToPoint round-trip` — `p.ToVector().ToPoint() == p`
- [ ] `DistanceTo` — distance between known points (e.g. `(0,0,0)→(1,0,0) == 1`, `(0,0,0)→(1,1,1) == sqrt(3)`)
- [ ] `Operators` — `Point3D + Vector3D`, `Point3D - Point3D` (→ Vector3D), `Point3D - Vector3D`, `Point3D * scalar`
- [ ] `Origin` — static `Origin()` returns `(0,0,0)`
- [ ] `Wkt` — `ToWkt` / `FromWkt` round-trip; invalid WKT strings throw
- [ ] `ToFile / FromFile` — serialize, deserialize, file exists check
- [ ] `are_collinear` — three collinear points return true; non-collinear return false
- [ ] `remove_duplicates` — list with duplicates → deduplicated list
- [ ] `remove_collinear` — list with collinear middle points → compressed list
- [ ] `linear_combination` — weighted sum matches manual calculation
- [ ] `average` — average of 3 known points matches centroid

---

### 2. `Vector3D` — `geompp/src/vector3d.cpp` / `geompp/include/vector3d.hpp`

**Description:** 3D vector class. Used everywhere in 3D geometry for direction, normals, cross products, and transformations.

**Why it matters:** High fan-in — `Line3D`, `Ray3D`, `Plane`, `Triangle3D` all rely on Vector3D arithmetic being correct. `Cross()` in particular is load-bearing for plane normal computation.

**Test plan:**
- [ ] `Constructor / Getters` — `Vector3D(x,y,z)`, `x()`, `y()`, `z()`
- [ ] `Equality / AlmostEquals`
- [ ] `Length` — `Vector3D(3,4,0).Length() == 5`
- [ ] `Dot` — `(1,0,0)·(0,1,0) == 0`, `(1,1,0)·(1,1,0) == 2`
- [ ] `Cross` — `BasisX() × BasisY() == BasisZ()`, cross product is anticommutative
- [ ] `Normalize` — result has length 1; normalize zero vector throws or returns zero
- [ ] `Perp` — result is perpendicular to input (`dot == 0`)
- [ ] `Operators` — `+`, `-`, `* scalar`, `/ scalar`, unary `-`, `* (dot)` operator
- [ ] `BasisX / BasisY / BasisZ` — verify static basis vectors
- [ ] `ToPoint` — converts back to a Point3D
- [ ] `Wkt / ToFile / FromFile` — round-trip serialization

---

### 3. `Line3D` — `geompp/src/line3d.cpp` / `geompp/include/line3d.hpp`

**Description:** Infinite 3D line defined by an origin point and a direction vector.

**Why it matters:** Used by `Plane::Intersection`, `Triangle3D::Intersection`, and the LSV parser. A large portion of 3D intersection logic runs through Line3D.

**Test plan:**
- [ ] `Make(Point3D, Point3D)` and `Make(Point3D, Vector3D)` — both factory paths
- [ ] `Origin / Direction / First / Last` — accessor correctness
- [ ] `Contains(Point3D)` — point on line returns true; off-line returns false
- [ ] `DistanceTo(Point3D)` — known distances from a horizontal line
- [ ] `ProjectOnto(Point3D)` — projected point lies on the line
- [ ] `Location(Point3D)` — parameter at start point is 0, at displaced point matches displacement
- [ ] `AlmostEquals` — two identical lines (possibly flipped direction) compare equal
- [ ] `Intersects / Intersection with Line3D` — parallel lines don't intersect; skew lines may not; coplanar crossing lines do
- [ ] `Wkt / ToFile / FromFile`

---

### 4. `LineSegment3D` — `geompp/src/line_segment3d.cpp` / `geompp/include/line_segment3d.hpp`

**Description:** Bounded 3D line segment between two endpoints.

**Why it matters:** Used in LSV parser output, Triangle3D intersection results, and Polyline3D. Many 3D operations return LineSegment3D as their intersection type.

**Test plan:**
- [ ] `Make(Point3D, Point3D)` — basic construction; duplicate endpoints throw
- [ ] `First / Last` — endpoint accessors
- [ ] `Length` — `Make((0,0,0),(3,0,0)).Length() == 3`
- [ ] `ToLine` — resulting Line3D contains both endpoints
- [ ] `Contains(Point3D)` — midpoint returns true; external point returns false
- [ ] `Location(Point3D)` — start=0, end=1, midpoint=0.5
- [ ] `Interpolate(t)` — `Interpolate(0.5)` equals midpoint
- [ ] `DistanceTo(Point3D)` — distance from external point
- [ ] `Intersects / Intersection with Line3D`
- [ ] `Wkt / ToFile / FromFile`

---

### 5. `Ray3D` — `geompp/src/ray3d.cpp` / `geompp/include/ray3d.hpp`

**Description:** 3D half-line from an origin point in a given direction.

**Why it matters:** Used in LSV parser and 3D intersection logic. Shares the same role as Ray2D but in 3D — important for the `feature/geom_3d` branch.

**Test plan:**
- [ ] `Make(Point3D, Vector3D)` — basic construction
- [ ] `Origin / Direction`
- [ ] `Contains(Point3D)` — on-ray returns true; off-ray or behind returns false
- [ ] `IsAhead / IsBehind` — points in front of and behind the origin
- [ ] `ToLine` — resulting Line3D contains origin and a point ahead on the ray
- [ ] `DistanceTo(Point3D)` — perpendicular distance from a point off the ray
- [ ] `Intersects / Intersection with Line3D`
- [ ] `Wkt / ToFile / FromFile`

---

### 6. `Triangle3D` (incomplete) — `geompp/src/triangle3d.cpp`

**Description:** 3D triangle. Constructor and Wkt are tested; 8 substantive test cases are currently commented out.

**Why it matters:** Depended upon by the feature branch's 3D surface logic. The entire `Contains`, `Areas`, `ToAxis`, `DistanceTo`, `Interpolate`, and `IntersectionWLine` test bodies already exist as comments and just need to be enabled.

**Test plan:**
- [ ] Uncomment and fix `Triangle3D, Contains` — re-adapt commented 2D test to use 3D points (add z=const)
- [ ] Uncomment and fix `Triangle3D, Areas` — `SignedArea`, `Area`, `Perimeter` with a flat XY triangle
- [ ] Uncomment and fix `Triangle3D, ToPolygon` — compare resulting Polygon3D vertices
- [ ] Uncomment and fix `Triangle3D, ToAxis` — verify U/V axis vectors are orthogonal and in-plane
- [ ] Uncomment and fix `Triangle3D, DistanceTo` — zero for interior/border, positive for exterior
- [ ] Uncomment and fix `Triangle3D, Interpolate` — vertex at (1,0) and (0,1), centroid at (1/3, 1/3)
- [ ] Enable `Triangle3D, IntersectionWLine` — test with Line3D passing through, parallel, and missing the triangle
- [ ] Add `Triangle3D, IntersectionWRay3D` — analagous to 2D version

---

### 7. `Plane` — `geompp/src/plane.cpp`

**Description:** 3D plane with origin, normal, and UV axes. Core of 3D projection and intersection logic.

**Why it matters:** `Plane` is the primary abstraction for working with 3D surfaces. Currently has two empty GTest stubs (`Constructor`, `IntersectionWLine`) and zero real assertions — entirely untested.

**Test plan:**
- [ ] `From3Points` — three non-collinear points; collinear points throw
- [ ] `FromOriginAndAxes` — verify `normal()` equals cross product of axes
- [ ] `FromOriginAndNormal` — verify axes are perpendicular to normal
- [ ] `XY / YZ / ZX` — static standard planes have correct normals
- [ ] `Contains(Point3D)` — point on plane returns true; above/below returns false
- [ ] `SignedDistanceTo / DistanceTo` — known distances from XY plane; sign flips below
- [ ] `ProjectOnto(Point3D)` — projected point lies on the plane; distance to projected point is 0
- [ ] `ProjectInto(Point3D)` — 3D point maps to correct 2D plane coordinates
- [ ] `Evaluate(Point2D)` — inverse of ProjectInto; round-trip equals original 3D point
- [ ] `Intersects / Intersection with Line3D` — parallel line doesn't intersect; oblique line gives correct point

---

### 8. `Polygon2D` — `geompp/src/polygon2d.cpp`

**Description:** 2D polygon with N vertices. Used as the output of `Triangle2D::ToPolygon()`.

**Why it matters:** While only 10 methods exist, `Polygon2D` is the output type of `Triangle2D::ToPolygon()` which IS tested — but the polygon itself is never validated beyond equality. Any regression in polygon construction would be invisible.

**Test plan:**
- [ ] `Make(vector<Point2D>)` — valid input; less than 3 unique points throws
- [ ] `Size` — correct vertex count
- [ ] `operator[]` — correct vertex at each index
- [ ] `AlmostEquals` and `operator==`
- [ ] `Wkt / FromWkt` — round-trip; invalid WKT throws
- [ ] `ToFile / FromFile`

---

### 9. `Polyline3D` — `geompp/src/polyline3d.cpp`

**Description:** 3D polyline (sequence of connected 3D segments).

**Why it matters:** 3D counterpart of the well-tested Polyline2D. All intersection, location, and interpolation methods are completely untested.

**Test plan:**
- [ ] Mirror the full `test_polyline2d.cpp` test suite, replacing Point2D/Vector2D with 3D equivalents and adding a non-trivial z component to all coordinates
- [ ] `Constructor` — collinear/duplicate removal in 3D
- [ ] `Contains / Location / Interpolate / DistanceTo`
- [ ] `Intersects / Intersection` with Line3D, Ray3D, LineSegment3D, Polyline3D

---

### 10. `BBox2D` — `geompp/src/bbox2d.cpp`

**Description:** 2D axis-aligned bounding box.

**Why it matters:** Bounding box tests are fast safety nets for spatial queries. The BBox2D constructors from Polyline2D, Polygon2D, Triangle2D, and LineSegment2D are untested.

**Test plan:**
- [ ] `BBox2D(min, max)` — correct min/max stored
- [ ] `BBox2D(LineSegment2D)` — bbox tightly wraps segment endpoints
- [ ] `BBox2D(Triangle2D)` — bbox tightly wraps all 3 vertices
- [ ] `BBox2D(Polygon2D)` — bbox tightly wraps all N vertices
- [ ] `BBox2D(Polyline2D)` — bbox tightly wraps all knots
- [ ] `Contains(Point2D)` — inside, on boundary, and outside cases
- [ ] `AlmostEquals / operator==`

---

### 11. `BBox3D` — `geompp/src/bbox3d.cpp`

**Description:** 3D axis-aligned bounding box.

**Why it matters:** Same rationale as BBox2D but for 3D. Likely minimal implementation right now, making this easy to test early.

**Test plan:**
- [ ] `BBox3D(min, max)` — `min()` and `max()` return correct corners
- [ ] `Contains(Point3D)` — inside, on face, and outside
- [ ] `AlmostEquals / operator==`

---

### 12. Untested methods in partially-covered 2D classes

#### `Line2D` — missing: `ProjectOnto`, `Location`, intersections with `Ray2D` / `LineSegment2D` / `Triangle2D`
- [ ] `ProjectOnto(Point2D)` — projected point lies on line; distance to projected point is 0
- [ ] `Location(Point2D)` — returns parameter t such that `Origin + t*Direction == point`
- [ ] `Intersects/Intersection with Ray2D` — test the 4 existing Ray2D test fixtures
- [ ] `Intersects/Intersection with LineSegment2D` — crossing and non-crossing segments
- [ ] `Intersects/Intersection with Triangle2D` — line bisects triangle → returns LineSegment2D; tangent → Point2D; miss → nullopt

#### `Vector2D` — missing: `Normalize`, `BasisX`, `BasisY`, unary `-`
- [ ] `Normalize` — result has length 1; direction preserved
- [ ] `BasisX / BasisY` — static helpers return `(1,0)` and `(0,1)`
- [ ] `Unary -` — negation flips sign of both components

#### `Point2D` — missing: `ToVector`, `linear_combination`, `AlmostEquals`
- [ ] `ToVector` — converts point to vector with same components
- [ ] `linear_combination` — weighted sum of points with given coefficients
- [ ] `AlmostEquals` — two nearby points within precision compare equal; too-far points don't

#### `utils` — missing: `round`, `sign`, `trim`, `to_upper`, `count_decimal_places` (all used indirectly but never explicitly tested)
- [ ] `round(x, precision)` — rounding at boundary values, negative numbers
- [ ] `sign(x)` — -1 / 0 / +1 for negative / zero / positive
- [ ] `trim(s)` — leading/trailing whitespace removed; inner whitespace preserved
- [ ] `to_upper(s)` — all lowercase → uppercase; non-alpha unchanged
- [ ] `count_decimal_places(d)` — `1.23` → 2, `1.0` → 0

---

## Quick wins (lowest effort, highest coverage gain)

| Item | Effort | Coverage gain |
|------|--------|---------------|
| Uncomment Triangle3D tests | Low — code already exists | 6 more tests |
| Add Plane test bodies | Low — copy Plane 2D pattern | All Plane methods |
| Add utils unit tests | Low — pure functions | 8 utils functions |
| Add Vector2D missing 3 methods | Low — append to existing file | 3 methods |
| Add BBox2D tests | Medium | All BBox2D methods |

