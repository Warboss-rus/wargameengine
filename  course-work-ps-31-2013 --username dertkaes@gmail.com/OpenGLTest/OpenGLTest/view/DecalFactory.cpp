#include "DecalFactory.h"
#include <stdlib.h>

C3DModel * LoadDecal(std::string const& path)
{
	std::string temp = path;
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	size_t index = temp.find_first_of(' ');
	float width = atof(temp.substr(0, index).c_str());
	temp = temp.substr(index + 1, temp.size() - index - 1);
	index = temp.find_last_of(' ');
	float height = (float)atof(temp.substr(0, index).c_str());
	temp = temp.substr(index + 1, temp.size() - index - 1);
	vertices.push_back(CVector3f(0.0f, 0.0f, 0.01f));
	vertices.push_back(CVector3f(width, 0.0f, 0.01f));
	vertices.push_back(CVector3f(0.0f, height, 0.01f));
	vertices.push_back(CVector3f(width, height, 0.01f));
	textureCoords.push_back(CVector2f(0.0f, 0.0f));
	textureCoords.push_back(CVector2f(1.0f, 0.0f));
	textureCoords.push_back(CVector2f(0.0f, 1.0f));
	textureCoords.push_back(CVector2f(1.0f, 1.0f));
	normals.push_back(CVector3f(0.0f, 0.0f, 1.0f));
	normals.push_back(CVector3f(0.0f, 0.0f, 1.0f));
	normals.push_back(CVector3f(0.0f, 0.0f, 1.0f));
	normals.push_back(CVector3f(0.0f, 0.0f, 1.0f));
	indexes.push_back(0);
	indexes.push_back(1);
	indexes.push_back(2);
	indexes.push_back(2);
	indexes.push_back(1);
	indexes.push_back(3);
	sMaterial material;
	material.texture = temp;
	materialManager.AddMaterial("decal", material);
	sMesh mesh;
	mesh.materialName = "decal";
	mesh.name = "decal";
	mesh.polygonIndex = 0;
	meshes.push_back(mesh);
	std::shared_ptr<IBounding> bounding (new CBoundingCompound());
	return new C3DModel(vertices, textureCoords, normals, indexes, materialManager, meshes, bounding, 1.0);
}