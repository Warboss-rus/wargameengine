
```
void SetUpdateCallback(string function)
```

## Summary ##
Sets the callback that triggers before drawing a new frame. The frequency of triggering this callback depends on a FPS so it can be called up to a thousand times per second. Use with
caution, heavy function may significantly drop perfomance. Passing an empty string as a parameter disables callback.

## Parameters ##
  * **function** - the name of LUA function to call.

## Output values ##
NONE

## Errors ##
  * **1 argument expected (funcName)**. Parameters number or type does not match.

## Example ##

```
function Callback()
--do smth
end

SetUpdateCallback("Callback")--callback is set
SetUpdateCallback("")--callback is unset
```