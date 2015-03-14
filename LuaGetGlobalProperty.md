
```
string GetGlobalProperty(string key)
```

## Summary ##
Retrieves a global property value by its name (key). If there is no such global property returns empty string. Global properties are set using [SetGlobalProperty](LuaSetGlobalProperty.md) function.

## Parameters ##
  * **key** - property name.

## Output values ##
  * **value** - property value.

## Errors ##
  * **1 argument expected (key)**. Parameters number or type does not match.

## Example ##
```
GetGlobalProperty("turn") --no such property, returns empty string.
SetGlobalProperty("turn", 1) --set a "turn" property
GetGlobalProperty("turn") --returns "1".
```