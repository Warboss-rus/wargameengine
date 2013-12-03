#include <vector>
#include <string>
#include "MaterialManager.h"
#include "Vector3.h"
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
	C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes);
	std::vector<CVector3f> m_vertices;
	std::vector<CVector2f> m_textureCoords;
	std::vector<CVector3f> m_normals;
	std::vector<size_t> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
};