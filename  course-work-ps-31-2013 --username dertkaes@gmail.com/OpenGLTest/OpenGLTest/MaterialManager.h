#include "Material.h"
#include "TextureManager.h"

class CMaterialManager
{
public:
	sMaterial * GetMaterial(std::string const& name);
	~CMaterialManager();
	void LoadMTL(std::string const& path);
private:
	std::map<std::string, sMaterial *> m_materials;
	CTextureManager m_textureManager;
};

