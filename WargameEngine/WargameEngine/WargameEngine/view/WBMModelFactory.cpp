#include "WBMModelFactory.h"
#include "3dModel.h"
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cwctype>
#include "../Utils.h"
#include "../MemoryStream.h"

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
	
	CReadMemoryStream stream(reinterpret_cast<char*>(data));
	std::map<std::string, sMaterial> materials;
	stream.ReadUnsigned();//version
	size_t size = stream.ReadSizeT();
	vertices.resize(size / sizeof(CVector3f));
	stream.ReadData(vertices.data(), size);
	size = stream.ReadSizeT();
	textureCoords.resize(size / sizeof(CVector2f));
	stream.ReadData(textureCoords.data(), size);
	size = stream.ReadSizeT();
	normals.resize(size / sizeof(CVector3f));
	stream.ReadData(normals.data(), size);
	size = stream.ReadSizeT();
	indexes.resize(size / sizeof(unsigned int));
	stream.ReadData(indexes.data(), size);
	size_t count = stream.ReadSizeT();
	for(size_t i = 0; i < count; ++i)
	{
		sMesh mesh;
		mesh.name = stream.ReadString();
		mesh.materialName = stream.ReadString();
		mesh.begin = stream.ReadSizeT();
		meshes.push_back(mesh);
	}
	count = stream.ReadSizeT();
	for(size_t i = 0; i < count; ++i)
	{
		std::string key = stream.ReadString();
		stream.ReadData(materials[key].ambient, sizeof(float) * 3);
		stream.ReadData(materials[key].diffuse, sizeof(float) * 3);
		stream.ReadData(materials[key].specular, sizeof(float) * 3);
		materials[key].shininess = stream.ReadFloat();
		materials[key].texture = Utf8ToWstring(stream.ReadString());
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
	std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);
	return extension == L"wbm";
}
