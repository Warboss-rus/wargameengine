#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views=2) in vec3 Position;

uniform mat4 mvp_matrix[2];

out vec3 v_textureDir;

void main() {
	v_textureDir = Position;
	gl_Position = mvp_matrix[gl_ViewID_OVR] * vec4(Position, 1.0);
}