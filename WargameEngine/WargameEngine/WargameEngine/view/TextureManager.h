#pragma once
#include <string>
#include <map>
#include <vector>
#include "IRenderer.h"

struct sImage;
class CAsyncFileProvider;
class IImageReader;
class CImage;

class CTextureManager
{
public:
	CTextureManager(ITextureHelper & helper, CAsyncFileProvider & asyncFileProvider);
	~CTextureManager();
	void SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	//doesn't set textureSize uniform
	void SetTexture(std::string const& path, TextureSlot slot, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	void Reset();
	void RegisterImageReader(std::unique_ptr<IImageReader> && reader);
protected:
	CTextureManager(CTextureManager const& other) = delete;
private:
	std::unique_ptr<ICachedTexture> LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, bool now = false, int flags = 0);
	void UseTexture(CImage const& img, ICachedTexture& texture, int additionalFlags);

	std::map<std::pair<std::string, std::vector<sTeamColor>>, std::unique_ptr<ICachedTexture>> m_textures;
	float m_anisotropyLevel = 1.0f;
	ITextureHelper & m_helper;
	CAsyncFileProvider & m_asyncFileProvider;
	std::vector<std::unique_ptr<IImageReader>> m_imageReaders;
};