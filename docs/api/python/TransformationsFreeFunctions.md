# `TransformationsFreeFunctions` (Python)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `transform`

`transform(poly: `[`Polygon2D`](Polygon2D.md)`, m: Matrix3) -> `[`Polygon2D`](Polygon2D.md)

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `m` (`Matrix3`)

`transform(poly: `[`Polygon3D`](Polygon3D.md)`, m: Matrix4) -> `[`Polygon3D`](Polygon3D.md)

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `m` (`Matrix4`)

`transform(p: `[`Point2D`](Point2D.md)`, m: Matrix3) -> `[`Point2D`](Point2D.md)

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `m` (`Matrix3`)

`transform(v: `[`Vector2D`](Vector2D.md)`, m: Matrix3) -> `[`Vector2D`](Vector2D.md)

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `m` (`Matrix3`)

`transform(ray: `[`Ray2D`](Ray2D.md)`, m: Matrix3) -> `[`Ray2D`](Ray2D.md)

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `m` (`Matrix3`)

`transform(line: `[`Line2D`](Line2D.md)`, m: Matrix3) -> `[`Line2D`](Line2D.md)

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `m` (`Matrix3`)

`transform(seg: `[`LineSegment2D`](LineSegment2D.md)`, m: Matrix3) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `m` (`Matrix3`)

`transform(polyline: `[`Polyline2D`](Polyline2D.md)`, m: Matrix3) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `m` (`Matrix3`)

`transform(tri: `[`Triangle2D`](Triangle2D.md)`, m: Matrix3) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `m` (`Matrix3`)

`transform(mesh: `[`Mesh2D`](Mesh2D.md)`, m: Matrix3) -> `[`Mesh2D`](Mesh2D.md)

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `m` (`Matrix3`)

`transform(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, m: Matrix3) -> `[`PolyMesh2D`](PolyMesh2D.md)

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `m` (`Matrix3`)

`transform(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, m: Matrix3) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `m` (`Matrix3`)

`transform(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, m: Matrix3) -> `[`GeometryCollection2D`](GeometryCollection2D.md)

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `m` (`Matrix3`)

`transform(p: `[`Point3D`](Point3D.md)`, m: Matrix4) -> `[`Point3D`](Point3D.md)

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `m` (`Matrix4`)

`transform(v: `[`Vector3D`](Vector3D.md)`, m: Matrix4) -> `[`Vector3D`](Vector3D.md)

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `m` (`Matrix4`)

`transform(ray: `[`Ray3D`](Ray3D.md)`, m: Matrix4) -> `[`Ray3D`](Ray3D.md)

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `m` (`Matrix4`)

`transform(line: `[`Line3D`](Line3D.md)`, m: Matrix4) -> `[`Line3D`](Line3D.md)

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `m` (`Matrix4`)

`transform(seg: `[`LineSegment3D`](LineSegment3D.md)`, m: Matrix4) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `m` (`Matrix4`)

`transform(polyline: `[`Polyline3D`](Polyline3D.md)`, m: Matrix4) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `m` (`Matrix4`)

`transform(tri: `[`Triangle3D`](Triangle3D.md)`, m: Matrix4) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `m` (`Matrix4`)

`transform(mesh: `[`Mesh3D`](Mesh3D.md)`, m: Matrix4) -> `[`Mesh3D`](Mesh3D.md)

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `m` (`Matrix4`)

`transform(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, m: Matrix4) -> `[`PolyMesh3D`](PolyMesh3D.md)

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `m` (`Matrix4`)

`transform(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, m: Matrix4) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `m` (`Matrix4`)

`transform(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, m: Matrix4) -> `[`GeometryCollection3D`](GeometryCollection3D.md)

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `m` (`Matrix4`)

## `translate`

`translate(p: `[`Point2D`](Point2D.md)`, offset: Vector2) -> `[`Point2D`](Point2D.md)

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `offset` (`Vector2`)

`translate(ray: `[`Ray2D`](Ray2D.md)`, offset: Vector2) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `offset` (`Vector2`)

`translate(line: `[`Line2D`](Line2D.md)`, offset: Vector2) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `offset` (`Vector2`)

`translate(seg: `[`LineSegment2D`](LineSegment2D.md)`, offset: Vector2) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `offset` (`Vector2`)

`translate(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, offset: Vector2) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `offset` (`Vector2`)

`translate(poly: `[`Polygon2D`](Polygon2D.md)`, offset: Vector2) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `offset` (`Vector2`)

`translate(polyline: `[`Polyline2D`](Polyline2D.md)`, offset: Vector2) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `offset` (`Vector2`)

`translate(tri: `[`Triangle2D`](Triangle2D.md)`, offset: Vector2) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `offset` (`Vector2`)

`translate(mesh: `[`Mesh2D`](Mesh2D.md)`, offset: Vector2) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `offset` (`Vector2`)

`translate(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, offset: Vector2) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `offset` (`Vector2`)

`translate(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, offset: Vector2) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `offset` (`Vector2`)

`translate(p: `[`Point3D`](Point3D.md)`, offset: Vector3) -> `[`Point3D`](Point3D.md)

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `offset` (`Vector3`)

`translate(ray: `[`Ray3D`](Ray3D.md)`, offset: Vector3) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `offset` (`Vector3`)

`translate(line: `[`Line3D`](Line3D.md)`, offset: Vector3) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `offset` (`Vector3`)

`translate(seg: `[`LineSegment3D`](LineSegment3D.md)`, offset: Vector3) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `offset` (`Vector3`)

`translate(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, offset: Vector3) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `offset` (`Vector3`)

`translate(poly: `[`Polygon3D`](Polygon3D.md)`, offset: Vector3) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `offset` (`Vector3`)

`translate(polyline: `[`Polyline3D`](Polyline3D.md)`, offset: Vector3) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `offset` (`Vector3`)

`translate(tri: `[`Triangle3D`](Triangle3D.md)`, offset: Vector3) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `offset` (`Vector3`)

`translate(mesh: `[`Mesh3D`](Mesh3D.md)`, offset: Vector3) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `offset` (`Vector3`)

`translate(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, offset: Vector3) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `offset` (`Vector3`)

`translate(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, offset: Vector3) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `offset` (`Vector3`)

## `rotate`

`rotate(p: `[`Point2D`](Point2D.md)`, angle_rad: float) -> `[`Point2D`](Point2D.md)

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `angle_rad` (`float`)

`rotate(v: `[`Vector2D`](Vector2D.md)`, angle_rad: float) -> `[`Vector2D`](Vector2D.md)

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `angle_rad` (`float`)

`rotate(ray: `[`Ray2D`](Ray2D.md)`, angle_rad: float) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `angle_rad` (`float`)

`rotate(line: `[`Line2D`](Line2D.md)`, angle_rad: float) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `angle_rad` (`float`)

`rotate(seg: `[`LineSegment2D`](LineSegment2D.md)`, angle_rad: float) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `angle_rad` (`float`)

`rotate(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, angle_rad: float) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `angle_rad` (`float`)

`rotate(poly: `[`Polygon2D`](Polygon2D.md)`, angle_rad: float) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `angle_rad` (`float`)

`rotate(polyline: `[`Polyline2D`](Polyline2D.md)`, angle_rad: float) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `angle_rad` (`float`)

`rotate(tri: `[`Triangle2D`](Triangle2D.md)`, angle_rad: float) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `angle_rad` (`float`)

`rotate(mesh: `[`Mesh2D`](Mesh2D.md)`, angle_rad: float) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `angle_rad` (`float`)

`rotate(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, angle_rad: float) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `angle_rad` (`float`)

`rotate(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, angle_rad: float) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `angle_rad` (`float`)

`rotate(p: `[`Point3D`](Point3D.md)`, angle_rad: float, axis: Vector3) -> `[`Point3D`](Point3D.md)

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(v: `[`Vector3D`](Vector3D.md)`, angle_rad: float, axis: Vector3) -> `[`Vector3D`](Vector3D.md)

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(ray: `[`Ray3D`](Ray3D.md)`, angle_rad: float, axis: Vector3) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(line: `[`Line3D`](Line3D.md)`, angle_rad: float, axis: Vector3) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(seg: `[`LineSegment3D`](LineSegment3D.md)`, angle_rad: float, axis: Vector3) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, angle_rad: float, axis: Vector3) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(poly: `[`Polygon3D`](Polygon3D.md)`, angle_rad: float, axis: Vector3) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(polyline: `[`Polyline3D`](Polyline3D.md)`, angle_rad: float, axis: Vector3) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(tri: `[`Triangle3D`](Triangle3D.md)`, angle_rad: float, axis: Vector3) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(mesh: `[`Mesh3D`](Mesh3D.md)`, angle_rad: float, axis: Vector3) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, angle_rad: float, axis: Vector3) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

`rotate(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, angle_rad: float, axis: Vector3) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `angle_rad` (`float`)
- `axis` (`Vector3`)

## `scale`

`scale(p: `[`Point2D`](Point2D.md)`, factor: float) -> `[`Point2D`](Point2D.md)

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `factor` (`float`)

`scale(p: `[`Point2D`](Point2D.md)`, sx: float, sy: float) -> `[`Point2D`](Point2D.md)

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(v: `[`Vector2D`](Vector2D.md)`, factor: float) -> `[`Vector2D`](Vector2D.md)

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `factor` (`float`)

`scale(v: `[`Vector2D`](Vector2D.md)`, sx: float, sy: float) -> `[`Vector2D`](Vector2D.md)

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(ray: `[`Ray2D`](Ray2D.md)`, factor: float) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `factor` (`float`)

`scale(ray: `[`Ray2D`](Ray2D.md)`, sx: float, sy: float) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(line: `[`Line2D`](Line2D.md)`, factor: float) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `factor` (`float`)

`scale(line: `[`Line2D`](Line2D.md)`, sx: float, sy: float) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(seg: `[`LineSegment2D`](LineSegment2D.md)`, factor: float) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `factor` (`float`)

`scale(seg: `[`LineSegment2D`](LineSegment2D.md)`, sx: float, sy: float) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, factor: float) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `factor` (`float`)

`scale(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, sx: float, sy: float) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(poly: `[`Polygon2D`](Polygon2D.md)`, factor: float) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `factor` (`float`)

`scale(poly: `[`Polygon2D`](Polygon2D.md)`, sx: float, sy: float) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(polyline: `[`Polyline2D`](Polyline2D.md)`, factor: float) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `factor` (`float`)

`scale(polyline: `[`Polyline2D`](Polyline2D.md)`, sx: float, sy: float) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(tri: `[`Triangle2D`](Triangle2D.md)`, factor: float) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `factor` (`float`)

`scale(tri: `[`Triangle2D`](Triangle2D.md)`, sx: float, sy: float) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: `[`Mesh2D`](Mesh2D.md)`, factor: float) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `factor` (`float`)

`scale(mesh: `[`Mesh2D`](Mesh2D.md)`, sx: float, sy: float) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, factor: float) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `factor` (`float`)

`scale(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, sx: float, sy: float) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, factor: float) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `factor` (`float`)

`scale(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, sx: float, sy: float) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(p: `[`Point3D`](Point3D.md)`, factor: float) -> `[`Point3D`](Point3D.md)

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `factor` (`float`)

`scale(p: `[`Point3D`](Point3D.md)`, sx: float, sy: float, sz: float) -> `[`Point3D`](Point3D.md)

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(v: `[`Vector3D`](Vector3D.md)`, factor: float) -> `[`Vector3D`](Vector3D.md)

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `factor` (`float`)

`scale(v: `[`Vector3D`](Vector3D.md)`, sx: float, sy: float, sz: float) -> `[`Vector3D`](Vector3D.md)

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(ray: `[`Ray3D`](Ray3D.md)`, factor: float) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `factor` (`float`)

`scale(ray: `[`Ray3D`](Ray3D.md)`, sx: float, sy: float, sz: float) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(line: `[`Line3D`](Line3D.md)`, factor: float) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `factor` (`float`)

`scale(line: `[`Line3D`](Line3D.md)`, sx: float, sy: float, sz: float) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(seg: `[`LineSegment3D`](LineSegment3D.md)`, factor: float) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `factor` (`float`)

`scale(seg: `[`LineSegment3D`](LineSegment3D.md)`, sx: float, sy: float, sz: float) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, factor: float) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `factor` (`float`)

`scale(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, sx: float, sy: float, sz: float) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(poly: `[`Polygon3D`](Polygon3D.md)`, factor: float) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `factor` (`float`)

`scale(poly: `[`Polygon3D`](Polygon3D.md)`, sx: float, sy: float, sz: float) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(polyline: `[`Polyline3D`](Polyline3D.md)`, factor: float) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `factor` (`float`)

`scale(polyline: `[`Polyline3D`](Polyline3D.md)`, sx: float, sy: float, sz: float) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(tri: `[`Triangle3D`](Triangle3D.md)`, factor: float) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `factor` (`float`)

`scale(tri: `[`Triangle3D`](Triangle3D.md)`, sx: float, sy: float, sz: float) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: `[`Mesh3D`](Mesh3D.md)`, factor: float) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `factor` (`float`)

`scale(mesh: `[`Mesh3D`](Mesh3D.md)`, sx: float, sy: float, sz: float) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, factor: float) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `factor` (`float`)

`scale(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, sx: float, sy: float, sz: float) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, factor: float) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `factor` (`float`)

`scale(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, sx: float, sy: float, sz: float) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

## `shear`

`shear(p: `[`Point2D`](Point2D.md)`, shx: float, shy: float) -> `[`Point2D`](Point2D.md)

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(v: `[`Vector2D`](Vector2D.md)`, shx: float, shy: float) -> `[`Vector2D`](Vector2D.md)

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(ray: `[`Ray2D`](Ray2D.md)`, shx: float, shy: float) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(line: `[`Line2D`](Line2D.md)`, shx: float, shy: float) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(seg: `[`LineSegment2D`](LineSegment2D.md)`, shx: float, shy: float) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, shx: float, shy: float) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(poly: `[`Polygon2D`](Polygon2D.md)`, shx: float, shy: float) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(polyline: `[`Polyline2D`](Polyline2D.md)`, shx: float, shy: float) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(tri: `[`Triangle2D`](Triangle2D.md)`, shx: float, shy: float) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: `[`Mesh2D`](Mesh2D.md)`, shx: float, shy: float) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, shx: float, shy: float) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, shx: float, shy: float) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(p: `[`Point3D`](Point3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Point3D`](Point3D.md)

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(v: `[`Vector3D`](Vector3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Vector3D`](Vector3D.md)

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(ray: `[`Ray3D`](Ray3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(line: `[`Line3D`](Line3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(seg: `[`LineSegment3D`](LineSegment3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(poly: `[`Polygon3D`](Polygon3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(polyline: `[`Polyline3D`](Polyline3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(tri: `[`Triangle3D`](Triangle3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: `[`Mesh3D`](Mesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

## `reflect`

`reflect(p: `[`Point2D`](Point2D.md)`, normal: Vector2) -> `[`Point2D`](Point2D.md)

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `normal` (`Vector2`)

`reflect(v: `[`Vector2D`](Vector2D.md)`, normal: Vector2) -> `[`Vector2D`](Vector2D.md)

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md))
- `normal` (`Vector2`)

`reflect(ray: `[`Ray2D`](Ray2D.md)`, normal: Vector2) -> `[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))
- `normal` (`Vector2`)

`reflect(line: `[`Line2D`](Line2D.md)`, normal: Vector2) -> `[`Line2D`](Line2D.md)


**Parameters**

- `line` ([`Line2D`](Line2D.md))
- `normal` (`Vector2`)

`reflect(seg: `[`LineSegment2D`](LineSegment2D.md)`, normal: Vector2) -> `[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))
- `normal` (`Vector2`)

`reflect(collection: `[`GeometryCollection2D`](GeometryCollection2D.md)`, normal: Vector2) -> `[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md))
- `normal` (`Vector2`)

`reflect(poly: `[`Polygon2D`](Polygon2D.md)`, normal: Vector2) -> `[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md))
- `normal` (`Vector2`)

`reflect(polyline: `[`Polyline2D`](Polyline2D.md)`, normal: Vector2) -> `[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))
- `normal` (`Vector2`)

`reflect(tri: `[`Triangle2D`](Triangle2D.md)`, normal: Vector2) -> `[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md))
- `normal` (`Vector2`)

`reflect(mesh: `[`Mesh2D`](Mesh2D.md)`, normal: Vector2) -> `[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md))
- `normal` (`Vector2`)

`reflect(mesh: `[`PolyMesh2D`](PolyMesh2D.md)`, normal: Vector2) -> `[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md))
- `normal` (`Vector2`)

`reflect(mesh: `[`ConnectedMesh2D`](ConnectedMesh2D.md)`, normal: Vector2) -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md))
- `normal` (`Vector2`)

`reflect(p: `[`Point3D`](Point3D.md)`, normal: Vector3) -> `[`Point3D`](Point3D.md)

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md))
- `normal` (`Vector3`)

`reflect(v: `[`Vector3D`](Vector3D.md)`, normal: Vector3) -> `[`Vector3D`](Vector3D.md)

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md))
- `normal` (`Vector3`)

`reflect(ray: `[`Ray3D`](Ray3D.md)`, normal: Vector3) -> `[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))
- `normal` (`Vector3`)

`reflect(line: `[`Line3D`](Line3D.md)`, normal: Vector3) -> `[`Line3D`](Line3D.md)


**Parameters**

- `line` ([`Line3D`](Line3D.md))
- `normal` (`Vector3`)

`reflect(seg: `[`LineSegment3D`](LineSegment3D.md)`, normal: Vector3) -> `[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))
- `normal` (`Vector3`)

`reflect(collection: `[`GeometryCollection3D`](GeometryCollection3D.md)`, normal: Vector3) -> `[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md))
- `normal` (`Vector3`)

`reflect(poly: `[`Polygon3D`](Polygon3D.md)`, normal: Vector3) -> `[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md))
- `normal` (`Vector3`)

`reflect(polyline: `[`Polyline3D`](Polyline3D.md)`, normal: Vector3) -> `[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))
- `normal` (`Vector3`)

`reflect(tri: `[`Triangle3D`](Triangle3D.md)`, normal: Vector3) -> `[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md))
- `normal` (`Vector3`)

`reflect(mesh: `[`Mesh3D`](Mesh3D.md)`, normal: Vector3) -> `[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md))
- `normal` (`Vector3`)

`reflect(mesh: `[`PolyMesh3D`](PolyMesh3D.md)`, normal: Vector3) -> `[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md))
- `normal` (`Vector3`)

`reflect(mesh: `[`ConnectedMesh3D`](ConnectedMesh3D.md)`, normal: Vector3) -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md))
- `normal` (`Vector3`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
