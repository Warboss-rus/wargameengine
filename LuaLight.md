WargameEngine allows you to set up 8 independent lighting sources by enabling\disabling them, setting their positions and colors for diffuse, ambient and specular lighting.
Note that you won't see any lighting unless you enable it by [EnableVertexLightning](LuaEnableVertexLighting.md) command or by [Shaders](Shaders.md). To unserstand lighting model of WargameEngine
please read about [Phong reflection model](http://en.wikipedia.org/wiki/Phong_reflection_model).

## EnableLight ##
```
void EnableLight(int index)
```
Enables Light source by its index (1 to 8).

## DisableLight ##
```
void DisableLight(int index)
```
Disables Light source by its index (1 to 8).

## SetLightPosition ##
```
void SetLightPosition(int index, float x, float y, float z)
```
Set light position.

## SetLightAmbient ##
```
void SetLightAmbient(int index, float r, float g, float b, float a)
```
Set light ambient color (by rgba components).

## SetLightDiffuse ##
```
void SetLightDiffuse(int index, float r, float g, float b, float a)
```
Set light diffuse color (by rgba components).

## SetLightSpecular ##
```
void SetLightSpecular(int index, float r, float g, float b, float a)
```
Set light specular color (by rgba components).

## Parameters ##
  * **index** - Light index [1..8].
  * **x**, **y**, **z** - light coordinates.
  * **r**, **g**, **b**, **a**, - light color.

## Output values ##
NONE

## Errors ##
  * **1 argument expected (index)**, **4 argument expected (index, x, y, z)** or **5 argument expected (index, r, g, b, a)**. Parameters number or type does not match.
  * **only 8 light sources are supported**. The index must be between 1 and 8.

## Example ##
```
EnableLight(1)
SetLightPosition(1, 0, 0, 50)
SetLightDiffuse(1, 1, 1, 1, 1)
SetLightAmbient(1, 0.5, 0.5, 0.5, 1)
SetLightSpecular(1, 1, 1, 1, 1)
DisableLight(2)
```
This code enables Light1, sets its position and colors and disables Light2