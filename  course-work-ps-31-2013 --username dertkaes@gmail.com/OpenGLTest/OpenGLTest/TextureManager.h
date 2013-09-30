#include <string>
#include <map>
#pragma once

class CTextureManager
{
public:
	unsigned int CTextureManager::GetTexture(std::string const& path);
private:
	std::map<std::string, unsigned int> m_textures;
};