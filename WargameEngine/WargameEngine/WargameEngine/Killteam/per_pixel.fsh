uniform sampler2D texture;
uniform sampler2D shadowMap;

varying vec3 normal;

varying vec3 eyeDir;	// eye direction

varying vec3 lightDir;
varying vec4 shadowCoord;

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
	vec2 pos = gl_TexCoord[0].st;
	vec4 color = vec4(texture2D(texture, pos).xyz, 1.0);

	vec4 diffuseMaterial = gl_FrontMaterial.diffuse;
	vec4 specularMaterial = gl_FrontMaterial.specular;
	
	// calculate diffuse lighting
	vec3 n = normalize(normal);
	
	vec3 l = normalize(lightDir);
	
	vec4 diffuseColor = CalculateDiffuseColor(
		n, 
		l, 
		gl_LightSource[0].diffuse, 
		diffuseMaterial, 1.0);	
	
	// normalized eye direction
	vec3 eye = normalize(eyeDir);
	
	vec3 reflectedLight = reflect(-l, n);
	
	vec4 specularColor = CalculateSpecularColor(
		reflectedLight, 
		eye, 
		gl_LightSource[0].specular, 
		specularMaterial, 
		gl_FrontMaterial.shininess, 
		1.0);
	
	vec4 ambientColor = gl_LightSource[0].ambient * diffuseMaterial;

	float shadowDist = texture2DProj(shadowMap, shadowCoord).z;
        float diffuseDist = texture2DProj(texture, gl_TexCoord[0]).z;
	float dist = (shadowDist < diffuseDist) ? 0.5 : 1.0;
	vec4 colorWithLight = vec4(color.xyz * ( diffuseColor + ambientColor + clamp(specularColor, 0.0, 1.0)).xyz, 1.0);
    
        vec4 shadowCoordinateWdivide = shadowCoord / shadowCoord.w ;
		
	shadowCoordinateWdivide.z += 0.0005;
	float distanceFromLight = texture2D(shadowMap, shadowCoordinateWdivide.st).z;
 	float shadow = 1.0;
 	if (shadowCoord.w > 0.0)
		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
	gl_FragColor =	 colorWithLight;
}
