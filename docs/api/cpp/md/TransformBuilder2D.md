# `TransformBuilder2D` (C++)

2D counterpart of [TransformBuilder3D](TransformBuilder3D.md) builds a composite Matrix3 by chaining translate() / rotate() /scale()/combine() calls with the same call-order composition semantics (see [TransformBuilder3D](TransformBuilder3D.md)::Combine() ).

## `Translate`

`TransformBuilder2D & Translate(Vector2 const & offset)`

Appends a translation by offset , applied after every operation already chained.

**Parameters**

- `offset` (`Vector2 const &`)

## `Rotate`

`TransformBuilder2D & Rotate(double angle_rad)`

Appends a rotation by angle_rad radians (CCW, right-hand rule) about the origin, applied after every operation already chained.

**Parameters**

- `angle_rad` (`double`)

## `Scale`

`TransformBuilder2D & Scale(double factor)`

Appends a uniform scale by factor (about the origin), applied after every operation already chained.

**Parameters**

- `factor` (`double`)

`TransformBuilder2D & Scale(double sx, double sy)`

Appends a non-uniform per-axis scale (about the origin), applied after every operation already chained.

**Parameters**

- `sx` (`double`)
- `sy` (`double`)

## `Shear`

`TransformBuilder2D & Shear(double shx, double shy)`

Appends a shear ( shx shears X by Y, shy shears Y by X), applied after every operation already chained.

**Parameters**

- `shx` (`double`)
- `shy` (`double`)

## `Reflect`

`TransformBuilder2D & Reflect(Vector2 const & normal)`

Appends a reflection across the line through the origin whose normal is normal , applied after every operation already chained.

**Parameters**

- `normal` (`Vector2 const &`)

## `Combine`

`TransformBuilder2D & Combine(Matrix3 const & mat)`

Appends an arbitrary caller-supplied Matrix3, applied after every operation already chained an escape hatch for a transform this builder has no dedicated method for.

**Parameters**

- `mat` (`Matrix3 const &`)

## `Get`

`Matrix3 const & Get() const`

The composed matrix so far.

## `Build`

`Matrix3 Build() const`

Copy of the composed matrix so far same value as Get() , but by value for a caller who wants to keep it independent of this builder's further chaining.
