#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 mvp_matrix;
uniform mat4 view_matrix;
uniform mat4 lightMatrix;

out vec4 v_lpos;
out vec2 v_texcoord;

void main(void)
{
	vec4 vpos = view_matrix * vec4(Position, 1.0);
	v_lpos = lightMatrix * vpos;
	v_texcoord = TexCoord;
	gl_Position = mvp_matrix * vec4(Position, 1.0);
}
