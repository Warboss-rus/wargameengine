#version 450

layout(set=0, binding=0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 out_Color;

void main() 
{
	out_Color = vec4(1.0, 0.0, 0.0, 1.0);//texture( u_Texture, v_TexCoord );
}