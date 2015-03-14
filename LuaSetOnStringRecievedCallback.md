
```
void SetOnStringRecievedCallback(string function)
```

## Summary ##
Sets the callback function that is triggered when a string is received over the [Network](Network.md). Passing an empty string as a parameter disables callback.

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

SetOnStringRecievedCallback("Callback")--callback is set
SetOnStringRecievedCallback("")--callback is unset
```