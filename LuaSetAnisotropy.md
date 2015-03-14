
```
void SetAnisotropy(float level)
```

## Summary ##
Anisotropic filtering (abbreviated AF) is a method of enhancing the image quality of textures on surfaces of computer graphics that are at oblique viewing angles with respect
to the camera where the projection of the texture (not the polygon or other primitive on which it is rendered) appears to be non-orthogonal. Improves texture quality, but
high AF values can descrease perfomance.

## Parameters ##
  * **level** - the level of AF. Suggested values are 1,2,4,8,16.

## Output values ##
NONE

## Errors ##
  * **1 argument expected (level)**. Parameters number or type does not match.

## Example ##
```
SetAnisotropy(16)--Enable 16X anisotropic filtering.
SetAnisotropy(1)--Disable anisotropic filtering.
```