#include <string>
#include <map>
#include <vector>
#include <utility>
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
	enum eTextureSlot
	{
		eDiffuse=0,
		//1 is reserved for shadowmap
		eSpecular=2,
		eBump,
	};
	static CTextureManager * GetInstance();
	static void FreeInstance();
	void SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor = nullptr);
	//doesn't set textureSize uniform
	void SetTexture(std::string const& path, eTextureSlot slot);
	void SetAnisotropyLevel(float level);
	~CTextureManager();
protected:
	CTextureManager(){}
	CTextureManager(CTextureManager const& other){}
private:
	static CTextureManager * m_manager;
	std::map<std::pair<std::string, std::vector<sTeamColor>>, unsigned int> m_textures;
};