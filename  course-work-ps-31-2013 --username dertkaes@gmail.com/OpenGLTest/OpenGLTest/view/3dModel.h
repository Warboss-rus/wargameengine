#include <vector>
#include <memory>
#include <string>
#include <set>
#include "MaterialManager.h"
#include "Bounding.h"
#pragma once

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
	C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, float scale);
	void Draw(const std::set<std::string>* hideMeshes = NULL);
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
private:
	std::vector<CVector3f> m_vertices;
	std::vector<CVector2f> m_textureCoords;
	std::vector<CVector3f> m_normals;
	std::vector<size_t> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
	std::shared_ptr<IBounding> m_bounding;
	float m_scale;
};