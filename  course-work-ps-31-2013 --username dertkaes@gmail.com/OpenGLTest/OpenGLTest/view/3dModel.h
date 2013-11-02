#include <vector>
#include <memory>
#include <string>
#include <set>
#include "MaterialManager.h"
#include "Bounding.h"
#pragma once

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

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t polygonIndex;
};

class C3DModel
{
public:
	C3DModel() {}
	C3DModel(std::vector<sPoint3> & vertices, std::vector<sPoint2> & textureCoords, std::vector<sPoint3> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale);
	void Draw(std::set<std::string> const& hideMeshes);
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
private:
	std::vector<sPoint3> m_vertices;
	std::vector<sPoint2> m_textureCoords;
	std::vector<sPoint3> m_normals;
	std::vector<size_t> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
	std::shared_ptr<IBounding> m_bounding;
	double m_scale;
};