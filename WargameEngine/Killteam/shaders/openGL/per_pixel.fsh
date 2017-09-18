#version 330 core
uniform sampler2D mainTexture;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec3 pos;
	float distance;
};
#define NUM_LIGHTS 4
uniform Light lights[NUM_LIGHTS];
uniform int lightsCount;

uniform vec3 viewPos;

in vec3 v_normal;
in vec3 v_pos;
in vec2 v_texcoord;

out vec4 fragColor;

vec4 CalculateDiffuseColor(vec3 normal,	vec3 light,	vec4 diffuseLight, vec4 diffuseMaterial, float attenuation)
{
	float diffuseFactor = max(dot(normal, light), 0.0) * attenuation;
	
	return diffuseMaterial * diffuseFactor * diffuseLight;
}

vec4 CalculateSpecularColor(vec3 reflectedLight, vec3 eye, vec4 specularLight, vec4 specularMaterial, float shininess, float attenuation)
{
	float specularFactor = max(dot(reflectedLight, eye), 0.0);	
	float specularIntensity = pow(specularFactor, shininess) * attenuation;
	if (dot(reflectedLight, eye) < 0.0)
	{
		 return vec4(0.4, 0.4, 0.4, 1.0);
	}
	return specularLight * specularIntensity * specularMaterial;
}

void main()
{
	vec4 color = texture(mainTexture, v_texcoord);
	vec3 normal = normalize(v_normal);
	vec3 lighting = vec3(0.0);
	
	for(int i = 0; i < min(lightsCount, NUM_LIGHTS); ++i)
	{
		// calculate diffuse lighting
		vec3 lightDir = normalize(lights[i].pos - v_pos);
		
		vec4 ambientColor = lights[i].ambient * material.ambient;
		
		vec4 diffuseColor = CalculateDiffuseColor(
			normal, 
			lightDir, 
			lights[i].diffuse, 
			material.diffuse, 1.0);	
		
		vec3 reflectedLight = reflect(-lightDir , normal);
		vec4 specularColor = CalculateSpecularColor(
			reflectedLight, 
			normalize(viewPos + v_pos), 
			lights[i].specular, 
			material.specular, 
			material.shininess,
			1.0);
			
		lighting += (diffuseColor + ambientColor + clamp(specularColor, 0.0, 1.0)).xyz;
	}

	fragColor = vec4(color.xyz * lighting, color.a);
}
