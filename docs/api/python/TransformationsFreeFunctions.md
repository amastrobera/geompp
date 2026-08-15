# `TransformationsFreeFunctions` (Python)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `transform`

`transform(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, m: maths::Matrix3) -> geometry::`[`Polygon2D`](Polygon2D.md)

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `m` (`maths::Matrix3`)

`transform(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, m: maths::Matrix4) -> geometry::`[`Polygon3D`](Polygon3D.md)

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `m` (`maths::Matrix4`)

`transform(p: geometry::`[`Point2D`](Point2D.md)`, m: maths::Matrix3) -> geometry::`[`Point2D`](Point2D.md)

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `m` (`maths::Matrix3`)

`transform(v: geometry::`[`Vector2D`](Vector2D.md)`, m: maths::Matrix3) -> geometry::`[`Vector2D`](Vector2D.md)

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `m` (`maths::Matrix3`)

`transform(ray: geometry::`[`Ray2D`](Ray2D.md)`, m: maths::Matrix3) -> geometry::`[`Ray2D`](Ray2D.md)

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `m` (`maths::Matrix3`)

`transform(line: geometry::`[`Line2D`](Line2D.md)`, m: maths::Matrix3) -> geometry::`[`Line2D`](Line2D.md)

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `m` (`maths::Matrix3`)

`transform(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, m: maths::Matrix3) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `m` (`maths::Matrix3`)

`transform(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, m: maths::Matrix3) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `m` (`maths::Matrix3`)

`transform(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, m: maths::Matrix3) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `m` (`maths::Matrix3`)

`transform(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, m: maths::Matrix3) -> geometry::`[`Mesh2D`](Mesh2D.md)

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `m` (`maths::Matrix3`)

`transform(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, m: maths::Matrix3) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `m` (`maths::Matrix3`)

`transform(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, m: maths::Matrix3) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `m` (`maths::Matrix3`)

`transform(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, m: maths::Matrix3) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `m` (`maths::Matrix3`)

`transform(p: geometry::`[`Point3D`](Point3D.md)`, m: maths::Matrix4) -> geometry::`[`Point3D`](Point3D.md)

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `m` (`maths::Matrix4`)

`transform(v: geometry::`[`Vector3D`](Vector3D.md)`, m: maths::Matrix4) -> geometry::`[`Vector3D`](Vector3D.md)

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `m` (`maths::Matrix4`)

`transform(ray: geometry::`[`Ray3D`](Ray3D.md)`, m: maths::Matrix4) -> geometry::`[`Ray3D`](Ray3D.md)

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `m` (`maths::Matrix4`)

`transform(line: geometry::`[`Line3D`](Line3D.md)`, m: maths::Matrix4) -> geometry::`[`Line3D`](Line3D.md)

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `m` (`maths::Matrix4`)

`transform(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, m: maths::Matrix4) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `m` (`maths::Matrix4`)

`transform(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, m: maths::Matrix4) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `m` (`maths::Matrix4`)

`transform(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, m: maths::Matrix4) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `m` (`maths::Matrix4`)

`transform(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, m: maths::Matrix4) -> geometry::`[`Mesh3D`](Mesh3D.md)

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `m` (`maths::Matrix4`)

`transform(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, m: maths::Matrix4) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `m` (`maths::Matrix4`)

`transform(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, m: maths::Matrix4) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `m` (`maths::Matrix4`)

`transform(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, m: maths::Matrix4) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `m` (`maths::Matrix4`)

## `translate`

`translate(p: geometry::`[`Point2D`](Point2D.md)`, offset: maths::Vector2) -> geometry::`[`Point2D`](Point2D.md)

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `offset` (`maths::Vector2`)

`translate(ray: geometry::`[`Ray2D`](Ray2D.md)`, offset: maths::Vector2) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `offset` (`maths::Vector2`)

`translate(line: geometry::`[`Line2D`](Line2D.md)`, offset: maths::Vector2) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `offset` (`maths::Vector2`)

`translate(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, offset: maths::Vector2) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `offset` (`maths::Vector2`)

`translate(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, offset: maths::Vector2) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `offset` (`maths::Vector2`)

`translate(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, offset: maths::Vector2) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `offset` (`maths::Vector2`)

`translate(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, offset: maths::Vector2) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `offset` (`maths::Vector2`)

`translate(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, offset: maths::Vector2) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `offset` (`maths::Vector2`)

`translate(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, offset: maths::Vector2) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `offset` (`maths::Vector2`)

`translate(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, offset: maths::Vector2) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `offset` (`maths::Vector2`)

`translate(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, offset: maths::Vector2) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `offset` (`maths::Vector2`)

`translate(p: geometry::`[`Point3D`](Point3D.md)`, offset: maths::Vector3) -> geometry::`[`Point3D`](Point3D.md)

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `offset` (`maths::Vector3`)

`translate(ray: geometry::`[`Ray3D`](Ray3D.md)`, offset: maths::Vector3) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `offset` (`maths::Vector3`)

`translate(line: geometry::`[`Line3D`](Line3D.md)`, offset: maths::Vector3) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `offset` (`maths::Vector3`)

`translate(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, offset: maths::Vector3) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `offset` (`maths::Vector3`)

`translate(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, offset: maths::Vector3) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `offset` (`maths::Vector3`)

`translate(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, offset: maths::Vector3) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `offset` (`maths::Vector3`)

`translate(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, offset: maths::Vector3) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `offset` (`maths::Vector3`)

`translate(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, offset: maths::Vector3) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `offset` (`maths::Vector3`)

`translate(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, offset: maths::Vector3) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `offset` (`maths::Vector3`)

`translate(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, offset: maths::Vector3) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `offset` (`maths::Vector3`)

`translate(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, offset: maths::Vector3) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `offset` (`maths::Vector3`)

## `rotate`

`rotate(p: geometry::`[`Point2D`](Point2D.md)`, angle_rad: float) -> geometry::`[`Point2D`](Point2D.md)

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `angle_rad` (`float`)

`rotate(v: geometry::`[`Vector2D`](Vector2D.md)`, angle_rad: float) -> geometry::`[`Vector2D`](Vector2D.md)

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `angle_rad` (`float`)

`rotate(ray: geometry::`[`Ray2D`](Ray2D.md)`, angle_rad: float) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `angle_rad` (`float`)

`rotate(line: geometry::`[`Line2D`](Line2D.md)`, angle_rad: float) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `angle_rad` (`float`)

`rotate(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, angle_rad: float) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `angle_rad` (`float`)

`rotate(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, angle_rad: float) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `angle_rad` (`float`)

`rotate(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, angle_rad: float) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `angle_rad` (`float`)

`rotate(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, angle_rad: float) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `angle_rad` (`float`)

`rotate(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, angle_rad: float) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `angle_rad` (`float`)

`rotate(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, angle_rad: float) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `angle_rad` (`float`)

`rotate(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, angle_rad: float) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `angle_rad` (`float`)

`rotate(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, angle_rad: float) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `angle_rad` (`float`)

`rotate(p: geometry::`[`Point3D`](Point3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Point3D`](Point3D.md)

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(v: geometry::`[`Vector3D`](Vector3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Vector3D`](Vector3D.md)

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(ray: geometry::`[`Ray3D`](Ray3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(line: geometry::`[`Line3D`](Line3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

`rotate(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, angle_rad: float, axis: maths::Vector3) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `angle_rad` (`float`)
- `axis` (`maths::Vector3`)

## `scale`

`scale(p: geometry::`[`Point2D`](Point2D.md)`, factor: float) -> geometry::`[`Point2D`](Point2D.md)

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `factor` (`float`)

`scale(p: geometry::`[`Point2D`](Point2D.md)`, sx: float, sy: float) -> geometry::`[`Point2D`](Point2D.md)

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(v: geometry::`[`Vector2D`](Vector2D.md)`, factor: float) -> geometry::`[`Vector2D`](Vector2D.md)

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `factor` (`float`)

`scale(v: geometry::`[`Vector2D`](Vector2D.md)`, sx: float, sy: float) -> geometry::`[`Vector2D`](Vector2D.md)

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(ray: geometry::`[`Ray2D`](Ray2D.md)`, factor: float) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `factor` (`float`)

`scale(ray: geometry::`[`Ray2D`](Ray2D.md)`, sx: float, sy: float) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(line: geometry::`[`Line2D`](Line2D.md)`, factor: float) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `factor` (`float`)

`scale(line: geometry::`[`Line2D`](Line2D.md)`, sx: float, sy: float) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, factor: float) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `factor` (`float`)

`scale(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, sx: float, sy: float) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, factor: float) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `factor` (`float`)

`scale(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, sx: float, sy: float) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, factor: float) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `factor` (`float`)

`scale(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, sx: float, sy: float) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, factor: float) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `factor` (`float`)

`scale(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, sx: float, sy: float) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, factor: float) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `factor` (`float`)

`scale(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, sx: float, sy: float) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, factor: float) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, sx: float, sy: float) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, factor: float) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, sx: float, sy: float) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, factor: float) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, sx: float, sy: float) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `sx` (`float`)
- `sy` (`float`)

`scale(p: geometry::`[`Point3D`](Point3D.md)`, factor: float) -> geometry::`[`Point3D`](Point3D.md)

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `factor` (`float`)

`scale(p: geometry::`[`Point3D`](Point3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Point3D`](Point3D.md)

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(v: geometry::`[`Vector3D`](Vector3D.md)`, factor: float) -> geometry::`[`Vector3D`](Vector3D.md)

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `factor` (`float`)

`scale(v: geometry::`[`Vector3D`](Vector3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Vector3D`](Vector3D.md)

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(ray: geometry::`[`Ray3D`](Ray3D.md)`, factor: float) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `factor` (`float`)

`scale(ray: geometry::`[`Ray3D`](Ray3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(line: geometry::`[`Line3D`](Line3D.md)`, factor: float) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `factor` (`float`)

`scale(line: geometry::`[`Line3D`](Line3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, factor: float) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `factor` (`float`)

`scale(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, factor: float) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `factor` (`float`)

`scale(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, factor: float) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `factor` (`float`)

`scale(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, factor: float) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `factor` (`float`)

`scale(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, factor: float) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `factor` (`float`)

`scale(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, factor: float) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, factor: float) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

`scale(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, factor: float) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `factor` (`float`)

`scale(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, sx: float, sy: float, sz: float) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

## `shear`

`shear(p: geometry::`[`Point2D`](Point2D.md)`, shx: float, shy: float) -> geometry::`[`Point2D`](Point2D.md)

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(v: geometry::`[`Vector2D`](Vector2D.md)`, shx: float, shy: float) -> geometry::`[`Vector2D`](Vector2D.md)

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(ray: geometry::`[`Ray2D`](Ray2D.md)`, shx: float, shy: float) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(line: geometry::`[`Line2D`](Line2D.md)`, shx: float, shy: float) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, shx: float, shy: float) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, shx: float, shy: float) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, shx: float, shy: float) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, shx: float, shy: float) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, shx: float, shy: float) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, shx: float, shy: float) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, shx: float, shy: float) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, shx: float, shy: float) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `shx` (`float`)
- `shy` (`float`)

`shear(p: geometry::`[`Point3D`](Point3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Point3D`](Point3D.md)

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(v: geometry::`[`Vector3D`](Vector3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Vector3D`](Vector3D.md)

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(ray: geometry::`[`Ray3D`](Ray3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(line: geometry::`[`Line3D`](Line3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

`shear(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

## `reflect`

`reflect(p: geometry::`[`Point2D`](Point2D.md)`, normal: maths::Vector2) -> geometry::`[`Point2D`](Point2D.md)

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `normal` (`maths::Vector2`)

`reflect(v: geometry::`[`Vector2D`](Vector2D.md)`, normal: maths::Vector2) -> geometry::`[`Vector2D`](Vector2D.md)

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `normal` (`maths::Vector2`)

`reflect(ray: geometry::`[`Ray2D`](Ray2D.md)`, normal: maths::Vector2) -> geometry::`[`Ray2D`](Ray2D.md)


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `normal` (`maths::Vector2`)

`reflect(line: geometry::`[`Line2D`](Line2D.md)`, normal: maths::Vector2) -> geometry::`[`Line2D`](Line2D.md)


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `normal` (`maths::Vector2`)

`reflect(seg: geometry::`[`LineSegment2D`](LineSegment2D.md)`, normal: maths::Vector2) -> geometry::`[`LineSegment2D`](LineSegment2D.md)


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `normal` (`maths::Vector2`)

`reflect(collection: geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)`, normal: maths::Vector2) -> geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `normal` (`maths::Vector2`)

`reflect(poly: geometry::`[`Polygon2D`](Polygon2D.md)`, normal: maths::Vector2) -> geometry::`[`Polygon2D`](Polygon2D.md)


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `normal` (`maths::Vector2`)

`reflect(polyline: geometry::`[`Polyline2D`](Polyline2D.md)`, normal: maths::Vector2) -> geometry::`[`Polyline2D`](Polyline2D.md)


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `normal` (`maths::Vector2`)

`reflect(tri: geometry::`[`Triangle2D`](Triangle2D.md)`, normal: maths::Vector2) -> geometry::`[`Triangle2D`](Triangle2D.md)


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `normal` (`maths::Vector2`)

`reflect(mesh: geometry::`[`Mesh2D`](Mesh2D.md)`, normal: maths::Vector2) -> geometry::`[`Mesh2D`](Mesh2D.md)


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `normal` (`maths::Vector2`)

`reflect(mesh: geometry::`[`PolyMesh2D`](PolyMesh2D.md)`, normal: maths::Vector2) -> geometry::`[`PolyMesh2D`](PolyMesh2D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `normal` (`maths::Vector2`)

`reflect(mesh: geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)`, normal: maths::Vector2) -> geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `normal` (`maths::Vector2`)

`reflect(p: geometry::`[`Point3D`](Point3D.md)`, normal: maths::Vector3) -> geometry::`[`Point3D`](Point3D.md)

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `normal` (`maths::Vector3`)

`reflect(v: geometry::`[`Vector3D`](Vector3D.md)`, normal: maths::Vector3) -> geometry::`[`Vector3D`](Vector3D.md)

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `normal` (`maths::Vector3`)

`reflect(ray: geometry::`[`Ray3D`](Ray3D.md)`, normal: maths::Vector3) -> geometry::`[`Ray3D`](Ray3D.md)


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `normal` (`maths::Vector3`)

`reflect(line: geometry::`[`Line3D`](Line3D.md)`, normal: maths::Vector3) -> geometry::`[`Line3D`](Line3D.md)


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `normal` (`maths::Vector3`)

`reflect(seg: geometry::`[`LineSegment3D`](LineSegment3D.md)`, normal: maths::Vector3) -> geometry::`[`LineSegment3D`](LineSegment3D.md)


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `normal` (`maths::Vector3`)

`reflect(collection: geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)`, normal: maths::Vector3) -> geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `normal` (`maths::Vector3`)

`reflect(poly: geometry::`[`Polygon3D`](Polygon3D.md)`, normal: maths::Vector3) -> geometry::`[`Polygon3D`](Polygon3D.md)


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `normal` (`maths::Vector3`)

`reflect(polyline: geometry::`[`Polyline3D`](Polyline3D.md)`, normal: maths::Vector3) -> geometry::`[`Polyline3D`](Polyline3D.md)


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `normal` (`maths::Vector3`)

`reflect(tri: geometry::`[`Triangle3D`](Triangle3D.md)`, normal: maths::Vector3) -> geometry::`[`Triangle3D`](Triangle3D.md)


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `normal` (`maths::Vector3`)

`reflect(mesh: geometry::`[`Mesh3D`](Mesh3D.md)`, normal: maths::Vector3) -> geometry::`[`Mesh3D`](Mesh3D.md)


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `normal` (`maths::Vector3`)

`reflect(mesh: geometry::`[`PolyMesh3D`](PolyMesh3D.md)`, normal: maths::Vector3) -> geometry::`[`PolyMesh3D`](PolyMesh3D.md)


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `normal` (`maths::Vector3`)

`reflect(mesh: geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)`, normal: maths::Vector3) -> geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `normal` (`maths::Vector3`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
