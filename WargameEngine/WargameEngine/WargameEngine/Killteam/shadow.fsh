uniform sampler2D texture;
uniform sampler2DShadow shadowMap;

varying vec4 lpos;
varying vec2 texCoord;

void main (void)
{
	vec4 poissonDisk[4] = vec4[](
		vec4( -0.94201624, -0.39906216, 0.0, 0.0 ),
		vec4( 0.94558609, -0.76890725, 0.0, 0.0 ),
		vec4( -0.094184101, -0.92938870, 0.0, 0.0 ),
		vec4( 0.34495938, 0.29387760, 0.0, 0.0 )
	);
	float shadow = 0;
	for (int i = 0; i < 4; i++)
	{
		shadow += shadow2DProj(shadowMap, lpos + poissonDisk[i]/50.0).x;
	}
	shadow = clamp(shadow / 4, 0.4, 1.0);
	vec4 color = texture2D(texture, texCoord);
	gl_FragColor = vec4(color.xyz * shadow, color.w);
}
