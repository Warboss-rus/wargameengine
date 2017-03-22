#version 450

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
struct Light {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec3 pos;
	float distance;
};
#define NUM_LIGHTS 1

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
	vec4 color;
	Material material;
	Light lights[NUM_LIGHTS];
	int lightsCount;
} ubo2;

layout(set = 1, binding = 2) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 out_Color;

void main() 
{
	vec4 tex = texture( u_Texture, v_TexCoord );
	out_Color = vec4(tex.xyz + ubo2.color.xyz, tex.a * ubo2.color.a);
}