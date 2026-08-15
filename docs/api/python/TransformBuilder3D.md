# `TransformBuilder3D` (Python)

Builds a composite Matrix4 by chaining translate() /rotate()/scale()/combine() calls, each PRE-multiplying the new operation onto the accumulated matrix ( new_op * accumulated ) so that operations apply in the order they're called: builder.translate(t).rotate(r) moves a point first by t , then rotates the result by r (not the other way around) matches how a reader expects a chain of method calls to read left-to-right as "do this, then this".

## `translate`

`translate(offset: Vector3) -> TransformBuilder3D`

Appends a translation by offset , applied after every operation already chained.

**Parameters**

- `offset` (`Vector3`)

## `rotate`

`rotate(angle_rad: float, axis: Vector3) -> TransformBuilder3D`

Appends a rotation by angle_rad radians about axis (through the origin, Rodrigues' formula), applied after every operation already chained.

**Parameters**

- `angle_rad` (`float`)
- `axis` (`Vector3`)

## `scale`

`scale(factor: float) -> TransformBuilder3D`

Appends a uniform scale by factor (about the origin), applied after every operation already chained.

**Parameters**

- `factor` (`float`)

`scale(sx: float, sy: float, sz: float) -> TransformBuilder3D`

Appends a non-uniform per-axis scale (about the origin), applied after every operation already chained.

**Parameters**

- `sx` (`float`)
- `sy` (`float`)
- `sz` (`float`)

## `shear`

`shear(xy: float, xz: float, yx: float, yz: float, zx: float, zy: float) -> TransformBuilder3D`

Appends a general shear (each axis offset by a multiple of the other two), applied after every operation already chained.

**Parameters**

- `xy` (`float`)
- `xz` (`float`)
- `yx` (`float`)
- `yz` (`float`)
- `zx` (`float`)
- `zy` (`float`)

## `reflect`

`reflect(normal: Vector3) -> TransformBuilder3D`

Appends a reflection across the plane through the origin whose normal is normal , applied after every operation already chained.

**Parameters**

- `normal` (`Vector3`)

## `combine`

`combine(mat: Matrix4) -> TransformBuilder3D`

Appends an arbitrary caller-supplied Matrix4, applied after every operation already chained an escape hatch for a transform this builder has no dedicated method for (perspective, a matrix loaded from a scene file, ...).

**Parameters**

- `mat` (`Matrix4`)

## `get`

`get() -> Matrix4`

The composed matrix so far.

## `build`

`build() -> Matrix4`

Copy of the composed matrix so far same value as Get() , but by value for a caller who wants to keep it independent of this builder's further chaining.
