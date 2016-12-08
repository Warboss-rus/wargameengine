precision mediump float;
uniform sampler2D texture;
uniform vec4 color;
varying vec2 v_texCoord;

void main()
{
	vec4 tex = texture2D(texture, v_texCoord);
	gl_FragColor = vec4(tex.xyz + color.xyz, tex.w * color.w);
}