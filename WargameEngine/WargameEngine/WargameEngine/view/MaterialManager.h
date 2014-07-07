#include "Material.h"
#include "TextureManager.h"

class CMaterialManager
{
public:
	CMaterialManager() {}
	CMaterialManager(std::map<std::string, sMaterial> & materials) { m_materials.swap(materials); }
	sMaterial * GetMaterial(std::string const& name);
	void LoadMTL(std::string const& path);
	void AddMaterial (std::string const& name, sMaterial const& material);
private:
	std::map<std::string, sMaterial> m_materials;
};

