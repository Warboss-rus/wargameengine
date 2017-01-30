uniform mat4 lightMatrix;

varying vec4 lpos;
varying vec2 texCoord;

void main(void)
{
	vec4 vpos = gl_ModelViewMatrix * gl_Vertex;
	lpos = lightMatrix * vpos;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texCoord = gl_MultiTexCoord0.st;
}
