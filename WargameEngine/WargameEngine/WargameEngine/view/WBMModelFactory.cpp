#include "OBJModelFactory.h"
#include <string>
#include <map>
#include <vector>
#include <cstring>

void LoadWbmModel(void* data, unsigned int dataSize, sOBJLoader & loader)
{
	std::map<std::string, sMaterial> materials;
	std::vector<sMesh> meshes;
	unsigned int count;
	unsigned int size;
	unsigned int version;
	memcpy(&version, data, sizeof(unsigned int));
	memcpy(&size, &((char*)data)[4], sizeof(unsigned int));
	loader.vertices.resize(size / sizeof(CVector3f));
	memcpy(&loader.vertices[0], &((char*)data)[8], size);
	unsigned int position = size + 8;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	loader.textureCoords.resize(size / sizeof(CVector2f));
	memcpy(&loader.textureCoords[0], &((char*)data)[position + 4], size);
	position += size + 4;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	loader.normals.resize(size / sizeof(CVector3f));
	memcpy(&loader.normals[0], &((char*)data)[position + 4], size);
	position += size + 4;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	loader.indexes.resize(size / sizeof(unsigned int));
	memcpy(&loader.indexes[0], &((char*)data)[position + 4], size);
	position += size + 4;
	memcpy(&count, &((char*)data)[position], sizeof(unsigned int));
	position += 4;
	for(unsigned int i = 0; i < count; ++i)
	{
		sMesh mesh;
		memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
		mesh.name.resize(size);
		memcpy(&mesh.name[0], &((char*)data)[position + 4], size);
		position += size + 4;
		memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
		mesh.materialName.resize(size);
		memcpy(&mesh.materialName[0], &((char*)data)[position + 4], size);
		position += size + 4;
		memcpy(&mesh.polygonIndex, &((char*)data)[position], sizeof(unsigned int));
		position += 4;
		loader.meshes.push_back(mesh);
	}
	memcpy(&count, &((char*)data)[position], sizeof(unsigned int));
	position += 4;
	for(unsigned int i = 0; i < count; ++i)
	{
		std::string key;
		memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
		key.resize(size);
		memcpy(&key[0], &((char*)data)[position + 4], size);
		position += size + 4;
		memcpy(&materials[key].ambient[0], &((char*)data)[position + 4], sizeof(float) * 3);
		memcpy(&materials[key].diffuse[0], &((char*)data)[position + 4 + sizeof(float) * 3], sizeof(float) * 3);
		memcpy(&materials[key].specular[0], &((char*)data)[position + 4 + sizeof(float) * 6], sizeof(float) * 3);
		memcpy(&materials[key].shininess, &((char*)data)[position + 4 + sizeof(float) * 9], sizeof(float));
		position += sizeof(float) * 10;
		memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
		materials[key].texture.resize(size);
		memcpy(&materials[key].texture[0], &((char*)data)[position + 4], size);
		position += size + 4;
	}
	loader.materialManager =  CMaterialManager(materials);
	delete [] data;
}