
```
void CameraSetLimits(float maxXtrans, float maxYtrans, float maxScale, float minScale)
```

## Summary ##
Sets camera properties so a camera translation x-coordinate cannot be more than **maxXtrans** or less than -**maxXtrans**, y-coordinate must be in [-**maxYtrans**..**maxYtrans**] range and a camera zooming must be between **minScale** and **maxScale**.

## Parameters ##
  * **maxXtrans** - the maximum possible translation on the X axis.
  * **maxYtrans** - the maximum possible translation on the Y axis.
  * **maxScale** - the maximum zooming.
  * **minScale** - the minimum zooming.

## Output values ##
NONE

## Errors ##
  * **4 argument expected (max trans x, max trans y, max scale, min scale)**. Parameters number or type does not match.

## Example ##

```
CameraSetLimits(15, 6, 5, 0.4)
```
After this call the camera must be within [(-15; -6) .. (15; 6)] coordinates and cannot be zoomed above 5x or below 0.4x.