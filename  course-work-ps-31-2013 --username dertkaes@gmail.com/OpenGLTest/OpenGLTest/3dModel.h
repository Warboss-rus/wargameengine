#include<vector>
#include "MaterialManager.h"

struct sUsingMaterial
{
	std::string materialName;
	unsigned int polygonIndex;
};

class C3DModel
{
public:
	C3DModel(std::string const& path);
	void OpenMTL(std::string const& path);
	void Draw();
private:
	std::vector<double> m_vertices;
	std::vector<double> m_textureCoords;
	std::vector<double> m_normals;
	std::vector<unsigned int> m_polygon;
	CMaterialManager m_materials;
	std::vector<sUsingMaterial> m_usedMaterials;
};