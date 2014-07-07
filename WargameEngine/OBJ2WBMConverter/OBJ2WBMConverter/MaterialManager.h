#include "Material.h"
#include <map>

class CMaterialManager
{
public:
	void LoadMTL(std::string const& path);
	std::map<std::string, sMaterial> m_materials;
};

