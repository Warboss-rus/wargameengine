
```
void SetShaders()
void SetShaders(string vertex)
void SetShaders(string vertex, string fragment)
void SetShaders(string vertex, string fragment, string geometry)
```

## Summary ##
Sets the [Shaders](Shaders.md) to render table and Objects with. Passing an empty string or less arguments disables some shader types.

## Parameters ##
  * **vertex** - path to vertex Shader file.
  * **fragment** - path to fragment Shader file.
  * **geometry** - path to geometry Shader file.

## Output values ##
NONE

## Errors ##
  * **up to 3 argument expected (vertex shader, fragment shader, geometry shader)**. Parameters number or type does not match.

## Example ##

```
SetShaders("vertex.vsh")--set a vertex shader.
SetShaders("shadow.vsh", "shadow.fsh") - set a vertex and a fragment shaders.
SetShaders("shadow.vsh", "shadow.fsh", "geom.gsh") - set a vertex, a fragment and a geometry shaders.
SetShaders() --don't use shaders
```