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

in vec3 v_normal;
in vec3 v_eyeDir;	// eye direction
in vec2 v_texcoord;

vec4 CalculateDiffuseColor(
	vec3 normal,	// must be normalized 
	vec3 light,		// must be normalized
	vec4 diffuseLight,
	vec4 diffuseMaterial,
	float attenuation
	)
{
	float diffuseFactor = max(dot(normal, light), 0.0) * attenuation;
	
	return diffuseMaterial * diffuseFactor * diffuseLight;
}

vec4 CalculateSpecularColor(
	vec3 reflectedLight,
	vec3 eye,
	vec4 specularLight,
	vec4 specularMaterial,
	float shininess,
	float attenuation
	)
{
	float specularFactor = max(dot(reflectedLight, eye), 0.0);	
	float specularIntensity = pow(specularFactor, shininess) * attenuation;
	if (dot(reflectedLight, eye) < 0)
        {
             return vec4(0.4, 0.4, 0.4, 1.0);
        }
	return specularLight * specularIntensity * specularMaterial;
}

void main()
{
	vec4 color = texture(mainTexture, v_texcoord);
	
	// calculate diffuse lighting
	vec3 n = normalize(v_normal);
	vec3 l = normalize(lights[0].pos + v_eyeDir);
	
	vec4 diffuseColor = CalculateDiffuseColor(
		n, 
		l, 
		lights[0].diffuse, 
		material.diffuse, 1.0);	
	
	vec3 reflectedLight = reflect(-l, n);
	
	vec4 specularColor = CalculateSpecularColor(
		reflectedLight, 
		normalize(v_eyeDir), 
		lights[0].specular, 
		material.specular, 
		material.shininess, 
		1.0);
	
	vec4 ambientColor = lights[0].ambient * material.diffuse;

	vec4 colorWithLight = vec4(color.xyz * ( diffuseColor + ambientColor + clamp(specularColor, 0.0, 1.0)).xyz, 1.0);
    
	gl_FragColor = colorWithLight;
}
