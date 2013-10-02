#include <string>
#include <map>
#pragma once

class CTextureManager
{
public:
	static CTextureManager * GetInstance();
	static void FreeInstance();
	unsigned int CTextureManager::GetTexture(std::string const& path);
protected:
	CTextureManager(){}
	CTextureManager(CTextureManager const& other){}
private:
	static CTextureManager * m_manager;
	std::map<std::string, unsigned int> m_textures;
};