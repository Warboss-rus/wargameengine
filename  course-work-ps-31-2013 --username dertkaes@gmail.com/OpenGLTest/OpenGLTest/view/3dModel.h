#include<vector>
#include <memory>
#include "MaterialManager.h"
#include "Bounding.h"
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
	C3DModel() {}
	C3DModel(std::vector<sPoint3> & vertices, std::vector<sPoint2> & textureCoords, std::vector<sPoint3> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sUsingMaterial> & usedMaterials, std::shared_ptr<IBounding> bounding);
	void Draw();
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
private:
	std::vector<sPoint3> m_vertices;
	std::vector<sPoint2> m_textureCoords;
	std::vector<sPoint3> m_normals;
	std::vector<unsigned int> m_indexes;
	CMaterialManager m_materials;
	std::vector<sUsingMaterial> m_usedMaterials;
	std::shared_ptr<IBounding> m_bounding;
};