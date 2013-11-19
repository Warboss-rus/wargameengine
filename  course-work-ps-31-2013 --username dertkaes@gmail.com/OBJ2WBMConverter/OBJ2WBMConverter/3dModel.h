#include <vector>
#include <memory>
#include <string>
#include <set>
#include "MaterialManager.h"
#include "Structs.h"
#pragma once

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t polygonIndex;
};

class IBounding;

class C3DModel
{
public:
	C3DModel() {}
	C3DModel(std::vector<sPoint3> & vertices, std::vector<sPoint2> & textureCoords, std::vector<sPoint3> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale);
	std::vector<sPoint3> m_vertices;
	std::vector<sPoint2> m_textureCoords;
	std::vector<sPoint3> m_normals;
	std::vector<size_t> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
};