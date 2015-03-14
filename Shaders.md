# GLSL #

GLSL is high-level shading language based on C syntax. It is compiled at runtime by a video driver. The full specification of the most recent version can be found [here](http://www.opengl.org/registry/doc/GLSLangSpec.4.40.pdf)
WargameEngine supports any GLSL version that is supported by your videocard (except it doesn't allow you to use tesselation shaders yet). If an error occurs during compilation it will be appended to a [Log File](Log.md).

# Exported uniforms #

  * **Currently binded texture** `uniform sampler2D texture`
  * **A shadowmap** `sampler2DShadow shadowMap`

You can export your own variables from LUA using **[LUA Uniform functions](LUAUniform.md)**.