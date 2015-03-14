
```
void ResizeWindow(int height, int width)
```

## Summary ##
Change the main windows size to a specified values. If a program is in [WargameEngine#FullScreen](WargameEngine#FullScreen.md) mode it will immediately exit it.

## Parameters ##
  * **height** - the new height of the window.
  * **width** - the new width of the window.

## Output values ##
NONE

## Errors ##
  * **2 argument expected (height, width)**. Parameters number or type does not match.

## Example ##

```
ResizeWindow(480, 640)
```
The main window is now has a size of 640\*480.