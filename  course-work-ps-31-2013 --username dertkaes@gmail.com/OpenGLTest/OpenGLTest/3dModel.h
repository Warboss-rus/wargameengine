#include<vector>
#include "MaterialManager.h"
#pragma once

struct sUsingMaterial
{
	std::string materialName;
	unsigned int polygonIndex;
};

struct sPoint3
{
	sPoint3(): x(0), y(0), z(0){}
	double x;
	double y;
	double z;
};

struct sPoint2
{
	sPoint2(): x(0), y(0){}
	double x;
	double y;
};

class C3DModel
{
public:
	C3DModel(std::string const& path);
	void OpenMTL(std::string const& path);
	void Draw();
private:
	std::vector<sPoint3> m_vertices;
	std::vector<sPoint2> m_textureCoords;
	std::vector<sPoint3> m_normals;
	std::vector<unsigned int> m_indexes;
	CMaterialManager m_materials;
	std::vector<sUsingMaterial> m_usedMaterials;
};