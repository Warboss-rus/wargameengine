uniform sampler2D texture;
uniform sampler2DShadow shadowMap;

varying vec3 normal;
varying vec3 eyeDir;	// eye direction
varying vec3 lightDir;
varying vec4 lpos;

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
	
	return specularLight * specularIntensity * specularMaterial;
}

void main()
{
	vec2 pos = gl_TexCoord[0].st;
	vec4 color = texture2D(texture, pos);

	vec4 diffuseMaterial = gl_FrontMaterial.diffuse;
	vec4 specularMaterial = gl_FrontMaterial.specular;
	
	// calculate diffuse lighting
	vec3 l = normalize(lightDir);
	float distance = length(lightDir);
	float attenuation = 
                  1.0 / (gl_LightSource[0].constantAttenuation 
                  + gl_LightSource[0].linearAttenuation * distance
                  + gl_LightSource[0].quadraticAttenuation 
                  * distance * distance);
	vec4 diffuseColor = CalculateDiffuseColor(
		normal, 
		l, 
		gl_LightSource[0].diffuse, 
		diffuseMaterial, attenuation);	
	
	// normalized eye direction
	vec3 eye = normalize(eyeDir);
	vec3 reflectedLight = reflect(-l, normal);
	vec4 specularColor = CalculateSpecularColor(
		reflectedLight, 
		eye, 
		gl_LightSource[0].specular, 
		specularMaterial, 
		gl_FrontMaterial.shininess, 
		0.7);
	
	vec4 ambientColor = gl_LightSource[0].ambient * diffuseMaterial;
	
	vec4 poissonDisk[4] = vec4[](
		vec4( -0.94201624, -0.39906216, 0.0, 0.0 ),
		vec4( 0.94558609, -0.76890725, 0.0, 0.0 ),
		vec4( -0.094184101, -0.92938870, 0.0, 0.0 ),
		vec4( 0.34495938, 0.29387760, 0.0, 0.0 )
	);
	
	float shadow = 0;
	for (int i = 0; i < 4; i++)
	{
		shadow += shadow2DProj(shadowMap, lpos + poissonDisk[i]/100.0).x;
	}
	vec4 diffuseSpecShadow = (diffuseColor + clamp(specularColor, 0.0, 1.0)) * shadow / 4;
	gl_FragColor =	 vec4(color.xyz * (ambientColor + diffuseSpecShadow).xyz, color.w);
}
