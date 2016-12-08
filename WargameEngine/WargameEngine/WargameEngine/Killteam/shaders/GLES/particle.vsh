attribute vec4 Position;
attribute vec3 Normal;
attribute vec2 TexCoord;
uniform mat4 mvp_matrix;
attribute vec4 instancePosition;
attribute vec2 instanceTexCoordPos;
varying vec2 v_texCoord;

void main()
{
	gl_Position = mvp_matrix * vec4(Position.xyz * instancePosition.w + instancePosition.xyz, Position.w);
	v_texCoord = TexCoord + instanceTexCoordPos;
}