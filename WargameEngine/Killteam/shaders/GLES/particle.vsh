#version 300 es
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 instancePosition;
in vec2 instanceTexCoordPos;

uniform mat4 mvp_matrix;

out vec2 v_texCoord;

void main()
{
	gl_Position = mvp_matrix * vec4(Position.xyz * instancePosition.w + instancePosition.xyz, 1.0);
	v_texCoord = TexCoord + instanceTexCoordPos;
}