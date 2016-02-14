#pragma once
#include <string>
#include <map>
#include <vector>
#include "IRenderer.h"

struct sImage;
class CAsyncFileProvider;

class CTextureManager
{
public:
	CTextureManager(ITextureHelper & helper, CAsyncFileProvider & asyncFileProvider);
	void SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	//doesn't set textureSize uniform
	void SetTexture(std::string const& path, TextureSlot slot, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	void Reset();
protected:
	CTextureManager(CTextureManager const& other) = delete;
private:
	std::unique_ptr<ICachedTexture> LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, bool now = false, int flags = 0);
	void UseTexture(sImage const& img, ICachedTexture& texture);

	std::map<std::pair<std::string, std::vector<sTeamColor>>, std::unique_ptr<ICachedTexture>> m_textures;
	float m_anisotropyLevel = 1.0f;
	ITextureHelper & m_helper;
	CAsyncFileProvider & m_asyncFileProvider;
};