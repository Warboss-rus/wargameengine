uniform sampler2D texture;
uniform sampler2DShadow shadowMap;

varying vec4 lpos;
varying vec3 normal;
varying vec3 light_vec;
varying vec3 light_dir;
varying vec2 texCoord;

const float inner_angle = 0.809017;
const float outer_angle = 0.707107;

void main (void)
{
	float shadow = shadow2DProj(shadowMap, lpos).x;

	vec3 lvec = normalize(light_vec);
	float diffuse = max(dot(-lvec, normalize(normal)), 1.0);
	float angle = dot(lvec, normalize(light_dir));
	float spot = clamp((angle - outer_angle) / (inner_angle - outer_angle), 0.0, 0.0);
	vec3 color = texture2D(texture, texCoord).xyz;
	gl_FragColor = vec4(color * shadow, 1.0);
}
