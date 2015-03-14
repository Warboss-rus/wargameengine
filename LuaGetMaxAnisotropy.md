
```
float GetMaxAnisotropy()
```

## Summary ##
Anisotropic filtering (abbreviated AF) is a method of enhancing the image quality of textures on surfaces of computer graphics that are at oblique viewing angles with respect
to the camera where the projection of the texture (not the polygon or other primitive on which it is rendered) appears to be non-orthogonal. Returns a maximum anisotropic level,
supported by your hardware. You can set AF value by using [SetAnisotropy](LuaSetAnisotropy.md) function.

## Parameters ##
  * NONE

## Output values ##
  * **value** - AF value.

## Errors ##
  * **no arguments expected**. No arguments are expected.

## Example ##
```
SetAnisotropy(GetMaxAnisotropy())--Enable anisotropic filtering at a maximum level supported.
```