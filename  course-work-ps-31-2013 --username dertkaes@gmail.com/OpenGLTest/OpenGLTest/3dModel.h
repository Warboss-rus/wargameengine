#include<vector>
#include "MaterialManager.h"

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
	std::string m_usedMaterial;
};