#pragma once
#include "../Typedefs.h"
#include "../model/TeamColor.h"
#include <unordered_map>
#include <map>
#include <vector>
#include "ITextureHelper.h"

namespace wargameEngine
{
class AsyncFileProvider;

namespace view
{
class IImageReader;
class Image;

class TextureManager
{
public:
	TextureManager(ITextureHelper & helper, AsyncFileProvider & asyncFileProvider);
	~TextureManager();
	std::unique_ptr<ICachedTexture> CreateCubemapTexture(const Path& right, const Path& left, const Path& back, const Path& front, const Path& top, const Path& bottom, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(const Path& path, int flags = 0);
	void Reset();
	void RegisterImageReader(std::unique_ptr<IImageReader>&& reader);
	ICachedTexture* GetTexturePtr(const Path& texture, const std::vector<model::TeamColor>* teamcolor = nullptr, int flags = 0);
protected:
	TextureManager(TextureManager const& other) = delete;
private:
	std::unique_ptr<ICachedTexture> LoadTexture(const Path& path, const std::vector<model::TeamColor>& teamcolor, bool now = false, int flags = 0);
	void UseTexture(Image const& img, ICachedTexture& texture, int additionalFlags);

	std::unordered_map<Path, std::unique_ptr<ICachedTexture>> m_textures;
	std::map<std::pair<Path, std::vector<model::TeamColor>>, std::unique_ptr<ICachedTexture>> m_teamcolorTextures;
	float m_anisotropyLevel = 1.0f;
	ITextureHelper & m_helper;
	AsyncFileProvider & m_asyncFileProvider;
	std::vector<std::unique_ptr<IImageReader>> m_imageReaders;
};
}
}