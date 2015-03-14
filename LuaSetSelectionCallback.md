
```
void SetSelectionCallback(string function)
```

## Summary ##
Sets the callback that triggers when an object is selected. Passing an empty string as a parameter disables callback.

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

SetSelectionCallback("Callback")--callback is set
SetSelectionCallback("")--callback is unset
```