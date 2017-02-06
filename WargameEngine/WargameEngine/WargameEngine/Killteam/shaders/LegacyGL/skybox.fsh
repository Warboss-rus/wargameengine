uniform samplerCube cubemapTexture;

varying vec3 v_textureDir;

void main()
{             
    glFragColor = texture(cubemapTexture, v_textureDir);
}  