# `TransformBuilder2D` (Python)

2D counterpart of [TransformBuilder3D](TransformBuilder3D.md) builds a composite Matrix3 by chaining translate() / rotate() /scale()/combine() calls with the same call-order composition semantics (see [TransformBuilder3D](TransformBuilder3D.md)::Combine() ).

## `translate`

`translate(offset: maths::Vector2) -> TransformBuilder2D`

Appends a translation by offset , applied after every operation already chained.

**Parameters**

- `offset` (`maths::Vector2`)

## `rotate`

`rotate(angle_rad: float) -> TransformBuilder2D`

Appends a rotation by angle_rad radians (CCW, right-hand rule) about the origin, applied after every operation already chained.

**Parameters**

- `angle_rad` (`float`)

## `scale`

`scale(factor: float) -> TransformBuilder2D`

Appends a uniform scale by factor (about the origin), applied after every operation already chained.

**Parameters**

- `factor` (`float`)

`scale(sx: float, sy: float) -> TransformBuilder2D`

Appends a non-uniform per-axis scale (about the origin), applied after every operation already chained.

**Parameters**

- `sx` (`float`)
- `sy` (`float`)

## `shear`

`shear(shx: float, shy: float) -> TransformBuilder2D`

Appends a shear ( shx shears X by Y, shy shears Y by X), applied after every operation already chained.

**Parameters**

- `shx` (`float`)
- `shy` (`float`)

## `reflect`

`reflect(normal: maths::Vector2) -> TransformBuilder2D`

Appends a reflection across the line through the origin whose normal is normal , applied after every operation already chained.

**Parameters**

- `normal` (`maths::Vector2`)

## `combine`

`combine(mat: maths::Matrix3) -> TransformBuilder2D`

Appends an arbitrary caller-supplied Matrix3, applied after every operation already chained an escape hatch for a transform this builder has no dedicated method for.

**Parameters**

- `mat` (`maths::Matrix3`)

## `get`

`get() -> maths::Matrix3`

The composed matrix so far.

## `build`

`build() -> maths::Matrix3`

Copy of the composed matrix so far same value as Get() , but by value for a caller who wants to keep it independent of this builder's further chaining.
