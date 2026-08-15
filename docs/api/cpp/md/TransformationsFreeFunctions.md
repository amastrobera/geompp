# `TransformationsFreeFunctions` (C++)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `transform`

`geometry::`[`Polygon2D`](Polygon2D.md)` transform(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, maths::Matrix3 const & m)`

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Polygon3D`](Polygon3D.md)` transform(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, maths::Matrix4 const & m)`

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Point2D`](Point2D.md)` transform(geometry::`[`Point2D`](Point2D.md)` const & p, maths::Matrix3 const & m)`

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Vector2D`](Vector2D.md)` transform(geometry::`[`Vector2D`](Vector2D.md)` const & v, maths::Matrix3 const & m)`

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Ray2D`](Ray2D.md)` transform(geometry::`[`Ray2D`](Ray2D.md)` const & ray, maths::Matrix3 const & m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Line2D`](Line2D.md)` transform(geometry::`[`Line2D`](Line2D.md)` const & line, maths::Matrix3 const & m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` transform(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, maths::Matrix3 const & m)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Polyline2D`](Polyline2D.md)` transform(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, maths::Matrix3 const & m)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Triangle2D`](Triangle2D.md)` transform(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, maths::Matrix3 const & m)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Mesh2D`](Mesh2D.md)` transform(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, maths::Matrix3 const & m)`

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` transform(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, maths::Matrix3 const & m)`

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` transform(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, maths::Matrix3 const & m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` transform(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, maths::Matrix3 const & m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `m` (`maths::Matrix3 const &`)

`geometry::`[`Point3D`](Point3D.md)` transform(geometry::`[`Point3D`](Point3D.md)` const & p, maths::Matrix4 const & m)`

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Vector3D`](Vector3D.md)` transform(geometry::`[`Vector3D`](Vector3D.md)` const & v, maths::Matrix4 const & m)`

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Ray3D`](Ray3D.md)` transform(geometry::`[`Ray3D`](Ray3D.md)` const & ray, maths::Matrix4 const & m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Line3D`](Line3D.md)` transform(geometry::`[`Line3D`](Line3D.md)` const & line, maths::Matrix4 const & m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` transform(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, maths::Matrix4 const & m)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Polyline3D`](Polyline3D.md)` transform(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, maths::Matrix4 const & m)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Triangle3D`](Triangle3D.md)` transform(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, maths::Matrix4 const & m)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`Mesh3D`](Mesh3D.md)` transform(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, maths::Matrix4 const & m)`

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` transform(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, maths::Matrix4 const & m)`

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` transform(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, maths::Matrix4 const & m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `m` (`maths::Matrix4 const &`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` transform(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, maths::Matrix4 const & m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `m` (`maths::Matrix4 const &`)

## `translate`

`geometry::`[`Point2D`](Point2D.md)` translate(geometry::`[`Point2D`](Point2D.md)` const & p, maths::Vector2 const & offset)`

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Ray2D`](Ray2D.md)` translate(geometry::`[`Ray2D`](Ray2D.md)` const & ray, maths::Vector2 const & offset)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Line2D`](Line2D.md)` translate(geometry::`[`Line2D`](Line2D.md)` const & line, maths::Vector2 const & offset)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` translate(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, maths::Vector2 const & offset)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` translate(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, maths::Vector2 const & offset)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Polygon2D`](Polygon2D.md)` translate(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, maths::Vector2 const & offset)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Polyline2D`](Polyline2D.md)` translate(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, maths::Vector2 const & offset)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Triangle2D`](Triangle2D.md)` translate(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, maths::Vector2 const & offset)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Mesh2D`](Mesh2D.md)` translate(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, maths::Vector2 const & offset)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` translate(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, maths::Vector2 const & offset)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` translate(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, maths::Vector2 const & offset)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `offset` (`maths::Vector2 const &`)

`geometry::`[`Point3D`](Point3D.md)` translate(geometry::`[`Point3D`](Point3D.md)` const & p, maths::Vector3 const & offset)`

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Ray3D`](Ray3D.md)` translate(geometry::`[`Ray3D`](Ray3D.md)` const & ray, maths::Vector3 const & offset)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Line3D`](Line3D.md)` translate(geometry::`[`Line3D`](Line3D.md)` const & line, maths::Vector3 const & offset)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` translate(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, maths::Vector3 const & offset)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` translate(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, maths::Vector3 const & offset)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Polygon3D`](Polygon3D.md)` translate(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, maths::Vector3 const & offset)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Polyline3D`](Polyline3D.md)` translate(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, maths::Vector3 const & offset)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Triangle3D`](Triangle3D.md)` translate(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, maths::Vector3 const & offset)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`Mesh3D`](Mesh3D.md)` translate(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, maths::Vector3 const & offset)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` translate(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, maths::Vector3 const & offset)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `offset` (`maths::Vector3 const &`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` translate(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, maths::Vector3 const & offset)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `offset` (`maths::Vector3 const &`)

## `rotate`

`geometry::`[`Point2D`](Point2D.md)` rotate(geometry::`[`Point2D`](Point2D.md)` const & p, double angle_rad)`

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` rotate(geometry::`[`Vector2D`](Vector2D.md)` const & v, double angle_rad)`

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` rotate(geometry::`[`Ray2D`](Ray2D.md)` const & ray, double angle_rad)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Line2D`](Line2D.md)` rotate(geometry::`[`Line2D`](Line2D.md)` const & line, double angle_rad)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` rotate(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, double angle_rad)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` rotate(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double angle_rad)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` rotate(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, double angle_rad)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` rotate(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, double angle_rad)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` rotate(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, double angle_rad)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` rotate(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` rotate(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` rotate(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `angle_rad` (`double`)

`geometry::`[`Point3D`](Point3D.md)` rotate(geometry::`[`Point3D`](Point3D.md)` const & p, double angle_rad, maths::Vector3 const & axis)`

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Vector3D`](Vector3D.md)` rotate(geometry::`[`Vector3D`](Vector3D.md)` const & v, double angle_rad, maths::Vector3 const & axis)`

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Ray3D`](Ray3D.md)` rotate(geometry::`[`Ray3D`](Ray3D.md)` const & ray, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Line3D`](Line3D.md)` rotate(geometry::`[`Line3D`](Line3D.md)` const & line, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` rotate(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` rotate(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Polygon3D`](Polygon3D.md)` rotate(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Polyline3D`](Polyline3D.md)` rotate(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Triangle3D`](Triangle3D.md)` rotate(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`Mesh3D`](Mesh3D.md)` rotate(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` rotate(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` rotate(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double angle_rad, maths::Vector3 const & axis)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

## `scale`

`geometry::`[`Point2D`](Point2D.md)` scale(geometry::`[`Point2D`](Point2D.md)` const & p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `factor` (`double`)

`geometry::`[`Point2D`](Point2D.md)` scale(geometry::`[`Point2D`](Point2D.md)` const & p, double sx, double sy)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` scale(geometry::`[`Vector2D`](Vector2D.md)` const & v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `factor` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` scale(geometry::`[`Vector2D`](Vector2D.md)` const & v, double sx, double sy)`

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` scale(geometry::`[`Ray2D`](Ray2D.md)` const & ray, double factor)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `factor` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` scale(geometry::`[`Ray2D`](Ray2D.md)` const & ray, double sx, double sy)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Line2D`](Line2D.md)` scale(geometry::`[`Line2D`](Line2D.md)` const & line, double factor)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `factor` (`double`)

`geometry::`[`Line2D`](Line2D.md)` scale(geometry::`[`Line2D`](Line2D.md)` const & line, double sx, double sy)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` scale(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, double factor)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `factor` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` scale(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, double sx, double sy)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` scale(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double factor)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `factor` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` scale(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double sx, double sy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` scale(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, double factor)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `factor` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` scale(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, double sx, double sy)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` scale(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, double factor)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `factor` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` scale(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, double sx, double sy)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` scale(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, double factor)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `factor` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` scale(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, double sx, double sy)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` scale(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `factor` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` scale(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` scale(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `factor` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` scale(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` scale(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `factor` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` scale(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Point3D`](Point3D.md)` scale(geometry::`[`Point3D`](Point3D.md)` const & p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `factor` (`double`)

`geometry::`[`Point3D`](Point3D.md)` scale(geometry::`[`Point3D`](Point3D.md)` const & p, double sx, double sy, double sz)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` scale(geometry::`[`Vector3D`](Vector3D.md)` const & v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `factor` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` scale(geometry::`[`Vector3D`](Vector3D.md)` const & v, double sx, double sy, double sz)`

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` scale(geometry::`[`Ray3D`](Ray3D.md)` const & ray, double factor)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `factor` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` scale(geometry::`[`Ray3D`](Ray3D.md)` const & ray, double sx, double sy, double sz)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Line3D`](Line3D.md)` scale(geometry::`[`Line3D`](Line3D.md)` const & line, double factor)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `factor` (`double`)

`geometry::`[`Line3D`](Line3D.md)` scale(geometry::`[`Line3D`](Line3D.md)` const & line, double sx, double sy, double sz)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` scale(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, double factor)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `factor` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` scale(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, double sx, double sy, double sz)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` scale(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double factor)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `factor` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` scale(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double sx, double sy, double sz)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` scale(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, double factor)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `factor` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` scale(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, double sx, double sy, double sz)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` scale(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, double factor)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `factor` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` scale(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, double sx, double sy, double sz)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` scale(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, double factor)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `factor` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` scale(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, double sx, double sy, double sz)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` scale(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `factor` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` scale(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` scale(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `factor` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` scale(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` scale(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `factor` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` scale(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `shear`

`geometry::`[`Point2D`](Point2D.md)` shear(geometry::`[`Point2D`](Point2D.md)` const & p, double shx, double shy)`

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` shear(geometry::`[`Vector2D`](Vector2D.md)` const & v, double shx, double shy)`

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` shear(geometry::`[`Ray2D`](Ray2D.md)` const & ray, double shx, double shy)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Line2D`](Line2D.md)` shear(geometry::`[`Line2D`](Line2D.md)` const & line, double shx, double shy)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` shear(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, double shx, double shy)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` shear(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double shx, double shy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` shear(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, double shx, double shy)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` shear(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, double shx, double shy)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` shear(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, double shx, double shy)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` shear(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` shear(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` shear(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Point3D`](Point3D.md)` shear(geometry::`[`Point3D`](Point3D.md)` const & p, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` shear(geometry::`[`Vector3D`](Vector3D.md)` const & v, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` shear(geometry::`[`Ray3D`](Ray3D.md)` const & ray, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Line3D`](Line3D.md)` shear(geometry::`[`Line3D`](Line3D.md)` const & line, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` shear(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` shear(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` shear(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` shear(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` shear(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` shear(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` shear(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` shear(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `reflect`

`geometry::`[`Point2D`](Point2D.md)` reflect(geometry::`[`Point2D`](Point2D.md)` const & p, maths::Vector2 const & normal)`

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Vector2D`](Vector2D.md)` reflect(geometry::`[`Vector2D`](Vector2D.md)` const & v, maths::Vector2 const & normal)`

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Ray2D`](Ray2D.md)` reflect(geometry::`[`Ray2D`](Ray2D.md)` const & ray, maths::Vector2 const & normal)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Line2D`](Line2D.md)` reflect(geometry::`[`Line2D`](Line2D.md)` const & line, maths::Vector2 const & normal)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` reflect(geometry::`[`LineSegment2D`](LineSegment2D.md)` const & seg, maths::Vector2 const & normal)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` reflect(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, maths::Vector2 const & normal)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Polygon2D`](Polygon2D.md)` reflect(geometry::`[`Polygon2D`](Polygon2D.md)` const & poly, maths::Vector2 const & normal)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Polyline2D`](Polyline2D.md)` reflect(geometry::`[`Polyline2D`](Polyline2D.md)` const & polyline, maths::Vector2 const & normal)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Triangle2D`](Triangle2D.md)` reflect(geometry::`[`Triangle2D`](Triangle2D.md)` const & tri, maths::Vector2 const & normal)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Mesh2D`](Mesh2D.md)` reflect(geometry::`[`Mesh2D`](Mesh2D.md)` const & mesh, maths::Vector2 const & normal)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` reflect(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, maths::Vector2 const & normal)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` reflect(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, maths::Vector2 const & normal)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `normal` (`maths::Vector2 const &`)

`geometry::`[`Point3D`](Point3D.md)` reflect(geometry::`[`Point3D`](Point3D.md)` const & p, maths::Vector3 const & normal)`

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Vector3D`](Vector3D.md)` reflect(geometry::`[`Vector3D`](Vector3D.md)` const & v, maths::Vector3 const & normal)`

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Ray3D`](Ray3D.md)` reflect(geometry::`[`Ray3D`](Ray3D.md)` const & ray, maths::Vector3 const & normal)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Line3D`](Line3D.md)` reflect(geometry::`[`Line3D`](Line3D.md)` const & line, maths::Vector3 const & normal)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` reflect(geometry::`[`LineSegment3D`](LineSegment3D.md)` const & seg, maths::Vector3 const & normal)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` reflect(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, maths::Vector3 const & normal)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Polygon3D`](Polygon3D.md)` reflect(geometry::`[`Polygon3D`](Polygon3D.md)` const & poly, maths::Vector3 const & normal)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Polyline3D`](Polyline3D.md)` reflect(geometry::`[`Polyline3D`](Polyline3D.md)` const & polyline, maths::Vector3 const & normal)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Triangle3D`](Triangle3D.md)` reflect(geometry::`[`Triangle3D`](Triangle3D.md)` const & tri, maths::Vector3 const & normal)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`Mesh3D`](Mesh3D.md)` reflect(geometry::`[`Mesh3D`](Mesh3D.md)` const & mesh, maths::Vector3 const & normal)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` reflect(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, maths::Vector3 const & normal)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md) const &)
- `normal` (`maths::Vector3 const &`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` reflect(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, maths::Vector3 const & normal)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `normal` (`maths::Vector3 const &`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
