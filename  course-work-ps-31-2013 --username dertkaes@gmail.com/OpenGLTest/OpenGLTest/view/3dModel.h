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
	C3DModel(std::vector<CVector3d> & vertices, std::vector<CVector2d> & textureCoords, std::vector<CVector3d> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale);
	void Draw(const std::set<std::string>* hideMeshes = NULL);
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
private:
	std::vector<CVector3d> m_vertices;
	std::vector<CVector2d> m_textureCoords;
	std::vector<CVector3d> m_normals;
	std::vector<size_t> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
	std::shared_ptr<IBounding> m_bounding;
	double m_scale;
};