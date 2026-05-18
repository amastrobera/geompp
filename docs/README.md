# API documentation pipeline

The C++ headers in `geompp/include/` are the single source of truth for the API
across all three language bindings (C++, Python, C#). Public methods are
documented inline with Doxygen comments (`///@brief`, `@param`, `@return`).

Three outputs are produced from those comments:

| Output | Path | Generator |
|---|---|---|
| C++ HTML reference | `docs/api/cpp/html/index.html` | Doxygen |
| C++ Markdown (one file per class) | `docs/api/cpp/md/<Class>.md` | `gen_bindings_md.py` |
| Python Markdown (one file per class) | `docs/api/python/<Class>.md` | `gen_bindings_md.py` |
| C# Markdown (one file per class) | `docs/api/csharp/<Class>.md` | `gen_bindings_md.py` |

**Cross-linking**: every occurrence of a known class name in parameter types,
return descriptions, and `@brief`/`@param`/`@return` prose becomes a Markdown
link to that class's `.md` file in the same directory. Each file ends with a
`**See also:** …` footer listing every other class it references. Click any
class name to jump to its docs.

The Python/C# Markdown is produced by transforming Doxygen's XML output:
PascalCase → `snake_case` for Python, `std::optional<X>` → `X | None`, and so on.
The wording (`@brief` / `@param` / `@return`) is shared across all three.

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
declaration. Class-level comments (above the `class Foo {` line) flow into the
introductory paragraph of each generated `.md` file.

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
| Method names | PascalCase preserved |

If the transformations don't fit a future case (e.g. a new return type), edit
the `cpp_to_py_type` / `cpp_to_cs_type` / `cpp_to_py_name` functions in
`gen_bindings_md.py`.
