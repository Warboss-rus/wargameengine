
```
void CreateSkybox(float size, string textureFolder)
```

## Summary ##
Creates a skybox of specified size and textures. The texture folder must contain 6 files named "front.bmp", "back.bmp", "left.bmp", "right.bmp", "top.bmp", "bottom.bmp" representing respective sides of the skybox.

## Parameters ##
  * **size** - the size of the skybox created.
  * **texture** - the path to a folder with textures.

## Output values ##
NONE

## Errors ##
  * **2 argument expected (size, texture folder)**. Parameters number or type does not match.

## Example ##

```
CreateSkybox(50, "skybox")
```
Creates a skybox with a size of 50 and the side textures will be "**textures**\skybox\front.bmp", "**textures**\skybox\back.bmp", e.t.c