#pragma once
#include <string>
#include <map>
#include <vector>
#include "IRenderer.h"

inline bool operator< (sTeamColor const& one, sTeamColor const& two) { return one.suffix < two.suffix || memcmp(one.color, two.color, 3) < 0; }

class CTextureManager
{
public:
	CTextureManager() = default;
	~CTextureManager();
	void SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	//doesn't set textureSize uniform
	void SetTexture(std::string const& path, TextureSlot slot, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	void Reset();
protected:
	CTextureManager(CTextureManager const& other) = delete;
private:
	std::map<std::pair<std::string, std::vector<sTeamColor>>, unsigned int> m_textures;
	float m_anisotropyLevel = 1.0f;
};