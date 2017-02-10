#version 450

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec2 v_TexCoord;

void main() 
{
	v_TexCoord = TexCoord;
	gl_Position = vec4(Position, 1.0 );
}