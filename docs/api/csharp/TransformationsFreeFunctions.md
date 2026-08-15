# `TransformationsFreeFunctions` (C# / .NET)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `Transform`

`geometry::`[`Polygon2D`](Polygon2D.md)` Transform(geometry::`[`Polygon2D`](Polygon2D.md)` poly, maths::Matrix3 m)`

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Transform(geometry::`[`Polygon3D`](Polygon3D.md)` poly, maths::Matrix4 m)`

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Point2D`](Point2D.md)` Transform(geometry::`[`Point2D`](Point2D.md)` p, maths::Matrix3 m)`

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Vector2D`](Vector2D.md)` Transform(geometry::`[`Vector2D`](Vector2D.md)` v, maths::Matrix3 m)`

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Ray2D`](Ray2D.md)` Transform(geometry::`[`Ray2D`](Ray2D.md)` ray, maths::Matrix3 m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Line2D`](Line2D.md)` Transform(geometry::`[`Line2D`](Line2D.md)` line, maths::Matrix3 m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Transform(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, maths::Matrix3 m)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Transform(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, maths::Matrix3 m)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Transform(geometry::`[`Triangle2D`](Triangle2D.md)` tri, maths::Matrix3 m)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Transform(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, maths::Matrix3 m)`

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Transform(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, maths::Matrix3 m)`

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Transform(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, maths::Matrix3 m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Transform(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, maths::Matrix3 m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `m` (`maths::Matrix3`)

`geometry::`[`Point3D`](Point3D.md)` Transform(geometry::`[`Point3D`](Point3D.md)` p, maths::Matrix4 m)`

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Vector3D`](Vector3D.md)` Transform(geometry::`[`Vector3D`](Vector3D.md)` v, maths::Matrix4 m)`

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Ray3D`](Ray3D.md)` Transform(geometry::`[`Ray3D`](Ray3D.md)` ray, maths::Matrix4 m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Line3D`](Line3D.md)` Transform(geometry::`[`Line3D`](Line3D.md)` line, maths::Matrix4 m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Transform(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, maths::Matrix4 m)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Transform(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, maths::Matrix4 m)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Transform(geometry::`[`Triangle3D`](Triangle3D.md)` tri, maths::Matrix4 m)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Transform(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, maths::Matrix4 m)`

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Transform(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, maths::Matrix4 m)`

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Transform(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, maths::Matrix4 m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `m` (`maths::Matrix4`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Transform(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, maths::Matrix4 m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `m` (`maths::Matrix4`)

## `Translate`

`geometry::`[`Point2D`](Point2D.md)` Translate(geometry::`[`Point2D`](Point2D.md)` p, maths::Vector2 offset)`

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Ray2D`](Ray2D.md)` Translate(geometry::`[`Ray2D`](Ray2D.md)` ray, maths::Vector2 offset)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Line2D`](Line2D.md)` Translate(geometry::`[`Line2D`](Line2D.md)` line, maths::Vector2 offset)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Translate(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, maths::Vector2 offset)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Translate(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, maths::Vector2 offset)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Translate(geometry::`[`Polygon2D`](Polygon2D.md)` poly, maths::Vector2 offset)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Translate(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, maths::Vector2 offset)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Translate(geometry::`[`Triangle2D`](Triangle2D.md)` tri, maths::Vector2 offset)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Translate(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, maths::Vector2 offset)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Translate(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, maths::Vector2 offset)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Translate(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, maths::Vector2 offset)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `offset` (`maths::Vector2`)

`geometry::`[`Point3D`](Point3D.md)` Translate(geometry::`[`Point3D`](Point3D.md)` p, maths::Vector3 offset)`

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Ray3D`](Ray3D.md)` Translate(geometry::`[`Ray3D`](Ray3D.md)` ray, maths::Vector3 offset)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Line3D`](Line3D.md)` Translate(geometry::`[`Line3D`](Line3D.md)` line, maths::Vector3 offset)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Translate(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, maths::Vector3 offset)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Translate(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, maths::Vector3 offset)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Translate(geometry::`[`Polygon3D`](Polygon3D.md)` poly, maths::Vector3 offset)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Translate(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, maths::Vector3 offset)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Translate(geometry::`[`Triangle3D`](Triangle3D.md)` tri, maths::Vector3 offset)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Translate(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, maths::Vector3 offset)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Translate(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, maths::Vector3 offset)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `offset` (`maths::Vector3`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Translate(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, maths::Vector3 offset)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `offset` (`maths::Vector3`)

## `Rotate`

`geometry::`[`Point2D`](Point2D.md)` Rotate(geometry::`[`Point2D`](Point2D.md)` p, double angle_rad)`

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `angle_rad` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` Rotate(geometry::`[`Vector2D`](Vector2D.md)` v, double angle_rad)`

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `angle_rad` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` Rotate(geometry::`[`Ray2D`](Ray2D.md)` ray, double angle_rad)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `angle_rad` (`double`)

`geometry::`[`Line2D`](Line2D.md)` Rotate(geometry::`[`Line2D`](Line2D.md)` line, double angle_rad)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `angle_rad` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Rotate(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, double angle_rad)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `angle_rad` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Rotate(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, double angle_rad)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `angle_rad` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Rotate(geometry::`[`Polygon2D`](Polygon2D.md)` poly, double angle_rad)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `angle_rad` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Rotate(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, double angle_rad)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `angle_rad` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Rotate(geometry::`[`Triangle2D`](Triangle2D.md)` tri, double angle_rad)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `angle_rad` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Rotate(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `angle_rad` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Rotate(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `angle_rad` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Rotate(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, double angle_rad)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `angle_rad` (`double`)

`geometry::`[`Point3D`](Point3D.md)` Rotate(geometry::`[`Point3D`](Point3D.md)` p, double angle_rad, maths::Vector3 axis)`

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Vector3D`](Vector3D.md)` Rotate(geometry::`[`Vector3D`](Vector3D.md)` v, double angle_rad, maths::Vector3 axis)`

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Ray3D`](Ray3D.md)` Rotate(geometry::`[`Ray3D`](Ray3D.md)` ray, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Line3D`](Line3D.md)` Rotate(geometry::`[`Line3D`](Line3D.md)` line, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Rotate(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Rotate(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Rotate(geometry::`[`Polygon3D`](Polygon3D.md)` poly, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Rotate(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Rotate(geometry::`[`Triangle3D`](Triangle3D.md)` tri, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Rotate(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Rotate(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Rotate(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, double angle_rad, maths::Vector3 axis)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `angle_rad` (`double`)
- `axis` (`maths::Vector3`)

## `Scale`

`geometry::`[`Point2D`](Point2D.md)` Scale(geometry::`[`Point2D`](Point2D.md)` p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `factor` (`double`)

`geometry::`[`Point2D`](Point2D.md)` Scale(geometry::`[`Point2D`](Point2D.md)` p, double sx, double sy)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` Scale(geometry::`[`Vector2D`](Vector2D.md)` v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `factor` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` Scale(geometry::`[`Vector2D`](Vector2D.md)` v, double sx, double sy)`

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` Scale(geometry::`[`Ray2D`](Ray2D.md)` ray, double factor)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `factor` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` Scale(geometry::`[`Ray2D`](Ray2D.md)` ray, double sx, double sy)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Line2D`](Line2D.md)` Scale(geometry::`[`Line2D`](Line2D.md)` line, double factor)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `factor` (`double`)

`geometry::`[`Line2D`](Line2D.md)` Scale(geometry::`[`Line2D`](Line2D.md)` line, double sx, double sy)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Scale(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, double factor)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `factor` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Scale(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, double sx, double sy)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Scale(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, double factor)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `factor` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Scale(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, double sx, double sy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Scale(geometry::`[`Polygon2D`](Polygon2D.md)` poly, double factor)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `factor` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Scale(geometry::`[`Polygon2D`](Polygon2D.md)` poly, double sx, double sy)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Scale(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, double factor)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `factor` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Scale(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, double sx, double sy)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Scale(geometry::`[`Triangle2D`](Triangle2D.md)` tri, double factor)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `factor` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Scale(geometry::`[`Triangle2D`](Triangle2D.md)` tri, double sx, double sy)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Scale(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `factor` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Scale(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Scale(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `factor` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Scale(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Scale(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `factor` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Scale(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, double sx, double sy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `sx` (`double`)
- `sy` (`double`)

`geometry::`[`Point3D`](Point3D.md)` Scale(geometry::`[`Point3D`](Point3D.md)` p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `factor` (`double`)

`geometry::`[`Point3D`](Point3D.md)` Scale(geometry::`[`Point3D`](Point3D.md)` p, double sx, double sy, double sz)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` Scale(geometry::`[`Vector3D`](Vector3D.md)` v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `factor` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` Scale(geometry::`[`Vector3D`](Vector3D.md)` v, double sx, double sy, double sz)`

Scales a vector's components independently per axis.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` Scale(geometry::`[`Ray3D`](Ray3D.md)` ray, double factor)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `factor` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` Scale(geometry::`[`Ray3D`](Ray3D.md)` ray, double sx, double sy, double sz)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Line3D`](Line3D.md)` Scale(geometry::`[`Line3D`](Line3D.md)` line, double factor)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `factor` (`double`)

`geometry::`[`Line3D`](Line3D.md)` Scale(geometry::`[`Line3D`](Line3D.md)` line, double sx, double sy, double sz)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Scale(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, double factor)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `factor` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Scale(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, double sx, double sy, double sz)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Scale(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, double factor)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `factor` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Scale(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, double sx, double sy, double sz)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Scale(geometry::`[`Polygon3D`](Polygon3D.md)` poly, double factor)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `factor` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Scale(geometry::`[`Polygon3D`](Polygon3D.md)` poly, double sx, double sy, double sz)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Scale(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, double factor)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `factor` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Scale(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, double sx, double sy, double sz)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Scale(geometry::`[`Triangle3D`](Triangle3D.md)` tri, double factor)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `factor` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Scale(geometry::`[`Triangle3D`](Triangle3D.md)` tri, double sx, double sy, double sz)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Scale(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `factor` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Scale(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Scale(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `factor` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Scale(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Scale(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, double factor)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `factor` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Scale(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `Shear`

`geometry::`[`Point2D`](Point2D.md)` Shear(geometry::`[`Point2D`](Point2D.md)` p, double shx, double shy)`

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Vector2D`](Vector2D.md)` Shear(geometry::`[`Vector2D`](Vector2D.md)` v, double shx, double shy)`

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Ray2D`](Ray2D.md)` Shear(geometry::`[`Ray2D`](Ray2D.md)` ray, double shx, double shy)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Line2D`](Line2D.md)` Shear(geometry::`[`Line2D`](Line2D.md)` line, double shx, double shy)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Shear(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, double shx, double shy)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Shear(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, double shx, double shy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Shear(geometry::`[`Polygon2D`](Polygon2D.md)` poly, double shx, double shy)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Shear(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, double shx, double shy)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Shear(geometry::`[`Triangle2D`](Triangle2D.md)` tri, double shx, double shy)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Shear(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Shear(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Shear(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, double shx, double shy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `shx` (`double`)
- `shy` (`double`)

`geometry::`[`Point3D`](Point3D.md)` Shear(geometry::`[`Point3D`](Point3D.md)` p, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Vector3D`](Vector3D.md)` Shear(geometry::`[`Vector3D`](Vector3D.md)` v, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Ray3D`](Ray3D.md)` Shear(geometry::`[`Ray3D`](Ray3D.md)` ray, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Line3D`](Line3D.md)` Shear(geometry::`[`Line3D`](Line3D.md)` line, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Shear(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Shear(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Shear(geometry::`[`Polygon3D`](Polygon3D.md)` poly, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Shear(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Shear(geometry::`[`Triangle3D`](Triangle3D.md)` tri, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Shear(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Shear(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Shear(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `Reflect`

`geometry::`[`Point2D`](Point2D.md)` Reflect(geometry::`[`Point2D`](Point2D.md)` p, maths::Vector2 normal)`

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point2D`](Point2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Vector2D`](Vector2D.md)` Reflect(geometry::`[`Vector2D`](Vector2D.md)` v, maths::Vector2 normal)`

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector2D`](Vector2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Ray2D`](Ray2D.md)` Reflect(geometry::`[`Ray2D`](Ray2D.md)` ray, maths::Vector2 normal)`


**Parameters**

- `ray` (geometry::[`Ray2D`](Ray2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Line2D`](Line2D.md)` Reflect(geometry::`[`Line2D`](Line2D.md)` line, maths::Vector2 normal)`


**Parameters**

- `line` (geometry::[`Line2D`](Line2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`LineSegment2D`](LineSegment2D.md)` Reflect(geometry::`[`LineSegment2D`](LineSegment2D.md)` seg, maths::Vector2 normal)`


**Parameters**

- `seg` (geometry::[`LineSegment2D`](LineSegment2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` Reflect(geometry::`[`GeometryCollection2D`](GeometryCollection2D.md)` collection, maths::Vector2 normal)`


**Parameters**

- `collection` (geometry::[`GeometryCollection2D`](GeometryCollection2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Polygon2D`](Polygon2D.md)` Reflect(geometry::`[`Polygon2D`](Polygon2D.md)` poly, maths::Vector2 normal)`


**Parameters**

- `poly` (geometry::[`Polygon2D`](Polygon2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Polyline2D`](Polyline2D.md)` Reflect(geometry::`[`Polyline2D`](Polyline2D.md)` polyline, maths::Vector2 normal)`


**Parameters**

- `polyline` (geometry::[`Polyline2D`](Polyline2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Triangle2D`](Triangle2D.md)` Reflect(geometry::`[`Triangle2D`](Triangle2D.md)` tri, maths::Vector2 normal)`


**Parameters**

- `tri` (geometry::[`Triangle2D`](Triangle2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Mesh2D`](Mesh2D.md)` Reflect(geometry::`[`Mesh2D`](Mesh2D.md)` mesh, maths::Vector2 normal)`


**Parameters**

- `mesh` (geometry::[`Mesh2D`](Mesh2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`PolyMesh2D`](PolyMesh2D.md)` Reflect(geometry::`[`PolyMesh2D`](PolyMesh2D.md)` mesh, maths::Vector2 normal)`


**Parameters**

- `mesh` (geometry::[`PolyMesh2D`](PolyMesh2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` Reflect(geometry::`[`ConnectedMesh2D`](ConnectedMesh2D.md)` mesh, maths::Vector2 normal)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh2D`](ConnectedMesh2D.md))
- `normal` (`maths::Vector2`)

`geometry::`[`Point3D`](Point3D.md)` Reflect(geometry::`[`Point3D`](Point3D.md)` p, maths::Vector3 normal)`

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` (geometry::[`Point3D`](Point3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Vector3D`](Vector3D.md)` Reflect(geometry::`[`Vector3D`](Vector3D.md)` v, maths::Vector3 normal)`

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` (geometry::[`Vector3D`](Vector3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Ray3D`](Ray3D.md)` Reflect(geometry::`[`Ray3D`](Ray3D.md)` ray, maths::Vector3 normal)`


**Parameters**

- `ray` (geometry::[`Ray3D`](Ray3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Line3D`](Line3D.md)` Reflect(geometry::`[`Line3D`](Line3D.md)` line, maths::Vector3 normal)`


**Parameters**

- `line` (geometry::[`Line3D`](Line3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`LineSegment3D`](LineSegment3D.md)` Reflect(geometry::`[`LineSegment3D`](LineSegment3D.md)` seg, maths::Vector3 normal)`


**Parameters**

- `seg` (geometry::[`LineSegment3D`](LineSegment3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` Reflect(geometry::`[`GeometryCollection3D`](GeometryCollection3D.md)` collection, maths::Vector3 normal)`


**Parameters**

- `collection` (geometry::[`GeometryCollection3D`](GeometryCollection3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Polygon3D`](Polygon3D.md)` Reflect(geometry::`[`Polygon3D`](Polygon3D.md)` poly, maths::Vector3 normal)`


**Parameters**

- `poly` (geometry::[`Polygon3D`](Polygon3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Polyline3D`](Polyline3D.md)` Reflect(geometry::`[`Polyline3D`](Polyline3D.md)` polyline, maths::Vector3 normal)`


**Parameters**

- `polyline` (geometry::[`Polyline3D`](Polyline3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Triangle3D`](Triangle3D.md)` Reflect(geometry::`[`Triangle3D`](Triangle3D.md)` tri, maths::Vector3 normal)`


**Parameters**

- `tri` (geometry::[`Triangle3D`](Triangle3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`Mesh3D`](Mesh3D.md)` Reflect(geometry::`[`Mesh3D`](Mesh3D.md)` mesh, maths::Vector3 normal)`


**Parameters**

- `mesh` (geometry::[`Mesh3D`](Mesh3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`PolyMesh3D`](PolyMesh3D.md)` Reflect(geometry::`[`PolyMesh3D`](PolyMesh3D.md)` mesh, maths::Vector3 normal)`


**Parameters**

- `mesh` (geometry::[`PolyMesh3D`](PolyMesh3D.md))
- `normal` (`maths::Vector3`)

`geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` Reflect(geometry::`[`ConnectedMesh3D`](ConnectedMesh3D.md)` mesh, maths::Vector3 normal)`


**Parameters**

- `mesh` (geometry::[`ConnectedMesh3D`](ConnectedMesh3D.md))
- `normal` (`maths::Vector3`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
