#pragma once
#include <map>
#include "Material.h"

class CMaterialManager
{
public:
	CMaterialManager() {}
	CMaterialManager(std::map<std::string, sMaterial> & materials) { m_materials.swap(materials); }
	void InsertMaterials(std::map<std::string, sMaterial> const& materials);
	sMaterial * GetMaterial(std::string const& name);
	void AddMaterial (std::string const& name, sMaterial const& material);
private:
	std::map<std::string, sMaterial> m_materials;
};

