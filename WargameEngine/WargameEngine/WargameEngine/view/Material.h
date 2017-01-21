#include <string>

class ICachedTexture;

struct sMaterial
{
	sMaterial()
	{
		for(unsigned int i = 0; i < 3; ++i)
		{
			ambient[i] = 0.2f;
			diffuse[i] = 0.8f;
			specular[i] = 0.0f;
		}
	}
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess = 0.0f;
	std::wstring texture;
	std::wstring specularMap;
	std::wstring bumpMap;
	ICachedTexture* texturePtr = nullptr;
};