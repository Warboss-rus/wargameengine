
```
void SetGlobalProperty(string key, string value)
```

## Summary ##
Sets a global property value. If there is already a property with this key it will be overwritten. Global properties can be retrieved using [GetGlobalProperty](LuaGetGlobalProperty.md) function.

## Parameters ##
  * **key** - property name.
  * **value** - property value.

## Output values ##
NONE

## Errors ##
  * **2 arguments expected (key, value)**. Parameters number or type does not match.

## Example ##
```
GetGlobalProperty("turn") --no such property, returns empty string.
SetGlobalProperty("turn", 1) --set a "turn" property
GetGlobalProperty("turn") --returns "1".
```