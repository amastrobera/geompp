# `TransformationsFreeFunctions` (C# / .NET)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `Transform`

[`Polygon2D`](Polygon2D.md)`^ Transform(`[`Polygon2D`](Polygon2D.md)`^ poly, Matrix3^ m)`

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `m` (`Matrix3^`)

[`Polygon3D`](Polygon3D.md)`^ Transform(`[`Polygon3D`](Polygon3D.md)`^ poly, Matrix4^ m)`

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `m` (`Matrix4^`)

[`Point2D`](Point2D.md)`^ Transform(`[`Point2D`](Point2D.md)`^ p, Matrix3^ m)`

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `m` (`Matrix3^`)

[`Vector2D`](Vector2D.md)`^ Transform(`[`Vector2D`](Vector2D.md)`^ v, Matrix3^ m)`

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `m` (`Matrix3^`)

[`Ray2D`](Ray2D.md)`^ Transform(`[`Ray2D`](Ray2D.md)`^ ray, Matrix3^ m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `m` (`Matrix3^`)

[`Line2D`](Line2D.md)`^ Transform(`[`Line2D`](Line2D.md)`^ line, Matrix3^ m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `m` (`Matrix3^`)

[`LineSegment2D`](LineSegment2D.md)`^ Transform(`[`LineSegment2D`](LineSegment2D.md)`^ seg, Matrix3^ m)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `m` (`Matrix3^`)

[`Polyline2D`](Polyline2D.md)`^ Transform(`[`Polyline2D`](Polyline2D.md)`^ polyline, Matrix3^ m)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `m` (`Matrix3^`)

[`Triangle2D`](Triangle2D.md)`^ Transform(`[`Triangle2D`](Triangle2D.md)`^ tri, Matrix3^ m)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `m` (`Matrix3^`)

[`Mesh2D`](Mesh2D.md)`^ Transform(`[`Mesh2D`](Mesh2D.md)`^ mesh, Matrix3^ m)`

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `m` (`Matrix3^`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Transform(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, Matrix3^ m)`

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `m` (`Matrix3^`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Transform(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, Matrix3^ m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `m` (`Matrix3^`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Transform(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, Matrix3^ m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `m` (`Matrix3^`)

[`Point3D`](Point3D.md)`^ Transform(`[`Point3D`](Point3D.md)`^ p, Matrix4^ m)`

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `m` (`Matrix4^`)

[`Vector3D`](Vector3D.md)`^ Transform(`[`Vector3D`](Vector3D.md)`^ v, Matrix4^ m)`

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `m` (`Matrix4^`)

[`Ray3D`](Ray3D.md)`^ Transform(`[`Ray3D`](Ray3D.md)`^ ray, Matrix4^ m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `m` (`Matrix4^`)

[`Line3D`](Line3D.md)`^ Transform(`[`Line3D`](Line3D.md)`^ line, Matrix4^ m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `m` (`Matrix4^`)

[`LineSegment3D`](LineSegment3D.md)`^ Transform(`[`LineSegment3D`](LineSegment3D.md)`^ seg, Matrix4^ m)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `m` (`Matrix4^`)

[`Polyline3D`](Polyline3D.md)`^ Transform(`[`Polyline3D`](Polyline3D.md)`^ polyline, Matrix4^ m)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `m` (`Matrix4^`)

[`Triangle3D`](Triangle3D.md)`^ Transform(`[`Triangle3D`](Triangle3D.md)`^ tri, Matrix4^ m)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `m` (`Matrix4^`)

[`Mesh3D`](Mesh3D.md)`^ Transform(`[`Mesh3D`](Mesh3D.md)`^ mesh, Matrix4^ m)`

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `m` (`Matrix4^`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Transform(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, Matrix4^ m)`

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `m` (`Matrix4^`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Transform(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, Matrix4^ m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `m` (`Matrix4^`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Transform(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, Matrix4^ m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `m` (`Matrix4^`)

## `Translate`

[`Point2D`](Point2D.md)`^ Translate(`[`Point2D`](Point2D.md)`^ p, Vector2^ offset)`

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `offset` (`Vector2^`)

[`Ray2D`](Ray2D.md)`^ Translate(`[`Ray2D`](Ray2D.md)`^ ray, Vector2^ offset)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `offset` (`Vector2^`)

[`Line2D`](Line2D.md)`^ Translate(`[`Line2D`](Line2D.md)`^ line, Vector2^ offset)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `offset` (`Vector2^`)

[`LineSegment2D`](LineSegment2D.md)`^ Translate(`[`LineSegment2D`](LineSegment2D.md)`^ seg, Vector2^ offset)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `offset` (`Vector2^`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Translate(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, Vector2^ offset)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `offset` (`Vector2^`)

[`Polygon2D`](Polygon2D.md)`^ Translate(`[`Polygon2D`](Polygon2D.md)`^ poly, Vector2^ offset)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `offset` (`Vector2^`)

[`Polyline2D`](Polyline2D.md)`^ Translate(`[`Polyline2D`](Polyline2D.md)`^ polyline, Vector2^ offset)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `offset` (`Vector2^`)

[`Triangle2D`](Triangle2D.md)`^ Translate(`[`Triangle2D`](Triangle2D.md)`^ tri, Vector2^ offset)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `offset` (`Vector2^`)

[`Mesh2D`](Mesh2D.md)`^ Translate(`[`Mesh2D`](Mesh2D.md)`^ mesh, Vector2^ offset)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `offset` (`Vector2^`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Translate(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, Vector2^ offset)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `offset` (`Vector2^`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Translate(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, Vector2^ offset)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `offset` (`Vector2^`)

[`Point3D`](Point3D.md)`^ Translate(`[`Point3D`](Point3D.md)`^ p, Vector3^ offset)`

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `offset` (`Vector3^`)

[`Ray3D`](Ray3D.md)`^ Translate(`[`Ray3D`](Ray3D.md)`^ ray, Vector3^ offset)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `offset` (`Vector3^`)

[`Line3D`](Line3D.md)`^ Translate(`[`Line3D`](Line3D.md)`^ line, Vector3^ offset)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `offset` (`Vector3^`)

[`LineSegment3D`](LineSegment3D.md)`^ Translate(`[`LineSegment3D`](LineSegment3D.md)`^ seg, Vector3^ offset)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `offset` (`Vector3^`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Translate(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, Vector3^ offset)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `offset` (`Vector3^`)

[`Polygon3D`](Polygon3D.md)`^ Translate(`[`Polygon3D`](Polygon3D.md)`^ poly, Vector3^ offset)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `offset` (`Vector3^`)

[`Polyline3D`](Polyline3D.md)`^ Translate(`[`Polyline3D`](Polyline3D.md)`^ polyline, Vector3^ offset)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `offset` (`Vector3^`)

[`Triangle3D`](Triangle3D.md)`^ Translate(`[`Triangle3D`](Triangle3D.md)`^ tri, Vector3^ offset)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `offset` (`Vector3^`)

[`Mesh3D`](Mesh3D.md)`^ Translate(`[`Mesh3D`](Mesh3D.md)`^ mesh, Vector3^ offset)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `offset` (`Vector3^`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Translate(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, Vector3^ offset)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `offset` (`Vector3^`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Translate(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, Vector3^ offset)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `offset` (`Vector3^`)

## `Rotate`

[`Point2D`](Point2D.md)`^ Rotate(`[`Point2D`](Point2D.md)`^ p, double angle_rad)`

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `angle_rad` (`double`)

[`Vector2D`](Vector2D.md)`^ Rotate(`[`Vector2D`](Vector2D.md)`^ v, double angle_rad)`

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `angle_rad` (`double`)

[`Ray2D`](Ray2D.md)`^ Rotate(`[`Ray2D`](Ray2D.md)`^ ray, double angle_rad)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `angle_rad` (`double`)

[`Line2D`](Line2D.md)`^ Rotate(`[`Line2D`](Line2D.md)`^ line, double angle_rad)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `angle_rad` (`double`)

[`LineSegment2D`](LineSegment2D.md)`^ Rotate(`[`LineSegment2D`](LineSegment2D.md)`^ seg, double angle_rad)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `angle_rad` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Rotate(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, double angle_rad)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `angle_rad` (`double`)

[`Polygon2D`](Polygon2D.md)`^ Rotate(`[`Polygon2D`](Polygon2D.md)`^ poly, double angle_rad)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `angle_rad` (`double`)

[`Polyline2D`](Polyline2D.md)`^ Rotate(`[`Polyline2D`](Polyline2D.md)`^ polyline, double angle_rad)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `angle_rad` (`double`)

[`Triangle2D`](Triangle2D.md)`^ Rotate(`[`Triangle2D`](Triangle2D.md)`^ tri, double angle_rad)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `angle_rad` (`double`)

[`Mesh2D`](Mesh2D.md)`^ Rotate(`[`Mesh2D`](Mesh2D.md)`^ mesh, double angle_rad)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `angle_rad` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Rotate(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, double angle_rad)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `angle_rad` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Rotate(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, double angle_rad)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `angle_rad` (`double`)

[`Point3D`](Point3D.md)`^ Rotate(`[`Point3D`](Point3D.md)`^ p, double angle_rad, Vector3^ axis)`

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Vector3D`](Vector3D.md)`^ Rotate(`[`Vector3D`](Vector3D.md)`^ v, double angle_rad, Vector3^ axis)`

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Ray3D`](Ray3D.md)`^ Rotate(`[`Ray3D`](Ray3D.md)`^ ray, double angle_rad, Vector3^ axis)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Line3D`](Line3D.md)`^ Rotate(`[`Line3D`](Line3D.md)`^ line, double angle_rad, Vector3^ axis)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`LineSegment3D`](LineSegment3D.md)`^ Rotate(`[`LineSegment3D`](LineSegment3D.md)`^ seg, double angle_rad, Vector3^ axis)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Rotate(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, double angle_rad, Vector3^ axis)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Polygon3D`](Polygon3D.md)`^ Rotate(`[`Polygon3D`](Polygon3D.md)`^ poly, double angle_rad, Vector3^ axis)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Polyline3D`](Polyline3D.md)`^ Rotate(`[`Polyline3D`](Polyline3D.md)`^ polyline, double angle_rad, Vector3^ axis)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Triangle3D`](Triangle3D.md)`^ Rotate(`[`Triangle3D`](Triangle3D.md)`^ tri, double angle_rad, Vector3^ axis)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`Mesh3D`](Mesh3D.md)`^ Rotate(`[`Mesh3D`](Mesh3D.md)`^ mesh, double angle_rad, Vector3^ axis)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Rotate(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, double angle_rad, Vector3^ axis)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Rotate(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, double angle_rad, Vector3^ axis)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `angle_rad` (`double`)
- `axis` (`Vector3^`)

## `Scale`

[`Point2D`](Point2D.md)`^ Scale(`[`Point2D`](Point2D.md)`^ p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `factor` (`double`)

[`Point2D`](Point2D.md)`^ Scale(`[`Point2D`](Point2D.md)`^ p, double sx, double sy)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Vector2D`](Vector2D.md)`^ Scale(`[`Vector2D`](Vector2D.md)`^ v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `factor` (`double`)

[`Vector2D`](Vector2D.md)`^ Scale(`[`Vector2D`](Vector2D.md)`^ v, double sx, double sy)`

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Ray2D`](Ray2D.md)`^ Scale(`[`Ray2D`](Ray2D.md)`^ ray, double factor)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `factor` (`double`)

[`Ray2D`](Ray2D.md)`^ Scale(`[`Ray2D`](Ray2D.md)`^ ray, double sx, double sy)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Line2D`](Line2D.md)`^ Scale(`[`Line2D`](Line2D.md)`^ line, double factor)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `factor` (`double`)

[`Line2D`](Line2D.md)`^ Scale(`[`Line2D`](Line2D.md)`^ line, double sx, double sy)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`LineSegment2D`](LineSegment2D.md)`^ Scale(`[`LineSegment2D`](LineSegment2D.md)`^ seg, double factor)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `factor` (`double`)

[`LineSegment2D`](LineSegment2D.md)`^ Scale(`[`LineSegment2D`](LineSegment2D.md)`^ seg, double sx, double sy)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Scale(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, double factor)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `factor` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Scale(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, double sx, double sy)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Polygon2D`](Polygon2D.md)`^ Scale(`[`Polygon2D`](Polygon2D.md)`^ poly, double factor)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `factor` (`double`)

[`Polygon2D`](Polygon2D.md)`^ Scale(`[`Polygon2D`](Polygon2D.md)`^ poly, double sx, double sy)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Polyline2D`](Polyline2D.md)`^ Scale(`[`Polyline2D`](Polyline2D.md)`^ polyline, double factor)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `factor` (`double`)

[`Polyline2D`](Polyline2D.md)`^ Scale(`[`Polyline2D`](Polyline2D.md)`^ polyline, double sx, double sy)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Triangle2D`](Triangle2D.md)`^ Scale(`[`Triangle2D`](Triangle2D.md)`^ tri, double factor)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `factor` (`double`)

[`Triangle2D`](Triangle2D.md)`^ Scale(`[`Triangle2D`](Triangle2D.md)`^ tri, double sx, double sy)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Mesh2D`](Mesh2D.md)`^ Scale(`[`Mesh2D`](Mesh2D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `factor` (`double`)

[`Mesh2D`](Mesh2D.md)`^ Scale(`[`Mesh2D`](Mesh2D.md)`^ mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Scale(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `factor` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Scale(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Scale(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `factor` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Scale(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `sx` (`double`)
- `sy` (`double`)

[`Point3D`](Point3D.md)`^ Scale(`[`Point3D`](Point3D.md)`^ p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `factor` (`double`)

[`Point3D`](Point3D.md)`^ Scale(`[`Point3D`](Point3D.md)`^ p, double sx, double sy, double sz)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Vector3D`](Vector3D.md)`^ Scale(`[`Vector3D`](Vector3D.md)`^ v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `factor` (`double`)

[`Vector3D`](Vector3D.md)`^ Scale(`[`Vector3D`](Vector3D.md)`^ v, double sx, double sy, double sz)`

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Ray3D`](Ray3D.md)`^ Scale(`[`Ray3D`](Ray3D.md)`^ ray, double factor)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `factor` (`double`)

[`Ray3D`](Ray3D.md)`^ Scale(`[`Ray3D`](Ray3D.md)`^ ray, double sx, double sy, double sz)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Line3D`](Line3D.md)`^ Scale(`[`Line3D`](Line3D.md)`^ line, double factor)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `factor` (`double`)

[`Line3D`](Line3D.md)`^ Scale(`[`Line3D`](Line3D.md)`^ line, double sx, double sy, double sz)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`LineSegment3D`](LineSegment3D.md)`^ Scale(`[`LineSegment3D`](LineSegment3D.md)`^ seg, double factor)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `factor` (`double`)

[`LineSegment3D`](LineSegment3D.md)`^ Scale(`[`LineSegment3D`](LineSegment3D.md)`^ seg, double sx, double sy, double sz)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Scale(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, double factor)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `factor` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Scale(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, double sx, double sy, double sz)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Polygon3D`](Polygon3D.md)`^ Scale(`[`Polygon3D`](Polygon3D.md)`^ poly, double factor)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `factor` (`double`)

[`Polygon3D`](Polygon3D.md)`^ Scale(`[`Polygon3D`](Polygon3D.md)`^ poly, double sx, double sy, double sz)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Polyline3D`](Polyline3D.md)`^ Scale(`[`Polyline3D`](Polyline3D.md)`^ polyline, double factor)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `factor` (`double`)

[`Polyline3D`](Polyline3D.md)`^ Scale(`[`Polyline3D`](Polyline3D.md)`^ polyline, double sx, double sy, double sz)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Triangle3D`](Triangle3D.md)`^ Scale(`[`Triangle3D`](Triangle3D.md)`^ tri, double factor)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `factor` (`double`)

[`Triangle3D`](Triangle3D.md)`^ Scale(`[`Triangle3D`](Triangle3D.md)`^ tri, double sx, double sy, double sz)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Mesh3D`](Mesh3D.md)`^ Scale(`[`Mesh3D`](Mesh3D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `factor` (`double`)

[`Mesh3D`](Mesh3D.md)`^ Scale(`[`Mesh3D`](Mesh3D.md)`^ mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Scale(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `factor` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Scale(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Scale(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, double factor)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `factor` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Scale(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `Shear`

[`Point2D`](Point2D.md)`^ Shear(`[`Point2D`](Point2D.md)`^ p, double shx, double shy)`

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Vector2D`](Vector2D.md)`^ Shear(`[`Vector2D`](Vector2D.md)`^ v, double shx, double shy)`

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Ray2D`](Ray2D.md)`^ Shear(`[`Ray2D`](Ray2D.md)`^ ray, double shx, double shy)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Line2D`](Line2D.md)`^ Shear(`[`Line2D`](Line2D.md)`^ line, double shx, double shy)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`LineSegment2D`](LineSegment2D.md)`^ Shear(`[`LineSegment2D`](LineSegment2D.md)`^ seg, double shx, double shy)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Shear(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, double shx, double shy)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Polygon2D`](Polygon2D.md)`^ Shear(`[`Polygon2D`](Polygon2D.md)`^ poly, double shx, double shy)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Polyline2D`](Polyline2D.md)`^ Shear(`[`Polyline2D`](Polyline2D.md)`^ polyline, double shx, double shy)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Triangle2D`](Triangle2D.md)`^ Shear(`[`Triangle2D`](Triangle2D.md)`^ tri, double shx, double shy)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Mesh2D`](Mesh2D.md)`^ Shear(`[`Mesh2D`](Mesh2D.md)`^ mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Shear(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Shear(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `shx` (`double`)
- `shy` (`double`)

[`Point3D`](Point3D.md)`^ Shear(`[`Point3D`](Point3D.md)`^ p, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Vector3D`](Vector3D.md)`^ Shear(`[`Vector3D`](Vector3D.md)`^ v, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Ray3D`](Ray3D.md)`^ Shear(`[`Ray3D`](Ray3D.md)`^ ray, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Line3D`](Line3D.md)`^ Shear(`[`Line3D`](Line3D.md)`^ line, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`LineSegment3D`](LineSegment3D.md)`^ Shear(`[`LineSegment3D`](LineSegment3D.md)`^ seg, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Shear(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Polygon3D`](Polygon3D.md)`^ Shear(`[`Polygon3D`](Polygon3D.md)`^ poly, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Polyline3D`](Polyline3D.md)`^ Shear(`[`Polyline3D`](Polyline3D.md)`^ polyline, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Triangle3D`](Triangle3D.md)`^ Shear(`[`Triangle3D`](Triangle3D.md)`^ tri, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Mesh3D`](Mesh3D.md)`^ Shear(`[`Mesh3D`](Mesh3D.md)`^ mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Shear(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Shear(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `Reflect`

[`Point2D`](Point2D.md)`^ Reflect(`[`Point2D`](Point2D.md)`^ p, Vector2^ normal)`

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)
- `normal` (`Vector2^`)

[`Vector2D`](Vector2D.md)`^ Reflect(`[`Vector2D`](Vector2D.md)`^ v, Vector2^ normal)`

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md)^)
- `normal` (`Vector2^`)

[`Ray2D`](Ray2D.md)`^ Reflect(`[`Ray2D`](Ray2D.md)`^ ray, Vector2^ normal)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)
- `normal` (`Vector2^`)

[`Line2D`](Line2D.md)`^ Reflect(`[`Line2D`](Line2D.md)`^ line, Vector2^ normal)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)
- `normal` (`Vector2^`)

[`LineSegment2D`](LineSegment2D.md)`^ Reflect(`[`LineSegment2D`](LineSegment2D.md)`^ seg, Vector2^ normal)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)
- `normal` (`Vector2^`)

[`GeometryCollection2D`](GeometryCollection2D.md)`^ Reflect(`[`GeometryCollection2D`](GeometryCollection2D.md)`^ collection, Vector2^ normal)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md)^)
- `normal` (`Vector2^`)

[`Polygon2D`](Polygon2D.md)`^ Reflect(`[`Polygon2D`](Polygon2D.md)`^ poly, Vector2^ normal)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md)^)
- `normal` (`Vector2^`)

[`Polyline2D`](Polyline2D.md)`^ Reflect(`[`Polyline2D`](Polyline2D.md)`^ polyline, Vector2^ normal)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)
- `normal` (`Vector2^`)

[`Triangle2D`](Triangle2D.md)`^ Reflect(`[`Triangle2D`](Triangle2D.md)`^ tri, Vector2^ normal)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md)^)
- `normal` (`Vector2^`)

[`Mesh2D`](Mesh2D.md)`^ Reflect(`[`Mesh2D`](Mesh2D.md)`^ mesh, Vector2^ normal)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md)^)
- `normal` (`Vector2^`)

[`PolyMesh2D`](PolyMesh2D.md)`^ Reflect(`[`PolyMesh2D`](PolyMesh2D.md)`^ mesh, Vector2^ normal)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md)^)
- `normal` (`Vector2^`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ Reflect(`[`ConnectedMesh2D`](ConnectedMesh2D.md)`^ mesh, Vector2^ normal)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md)^)
- `normal` (`Vector2^`)

[`Point3D`](Point3D.md)`^ Reflect(`[`Point3D`](Point3D.md)`^ p, Vector3^ normal)`

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `normal` (`Vector3^`)

[`Vector3D`](Vector3D.md)`^ Reflect(`[`Vector3D`](Vector3D.md)`^ v, Vector3^ normal)`

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md)^)
- `normal` (`Vector3^`)

[`Ray3D`](Ray3D.md)`^ Reflect(`[`Ray3D`](Ray3D.md)`^ ray, Vector3^ normal)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)
- `normal` (`Vector3^`)

[`Line3D`](Line3D.md)`^ Reflect(`[`Line3D`](Line3D.md)`^ line, Vector3^ normal)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)
- `normal` (`Vector3^`)

[`LineSegment3D`](LineSegment3D.md)`^ Reflect(`[`LineSegment3D`](LineSegment3D.md)`^ seg, Vector3^ normal)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)
- `normal` (`Vector3^`)

[`GeometryCollection3D`](GeometryCollection3D.md)`^ Reflect(`[`GeometryCollection3D`](GeometryCollection3D.md)`^ collection, Vector3^ normal)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md)^)
- `normal` (`Vector3^`)

[`Polygon3D`](Polygon3D.md)`^ Reflect(`[`Polygon3D`](Polygon3D.md)`^ poly, Vector3^ normal)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md)^)
- `normal` (`Vector3^`)

[`Polyline3D`](Polyline3D.md)`^ Reflect(`[`Polyline3D`](Polyline3D.md)`^ polyline, Vector3^ normal)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)
- `normal` (`Vector3^`)

[`Triangle3D`](Triangle3D.md)`^ Reflect(`[`Triangle3D`](Triangle3D.md)`^ tri, Vector3^ normal)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md)^)
- `normal` (`Vector3^`)

[`Mesh3D`](Mesh3D.md)`^ Reflect(`[`Mesh3D`](Mesh3D.md)`^ mesh, Vector3^ normal)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md)^)
- `normal` (`Vector3^`)

[`PolyMesh3D`](PolyMesh3D.md)`^ Reflect(`[`PolyMesh3D`](PolyMesh3D.md)`^ mesh, Vector3^ normal)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md)^)
- `normal` (`Vector3^`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Reflect(`[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ mesh, Vector3^ normal)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md)^)
- `normal` (`Vector3^`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
