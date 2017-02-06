#pragma once
#include <memory>
#include <vector>

enum class TextureSlot
{
	eDiffuse = 0,
	eShadowMap = 1,
	eSpecular = 2,
	eBump = 3,
};
enum TextureFlags
{
	TEXTURE_NO_WRAP = 1,
	TEXTURE_BUILD_MIPMAPS = 2,
	TEXTURE_HAS_ALPHA = 4,
	TEXTURE_BGRA = 8,
	TEXTURE_COMPRESSION_DXT1_NO_ALPHA = 0,
	TEXTURE_COMPRESSION_DXT1 = 16,
	TEXTURE_COMPRESSION_DXT3 = 32,
	TEXTURE_COMPRESSION_DXT5 = 48,
	TEXTURE_COMPRESSION_MASK = 48,
};

class ICachedTexture
{
public:
	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;
	virtual ~ICachedTexture() {}
};

struct sTextureMipMap
{
	unsigned char * data;
	unsigned int width; 
	unsigned int height;
	size_t size;//compressed only
};
typedef std::vector<sTextureMipMap> TextureMipMaps;

class ITextureHelper
{
public:
	virtual ~ITextureHelper() {}

	virtual void ActivateTextureSlot(TextureSlot slot) = 0;
	virtual void UnbindTexture() = 0;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture(bool cubemap = false) = 0;
	virtual void SetTextureAnisotropy(float value = 1.0f) = 0;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) = 0;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) = 0;
	virtual void UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags) = 0;

	virtual bool Force32Bits() const = 0;
	virtual bool ForceFlipBMP() const = 0;
	virtual bool ConvertBgra() const = 0;
};