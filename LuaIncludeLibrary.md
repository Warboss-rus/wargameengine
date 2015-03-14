
```
void IncludeLibrary(string libName)
```

## Summary ##
Includes a standard LUA library. You can include the following libraries:
  * [\*base\*](http://www.lua.org/manual/5.2/manual.html#6.1).
  * [\*bit32\*](http://www.lua.org/manual/5.2/manual.html#6.7).
  * [\*coroutine\*](http://www.lua.org/manual/5.2/manual.html#6.2). [Coroutines](http://www.lua.org/manual/5.2/manual.html#2.6), the LUA analog of threads
  * [\*debug\*](http://www.lua.org/manual/5.2/manual.html#6.10).
  * [\*io\*](http://www.lua.org/manual/5.2/manual.html#6.8).
  * [\*math\*](http://www.lua.org/manual/5.2/manual.html#6.6).
  * [\*os\*](http://www.lua.org/manual/5.2/manual.html#6.9).
  * [\*package\*](http://www.lua.org/manual/5.2/manual.html#6.3).
  * [\*string\*](http://www.lua.org/manual/5.2/manual.html#6.4).
  * [\*table\*](http://www.lua.org/manual/5.2/manual.html#6.5).

## Parameters ##
  * **libName** - the library to include.

## Output values ##
NONE

## Errors ##
  * **1 argument expected (libName)**. Parameters number or type does not match.

## Example ##

```
IncludeLibrary("math")
```
Includes a math library.