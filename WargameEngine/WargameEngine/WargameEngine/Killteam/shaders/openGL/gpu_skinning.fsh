#version 330 core
uniform sampler2D mainTexture;
uniform vec4 color;

in vec3 v_normal;
in vec2 v_texCoord;

out vec4 fragColor;

void main (void)
{
	vec4 tex = texture(mainTexture, v_texCoord);
	float alpha = tex.w * color.w;
	if(alpha <= 0.01)
		discard;
	fragColor = vec4(tex.xyz + color.xyz, alpha);
}
