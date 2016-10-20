attribute vec4 Position;
attribute vec2 TexCoord;
attribute vec4 instancePosition;
attribute vec2 instanceTexCoordPos;
uniform mat4 mvp_matrix;

varying vec2 v_texCoord;

void main()
{
	gl_Position = mvp_matrix * vec4(Position.xyz * instancePosition.w + instancePosition.xyz, Position.w);
	v_texCoord = TexCoord + instanceTexCoordPos;
}