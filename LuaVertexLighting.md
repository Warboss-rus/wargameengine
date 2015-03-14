OpenGL built-in per-vertex lighting doesn't require any shaders, but less accurate then a per-pixel lighting.

## EnableVertexLighting ##
```
void EnableVertexLighting()
```
Enables per-vertex lighting.

## DisableVertexLighting ##
```
void DisableVertexLighting()
```
Disables per-vertex lighting.

## Parameters ##
NONE

## Output values ##
NONE

## Errors ##
  * **no arguments expected**. Parameters number or type does not match.

## Example ##

```
EnableLight(1)
SetLightPosition(1, 0, 0, 50)
SetLightDiffuse(1, 1, 1, 1, 1)
SetLightAmbient(1, 0.5, 0.5, 0.5, 1)
SetLightSpecular(1, 1, 1, 1, 1)
EnableVertexLighting()
...
DisableVertexLighting()
```
Sets the light source properties, enables per-vertex lighting and disable it.