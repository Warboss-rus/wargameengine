
```
void SetOnStateRecievedCallback(string function)
```

## Summary ##
Sets the callback that triggers when when a new gamestate is recieved from the [Network](Network.md) or save file. Passing an empty string as a parameter disables callback.

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

SetOnStateRecievedCallback("Callback")--callback is set
SetOnStateRecievedCallback("")--callback is unset
```