#include <string>
#include <map>
#include <vector>
#include <utility>
#include <string.h>
#pragma once

struct sTeamColor
{
	std::string suffix;
	unsigned char color[3];
};

inline bool operator< (sTeamColor const& one, sTeamColor const& two) { return one.suffix < two.suffix || memcmp(one.color, two.color, 3) < 0; }

class CTextureManager
{
public:
	enum class eTextureSlot
	{
		eDiffuse=0,
		//1 is reserved for shadowmap
		eSpecular=2,
		eBump,
	};
	enum eTextureFlags
	{
		TEXTURE_NO_WRAP = 1,
	};
	static CTextureManager * GetInstance();
	static void FreeInstance();
	void SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	//doesn't set textureSize uniform
	void SetTexture(std::string const& path, eTextureSlot slot, int flags = 0);
	void SetAnisotropyLevel(float level);
	void LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr, int flags = 0);
	~CTextureManager();
protected:
	CTextureManager() = default;
	CTextureManager(CTextureManager const& other) = default;
private:
	static CTextureManager * m_manager;
	std::map<std::pair<std::string, std::vector<sTeamColor>>, unsigned int> m_textures;
};