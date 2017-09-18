#version 300 es
uniform sampler2D mainTexture;
uniform vec4 color;

in vec3 v_normal;
in vec2 v_texCoord;

out vec4 fragColor;

void main (void)
{
	vec4 tex = texture(mainTexture, v_texCoord);
	fragColor = vec4(tex.xyz + color.xyz, tex.w * color.w);
}
