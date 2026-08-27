# `TransformBuilder3D` (C# / .NET)

Builds a composite Matrix4 by chaining translate() /rotate()/scale()/combine() calls, each PRE-multiplying the new operation onto the accumulated matrix ( new_op * accumulated ) so that operations apply in the order they're called: builder.translate(t).rotate(r) moves a point first by t , then rotates the result by r (not the other way around) matches how a reader expects a chain of method calls to read left-to-right as "do this, then this".

## `Translate`

`TransformBuilder3D^ Translate(Vector3^ offset)`

Appends a translation by offset , applied after every operation already chained.

**Parameters**

- `offset` (`Vector3^`)

## `Rotate`

`TransformBuilder3D^ Rotate(double angle_rad, Vector3^ axis)`

Appends a rotation by angle_rad radians about axis (through the origin, Rodrigues' formula), applied after every operation already chained.

**Parameters**

- `angle_rad` (`double`)
- `axis` (`Vector3^`)

## `Scale`

`TransformBuilder3D^ Scale(double factor)`

Appends a uniform scale by factor (about the origin), applied after every operation already chained.

**Parameters**

- `factor` (`double`)

`TransformBuilder3D^ Scale(double sx, double sy, double sz)`

Appends a non-uniform per-axis scale (about the origin), applied after every operation already chained.

**Parameters**

- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `Shear`

`TransformBuilder3D^ Shear(double xy, double xz, double yx, double yz, double zx, double zy)`

Appends a general shear (each axis offset by a multiple of the other two), applied after every operation already chained.

**Parameters**

- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `Reflect`

`TransformBuilder3D^ Reflect(Vector3^ normal)`

Appends a reflection across the plane through the origin whose normal is normal , applied after every operation already chained.

**Parameters**

- `normal` (`Vector3^`)

## `Combine`

`TransformBuilder3D^ Combine(Matrix4^ mat)`

Appends an arbitrary caller-supplied Matrix4, applied after every operation already chained an escape hatch for a transform this builder has no dedicated method for (perspective, a matrix loaded from a scene file, ...).

**Parameters**

- `mat` (`Matrix4^`)

## `Get`

`Matrix4^ Get()`

The composed matrix so far.

## `Build`

`Matrix4^ Build()`

Copy of the composed matrix so far same value as Get() , but by value for a caller who wants to keep it independent of this builder's further chaining.

## `Apply`

`T^ Apply(T^ shape)`

Applies the composed matrix to shape and returns the transformed copy shorthand for transform(shape, builder.Get()), for any 3D primitive transform() has an overload for ( [Point3D](Point3D.md) , [Polygon3D](Polygon3D.md) , [Mesh3D](Mesh3D.md) , ...).

Doesn't consume or store shape the builder keeps composing normally afterward, so the same chain can Apply() to several different shapes.

**Parameters**

- `shape` (`T^`)


---

**See also:** [Mesh3D](Mesh3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md)
