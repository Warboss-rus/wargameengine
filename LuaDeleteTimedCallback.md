
```
void DeleteTimedCallback(int id)
```

## Summary ##
Unsets the callback that is called at specific intervals of time. If a non-repetative callback wasn't called yet, it won't be executed. If a callback with this ID does not exists, does nothing.

## Parameters ##
  * **id** - the id of a callback to cancel. It's returned by the call of [SetTimedCallback](LuaSetTimedCallback.md) function.

## Output values ##
NONE

## Errors ##
  * **3 argument expected (funcName, time, repeat)**. Parameters number or type does not match.

## Example ##

```
function Callback()
--do smth
end

local id = SetTimedCallback("Callback", 10, false)--callback function will be called once in 10 ms. After that it will be unset automatically.
DeleteTimedCallback(id)--cancel the non-repetative callback so it won't be executed.
id = SetSelectionCallback("Callback", 50, true)--callback function will be called every 50 ms. until it's unset.
DeleteTimedCallback(id)--a repetative callback is unset and won't be executed in the future.
```