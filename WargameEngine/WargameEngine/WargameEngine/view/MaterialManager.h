#pragma once
#include "Material.h"
#include <unordered_map>

namespace wargameEngine
{
namespace view
{
class MaterialManager
{
public:
	MaterialManager() {}
	MaterialManager(std::unordered_map<std::string, Material>& materials) { m_materials.swap(materials); }
	void InsertMaterials(std::unordered_map<std::string, Material> const& materials);
	Material* GetMaterial(std::string const& name);
	void AddMaterial(std::string const& name, Material const& material);

private:
	std::unordered_map<std::string, Material> m_materials;
};
}
}