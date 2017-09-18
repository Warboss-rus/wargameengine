#version 330 core
uniform samplerCube cubemapTexture;

in vec3 v_textureDir;

out vec4 fragColor;

void main()
{             
    fragColor = texture(cubemapTexture, v_textureDir);
}  