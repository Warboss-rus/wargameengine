#pragma once

#include <string>
#include <map>
#include "../view/gl.h"


class CTextureManager
{
public:
	static CTextureManager * GetInstance();
	static void FreeInstance();
	void CTextureManager::SetTexture(std::string const& path);
	void CTextureManager::SetShaderVarLocation(unsigned int textureLocation);
protected:
	CTextureManager(): m_shaderTextureVarLocation(0){}
	CTextureManager(CTextureManager const& other){}
private:
	static CTextureManager * m_manager;
	std::map<std::string, unsigned int> m_textures;
	unsigned int m_shaderTextureVarLocation;
};