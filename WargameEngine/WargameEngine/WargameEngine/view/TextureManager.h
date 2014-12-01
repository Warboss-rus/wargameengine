#include <string>
#include <map>
#include <utility>
#pragma once

class CTextureManager
{
public:
	enum eTextureSlot
	{
		eDiffuse=0,
		//1 is reserved for shadowmap
		eSpecular=2,
		eNormal,
		//uses different UVs
		eBump,
	};
	static CTextureManager * GetInstance();
	static void FreeInstance();
	void SetTexture(std::string const& path);
	void SetTexture(std::string const& path, eTextureSlot slot);
	void SetAnisotropyLevel(float level);
	void SetTextureSize(unsigned int id, unsigned int width, unsigned int height);
	~CTextureManager();
protected:
	CTextureManager(){}
	CTextureManager(CTextureManager const& other){}
private:
	static CTextureManager * m_manager;
	std::map<std::string, unsigned int> m_textures;
	std::map<unsigned int, std::pair<unsigned int, unsigned int>> m_size;
};