#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include "MaterialManager.h"
#include "Bounding.h"
#pragma once

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t polygonIndex;
	bool operator== (sMesh const& other) { return name == other.name && materialName == other.materialName && polygonIndex == other.polygonIndex; }
	bool operator!= (sMesh const& other) { return !operator==(other); }
};

class C3DModel
{
public:
	C3DModel(std::shared_ptr<IBounding> bounding, double scale);
	C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale);
	~C3DModel();
	void SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes);
	void Draw(const std::set<std::string>* hideMeshes = NULL, bool vertexOnly=false);
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
	void SetBounding(std::shared_ptr<IBounding> bounding, double scale);
	void Preload() const;
private:
	void NewList(unsigned int & list, const std::set<std::string> * hideMeshes, bool vertexOnly);
	std::vector<CVector3f> m_vertices;
	std::vector<CVector2f> m_textureCoords;
	std::vector<CVector3f> m_normals;
	std::vector<unsigned int> m_indexes;
	std::vector<sMesh> m_meshes;
	CMaterialManager m_materials;
	std::shared_ptr<IBounding> m_bounding;
	std::map<std::set<std::string>, unsigned int> m_lists;
	std::map<std::set<std::string>, unsigned int> m_vertexLists;
	double m_scale;
	unsigned int m_vbo;
	int m_count;
};