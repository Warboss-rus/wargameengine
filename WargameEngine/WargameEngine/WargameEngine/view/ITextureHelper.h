#pragma once
#include <memory>

enum class TextureSlot
{
	eDiffuse = 0,
	//1 is reserved for shadowmap
	eSpecular = 2,
	eBump,
};
enum TextureFlags
{
	TEXTURE_NO_WRAP = 1,
	TEXTURE_BUILD_MIPMAPS = 2,
	TEXTURE_HAS_ALPHA = 4,
	TEXTURE_BGRA = 8,
};

class ICachedTexture
{
public:
	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;
	virtual ~ICachedTexture() {}
};

class ITextureHelper
{
public:
	virtual ~ITextureHelper() {}

	virtual void ActivateTextureSlot(TextureSlot slot) = 0;
	virtual void UnbindTexture() = 0;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture() = 0;
	virtual void SetTextureAnisotropy(float value = 1.0f) = 0;
	virtual void UploadTexture(unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags) = 0;
};