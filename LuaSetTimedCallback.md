
```
int SetTimedCallback(string function, int time, bool repeat)
```

## Summary ##
Sets the callback that is called at specific intervals of time. If a repeat parameter is false the function will be called once, otherwise it will be calling continuously,
until you delete a callback with a [DeleteTimedCallback](LuaDeleteTimedCallback.md) function. Passing an empty string as a function name does nothing.

## Parameters ##
  * **function** - the name of LUA function to call.
  * **time** - the interval in ms in which the function is called.
  * **repeat** - if the callback must be called continuously.

## Output values ##
  * **id**. Returns an unique ID that can be passed to a [DeleteTimedCallback](LuaDeleteTimedCallback.md) function to unset the callback.

## Errors ##
  * **3 argument expected (funcName, time, repeat)**. Parameters number or type does not match.

## Example ##

```
function Callback()
--do smth
end

SetTimedCallback("Callback", 10, false)--callback function will be called once in 10 ms. After that it will be unset automatically.
local id = SetSelectionCallback("Callback", 50, true)--callback function will be called every 50 ms. until it's unset.
DeleteTimedCallback(id)
```