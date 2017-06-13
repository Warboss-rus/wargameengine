#include "../Typedefs.h"

namespace wargameEngine
{
namespace view
{
class ICachedTexture;

struct Material
{
	float ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 0.0f;
	Path texture;
	Path specularMap;
	Path bumpMap;
	ICachedTexture* texturePtr = nullptr;
};
}
}