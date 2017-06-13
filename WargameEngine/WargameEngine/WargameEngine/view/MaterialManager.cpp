#include "MaterialManager.h"

namespace wargameEngine
{
namespace view
{
Material * MaterialManager::GetMaterial(std::string const& name)
{
	if (m_materials.find(name) == m_materials.end())
	{
		return NULL;
	}
	return &m_materials.find(name)->second;
}

void MaterialManager::AddMaterial(std::string const& name, Material const& material)
{
	m_materials[name] = material;
}

void MaterialManager::InsertMaterials(std::unordered_map<std::string, Material> const& materials)
{
	m_materials.insert(materials.begin(), materials.end());
}
}
}