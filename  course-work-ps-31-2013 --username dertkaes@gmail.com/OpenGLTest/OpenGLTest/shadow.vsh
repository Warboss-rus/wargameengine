uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform mat4 cameraModelViewMatrix;

varying vec4 lpos;
varying vec3 normal;
varying vec3 light_vec;
varying vec3 light_dir;
varying vec2 texCoord;

void main(void)
{
	vec4 vpos = gl_ModelViewMatrix * gl_Vertex;
	lpos = lightMatrix * inverse(cameraModelViewMatrix) * vpos;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	light_vec = vpos.xyz - lightPos;
	light_dir = gl_NormalMatrix * lightDir;
	normal = gl_NormalMatrix * gl_Normal;
	texCoord = gl_MultiTexCoord0.st;
}
