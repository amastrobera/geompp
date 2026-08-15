# `TransformationsFreeFunctions` (C++)

Free functions in `geompp::transformations` (not methods on a class — call them directly).

## `transform`

[`Polygon2D`](Polygon2D.md)` transform(`[`Polygon2D`](Polygon2D.md)` const & poly, Matrix3 const & m)`

Transforms every vertex of both the outer ring and every hole ring.

A pure rotation/ translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant m , e.g. Matrix3::Scale(-1, 1) ) flips it, same as it would for a hand-reversed point list callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later feed the result back through something that assumes CCW/CW.

**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `m` (`Matrix3 const &`)

[`Polygon3D`](Polygon3D.md)` transform(`[`Polygon3D`](Polygon3D.md)` const & poly, Matrix4 const & m)`

Transforms every vertex of both the outer ring and every hole ring.

See the 2D overload's docs for the winding-flip note under a reflection matrix.

**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `m` (`Matrix4 const &`)

[`Point2D`](Point2D.md)` transform(`[`Point2D`](Point2D.md)` const & p, Matrix3 const & m)`

Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to Cartesian (x', y').

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `m` (`Matrix3 const &`)

[`Vector2D`](Vector2D.md)` transform(`[`Vector2D`](Vector2D.md)` const & v, Matrix3 const & m)`

Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) the 0 in the homogeneous coordinate means any translation component of m has no effect, only rotation/scale/ shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `m` (`Matrix3 const &`)

[`Ray2D`](Ray2D.md)` transform(`[`Ray2D`](Ray2D.md)` const & ray, Matrix3 const & m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray2D](Ray2D.md)::Make().

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `m` (`Matrix3 const &`)

[`Line2D`](Line2D.md)` transform(`[`Line2D`](Line2D.md)` const & line, Matrix3 const & m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line2D](Line2D.md)::Make().

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `m` (`Matrix3 const &`)

[`LineSegment2D`](LineSegment2D.md)` transform(`[`LineSegment2D`](LineSegment2D.md)` const & seg, Matrix3 const & m)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `m` (`Matrix3 const &`)

[`Polyline2D`](Polyline2D.md)` transform(`[`Polyline2D`](Polyline2D.md)` const & polyline, Matrix3 const & m)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `m` (`Matrix3 const &`)

[`Triangle2D`](Triangle2D.md)` transform(`[`Triangle2D`](Triangle2D.md)` const & tri, Matrix3 const & m)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `m` (`Matrix3 const &`)

[`Mesh2D`](Mesh2D.md)` transform(`[`Mesh2D`](Mesh2D.md)` const & mesh, Matrix3 const & m)`

Transforms every facet independently and rebuilds via [Mesh2D](Mesh2D.md)::FromTriangles() cheaper than it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other mesh construction uses.

**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `m` (`Matrix3 const &`)

[`PolyMesh2D`](PolyMesh2D.md)` transform(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, Matrix3 const & m)`

Transforms every facet independently and rebuilds via [PolyMesh2D](PolyMesh2D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `m` (`Matrix3 const &`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` transform(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, Matrix3 const & m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle2D](Triangle2D.md) ) and rebuilds via [ConnectedMesh2D](ConnectedMesh2D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh2D](ConnectedMesh2D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `m` (`Matrix3 const &`)

[`GeometryCollection2D`](GeometryCollection2D.md)` transform(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, Matrix3 const & m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection2D](GeometryCollection2D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `m` (`Matrix3 const &`)

[`Point3D`](Point3D.md)` transform(`[`Point3D`](Point3D.md)` const & p, Matrix4 const & m)`

Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops back to Cartesian (x', y', z').

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `m` (`Matrix4 const &`)

[`Vector3D`](Vector3D.md)` transform(`[`Vector3D`](Vector3D.md)` const & v, Matrix4 const & m)`

Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) any translation component of m has no effect, only rotation/scale/shear do (the correct behavior for a displacement, which has no position to translate).

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `m` (`Matrix4 const &`)

[`Ray3D`](Ray3D.md)` transform(`[`Ray3D`](Ray3D.md)` const & ray, Matrix4 const & m)`

Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect it) and rebuilds via [Ray3D](Ray3D.md)::Make().

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `m` (`Matrix4 const &`)

[`Line3D`](Line3D.md)` transform(`[`Line3D`](Line3D.md)` const & line, Matrix4 const & m)`

Transforms the origin (as a point) and direction (as a vector) and rebuilds via [Line3D](Line3D.md)::Make().

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `m` (`Matrix4 const &`)

[`LineSegment3D`](LineSegment3D.md)` transform(`[`LineSegment3D`](LineSegment3D.md)` const & seg, Matrix4 const & m)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `m` (`Matrix4 const &`)

[`Polyline3D`](Polyline3D.md)` transform(`[`Polyline3D`](Polyline3D.md)` const & polyline, Matrix4 const & m)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `m` (`Matrix4 const &`)

[`Triangle3D`](Triangle3D.md)` transform(`[`Triangle3D`](Triangle3D.md)` const & tri, Matrix4 const & m)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `m` (`Matrix4 const &`)

[`Mesh3D`](Mesh3D.md)` transform(`[`Mesh3D`](Mesh3D.md)` const & mesh, Matrix4 const & m)`

Transforms every facet independently and rebuilds via [Mesh3D](Mesh3D.md)::FromTriangles().

**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `m` (`Matrix4 const &`)

[`PolyMesh3D`](PolyMesh3D.md)` transform(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, Matrix4 const & m)`

Transforms every facet independently and rebuilds via [PolyMesh3D](PolyMesh3D.md)::FromPolygons().

**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `m` (`Matrix4 const &`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` transform(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, Matrix4 const & m)`

Transforms every facet (via Faces(), each rebuilt as a [Triangle3D](Triangle3D.md) ) and rebuilds via [ConnectedMesh3D](ConnectedMesh3D.md)::FromTriangles() adjacency is recomputed from scratch, same as building a fresh [ConnectedMesh3D](ConnectedMesh3D.md) from any other triangle set.

**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `m` (`Matrix4 const &`)

[`GeometryCollection3D`](GeometryCollection3D.md)` transform(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, Matrix4 const & m)`

Transforms every contained geometry (recursively, for nested [GeometryCollection3D](GeometryCollection3D.md) ) and rebuilds a fresh collection via Add().

**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `m` (`Matrix4 const &`)

## `translate`

[`Point2D`](Point2D.md)` translate(`[`Point2D`](Point2D.md)` const & p, Vector2 const & offset)`

Translates a point by a plain (x, y) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `offset` (`Vector2 const &`)

[`Ray2D`](Ray2D.md)` translate(`[`Ray2D`](Ray2D.md)` const & ray, Vector2 const & offset)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `offset` (`Vector2 const &`)

[`Line2D`](Line2D.md)` translate(`[`Line2D`](Line2D.md)` const & line, Vector2 const & offset)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `offset` (`Vector2 const &`)

[`LineSegment2D`](LineSegment2D.md)` translate(`[`LineSegment2D`](LineSegment2D.md)` const & seg, Vector2 const & offset)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `offset` (`Vector2 const &`)

[`GeometryCollection2D`](GeometryCollection2D.md)` translate(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, Vector2 const & offset)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `offset` (`Vector2 const &`)

[`Polygon2D`](Polygon2D.md)` translate(`[`Polygon2D`](Polygon2D.md)` const & poly, Vector2 const & offset)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `offset` (`Vector2 const &`)

[`Polyline2D`](Polyline2D.md)` translate(`[`Polyline2D`](Polyline2D.md)` const & polyline, Vector2 const & offset)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `offset` (`Vector2 const &`)

[`Triangle2D`](Triangle2D.md)` translate(`[`Triangle2D`](Triangle2D.md)` const & tri, Vector2 const & offset)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `offset` (`Vector2 const &`)

[`Mesh2D`](Mesh2D.md)` translate(`[`Mesh2D`](Mesh2D.md)` const & mesh, Vector2 const & offset)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `offset` (`Vector2 const &`)

[`PolyMesh2D`](PolyMesh2D.md)` translate(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, Vector2 const & offset)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `offset` (`Vector2 const &`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` translate(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, Vector2 const & offset)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `offset` (`Vector2 const &`)

[`Point3D`](Point3D.md)` translate(`[`Point3D`](Point3D.md)` const & p, Vector3 const & offset)`

Translates a point by a plain (x, y, z) offset. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `offset` (`Vector3 const &`)

[`Ray3D`](Ray3D.md)` translate(`[`Ray3D`](Ray3D.md)` const & ray, Vector3 const & offset)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `offset` (`Vector3 const &`)

[`Line3D`](Line3D.md)` translate(`[`Line3D`](Line3D.md)` const & line, Vector3 const & offset)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `offset` (`Vector3 const &`)

[`LineSegment3D`](LineSegment3D.md)` translate(`[`LineSegment3D`](LineSegment3D.md)` const & seg, Vector3 const & offset)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `offset` (`Vector3 const &`)

[`GeometryCollection3D`](GeometryCollection3D.md)` translate(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, Vector3 const & offset)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `offset` (`Vector3 const &`)

[`Polygon3D`](Polygon3D.md)` translate(`[`Polygon3D`](Polygon3D.md)` const & poly, Vector3 const & offset)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `offset` (`Vector3 const &`)

[`Polyline3D`](Polyline3D.md)` translate(`[`Polyline3D`](Polyline3D.md)` const & polyline, Vector3 const & offset)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `offset` (`Vector3 const &`)

[`Triangle3D`](Triangle3D.md)` translate(`[`Triangle3D`](Triangle3D.md)` const & tri, Vector3 const & offset)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `offset` (`Vector3 const &`)

[`Mesh3D`](Mesh3D.md)` translate(`[`Mesh3D`](Mesh3D.md)` const & mesh, Vector3 const & offset)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `offset` (`Vector3 const &`)

[`PolyMesh3D`](PolyMesh3D.md)` translate(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, Vector3 const & offset)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `offset` (`Vector3 const &`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` translate(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, Vector3 const & offset)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `offset` (`Vector3 const &`)

## `rotate`

[`Point2D`](Point2D.md)` rotate(`[`Point2D`](Point2D.md)` const & p, double angle_rad)`

Rotates a point about the origin by angle_rad radians (CCW, right-hand rule in the XY plane).

Direct cos / sin arithmetic no matrix built. To rotate about a different pivot, first translate that pivot to the origin, rotate, then translate back (or use transform() with a composed Matrix3).

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `angle_rad` (`double`)

[`Vector2D`](Vector2D.md)` rotate(`[`Vector2D`](Vector2D.md)` const & v, double angle_rad)`

Rotates a vector (direction) by angle_rad radians (CCW).

Direct arithmetic no matrix built. No translate() overload a direction has no position to translate.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `angle_rad` (`double`)

[`Ray2D`](Ray2D.md)` rotate(`[`Ray2D`](Ray2D.md)` const & ray, double angle_rad)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `angle_rad` (`double`)

[`Line2D`](Line2D.md)` rotate(`[`Line2D`](Line2D.md)` const & line, double angle_rad)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `angle_rad` (`double`)

[`LineSegment2D`](LineSegment2D.md)` rotate(`[`LineSegment2D`](LineSegment2D.md)` const & seg, double angle_rad)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `angle_rad` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)` rotate(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double angle_rad)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `angle_rad` (`double`)

[`Polygon2D`](Polygon2D.md)` rotate(`[`Polygon2D`](Polygon2D.md)` const & poly, double angle_rad)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `angle_rad` (`double`)

[`Polyline2D`](Polyline2D.md)` rotate(`[`Polyline2D`](Polyline2D.md)` const & polyline, double angle_rad)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `angle_rad` (`double`)

[`Triangle2D`](Triangle2D.md)` rotate(`[`Triangle2D`](Triangle2D.md)` const & tri, double angle_rad)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `angle_rad` (`double`)

[`Mesh2D`](Mesh2D.md)` rotate(`[`Mesh2D`](Mesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `angle_rad` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)` rotate(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `angle_rad` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` rotate(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double angle_rad)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `angle_rad` (`double`)

[`Point3D`](Point3D.md)` rotate(`[`Point3D`](Point3D.md)` const & p, double angle_rad, Vector3 const & axis)`

Rotates a point about the origin, around axis , by angle_rad radians (Rodrigues' formula, right-hand rule).

Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Vector3D`](Vector3D.md)` rotate(`[`Vector3D`](Vector3D.md)` const & v, double angle_rad, Vector3 const & axis)`

Rotates a vector (direction) about axis by angle_rad radians (Rodrigues' formula).

Builds a Matrix4::Rotation() internally, same as [Point3D](Point3D.md)::rotate() see its docs and the file docs for why this is the one exception to the direct-arithmetic fast path.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Ray3D`](Ray3D.md)` rotate(`[`Ray3D`](Ray3D.md)` const & ray, double angle_rad, Vector3 const & axis)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Line3D`](Line3D.md)` rotate(`[`Line3D`](Line3D.md)` const & line, double angle_rad, Vector3 const & axis)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`LineSegment3D`](LineSegment3D.md)` rotate(`[`LineSegment3D`](LineSegment3D.md)` const & seg, double angle_rad, Vector3 const & axis)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`GeometryCollection3D`](GeometryCollection3D.md)` rotate(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double angle_rad, Vector3 const & axis)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Polygon3D`](Polygon3D.md)` rotate(`[`Polygon3D`](Polygon3D.md)` const & poly, double angle_rad, Vector3 const & axis)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Polyline3D`](Polyline3D.md)` rotate(`[`Polyline3D`](Polyline3D.md)` const & polyline, double angle_rad, Vector3 const & axis)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Triangle3D`](Triangle3D.md)` rotate(`[`Triangle3D`](Triangle3D.md)` const & tri, double angle_rad, Vector3 const & axis)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`Mesh3D`](Mesh3D.md)` rotate(`[`Mesh3D`](Mesh3D.md)` const & mesh, double angle_rad, Vector3 const & axis)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`PolyMesh3D`](PolyMesh3D.md)` rotate(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double angle_rad, Vector3 const & axis)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` rotate(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double angle_rad, Vector3 const & axis)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `angle_rad` (`double`)
- `axis` (`Vector3 const &`)

## `scale`

[`Point2D`](Point2D.md)` scale(`[`Point2D`](Point2D.md)` const & p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `factor` (`double`)

[`Point2D`](Point2D.md)` scale(`[`Point2D`](Point2D.md)` const & p, double sx, double sy)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Vector2D`](Vector2D.md)` scale(`[`Vector2D`](Vector2D.md)` const & v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `factor` (`double`)

[`Vector2D`](Vector2D.md)` scale(`[`Vector2D`](Vector2D.md)` const & v, double sx, double sy)`

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Ray2D`](Ray2D.md)` scale(`[`Ray2D`](Ray2D.md)` const & ray, double factor)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `factor` (`double`)

[`Ray2D`](Ray2D.md)` scale(`[`Ray2D`](Ray2D.md)` const & ray, double sx, double sy)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Line2D`](Line2D.md)` scale(`[`Line2D`](Line2D.md)` const & line, double factor)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `factor` (`double`)

[`Line2D`](Line2D.md)` scale(`[`Line2D`](Line2D.md)` const & line, double sx, double sy)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`LineSegment2D`](LineSegment2D.md)` scale(`[`LineSegment2D`](LineSegment2D.md)` const & seg, double factor)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `factor` (`double`)

[`LineSegment2D`](LineSegment2D.md)` scale(`[`LineSegment2D`](LineSegment2D.md)` const & seg, double sx, double sy)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)` scale(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double factor)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `factor` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)` scale(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double sx, double sy)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Polygon2D`](Polygon2D.md)` scale(`[`Polygon2D`](Polygon2D.md)` const & poly, double factor)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `factor` (`double`)

[`Polygon2D`](Polygon2D.md)` scale(`[`Polygon2D`](Polygon2D.md)` const & poly, double sx, double sy)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Polyline2D`](Polyline2D.md)` scale(`[`Polyline2D`](Polyline2D.md)` const & polyline, double factor)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `factor` (`double`)

[`Polyline2D`](Polyline2D.md)` scale(`[`Polyline2D`](Polyline2D.md)` const & polyline, double sx, double sy)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Triangle2D`](Triangle2D.md)` scale(`[`Triangle2D`](Triangle2D.md)` const & tri, double factor)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `factor` (`double`)

[`Triangle2D`](Triangle2D.md)` scale(`[`Triangle2D`](Triangle2D.md)` const & tri, double sx, double sy)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Mesh2D`](Mesh2D.md)` scale(`[`Mesh2D`](Mesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `factor` (`double`)

[`Mesh2D`](Mesh2D.md)` scale(`[`Mesh2D`](Mesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)` scale(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `factor` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)` scale(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` scale(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `factor` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` scale(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double sx, double sy)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `sx` (`double`)
- `sy` (`double`)

[`Point3D`](Point3D.md)` scale(`[`Point3D`](Point3D.md)` const & p, double factor)`

Scales a point's coordinates about the origin by a uniform factor.

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `factor` (`double`)

[`Point3D`](Point3D.md)` scale(`[`Point3D`](Point3D.md)` const & p, double sx, double sy, double sz)`

Scales a point's coordinates about the origin independently per axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Vector3D`](Vector3D.md)` scale(`[`Vector3D`](Vector3D.md)` const & v, double factor)`

Scales a vector's components by a uniform factor. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `factor` (`double`)

[`Vector3D`](Vector3D.md)` scale(`[`Vector3D`](Vector3D.md)` const & v, double sx, double sy, double sz)`

Scales a vector's components independently per axis.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Ray3D`](Ray3D.md)` scale(`[`Ray3D`](Ray3D.md)` const & ray, double factor)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `factor` (`double`)

[`Ray3D`](Ray3D.md)` scale(`[`Ray3D`](Ray3D.md)` const & ray, double sx, double sy, double sz)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Line3D`](Line3D.md)` scale(`[`Line3D`](Line3D.md)` const & line, double factor)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `factor` (`double`)

[`Line3D`](Line3D.md)` scale(`[`Line3D`](Line3D.md)` const & line, double sx, double sy, double sz)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`LineSegment3D`](LineSegment3D.md)` scale(`[`LineSegment3D`](LineSegment3D.md)` const & seg, double factor)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `factor` (`double`)

[`LineSegment3D`](LineSegment3D.md)` scale(`[`LineSegment3D`](LineSegment3D.md)` const & seg, double sx, double sy, double sz)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)` scale(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double factor)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `factor` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)` scale(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double sx, double sy, double sz)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Polygon3D`](Polygon3D.md)` scale(`[`Polygon3D`](Polygon3D.md)` const & poly, double factor)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `factor` (`double`)

[`Polygon3D`](Polygon3D.md)` scale(`[`Polygon3D`](Polygon3D.md)` const & poly, double sx, double sy, double sz)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Polyline3D`](Polyline3D.md)` scale(`[`Polyline3D`](Polyline3D.md)` const & polyline, double factor)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `factor` (`double`)

[`Polyline3D`](Polyline3D.md)` scale(`[`Polyline3D`](Polyline3D.md)` const & polyline, double sx, double sy, double sz)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Triangle3D`](Triangle3D.md)` scale(`[`Triangle3D`](Triangle3D.md)` const & tri, double factor)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `factor` (`double`)

[`Triangle3D`](Triangle3D.md)` scale(`[`Triangle3D`](Triangle3D.md)` const & tri, double sx, double sy, double sz)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`Mesh3D`](Mesh3D.md)` scale(`[`Mesh3D`](Mesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `factor` (`double`)

[`Mesh3D`](Mesh3D.md)` scale(`[`Mesh3D`](Mesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)` scale(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `factor` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)` scale(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` scale(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double factor)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `factor` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` scale(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double sx, double sy, double sz)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `shear`

[`Point2D`](Point2D.md)` shear(`[`Point2D`](Point2D.md)` const & p, double shx, double shy)`

Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Vector2D`](Vector2D.md)` shear(`[`Vector2D`](Vector2D.md)` const & v, double shx, double shy)`

Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Ray2D`](Ray2D.md)` shear(`[`Ray2D`](Ray2D.md)` const & ray, double shx, double shy)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Line2D`](Line2D.md)` shear(`[`Line2D`](Line2D.md)` const & line, double shx, double shy)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`LineSegment2D`](LineSegment2D.md)` shear(`[`LineSegment2D`](LineSegment2D.md)` const & seg, double shx, double shy)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`GeometryCollection2D`](GeometryCollection2D.md)` shear(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, double shx, double shy)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Polygon2D`](Polygon2D.md)` shear(`[`Polygon2D`](Polygon2D.md)` const & poly, double shx, double shy)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Polyline2D`](Polyline2D.md)` shear(`[`Polyline2D`](Polyline2D.md)` const & polyline, double shx, double shy)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Triangle2D`](Triangle2D.md)` shear(`[`Triangle2D`](Triangle2D.md)` const & tri, double shx, double shy)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Mesh2D`](Mesh2D.md)` shear(`[`Mesh2D`](Mesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`PolyMesh2D`](PolyMesh2D.md)` shear(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` shear(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, double shx, double shy)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `shx` (`double`)
- `shy` (`double`)

[`Point3D`](Point3D.md)` shear(`[`Point3D`](Point3D.md)` const & p, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a point: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Vector3D`](Vector3D.md)` shear(`[`Vector3D`](Vector3D.md)` const & v, double xy, double xz, double yx, double yz, double zx, double zy)`

Shears a vector: each axis is offset by a multiple of the other two.

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Ray3D`](Ray3D.md)` shear(`[`Ray3D`](Ray3D.md)` const & ray, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Line3D`](Line3D.md)` shear(`[`Line3D`](Line3D.md)` const & line, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`LineSegment3D`](LineSegment3D.md)` shear(`[`LineSegment3D`](LineSegment3D.md)` const & seg, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`GeometryCollection3D`](GeometryCollection3D.md)` shear(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Polygon3D`](Polygon3D.md)` shear(`[`Polygon3D`](Polygon3D.md)` const & poly, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Polyline3D`](Polyline3D.md)` shear(`[`Polyline3D`](Polyline3D.md)` const & polyline, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Triangle3D`](Triangle3D.md)` shear(`[`Triangle3D`](Triangle3D.md)` const & tri, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`Mesh3D`](Mesh3D.md)` shear(`[`Mesh3D`](Mesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`PolyMesh3D`](PolyMesh3D.md)` shear(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` shear(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, double xy, double xz, double yx, double yz, double zx, double zy)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `reflect`

[`Point2D`](Point2D.md)` reflect(`[`Point2D`](Point2D.md)` const & p, Vector2 const & normal)`

Reflects a point across the line through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)
- `normal` (`Vector2 const &`)

[`Vector2D`](Vector2D.md)` reflect(`[`Vector2D`](Vector2D.md)` const & v, Vector2 const & normal)`

Reflects a vector across the line through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector2D`](Vector2D.md) const &)
- `normal` (`Vector2 const &`)

[`Ray2D`](Ray2D.md)` reflect(`[`Ray2D`](Ray2D.md)` const & ray, Vector2 const & normal)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)
- `normal` (`Vector2 const &`)

[`Line2D`](Line2D.md)` reflect(`[`Line2D`](Line2D.md)` const & line, Vector2 const & normal)`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)
- `normal` (`Vector2 const &`)

[`LineSegment2D`](LineSegment2D.md)` reflect(`[`LineSegment2D`](LineSegment2D.md)` const & seg, Vector2 const & normal)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)
- `normal` (`Vector2 const &`)

[`GeometryCollection2D`](GeometryCollection2D.md)` reflect(`[`GeometryCollection2D`](GeometryCollection2D.md)` const & collection, Vector2 const & normal)`


**Parameters**

- `collection` ([`GeometryCollection2D`](GeometryCollection2D.md) const &)
- `normal` (`Vector2 const &`)

[`Polygon2D`](Polygon2D.md)` reflect(`[`Polygon2D`](Polygon2D.md)` const & poly, Vector2 const & normal)`


**Parameters**

- `poly` ([`Polygon2D`](Polygon2D.md) const &)
- `normal` (`Vector2 const &`)

[`Polyline2D`](Polyline2D.md)` reflect(`[`Polyline2D`](Polyline2D.md)` const & polyline, Vector2 const & normal)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)
- `normal` (`Vector2 const &`)

[`Triangle2D`](Triangle2D.md)` reflect(`[`Triangle2D`](Triangle2D.md)` const & tri, Vector2 const & normal)`


**Parameters**

- `tri` ([`Triangle2D`](Triangle2D.md) const &)
- `normal` (`Vector2 const &`)

[`Mesh2D`](Mesh2D.md)` reflect(`[`Mesh2D`](Mesh2D.md)` const & mesh, Vector2 const & normal)`


**Parameters**

- `mesh` ([`Mesh2D`](Mesh2D.md) const &)
- `normal` (`Vector2 const &`)

[`PolyMesh2D`](PolyMesh2D.md)` reflect(`[`PolyMesh2D`](PolyMesh2D.md)` const & mesh, Vector2 const & normal)`


**Parameters**

- `mesh` ([`PolyMesh2D`](PolyMesh2D.md) const &)
- `normal` (`Vector2 const &`)

[`ConnectedMesh2D`](ConnectedMesh2D.md)` reflect(`[`ConnectedMesh2D`](ConnectedMesh2D.md)` const & mesh, Vector2 const & normal)`


**Parameters**

- `mesh` ([`ConnectedMesh2D`](ConnectedMesh2D.md) const &)
- `normal` (`Vector2 const &`)

[`Point3D`](Point3D.md)` reflect(`[`Point3D`](Point3D.md)` const & p, Vector3 const & normal)`

Reflects a point across the plane through the origin whose normal is normal (Householder reflection).

Direct arithmetic no matrix built.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)
- `normal` (`Vector3 const &`)

[`Vector3D`](Vector3D.md)` reflect(`[`Vector3D`](Vector3D.md)` const & v, Vector3 const & normal)`

Reflects a vector across the plane through the origin whose normal is normal .

Direct arithmetic no matrix built.

**Parameters**

- `v` ([`Vector3D`](Vector3D.md) const &)
- `normal` (`Vector3 const &`)

[`Ray3D`](Ray3D.md)` reflect(`[`Ray3D`](Ray3D.md)` const & ray, Vector3 const & normal)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)
- `normal` (`Vector3 const &`)

[`Line3D`](Line3D.md)` reflect(`[`Line3D`](Line3D.md)` const & line, Vector3 const & normal)`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)
- `normal` (`Vector3 const &`)

[`LineSegment3D`](LineSegment3D.md)` reflect(`[`LineSegment3D`](LineSegment3D.md)` const & seg, Vector3 const & normal)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)
- `normal` (`Vector3 const &`)

[`GeometryCollection3D`](GeometryCollection3D.md)` reflect(`[`GeometryCollection3D`](GeometryCollection3D.md)` const & collection, Vector3 const & normal)`


**Parameters**

- `collection` ([`GeometryCollection3D`](GeometryCollection3D.md) const &)
- `normal` (`Vector3 const &`)

[`Polygon3D`](Polygon3D.md)` reflect(`[`Polygon3D`](Polygon3D.md)` const & poly, Vector3 const & normal)`


**Parameters**

- `poly` ([`Polygon3D`](Polygon3D.md) const &)
- `normal` (`Vector3 const &`)

[`Polyline3D`](Polyline3D.md)` reflect(`[`Polyline3D`](Polyline3D.md)` const & polyline, Vector3 const & normal)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)
- `normal` (`Vector3 const &`)

[`Triangle3D`](Triangle3D.md)` reflect(`[`Triangle3D`](Triangle3D.md)` const & tri, Vector3 const & normal)`


**Parameters**

- `tri` ([`Triangle3D`](Triangle3D.md) const &)
- `normal` (`Vector3 const &`)

[`Mesh3D`](Mesh3D.md)` reflect(`[`Mesh3D`](Mesh3D.md)` const & mesh, Vector3 const & normal)`


**Parameters**

- `mesh` ([`Mesh3D`](Mesh3D.md) const &)
- `normal` (`Vector3 const &`)

[`PolyMesh3D`](PolyMesh3D.md)` reflect(`[`PolyMesh3D`](PolyMesh3D.md)` const & mesh, Vector3 const & normal)`


**Parameters**

- `mesh` ([`PolyMesh3D`](PolyMesh3D.md) const &)
- `normal` (`Vector3 const &`)

[`ConnectedMesh3D`](ConnectedMesh3D.md)` reflect(`[`ConnectedMesh3D`](ConnectedMesh3D.md)` const & mesh, Vector3 const & normal)`


**Parameters**

- `mesh` ([`ConnectedMesh3D`](ConnectedMesh3D.md) const &)
- `normal` (`Vector3 const &`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection2D](GeometryCollection2D.md), [GeometryCollection3D](GeometryCollection3D.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Mesh3D](Mesh3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [PolyMesh3D](PolyMesh3D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [Ray2D](Ray2D.md), [Ray3D](Ray3D.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [Vector2D](Vector2D.md), [Vector3D](Vector3D.md)
