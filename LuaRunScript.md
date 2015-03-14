
```
void RunScript(string filename)
```

## Summary ##
Calls a script by specified path. The currently executed script is paused until a called script execution is complete.

## Parameters ##
  * **filename** - a path to script to execute.

## Output values ##
NONE

## Errors ##
  * **1 argument expected (filename)**. Parameters number or type does not match.

## Example ##

```
RunScript("test1.lua")
```
Executes a "test1.lua" script.