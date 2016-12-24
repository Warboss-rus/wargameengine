#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec4 instancePosition;
layout (location = 4) in vec2 instanceTexCoordPos;

uniform mat4 mvp_matrix;

out vec2 v_texCoord;

void main()
{
	gl_Position = mvp_matrix * vec4(Position * instancePosition.w + instancePosition.xyz, 1.0);
	v_texCoord =TexCoord + instanceTexCoordPos;
}