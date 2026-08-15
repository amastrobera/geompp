# `TransformBuilder3D` (C++)

Builds a composite Matrix4 by chaining translate() /rotate()/scale()/combine() calls, each PRE-multiplying the new operation onto the accumulated matrix ( new_op * accumulated ) so that operations apply in the order they're called: builder.translate(t).rotate(r) moves a point first by t , then rotates the result by r (not the other way around) matches how a reader expects a chain of method calls to read left-to-right as "do this, then this".

## `Translate`

`TransformBuilder3D & Translate(maths::Vector3 const & offset)`

Appends a translation by offset , applied after every operation already chained.

**Parameters**

- `offset` (`maths::Vector3 const &`)

## `Rotate`

`TransformBuilder3D & Rotate(double angle_rad, maths::Vector3 const & axis)`

Appends a rotation by angle_rad radians about axis (through the origin, Rodrigues' formula), applied after every operation already chained.

**Parameters**

- `angle_rad` (`double`)
- `axis` (`maths::Vector3 const &`)

## `Scale`

`TransformBuilder3D & Scale(double factor)`

Appends a uniform scale by factor (about the origin), applied after every operation already chained.

**Parameters**

- `factor` (`double`)

`TransformBuilder3D & Scale(double sx, double sy, double sz)`

Appends a non-uniform per-axis scale (about the origin), applied after every operation already chained.

**Parameters**

- `sx` (`double`)
- `sy` (`double`)
- `sz` (`double`)

## `Shear`

`TransformBuilder3D & Shear(double xy, double xz, double yx, double yz, double zx, double zy)`

Appends a general shear (each axis offset by a multiple of the other two), applied after every operation already chained.

**Parameters**

- `xy` (`double`)
- `xz` (`double`)
- `yx` (`double`)
- `yz` (`double`)
- `zx` (`double`)
- `zy` (`double`)

## `Reflect`

`TransformBuilder3D & Reflect(maths::Vector3 const & normal)`

Appends a reflection across the plane through the origin whose normal is normal , applied after every operation already chained.

**Parameters**

- `normal` (`maths::Vector3 const &`)

## `Combine`

`TransformBuilder3D & Combine(maths::Matrix4 const & mat)`

Appends an arbitrary caller-supplied Matrix4, applied after every operation already chained an escape hatch for a transform this builder has no dedicated method for (perspective, a matrix loaded from a scene file, ...).

**Parameters**

- `mat` (`maths::Matrix4 const &`)

## `Get`

`maths::Matrix4 const & Get() const`

The composed matrix so far.

## `Build`

`maths::Matrix4 Build() const`

Copy of the composed matrix so far same value as Get() , but by value for a caller who wants to keep it independent of this builder's further chaining.
