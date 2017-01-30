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
	void SetTexture(std::wstring const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	//doesn't set textureSize uniform
	void SetTexture(std::wstring const& path, TextureSlot slot, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(std::wstring const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	void Reset();
	void RegisterImageReader(std::unique_ptr<IImageReader> && reader);
	ICachedTexture* GetTexturePtr(std::wstring const& texture);
protected:
	CTextureManager(CTextureManager const& other) = delete;
private:
	std::unique_ptr<ICachedTexture> LoadTexture(std::wstring const& path, std::vector<sTeamColor> const& teamcolor, bool now = false, int flags = 0);
	void UseTexture(CImage const& img, ICachedTexture& texture, int additionalFlags);

	std::map<std::pair<std::wstring, std::vector<sTeamColor>>, std::unique_ptr<ICachedTexture>> m_textures;
	float m_anisotropyLevel = 1.0f;
	ITextureHelper & m_helper;
	CAsyncFileProvider & m_asyncFileProvider;
	std::vector<std::unique_ptr<IImageReader>> m_imageReaders;
};