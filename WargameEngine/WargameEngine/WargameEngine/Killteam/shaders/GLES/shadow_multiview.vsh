#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views=2) in vec3 Position;
layout(num_views=2) in vec3 Normal;
layout(num_views=2) in vec2 TexCoord;

uniform mat4 mvp_matrix[2];
uniform mat4 view_matrix[2];
uniform mat4 lightMatrix;

out vec4 v_lpos;
out vec2 v_texcoord;

void main(void)
{
	vec4 vpos = view_matrix[gl_ViewID_OVR] * vec4(Position, 1.0);
	v_lpos = lightMatrix * vpos;
	v_texcoord = TexCoord;
	gl_Position = mvp_matrix[gl_ViewID_OVR] * vec4(Position, 1.0);
}
