#version 300 es
in vec3 Position;

uniform mat4 mvp_matrix;

out vec3 v_textureDir;

void main() {
	v_textureDir = Position;
	gl_Position = mvp_matrix * vec4(Position, 1.0);
}