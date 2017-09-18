#include "WBMModelFactory.h"
#include "../MemoryStream.h"
#include "../Utils.h"
#include "3dModel.h"
#include <algorithm>
#include <cstring>
#include <cwctype>
#include <unordered_map>
#include <string>
#include <vector>

namespace wargameEngine
{
namespace view
{
std::unique_ptr<C3DModel> CWBMModelFactory::LoadModel(unsigned char* data, size_t /*size*/, C3DModel const& dummyModel, const Path& /*filePath*/)
{
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::vector<unsigned int> indexes;
	MaterialManager materialManager;
	std::vector<sMesh> meshes;
	std::vector<unsigned int> weightsCount;
	std::vector<unsigned int> weightsIndexes;
	std::vector<float> weights;
	std::vector<sJoint> joints;
	std::vector<sAnimation> animations;

	ReadMemoryStream stream(reinterpret_cast<char*>(data));
	std::unordered_map<std::string, Material> materials;
	stream.ReadUnsigned(); //version
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
	for (size_t i = 0; i < count; ++i)
	{
		sMesh mesh;
		mesh.name = stream.ReadString();
		mesh.materialName = stream.ReadString();
		mesh.begin = stream.ReadSizeT();
		meshes.push_back(mesh);
	}
	count = stream.ReadSizeT();
	for (size_t i = 0; i < count; ++i)
	{
		std::string key = stream.ReadString();
		stream.ReadData(materials[key].ambient, sizeof(float) * 3);
		stream.ReadData(materials[key].diffuse, sizeof(float) * 3);
		stream.ReadData(materials[key].specular, sizeof(float) * 3);
		materials[key].shininess = stream.ReadFloat();
		materials[key].texture = stream.ReadString();
	}
	materialManager = MaterialManager(materials);
	auto result = std::make_unique<C3DModel>(dummyModel.GetScale(), dummyModel.GetRotation());
	result->SetModel(vertices, textureCoords, normals, indexes, materialManager, meshes);
	result->SetAnimation(weightsCount, weightsIndexes, weights, joints, animations);
	return result;
}

bool CWBMModelFactory::ModelIsSupported(unsigned char* /*data*/, size_t /*size*/, const Path& filePath) const
{
	size_t dotCoord = filePath.native().find_last_of('.') + 1;
	Path::string_type extension = filePath.native().substr(dotCoord, filePath.native().length() - dotCoord);
	std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);
	return extension == L"wbm";
}
}
}