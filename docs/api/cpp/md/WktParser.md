# `WktParser` (C++)

## `Open`

**static** `WktParser Open(std::string const & fle_path)`

Opens a WKT file for streaming reads.

**Parameters**

- `fle_path` (`std::string const &`) — Filesystem path to the WKT file.

**Returns** — A WktParser positioned at the start of the file.

## `FromWkt`

**static** `ReturnSet FromWkt(std::string const & wkt)`

Parses a single WKT string into one of the supported geometry types.

**Parameters**

- `wkt` (`std::string const &`) — Well-Known Text string for any 2D or 3D primitive (POINT, LINE, RAY, LINESTRING, POLYGON, ...).

**Returns** — The parsed geometry wrapped in the variant, or std::nullopt if the WKT could not be parsed.

## `ToWkt`

**static** `std::string ToWkt(ReturnSet const & shape)`

Serializes any of the supported geometry variants back to WKT.

**Parameters**

- `shape` (`ReturnSet const &`) — The geometry to serialize. std::nullopt produces an empty string.

**Returns** — WKT representation at the current decimal precision.

## `HasNext`

`bool HasNext() const`

Tests whether the file has at least one more geometry to read.

**Returns** — true if a subsequent call to Next will return a value.

## `GetFilePath`

`std::string GetFilePath() const`

Path of the file currently being parsed.

**Returns** — The file path passed to Open .

## `Next`

`ReturnSet Next()`

Reads the next geometry from the open file.

**Returns** — The next geometry as a variant, or std::nullopt on end-of-file or parse failure.
