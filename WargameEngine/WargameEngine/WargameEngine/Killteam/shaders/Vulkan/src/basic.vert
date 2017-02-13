#version 450

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvp_matrix;
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 proj_matrix;
} ubo;

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
	gl_Position = ubo.mvp_matrix * vec4(Position, 1.0 );
}