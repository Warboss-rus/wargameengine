A uniform is a variable that is passed from program to the shader. If no shader is used does nothing.

# Uniform1i #

```
void Uniform1i(string name, int value)
```
Passes a single integer into the shader. Can be used in any type of shader by defining
```
uniform int *name*;
```

# Uniform1f #

```
void Uniform1f(string name, float value)
```
Passes a single floating point number into the shader. Can be used in any type of shader by defining
```
uniform float *name*;
```

# Uniform1fv #

```
void Uniform1fv(string name, int count, array<float> value)
```
Passes a specified number of floats from the array. Can be used in any type of shader by defining
```
uniform float *name*[*count*];
```

# Uniform2fv #

```
void Uniform2fv(string name, int count, array<float> value)
```
Passes a specified number of two-dimensional vectors (specified by 2 successive floats) from the array. The count is a number of vectors, not floats! Can be used in any type of shader by defining
```
uniform vec2 *name*[*count*];
```

# Uniform3fv #

```
void Uniform3fv(string name, int count, array<float> value)
```
Passes a specified number of three-dimensional vectors (specified by 3 successive floats) from the array. The count is a number of vectors, not floats! Can be used in any type of shader by defining
```
uniform vec3 *name*[*count*];
```

# Uniform4fv #

```
void Uniform4fv(string name, int count, array<float> value)
```
Passes a specified number of four-dimensional vectors (specified by 4 successive floats) from the array. The count is a number of vectors, not floats! Can be used in any type of shader by defining
```
uniform vec4 *name*[*count*];
```

# UniformMatrix4fv #

```
void UniformMatrix4fv(string name, int count, array<float> value)
```
Passes a specified number of matrices from the array. Can be used in any type of shader by defining either
```
uniform mat4 *name*[*count*];
```

# Errors #

  * **2 arguments expected (uniform name, value)** or **3 arguments expected (uniform name, values count, values array)**. Parameters number of type does not match.
  * **Not enough elements in the array**. An array passed has not enough elements then specified by **count** parameter.

# Examples #

LUA code:
```
Uniform1i("int1", 1)
Uniform1i("int2", 1.01)
Uniform1f("float1", 0.55)
Uniform1fv("array1", 3, {1.01, 2, 3.005})
Uniform2fv("array2", 2, {5.02, 6.1, 0.0, 999.9})
Uniform3fv("array3", 1, {1, 2, 3})
Uniform4fv("array4", 0, {})
local mat = 
{1.0, 2.0, 3.0 4.0,
5.0, 6.0, 7.0, 8.0,
9.0, 10.0, 11.0, 12.0,
13.0, 14.0, 15.0, 16.0}
UniformMatrix4fv("mat1", 1, mat)
```
Shader code:
```
uniform int int1; //value=1
uniform int int2; //value=1
uniform float float1; //value=0.55
uniform float array1[3]; //value=[1.01, 2, 3.005]
uniform vec2 array2[2]; //value=[5.02, 6.1, 0.0, 999.9]
uniform vec3 array3; value=[1, 2, 3]
uniform vec4 array4; //uninitialized
uniform mat4 mat1; //value=[1.0, 2.0, 3.0 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0]
```