
```
void BindKey(char key, bool shift, bool ctrl, bool alt, string function)
```

## Summary ##
Sets the callback that triggers when the key combination is pressed. Not all keys on a keyboard can be set with this functions. Keys like _F1-F2, directional keys, Page Up, Page Down,
Home, End, Insert_ cannot be passed. _Shift, Alt_ and _Ctrl_ can only be used in key combination and cannot trigger a callback themselves.
Passing an empty string as a parameter disables callback for this combination.

## Parameters ##
  * **key** - the key that will trigger the callback.
  * **shift** - is shift key must be pressed in order to trigger the callback.
  * **ctrl** - is ctrl key must be pressed in order to trigger the callback.
  * **alt** - is alt key must be pressed in order to trigger the callback.
  * **function** - the name of LUA function to call.

## Output values ##
NONE

## Errors ##
  * **5 argument expected (key, shift, ctrl, alt, funcName)**. Parameters number or type does not match.

## Example ##

```
function Callback()
--do smth
end

BindKey(127, false, false, false, "Callback")--callback is set to Delete button
BindKey(27, true, false, false, "Callback")--callback is unset to Shift+Esc combination
BindKey(8, true, true, true, "") --unset callback to Shift+Ctrl+Alt+BackSpace
```