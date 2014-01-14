#include "WBMModelFactory.h"
#include <string>
#include <map>
#include <vector>

C3DModel * LoadWbmModel(std::string const& path)
{
	std::vector<CVector3f> newVertices;
	std::vector<CVector2f> newTextureCoords;
	std::vector<CVector3f> newNormals;
	std::vector<unsigned int> indexes;
	std::map<std::string, sMaterial> materials;
	std::vector<sMesh> meshes;
	FILE* iFile = fopen(path.c_str(), "rb");
	unsigned int size;
	unsigned int count;
	fread(&size, sizeof(size_t), 1, iFile);
	newVertices.resize(size / sizeof(CVector3f));
	fread(&newVertices[0], size, 1, iFile);
	fread(&size, sizeof(size_t), 1, iFile);
	newTextureCoords.resize(size / sizeof(CVector2f));
	fread(&newTextureCoords[0], size, 1, iFile);
	fread(&size, sizeof(size_t), 1, iFile);
	newNormals.resize(size / sizeof(CVector3f));
	fread(&newNormals[0], size, 1, iFile);
	fread(&size, sizeof(size_t), 1, iFile);
	indexes.resize(size / sizeof(size_t));
	fread(&indexes[0], size, 1, iFile);
	fread(&count, sizeof(size_t), 1, iFile);
	for(size_t i = 0; i < count; ++i)
	{
		sMesh mesh;
		fread(&size, sizeof(size_t), 1, iFile);
		mesh.name.resize(size);
		fread(&mesh.name[0], size, 1, iFile);
		fread(&size, sizeof(size_t), 1, iFile);
		mesh.materialName.resize(size);
		fread(&mesh.materialName[0], size, 1, iFile);
		fread(&mesh.polygonIndex, sizeof(size_t), 1, iFile);
		meshes.push_back(mesh);
	}
	fread(&count, sizeof(size_t), 1, iFile);
	for(size_t i = 0; i < count; ++i)
	{
		std::string key;
		fread(&size, sizeof(size_t), 1, iFile);
		key.resize(size);
		fread(&key[0], size, 1, iFile);
		fread(&materials[key].ambient[0], sizeof(float) * 3, 1, iFile);
		fread(&materials[key].diffuse[0], sizeof(float) * 3, 1, iFile);
		fread(&materials[key].specular[0], sizeof(float) * 3, 1, iFile);
		fread(&materials[key].shininess, sizeof(float), 1, iFile);
		fread(&size, sizeof(size_t), 1, iFile);
		materials[key].texture.resize(size);
		fread(&materials[key].texture[0], size, 1, iFile);
	}
	fclose(iFile);
	CMaterialManager materialManager(materials);
	std::string boundingPath = path.substr(0, path.find_last_of('.')) + ".txt";
	float scale = 1.0;
	std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(boundingPath, scale);
	return new C3DModel(newVertices, newTextureCoords, newNormals, indexes, materialManager, meshes, bounding, scale);
}