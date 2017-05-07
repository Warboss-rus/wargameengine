#include "../Typedefs.h"

class ICachedTexture;

struct sMaterial
{
	float ambient[3] = {0.2f, 0.2f, 0.2f};
	float diffuse[3] = {0.8f, 0.8f, 0.8f};
	float specular[3] = {0.0f, 0.0f, 0.0f};
	float shininess = 0.0f;
	Path texture;
	Path specularMap;
	Path bumpMap;
	ICachedTexture* texturePtr = nullptr;
};