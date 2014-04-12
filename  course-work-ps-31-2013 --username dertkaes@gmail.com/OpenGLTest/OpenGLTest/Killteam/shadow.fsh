uniform sampler2D texture;
uniform sampler2DShadow shadowMap;

varying vec4 lpos;
varying vec2 texCoord;

void main (void)
{
	float shadow = clamp(shadow2DProj(shadowMap, lpos).x, 0.4, 1.0);
	vec3 color = texture2D(texture, texCoord).xyz;
	gl_FragColor = vec4(color * shadow, 1.0);
}
