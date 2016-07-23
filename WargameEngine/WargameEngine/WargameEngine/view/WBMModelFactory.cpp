#include "WBMModelFactory.h"
#include "3dModel.h"
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <algorithm>
#include "..\Utils.h"

std::unique_ptr<C3DModel> CWBMModelFactory::LoadModel(unsigned char * data, size_t /*size*/, C3DModel const& dummyModel, std::wstring const& /*filePath*/)
{
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	std::vector<unsigned int> weightsCount;
	std::vector<unsigned int> weightsIndexes;
	std::vector<float> weights;
	std::vector<sJoint> joints;
	std::vector<sAnimation> animations;
	
	std::map<std::string, sMaterial> materials;
	unsigned int count;
	unsigned int size;
	unsigned int version;
	memcpy(&version, data, sizeof(unsigned int));
	memcpy(&size, &((char*)data)[4], sizeof(unsigned int));
	vertices.resize(size / sizeof(CVector3f));
	memcpy(&vertices[0], &((char*)data)[8], size);
	unsigned int position = size + 8;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	textureCoords.resize(size / sizeof(CVector2f));
	memcpy(&textureCoords[0], &((char*)data)[position + 4], size);
	position += size + 4;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	normals.resize(size / sizeof(CVector3f));
	memcpy(&normals[0], &((char*)data)[position + 4], size);
	position += size + 4;
	memcpy(&size, &((char*)data)[position], sizeof(unsigned int));
	indexes.resize(size / sizeof(unsigned int));
	memcpy(&indexes[0], &((char*)data)[position + 4], size);
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
		meshes.push_back(mesh);
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
		std::string texture;
		texture.resize(size);
		memcpy(&texture[0], &((char*)data)[position + 4], size);
		materials[key].texture = Utf8ToWstring(texture);
		position += size + 4;
	}
	materialManager =  CMaterialManager(materials);
	auto result = std::make_unique<C3DModel>(dummyModel);
	result->SetModel(vertices, textureCoords, normals, indexes, materialManager, meshes);
	result->SetAnimation(weightsCount, weightsIndexes, weights, joints, animations);
	return result;
}

bool CWBMModelFactory::ModelIsSupported(unsigned char * /*data*/, size_t /*size*/, std::wstring const& filePath) const
{
	size_t dotCoord = filePath.find_last_of('.') + 1;
	std::wstring extension = filePath.substr(dotCoord, filePath.length() - dotCoord);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	return extension == L"wbm";
}
