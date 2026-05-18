# `WktParser` (Python)

## `open`

**static** `open(fle_path: str) -> WktParser`

Opens a WKT file for streaming reads.

**Parameters**

- `fle_path` (`str`) — Filesystem path to the WKT file.

**Returns** — A WktParser positioned at the start of the file.

## `from_wkt`

**static** `from_wkt(wkt: str) -> `[`Point3D`](Point3D.md)` | None`

Parses a single WKT string into one of the supported geometry types.

**Parameters**

- `wkt` (`str`) — Well-Known Text string for any 2D or 3D primitive (POINT, LINE, RAY, LINESTRING, POLYGON, ...).

**Returns** — The parsed geometry wrapped in the variant, or std::nullopt if the WKT could not be parsed.

## `to_wkt`

**static** `to_wkt(shape: `[`Point3D`](Point3D.md)` | None) -> str`

Serializes any of the supported geometry variants back to WKT.

**Parameters**

- `shape` ([`Point3D`](Point3D.md) | None) — The geometry to serialize. std::nullopt produces an empty string.

**Returns** — WKT representation at the current decimal precision.

## `has_next`

`has_next() -> bool`

Tests whether the file has at least one more geometry to read.

**Returns** — true if a subsequent call to Next will return a value.

## `get_file_path`

`get_file_path() -> str`

Path of the file currently being parsed.

**Returns** — The file path passed to Open .

## `next`

`next() -> `[`Point3D`](Point3D.md)` | None`

Reads the next geometry from the open file.

**Returns** — The next geometry as a variant, or std::nullopt on end-of-file or parse failure.
