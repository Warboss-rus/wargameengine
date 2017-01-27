#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;

out vec3 v_normal;
out vec3 v_pos;
out vec2 v_texcoord;

void main()
{
	gl_Position = mvp_matrix * vec4(Position, 1.0);
	
	v_normal = mat3(transpose(inverse(model_matrix))) * Normal;
	
	v_pos = (model_matrix * vec4(Position, 1.0)).xyz;
	
	v_texcoord = TexCoord;
}
