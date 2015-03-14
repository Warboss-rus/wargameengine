
```
void DisableShadowMap()
```

## Summary ##
Disables the rendering of a shadowmap before each new frame. It may be enabled with a [EnableShadowMap](LuaEnableShadowMap.md) function.

## Parameters ##
NONE

## Output values ##
NONE

## Errors ##
  * **no arguments expected**. Parameters number or type does not match.

## Example ##

```
EnableShadowMap(4096, 65)
DisableShadowMap()
```
Enables the rendering of a shadowmap with a texture size of 4096\*4096 and a view angle of 65 and then disables it.