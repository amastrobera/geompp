# API documentation pipeline

The C++ headers in `geompp/include/` are the single source of truth for the API
across all three language bindings (C++, Python, C#). Public methods, and public
struct fields, are documented inline with Doxygen comments (`///@brief`, `@param`,
`@return`).

Three outputs are produced from those comments:

| Output | Path | Generator |
|---|---|---|
| C++ HTML reference | `docs/api/cpp/html/index.html` | Doxygen |
| C++ Markdown (one file per class/struct/free-function group) | `docs/api/cpp/md/<Name>.md` | `gen_bindings_md.py` |
| Python Markdown (one file per class/struct/free-function group) | `docs/api/python/<Name>.md` | `gen_bindings_md.py` |
| C# Markdown (one file per class/struct/free-function group) | `docs/api/csharp/<Name>.md` | `gen_bindings_md.py` |

**Scope**: every public `class`/`struct` in `geompp::geometry`, `geompp::maths`, and
`geompp::transformations` gets a page (e.g. `GridCell2D`, `CoordinateFrame`,
`TriangulationParams` — plain-data structs get a `## Fields` section instead of/alongside
methods). Anything under a `detail::` sub-namespace is skipped — those are internal-only
per CLAUDE.md's `detail::` convention and aren't bound to Python/C#. A nested type whose
bare name collides with another (e.g. `GeometryCollection2D::Entry` vs.
`GeometryCollection3D::Entry`) is qualified as `Outer.Inner` in its filename/heading;
everything else keeps its plain name.

**Free functions**: each of the three free-function namespaces gets its own page per
language — `FreeFunctions.md` (`geompp::geometry`), `MathsFreeFunctions.md`
(`geompp::maths`), `TransformationsFreeFunctions.md` (`geompp::transformations`) — grouping
every overload under one `##` heading per function name, same as a class's methods.
**Only functions actually bound to Python/C#** are included: `GEOMETRY_FREE_FUNCTIONS` /
`MATHS_FREE_FUNCTIONS` / `TRANSFORMATIONS_FREE_FUNCTIONS` near the top of
`gen_bindings_md.py` are a hand-maintained mirror of the `m.def(...)` names in
`geompp_python/src/bind_free_functions.cpp` / the static methods in
`geompp_csharp/src/GeomUtil.hpp` — the same "independent hand-duplicated list" pattern
CLAUDE.md already flags for the C# `.vcxproj` source lists and `geompp_python/CMakeLists.txt`'s
`GEOMPP_SOURCES`. **Whenever a free function is newly bound (or unbound), update these three
sets too**, or it silently falls out of (or never enters) the generated docs — the script
prints a `WARNING` to stderr if a name in one of these sets isn't found in the Doxygen XML
(renamed/undocumented/removed), but it can't detect the opposite case (a newly-bound function
nobody added to the set).

**Cross-linking**: every occurrence of a known class/struct name in parameter types,
return descriptions, and `@brief`/`@param`/`@return` prose becomes a Markdown
link to that class's `.md` file in the same directory. Each class/struct page ends with a
`**See also:** …` footer listing every other class it references. Click any
class name to jump to its docs. (Free-function pages don't get cross-links to *other
functions* — only the class-name linking above applies to their prose/signatures.)

The Python/C# Markdown is produced by transforming Doxygen's XML output:
PascalCase → `snake_case` for Python, `snake_case` → `PascalCase` for C# free functions,
`std::optional<X>` → `X | None`, and so on. The wording (`@brief` / `@param` / `@return`)
is shared across all three. Every regeneration first deletes all `*.md` in `docs/api/{python,csharp,cpp/md}/`
before writing — those three directories hold nothing but this script's output, so a
class/struct/function that's renamed, removed, or moved under `detail::` doesn't leave a
stale page behind.

---

## Prerequisites

- **Doxygen** (1.9+). On Windows: `winget install --id DimitriVanHeesch.Doxygen`
- **Python 3.8+** (already available via `venv/`).

## Regenerate

From the repo root:

```powershell
cd docs
doxygen Doxyfile
python gen_bindings_md.py
```

Output lands under `docs/api/`. The first command writes both HTML and XML
(the latter is what the second script consumes).

## Adding documentation to a new class

1. Open the header in `geompp/include/<class>.hpp`.
2. Add Doxygen comments above each public member:

   ```cpp
   /// @brief One-sentence summary of what the method does.
   /// @param foo Explanation of the parameter.
   /// @return Explanation of the return value.
   /// @throws std::runtime_error If anything can throw, list the conditions.
   ReturnType Method(ParamType const& foo) const;
   ```

3. Re-run the two commands above.

The Doxygen XML is keyed off the comment block immediately above each member
declaration. Class-level comments (above the `class Foo {`/`struct Foo {` line) flow into
the introductory paragraph of each generated `.md` file.

A new public `class`/`struct` under `geompp::geometry`/`maths`/`transformations` needs no
script changes — it's picked up automatically next run. A new **free function**, though,
needs its name added to the matching `GEOMETRY_FREE_FUNCTIONS`/`MATHS_FREE_FUNCTIONS`/
`TRANSFORMATIONS_FREE_FUNCTIONS` set in `gen_bindings_md.py` once it's bound to Python/C#
(see "Free functions" above) — this mirrors the "add the binding" step in the
`validation_and_expansion` skill's step 4, not something to do separately later.

## What gets transformed

For Python output, the generator applies:

| C++ | Python |
|---|---|
| `Make` → static factory | `make` |
| `LineSegment3D` (return) | `LineSegment3D` |
| `LineSegment3D const&` (param) | `LineSegment3D` |
| `std::optional<X>` | `X \| None` |
| `std::optional<std::variant<X>>` | `X \| None` |
| `std::vector<X>` | `list[X]` |
| `std::string` | `str` |
| `double` | `float` |
| `void` (return) | `None` |
| `DistanceTo`, `ProjectOnto`, etc. | `distance_to`, `project_onto`, ... |

For C# output:

| C++ | C# / .NET |
|---|---|
| `LineSegment3D const&` | `LineSegment3D^` |
| `std::optional<X>` | `X^  (nullable)` |
| `std::string` | `System::String^` |
| Class method names | PascalCase preserved (already PascalCase in the C++ source) |
| Free function names (e.g. `closest_world_plane_to`) | PascalCase via `cpp_to_cs_name` (`ClosestWorldPlaneTo`) |

Free functions are `snake_case` in the C++ headers but the C# wrapper (`GeomUtil`) exposes
them as PascalCase, so — unlike class methods, whose C++ name is already PascalCase and
reused as-is — the C# free-function page runs each name through `cpp_to_cs_name`
(`snake_to_pascal`, with `CS_NAME_OVERRIDES` for acronym cases the naive
Title-Case-each-word split gets wrong, e.g. `are_ccw` → `AreCCW`, not `AreCcw`).

If the transformations don't fit a future case (e.g. a new return type), edit
the `cpp_to_py_type` / `cpp_to_cs_type` / `cpp_to_py_name` / `cpp_to_cs_name` functions in
`gen_bindings_md.py`.
