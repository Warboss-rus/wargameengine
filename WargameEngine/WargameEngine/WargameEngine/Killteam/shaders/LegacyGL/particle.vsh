#version 130
attribute vec4 instancePosition;
attribute vec2 instanceTexCoordPos;

out vec2 texCoord;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz * instancePosition.w + instancePosition.xyz, gl_Vertex.w);
	texCoord = gl_MultiTexCoord0.st + instanceTexCoordPos.st;
}