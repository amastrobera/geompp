"""
Reads Doxygen XML output (docs/api/cpp/xml/) and emits per-class Markdown
for the Python and C# bindings under docs/api/python/ and docs/api/csharp/.

Run from docs/ after `doxygen Doxyfile`:
    python gen_bindings_md.py

Why this script exists
----------------------
The Python and C# bindings wrap the same C++ classes 1:1, so the *meaning*
of each method is identical across all three languages. Rather than write
docs three times, we document once in the C++ headers (via Doxygen ///@brief
@param @return) and transform the Doxygen XML into language-flavoured
Markdown with type/naming adjustments:

  Python:  snake_case names, std::optional<X>     → 'X | None',
                               LineSegment3D const& → 'LineSegment3D'
  C#:      PascalCase names,  std::optional<X>     → 'X^ (nullable)',
                              LineSegment3D const& → 'LineSegment3D^'

Add Doxygen comments to a header → re-run doxygen → re-run this script.
"""
from __future__ import annotations

import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).parent
XML_DIR = ROOT / "api" / "cpp" / "xml"
PY_DIR = ROOT / "api" / "python"
CS_DIR = ROOT / "api" / "csharp"
CPP_DIR = ROOT / "api" / "cpp" / "md"


# ── Type transforms ────────────────────────────────────────────────────────

def strip_internal_namespaces(t: str) -> str:
    """Drop geompp's own inline/sub-namespace qualifiers ('geometry::', 'maths::',
    'transformations::', 'detail::') — Doxygen preserves them verbatim in cross-namespace
    signatures (e.g. geompp::transformations functions taking a geompp::geometry::Polygon2D),
    but every class already gets its own doc page under its bare name, so the qualifier is just
    noise here, not information."""
    return re.sub(r"\b(?:geometry|maths|transformations|detail)::", "", t)


def strip_cpp_quals(t: str) -> str:
    """Drop 'const', '&', '*', internal namespace qualifiers, and surrounding whitespace."""
    t = re.sub(r"\bconst\b", "", t)
    t = t.replace("&", "").replace("*", "")
    t = strip_internal_namespaces(t)
    return re.sub(r"\s+", " ", t).strip()


def clean_cpp_type(t: str) -> str:
    """Collapse whitespace/newlines in a raw C++ type string for display."""
    return re.sub(r"\s+", " ", strip_internal_namespaces(t)).strip()


def cpp_to_py_type(t: str) -> str:
    """Map a C++ type string to its Python binding equivalent."""
    t = strip_cpp_quals(t)
    # Resolve the per-class ReturnSet alias used for Intersection() (it's
    # `std::optional<std::variant<Point3D, ...>>`; in geompp the variant has
    # one alternative).
    if t == "ReturnSet":
        return "Point3D | None"
    # std::optional<std::variant<T>> → T | None  (variant has just one alt in geompp)
    m = re.fullmatch(r"std::optional<std::variant<([^<>]+)>>", t)
    if m:
        return f"{m.group(1).strip()} | None"
    m = re.fullmatch(r"std::optional<(.+)>", t)
    if m:
        return f"{m.group(1).strip()} | None"
    m = re.fullmatch(r"std::vector<(.+)>", t)
    if m:
        return f"list[{cpp_to_py_type(m.group(1))}]"
    # Common scalars
    return {
        "std::string": "str",
        "void": "None",
        "bool": "bool",
        "double": "float",
        "int": "int",
        "std::size_t": "int",
        "std::int64_t": "int",
        "std::int32_t": "int",
    }.get(t, t)


def cpp_to_cs_type(t: str) -> str:
    """Map a C++ type string to its C++/CLI binding equivalent."""
    t = strip_cpp_quals(t)
    if t == "ReturnSet":
        return "Point3D^  (nullable)"
    m = re.fullmatch(r"std::optional<std::variant<([^<>]+)>>", t)
    if m:
        return f"{m.group(1).strip()}^  (nullable)"
    m = re.fullmatch(r"std::optional<(.+)>", t)
    if m:
        return f"{m.group(1).strip()}^  (nullable)"
    return {
        "std::string": "System::String^",
        "void": "void",
        "bool": "bool",
        "double": "double",
        "int": "int",
        "std::size_t": "size_t",
        # ref-class types get '^'
        # (everything that isn't a primitive falls through with ^ appended)
    }.get(t, f"{t}^" if t and t[0].isupper() else t)


CS_NAME_OVERRIDES = {
    # snake_to_pascal's generic Title-Case-each-word pass gets these two wrong
    # (produces AreCcw/AreCw) because CCW/CW are acronyms, not words.
    "are_ccw": "AreCCW",
    "are_cw": "AreCW",
}


def cpp_to_cs_name(name: str) -> str:
    """snake_case free-function name -> PascalCase, matching the GeomUtil wrapper names
    (e.g. 'closest_world_plane_to' -> 'ClosestWorldPlaneTo')."""
    if name in CS_NAME_OVERRIDES:
        return CS_NAME_OVERRIDES[name]
    return "".join(part.capitalize() for part in name.split("_"))


def cpp_to_py_name(name: str) -> str:
    """PascalCase / camelCase → snake_case (with a few hardcoded mappings)."""
    overrides = {
        "Make": "make",
        "Zero": "zero",
        "BasisX": "basis_x",
        "BasisY": "basis_y",
        "BasisZ": "basis_z",
        "First": "first",
        "Last": "last",
        "Origin": "origin",
        "Direction": "direction",
        "ToLine": "to_line",
        "ToWkt": "to_wkt",
        "FromWkt": "from_wkt",
        "ToFile": "to_file",
        "FromFile": "from_file",
        "ToString": "__repr__",
    }
    if name in overrides:
        return overrides[name]
    # PascalCase → snake_case
    s = re.sub(r"(.)([A-Z][a-z]+)", r"\1_\2", name)
    s = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", s)
    return s.lower()


# ── XML parsing ────────────────────────────────────────────────────────────

def text_of(node, skip_tags=()):
    """Concatenate node text, child text, and tails — strip whitespace.
    Skip subtrees whose tag matches `skip_tags`."""
    if node is None:
        return ""
    parts = []
    if node.text:
        parts.append(node.text)
    for child in node:
        if child.tag in skip_tags:
            if child.tail:
                parts.append(child.tail)
            continue
        parts.append(text_of(child, skip_tags))
        if child.tail:
            parts.append(child.tail)
    return " ".join(s for s in (p.strip() for p in parts) if s)


def collect_descriptions(memberdef):
    brief = text_of(memberdef.find("briefdescription"))
    # The detailed description in Doxygen XML embeds <parameterlist> and
    # <simplesect kind="return"/"throws"> nodes — we render those separately,
    # so skip them here to avoid duplicate prose.
    detailed = text_of(memberdef.find("detaileddescription"),
                       skip_tags=("parameterlist", "simplesect"))
    return brief, detailed


def collect_params(memberdef):
    """Return list of (cpp_type, name, doc)."""
    out = []
    # Param types/names from <param>
    for p in memberdef.findall("param"):
        cpp_type = text_of(p.find("type"))
        decl_name = text_of(p.find("declname")) or text_of(p.find("defname")) or ""
        out.append([cpp_type, decl_name, ""])
    # Doc strings from <detaileddescription>//<parameterlist kind="param">
    pl = memberdef.find(".//parameterlist[@kind='param']")
    if pl is not None:
        for pi in pl.findall("parameteritem"):
            name = text_of(pi.find(".//parametername"))
            doc = text_of(pi.find("parameterdescription"))
            for row in out:
                if row[1] == name:
                    row[2] = doc
                    break
    return [tuple(r) for r in out]


def collect_return_doc(memberdef):
    rl = memberdef.find(".//simplesect[@kind='return']")
    return text_of(rl) if rl is not None else ""


def compound_name_parts(full_name: str) -> list[str]:
    """Strip the 'geompp' and 'geometry'/'maths'/'transformations' inline/sub-namespace
    prefixes off a Doxygen compoundname, leaving the class/struct path (possibly nested,
    e.g. ['ConnectedMesh2D', 'FaceView2D'])."""
    parts = full_name.split("::")
    if parts and parts[0] == "geompp":
        parts = parts[1:]
    if parts and parts[0] in ("geometry", "maths", "transformations"):
        parts = parts[1:]
    return parts


def parse_class_xml(xml_path: Path):
    """Parse a Doxygen compound XML for a class or struct. Returns dict with class info,
    or None for anything under a `detail` namespace (internal, not part of the bound API —
    see CLAUDE.md's `detail::` convention)."""
    tree = ET.parse(xml_path)
    root = tree.getroot()
    cd = root.find("compounddef")
    if cd is None or cd.get("kind") not in ("class", "struct"):
        return None

    full_name = text_of(cd.find("compoundname"))  # e.g. "geompp::geometry::Line3D"
    if "::detail::" in full_name or full_name.endswith("::detail"):
        return None
    name_parts = compound_name_parts(full_name)
    short_name = name_parts[-1]
    class_brief = text_of(cd.find("briefdescription"))
    class_detailed = text_of(cd.find("detaileddescription"))

    members = []
    fields = []
    for section in cd.findall("sectiondef"):
        kind = section.get("kind")
        if kind not in ("public-func", "public-static-func", "public-attrib", "public-static-attrib"):
            continue
        for m in section.findall("memberdef"):
            if m.get("prot") != "public":
                continue
            name = text_of(m.find("name"))
            if m.get("kind") == "variable":
                # A public data member (structs are typically plain data, e.g. GridCell2D.x/y).
                ftype = text_of(m.find("type"))
                fbrief, fdetailed = collect_descriptions(m)
                fields.append({"name": name, "type": ftype, "brief": fbrief or fdetailed})
                continue
            if m.get("kind") != "function":
                continue
            if name.startswith("~") or name == short_name:
                # skip dtors and ctors (constructors handled separately if you like)
                continue
            if name.startswith("operator"):
                # operators rarely need user-facing prose docs
                continue
            ret = text_of(m.find("type"))
            args = text_of(m.find("argsstring"))
            is_static = m.get("static") == "yes"
            # Use Doxygen's explicit attribute; falling back on argsstring suffix
            # would incorrectly flag any param that contains `const &`.
            is_const = m.get("const") == "yes"
            brief, detailed = collect_descriptions(m)
            params = collect_params(m)
            ret_doc = collect_return_doc(m)
            members.append({
                "name": name,
                "ret": ret,
                "args": args,
                "static": is_static,
                "const": is_const,
                "brief": brief,
                "detailed": detailed,
                "params": params,
                "ret_doc": ret_doc,
            })

    return {
        "name": short_name,
        "name_parts": name_parts,
        "full_name": full_name,
        "brief": class_brief,
        "detailed": class_detailed,
        "members": members,
        "fields": fields,
    }


# ── Free functions (namespace-level, not class members) ────────────────────
#
# Doxygen puts every geompp::geometry free function (geometry is an inline namespace) into
# namespacegeompp.xml, and geompp::maths / geompp::transformations into their own namespace
# compounds. We only want the subset that's actually bound to Python/C# — NOT every internal
# helper that happens to have a doc comment (e.g. is_left/sign/round/compare are geompp-internal,
# never bound). This list is a hand-maintained mirror of the m.def(...) names in
# geompp_python/src/bind_free_functions.cpp (geometry) / GeomUtil.hpp (all three groups) — the
# same "two independent hand-duplicated lists" gotcha CLAUDE.md already flags for the C# vcxproj
# source lists. Whenever a free function is newly bound (or unbound), update this set too, or it
# silently falls out of (or never enters) the generated docs.
GEOMETRY_FREE_FUNCTIONS = {
    "are_ccw", "are_collinear", "are_coplanar", "are_cw", "average",
    "bezier_smoothing_2", "centroid", "clip", "closest_world_plane_to",
    "convex_hull", "dist_decimation", "distance_to", "find_extreme_points",
    "find_intersections", "fix_adjacency", "has_intersections", "lerp",
    "linear_combination", "merge", "polygonize", "polyline_expansion",
    "principal_axes", "principal_direction", "principal_normal",
    "rdp_decimation", "remove_collinear", "remove_consecutive_duplicates",
    "remove_duplicates", "signed_area", "tangents_to", "triangulate",
    "validate_adjacency", "vw_decimation",
}
MATHS_FREE_FUNCTIONS = {"solve_gauss", "solve_cramer"}
TRANSFORMATIONS_FREE_FUNCTIONS = {"translate", "rotate", "scale", "shear", "reflect", "transform"}

# (namespace XML filename, display label, allowed-name set)
FREE_FUNCTION_GROUPS = [
    ("namespacegeompp.xml", "geompp::geometry", GEOMETRY_FREE_FUNCTIONS, "FreeFunctions"),
    ("namespacegeompp_1_1maths.xml", "geompp::maths", MATHS_FREE_FUNCTIONS, "MathsFreeFunctions"),
    ("namespacegeompp_1_1transformations.xml", "geompp::transformations", TRANSFORMATIONS_FREE_FUNCTIONS,
     "TransformationsFreeFunctions"),
]


def parse_namespace_functions(xml_path: Path, allowed_names: set[str]) -> list[dict]:
    """Parse a Doxygen namespace compound XML, returning member-dicts (same shape
    parse_class_xml uses for methods) for every direct free function in `allowed_names`."""
    if not xml_path.exists():
        return []
    tree = ET.parse(xml_path)
    root = tree.getroot()
    cd = root.find("compounddef")
    if cd is None:
        return []

    out = []
    for section in cd.findall("sectiondef"):
        if section.get("kind") != "func":
            continue
        for m in section.findall("memberdef"):
            if m.get("kind") != "function" or m.get("prot") != "public":
                continue
            name = text_of(m.find("name"))
            if name not in allowed_names:
                continue
            ret = text_of(m.find("type"))
            brief, detailed = collect_descriptions(m)
            params = collect_params(m)
            ret_doc = collect_return_doc(m)
            tmpl = [text_of(p.find("declname")) or text_of(p.find("type"))
                    for p in m.findall("templateparamlist/param")]
            out.append({
                "name": name,
                "ret": ret,
                "args": text_of(m.find("argsstring")),
                "static": False,
                "const": False,
                "brief": brief,
                "detailed": detailed,
                "params": params,
                "ret_doc": ret_doc,
                "template_params": tmpl,
            })
    return out


# ── Cross-class linking ────────────────────────────────────────────────────

# Word-or-identifier regex used for "find references in prose".
_IDENT_RE = re.compile(r"\b([A-Z][A-Za-z0-9_]*)\b")


def linkify(text: str, known: set[str], current_class: str) -> str:
    """Wrap occurrences of known class names with [Name](Name.md).

    - Does NOT touch text inside backtick code spans (so signatures stay tidy).
    - Skips the current class's own name (no self-links).
    - Longest names first so "LineSegment3D" wins over "Line3D".
    """
    if not text:
        return text
    parts = re.split(r"(`[^`]*`)", text)
    out = []
    for p in parts:
        if p.startswith("`") and p.endswith("`"):
            out.append(p)
            continue
        for name in sorted(known, key=len, reverse=True):
            if name == current_class:
                continue
            p = re.sub(rf"\b{re.escape(name)}\b", f"[{name}]({name}.md)", p)
        out.append(p)
    return "".join(out)


def collect_referenced_classes(cls: dict, known: set[str]) -> set[str]:
    """Return every known class name mentioned anywhere in the class's members,
    excluding the class itself. Used to build the 'See also' footer."""
    refs: set[str] = set()
    for m in cls["members"]:
        sources = [m["ret"], m["brief"], m["detailed"], m["ret_doc"]]
        for p_type, _p_name, p_doc in m["params"]:
            sources.extend([p_type, p_doc])
        for src in sources:
            for match in _IDENT_RE.findall(src):
                if match in known and match != cls["name"]:
                    refs.add(match)
    return refs


def see_also_footer(cls: dict, known: set[str]) -> list[str]:
    refs = collect_referenced_classes(cls, known)
    if not refs:
        return []
    return [
        "",
        "---",
        "",
        "**See also:** " + ", ".join(f"[{n}]({n}.md)" for n in sorted(refs)),
    ]


# ── Type rendering for the parameter-row code-span ────────────────────────

def signature_md(text: str, known: set[str], current_class: str) -> str:
    """Render a one-line signature with every known class name turned into a
    monospace link `[`Name`](Name.md)`, while keeping the surrounding text
    (parameter names, punctuation, `->`, `const`, ...) in monospace backticks.

    The current class's own name is not linked (no self-loops).
    """
    out: list[str] = []
    last = 0
    for m in _IDENT_RE.finditer(text):
        name = m.group(1)
        if name not in known or name == current_class:
            continue
        if last < m.start():
            out.append(f"`{text[last:m.start()]}`")
        out.append(f"[`{name}`]({name}.md)")
        last = m.end()
    if last < len(text):
        out.append(f"`{text[last:]}`")
    return "".join(out) if out else f"`{text}`"


def render_param_type_md(t: str, kind: str, known: set[str], current_class: str) -> str:
    """Format a parameter / return type for display, with an inline link to the
    target class's MD file when applicable.

    GitHub-flavored Markdown supports backticks-inside-link, e.g.
        [`Point3D`](Point3D.md)
    so we keep the monospace styling on the type AND make it clickable.
    """
    if kind == "py":
        rendered = cpp_to_py_type(t)
    elif kind == "cs":
        rendered = cpp_to_cs_type(t)
    else:  # cpp
        rendered = clean_cpp_type(t)

    # Try to find a known class name inside the rendered type and link it.
    # Multiple matches OK; we link each.
    def repl(m):
        name = m.group(1)
        if name in known and name != current_class:
            return f"[{name}]({name}.md)"
        return name

    linked = _IDENT_RE.sub(repl, rendered)

    # If linking happened, GFM lets us still wrap in backticks: `[Point3D](Point3D.md)`
    # — but the inner link won't render. The clean form is `[`Name`](Name.md)`.
    # If no linking, just backtick the whole thing.
    if "](" in linked:
        # Wrap each `[Name](Name.md)` as `` [`Name`](Name.md) `` and backtick the
        # rest of the type (e.g. `std::optional<` , `>`).
        def wrap_link(m):
            return f"[`{m.group(1)}`]({m.group(2)})"

        # Replace [Name](Name.md) -> [`Name`](Name.md)
        linked = re.sub(r"\[([^\]]+)\]\(([^)]+)\)", wrap_link, linked)
        # Backtick any non-link runs around the links (best-effort, no nested backticks).
        # Easiest: leave as-is; the link itself is monospace.
        return linked

    return f"`{rendered}`"


# ── Markdown emitters ──────────────────────────────────────────────────────

def emit_fields_md(fields: list[dict], kind: str, known: set[str], me: str) -> list[str]:
    """Render a struct's public data members (e.g. GridCell2D.x/y) as a Fields section."""
    if not fields:
        return []
    lines = ["## Fields", ""]
    for f in fields:
        if kind == "py":
            type_md = render_param_type_md(f["type"], "py", known, me)
        elif kind == "cs":
            type_md = render_param_type_md(f["type"], "cs", known, me)
        else:
            type_md = render_param_type_md(f["type"], "cpp", known, me)
        bullet = f"- `{f['name']}` ({type_md})"
        if f["brief"]:
            bullet += f" — {linkify(f['brief'], known, me)}"
        lines.append(bullet)
    lines.append("")
    return lines


def emit_python_md(cls: dict, known: set[str]) -> str:
    me = cls["name"]
    lines = [f"# `{me}` (Python)", ""]
    if cls["brief"]:
        lines += [linkify(cls["brief"], known, me), ""]
    if cls["detailed"]:
        lines += [linkify(cls["detailed"], known, me), ""]
    lines += emit_fields_md(cls.get("fields", []), "py", known, me)

    # Group by overload name
    by_name: dict[str, list] = {}
    for m in cls["members"]:
        by_name.setdefault(m["name"], []).append(m)

    for name, overloads in by_name.items():
        py_name = cpp_to_py_name(name)
        lines.append(f"## `{py_name}`")
        for m in overloads:
            params = ", ".join(
                f"{cpp_to_py_name(p_name) if p_name else '?'}: {cpp_to_py_type(p_type)}"
                for p_type, p_name, _ in m["params"]
            )
            ret_py = cpp_to_py_type(m["ret"])
            sig = signature_md(f"{py_name}({params}) -> {ret_py}", known, me)
            if m["static"]:
                sig = f"**static** {sig}"
            lines.append("")
            lines.append(sig)
            lines.append("")
            if m["brief"]:
                lines.append(linkify(m["brief"], known, me))
            if m["detailed"] and m["detailed"] != m["brief"]:
                lines += ["", linkify(m["detailed"], known, me)]
            if m["params"]:
                lines += ["", "**Parameters**", ""]
                for p_type, p_name, p_doc in m["params"]:
                    if p_name:
                        py_pn = cpp_to_py_name(p_name)
                        type_md = render_param_type_md(p_type, "py", known, me)
                        bullet = f"- `{py_pn}` ({type_md})"
                        if p_doc:
                            bullet += f" — {linkify(p_doc, known, me)}"
                        lines.append(bullet)
            if m["ret_doc"]:
                lines += ["", f"**Returns** — {linkify(m['ret_doc'], known, me)}"]
        lines.append("")
    lines += see_also_footer(cls, known)
    return "\n".join(lines).rstrip() + "\n"


def emit_csharp_md(cls: dict, known: set[str], name_transform=lambda n: n) -> str:
    me = cls["name"]
    lines = [f"# `{me}` (C# / .NET)", ""]
    if cls["brief"]:
        lines += [linkify(cls["brief"], known, me), ""]
    if cls["detailed"]:
        lines += [linkify(cls["detailed"], known, me), ""]
    lines += emit_fields_md(cls.get("fields", []), "cs", known, me)

    by_name: dict[str, list] = {}
    for m in cls["members"]:
        by_name.setdefault(m["name"], []).append(m)

    for name, overloads in by_name.items():
        cs_name = name_transform(name)
        lines.append(f"## `{cs_name}`")
        for m in overloads:
            params = ", ".join(
                f"{cpp_to_cs_type(p_type)} {p_name}"
                for p_type, p_name, _ in m["params"]
            )
            ret_cs = cpp_to_cs_type(m["ret"])
            sig = signature_md(f"{ret_cs} {cs_name}({params})", known, me)
            if m["static"]:
                sig = f"**static** {sig}"
            lines.append("")
            lines.append(sig)
            lines.append("")
            if m["brief"]:
                lines.append(linkify(m["brief"], known, me))
            if m["detailed"] and m["detailed"] != m["brief"]:
                lines += ["", linkify(m["detailed"], known, me)]
            if m["params"]:
                lines += ["", "**Parameters**", ""]
                for p_type, p_name, p_doc in m["params"]:
                    if p_name:
                        type_md = render_param_type_md(p_type, "cs", known, me)
                        bullet = f"- `{p_name}` ({type_md})"
                        if p_doc:
                            bullet += f" — {linkify(p_doc, known, me)}"
                        lines.append(bullet)
            if m["ret_doc"]:
                lines += ["", f"**Returns** — {linkify(m['ret_doc'], known, me)}"]
        lines.append("")
    lines += see_also_footer(cls, known)
    return "\n".join(lines).rstrip() + "\n"


def emit_cpp_md(cls: dict, known: set[str]) -> str:
    me = cls["name"]
    lines = [f"# `{me}` (C++)", ""]
    if cls["brief"]:
        lines += [linkify(cls["brief"], known, me), ""]
    if cls["detailed"]:
        lines += [linkify(cls["detailed"], known, me), ""]
    lines += emit_fields_md(cls.get("fields", []), "cpp", known, me)

    by_name: dict[str, list] = {}
    for m in cls["members"]:
        by_name.setdefault(m["name"], []).append(m)

    for name, overloads in by_name.items():
        lines.append(f"## `{name}`")
        for m in overloads:
            params = ", ".join(
                f"{clean_cpp_type(p_type)} {p_name}" for p_type, p_name, _ in m["params"]
            )
            ret_cpp = clean_cpp_type(m["ret"])
            suffix = " const" if m["const"] else ""
            tmpl = m.get("template_params") or []
            prefix = f"template <{', '.join(f'typename {t}' for t in tmpl)}> " if tmpl else ""
            sig = signature_md(f"{prefix}{ret_cpp} {name}({params}){suffix}", known, me)
            if m["static"]:
                sig = f"**static** {sig}"
            lines.append("")
            lines.append(sig)
            lines.append("")
            if m["brief"]:
                lines.append(linkify(m["brief"], known, me))
            if m["detailed"] and m["detailed"] != m["brief"]:
                lines += ["", linkify(m["detailed"], known, me)]
            if m["params"]:
                lines += ["", "**Parameters**", ""]
                for p_type, p_name, p_doc in m["params"]:
                    if p_name:
                        type_md = render_param_type_md(p_type, "cpp", known, me)
                        bullet = f"- `{p_name}` ({type_md})"
                        if p_doc:
                            bullet += f" — {linkify(p_doc, known, me)}"
                        lines.append(bullet)
            if m["ret_doc"]:
                lines += ["", f"**Returns** — {linkify(m['ret_doc'], known, me)}"]
        lines.append("")
    lines += see_also_footer(cls, known)
    return "\n".join(lines).rstrip() + "\n"


# ── Main ───────────────────────────────────────────────────────────────────

def main() -> int:
    if not XML_DIR.exists():
        print(f"ERROR: {XML_DIR} not found. Run `doxygen Doxyfile` first.", file=sys.stderr)
        return 1
    PY_DIR.mkdir(parents=True, exist_ok=True)
    CS_DIR.mkdir(parents=True, exist_ok=True)
    CPP_DIR.mkdir(parents=True, exist_ok=True)

    # These three directories hold nothing but this script's own output (see docs/README.md) —
    # clear them first so a renamed/removed/no-longer-public class doesn't leave a stale .md
    # behind (e.g. a class that moves under `detail::` and should stop being documented).
    for d in (PY_DIR, CS_DIR, CPP_DIR):
        for f in d.glob("*.md"):
            f.unlink()

    # First pass: parse every class/struct so we know the full set of names (needed for
    # cross-file linking). Anything under a `detail` namespace is filtered out inside
    # parse_class_xml (internal, not part of the bound API — see CLAUDE.md).
    parsed: list[dict] = []
    for xml_file in sorted(XML_DIR.glob("classgeompp_1_1*.xml")) + sorted(XML_DIR.glob("structgeompp_1_1*.xml")):
        cls = parse_class_xml(xml_file)
        if cls is None or not (cls["members"] or cls["fields"]):
            continue
        parsed.append(cls)

    # Disambiguate nested types that collide on their bare name (e.g. GeometryCollection2D::Entry
    # and GeometryCollection3D::Entry both being called "Entry") by qualifying just those with
    # their enclosing type. Everything else keeps its plain short name.
    name_counts: dict[str, int] = {}
    for c in parsed:
        name_counts[c["name"]] = name_counts.get(c["name"], 0) + 1
    for c in parsed:
        if name_counts[c["name"]] > 1:
            c["name"] = ".".join(c["name_parts"])

    known = {c["name"] for c in parsed}

    # Second pass: emit per-language Markdown, with class names cross-linked.
    for cls in parsed:
        (PY_DIR / f"{cls['name']}.md").write_text(emit_python_md(cls, known), encoding="utf-8")
        (CS_DIR / f"{cls['name']}.md").write_text(emit_csharp_md(cls, known), encoding="utf-8")
        (CPP_DIR / f"{cls['name']}.md").write_text(emit_cpp_md(cls, known), encoding="utf-8")
        print(f"  {cls['name']}.md -> python/, csharp/, cpp/md/")

    print(f"\nGenerated docs for {len(parsed)} class(es)/struct(s) across 3 languages.")

    # Third pass: free functions, grouped by namespace, documented as one page per group per
    # language (reusing the class emitters on a synthetic "class" whose members are the
    # namespace's free functions).
    ff_count = 0
    for xml_name, label, allowed, file_stem in FREE_FUNCTION_GROUPS:
        members = parse_namespace_functions(XML_DIR / xml_name, allowed)
        if not members:
            print(f"  WARNING: no free functions found for {label} (expected {len(allowed)}) "
                  f"— check FREE_FUNCTION_GROUPS against the bindings", file=sys.stderr)
            continue
        found_names = {m["name"] for m in members}
        missing = allowed - found_names
        if missing:
            print(f"  WARNING: {label} free functions not found in Doxygen XML (undocumented, "
                  f"renamed, or removed?): {sorted(missing)}", file=sys.stderr)
        synthetic = {
            "name": file_stem,
            "name_parts": [file_stem],
            "full_name": label,
            "brief": f"Free functions in `{label}` (not methods on a class — call them directly).",
            "detailed": "",
            "members": members,
            "fields": [],
        }
        (PY_DIR / f"{file_stem}.md").write_text(emit_python_md(synthetic, known), encoding="utf-8")
        (CS_DIR / f"{file_stem}.md").write_text(
            emit_csharp_md(synthetic, known, name_transform=cpp_to_cs_name), encoding="utf-8")
        (CPP_DIR / f"{file_stem}.md").write_text(emit_cpp_md(synthetic, known), encoding="utf-8")
        print(f"  {file_stem}.md -> python/, csharp/, cpp/md/ ({len(members)} function(s), {label})")
        ff_count += len(members)

    print(f"Generated free-function docs for {ff_count} function(s) across 3 language(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
