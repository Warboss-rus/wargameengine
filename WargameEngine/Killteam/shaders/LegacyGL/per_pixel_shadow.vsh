uniform mat4 lightMatrix;

varying vec3 normal;
varying vec3 eyeDir;	// eye direction
varying vec3 lightDir;
varying vec4 lpos;

void main()
{
	// Calculate vertex position
	gl_Position = ftransform();
	
	// calculate normal in eye space
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	eyeDir = -(gl_ModelViewMatrix * gl_Vertex).xyz;
	
	lightDir = vec3(gl_LightSource[0].position) + eyeDir;
	
	// copy multi texture coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec4 vpos = gl_ModelViewMatrix * gl_Vertex;
	lpos = lightMatrix * vpos;
}
