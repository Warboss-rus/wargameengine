#version 300 es
uniform samplerCube cubemapTexture;

in vec3 v_textureDir;

out vec4 fragColor;

void main()
{             
    fragColor = texture(cubemapTexture, v_textureDir);
}  