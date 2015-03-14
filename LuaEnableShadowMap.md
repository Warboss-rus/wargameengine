
```
void EnableShadowMap(int size, float angle)
```

## Summary ##
Enables the rendering of a shadowmap before each new frame. The shadowmap texture then will be availible from fragment shader as `sampler2DShadow shadowMap` uniform.
This texture will contain depth data rendered from a camera placed at [position of Light1](LuaLight#SetLightPosition.md) and targeted to the (0,0,0) with a specified view angle.
Causes heavy fps drop.

## Parameters ##
  * **size** - the size of the shadowmap texture. The more size the texture will be the less pixelization will Must be a power of two.
  * **angle** - the angle of view for shadowmap texture. The lesser angle may result in more quality shadows, but may not cover all table.

## Output values ##
NONE

## Errors ##
  * **2 arguments (shadowMap size, shadow max Angle) expected**. Parameters number or type does not match.

## Example ##

```
EnableShadowMap(4096, 65)
```
Enable the rendering of a shadowmap with a texture size of 4096\*4096 and a view angle of 65.