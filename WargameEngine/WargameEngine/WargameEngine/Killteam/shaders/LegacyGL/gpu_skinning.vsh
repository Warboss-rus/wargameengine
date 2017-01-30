#version 130
attribute ivec4 weightIndices;
attribute vec4 weights;

uniform mat4 joints[512];
uniform mat4 invBindMatrices[512];

varying vec3 normal;
varying vec2 texCoord;

void main() {
    vec4 newVertex = gl_Vertex;
	vec4 newNormal = vec4(gl_Normal, 1.0);
    
	if(weights.x > 0.0)//there are weights
	{
		newVertex = vec4(0.0);
		newNormal = vec4(0.0);
		for(int i = 0; i < 4; ++i)
		{
			newVertex += (gl_Vertex * invBindMatrices[weightIndices[i]] * joints[weightIndices[i]]) * weights[i];
			newNormal += (vec4(gl_Normal, 1.0) * invBindMatrices[weightIndices[i]] * joints[weightIndices[i]]) * weights[i];
		}
	}
	
    normal  = normalize(gl_NormalMatrix * newNormal.xyz); 
    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
    texCoord = gl_MultiTexCoord0.st;
}