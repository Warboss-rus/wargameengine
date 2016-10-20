uniform sampler2D texture;
uniform vec4 color;
varying vec3 v_normal;
varying vec2 v_texCoord;

void main (void)
{
	vec4 tex = texture2D(texture, v_texCoord);
	gl_FragColor = vec4(tex.xyz + color.xyz, tex.w * color.w);
}
