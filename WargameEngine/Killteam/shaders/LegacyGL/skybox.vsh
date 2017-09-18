uniform mat4 mvp_matrix;

varying vec3 v_textureDir;

void main() {
	v_textureDir = gl_Vertex.xyz;
	gl_Position = ftransform();
}