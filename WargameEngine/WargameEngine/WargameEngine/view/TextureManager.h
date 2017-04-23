#pragma once
#include "../Typedefs.h"
#include <map>
#include <vector>
#include "ITextureHelper.h"

struct sImage;
class CAsyncFileProvider;
class IImageReader;
class CImage;

class CTextureManager
{
public:
	CTextureManager(ITextureHelper & helper, CAsyncFileProvider & asyncFileProvider);
	~CTextureManager();
	void SetTexture(const Path& path, int flags = 0);
	void SetTexture(const Path& path, TextureSlot slot, const std::vector<sTeamColor>* teamcolor = nullptr, int flags = 0);
	std::unique_ptr<ICachedTexture> CreateCubemapTexture(const Path& right, const Path& left, const Path& back, const Path& front, const Path& top, const Path& bottom, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(const Path& path, const std::vector<sTeamColor>* teamcolor = nullptr, int flags = 0);
	void Reset();
	void RegisterImageReader(std::unique_ptr<IImageReader>&& reader);
	ICachedTexture* GetTexturePtr(const Path& texture);
protected:
	CTextureManager(CTextureManager const& other) = delete;
private:
	std::unique_ptr<ICachedTexture> LoadTexture(const Path& path, const std::vector<sTeamColor>& teamcolor, bool now = false, int flags = 0);
	void UseTexture(CImage const& img, ICachedTexture& texture, int additionalFlags);

	std::map<std::pair<Path, std::vector<sTeamColor>>, std::unique_ptr<ICachedTexture>> m_textures;
	float m_anisotropyLevel = 1.0f;
	ITextureHelper & m_helper;
	CAsyncFileProvider & m_asyncFileProvider;
	std::vector<std::unique_ptr<IImageReader>> m_imageReaders;
};