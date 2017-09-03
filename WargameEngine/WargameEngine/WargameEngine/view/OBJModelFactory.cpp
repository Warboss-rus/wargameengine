#include "OBJModelFactory.h"
#include "../AsyncFileProvider.h"
#include "../LogWriter.h"
#include "../Utils.h"
#include "3dModel.h"
#include "IRenderer.h"
#include <algorithm>
#include <cwctype>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

namespace wargameEngine
{
namespace view
{
namespace
{
struct FaceIndex
{
	size_t vertex;
	size_t normal;
	size_t textureCoord;
};

size_t ParseStringUntilSlash(std::stringstream& indexStream, char ch = 0)
{
	std::string index;
	if (ch == 0)
	{
		std::getline(indexStream, index);
	}
	else
	{
		std::getline(indexStream, index, ch);
	}
	return (!index.empty()) ? static_cast<size_t>(atol(index.c_str())) : 0u;
}

FaceIndex ParseFaceIndex(std::string const& str)
{
	std::stringstream indexStream(str);
	FaceIndex res;
	res.vertex = ParseStringUntilSlash(indexStream, '/');
	res.textureCoord = ParseStringUntilSlash(indexStream, '/');
	res.normal = ParseStringUntilSlash(indexStream);
	return res;
}

std::unordered_map<std::string, Material> LoadMTL(const Path& path)
{
	std::unordered_map<std::string, Material> materials;
	std::ifstream iFile(path);
	if (!iFile.good())
	{
		iFile.close();
		LogWriter::WriteLine("Error loading MTL " + path.string());
		return materials;
	}
	std::string line;
	std::string type;
	float dvalue;
	Material* lastMaterial = NULL;
	while (std::getline(iFile, line))
	{
		if (line.empty() || line[0] == '#') //Empty line or commentary
			continue;

		std::istringstream lineStream(line);
		lineStream >> type;

		if (type == "newmtl") //name
		{
			lineStream >> type;
			materials[type] = Material();
			lastMaterial = &materials[type];
		}

		if (type == "Ka" && lastMaterial) //ambient color
		{
			for (size_t i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->ambient[i] = dvalue;
			}
		}
		if (type == "Kd" && lastMaterial) //diffuse color
		{
			for (size_t i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->diffuse[i] = dvalue;
			}
		}
		if (type == "Ks" && lastMaterial) //specular color
		{
			for (size_t i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->specular[i] = dvalue;
			}
		}
		if (type == "Ns" && lastMaterial) //specular coefficient
		{
			lineStream >> dvalue;
			lastMaterial->shininess = dvalue;
		}
		if (type == "map_Kd" && lastMaterial) //texture
		{
			std::string texture;
			lineStream >> texture;
			lastMaterial->texture = texture;
		}
		if ((type == "map_bump" || type == "bump") && lastMaterial) //bump texture
		{
			std::string texture;
			lineStream >> texture;
			lastMaterial->bumpMap = texture;
		}
		if (type == "map_specular" && lastMaterial) //custom specular map extension
		{
			std::string texture;
			lineStream >> texture;
			lastMaterial->texture = texture;
		}
	}
	iFile.close();
	return materials;
}
}

std::unique_ptr<C3DModel> CObjModelFactory::LoadModel(unsigned char* data, size_t /*size*/, const C3DModel& dummyModel, const Path& filePath)
{
	std::vector<CVector3f> tempVertices;
	std::vector<CVector2f> tempTextureCoords;
	std::vector<CVector3f> tempNormals;
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::unordered_map<std::string, unsigned int> faces;
	std::vector<unsigned int> indexes;
	MaterialManager materialManager;
	std::vector<sMesh> meshes;
	std::vector<unsigned int> weightsCount;
	std::vector<unsigned int> weightsIndexes;
	std::vector<float> weights;
	std::vector<sJoint> joints;
	std::vector<sAnimation> animations;
	std::stringstream iFile((char*)data);
	std::string type;
	CVector3f p3;
	CVector2f p2;
	sMesh mesh;
	bool useFaces = false;
	bool useNormals = false;
	bool useUVs = false;
	while (iFile.good())
	{
		iFile >> type;
		if (type.empty() || type[0] == '#') //Empty line or commentary
			continue;

		if (type == "v") // Vertex
		{
			iFile >> p3.x;
			iFile >> p3.y;
			iFile >> p3.z;
			tempVertices.push_back(p3);
		}

		if (type == "vt") // Texture coords
		{
			useUVs = true;
			iFile >> p2.x;
			iFile >> p2.y;
			tempTextureCoords.push_back(p2);
		}
		if (type == "vn") // Normals
		{
			useNormals = true;
			iFile >> p3.x;
			iFile >> p3.y;
			iFile >> p3.z;
			tempNormals.push_back(p3);
		}
		if (type == "f") // faces
		{
			useFaces = true;
			for (unsigned int i = 0; i < 3; ++i)
			{
				std::string index3;
				iFile >> index3;
				if (faces.find(index3) != faces.end()) //This vertex/texture coord/normal already exist
				{
					indexes.push_back(faces[index3]);
				}
				else //std::make_unique<vertex/texcoord/normal
				{
					FaceIndex faceIndex = ParseFaceIndex(index3);
					vertices.push_back(tempVertices[faceIndex.vertex - 1]);
					if (faceIndex.textureCoord != 0)
					{
						textureCoords.push_back(tempTextureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						textureCoords.push_back(CVector2f());
					}
					if (faceIndex.normal != 0)
					{
						normals.push_back(tempNormals[faceIndex.normal - 1]);
					}
					else
					{
						normals.push_back(CVector3f());
					}
					indexes.push_back(static_cast<int>(vertices.size() - 1));
					faces[index3] = static_cast<unsigned>(vertices.size() - 1);
				}
			}
		}
		if (type == "mtllib") //Load materials file
		{
			std::string mtlPath;
			iFile >> mtlPath;
			if (mtlPath.size() > 2 && mtlPath.front() == '.')
			{
				mtlPath = mtlPath.substr(2);
			}
			materialManager.InsertMaterials(LoadMTL(filePath.parent_path() / mtlPath));
		}
		if (type == "usemtl") //apply material
		{
			iFile >> mesh.materialName;
			mesh.begin = indexes.size();
			if (!meshes.empty() && mesh.begin == meshes.back().begin)
			{
				meshes.back() = mesh;
			}
			else
			{
				meshes.push_back(mesh);
			}
		}
		if (type == "g") //apply material
		{
			std::string name;
			iFile.get();
			std::getline(iFile, name);
			if (!name.empty())
			{
				mesh.name = name;
				mesh.begin = indexes.size();
				if (!meshes.empty() && mesh.begin == meshes.back().begin)
				{
					meshes.back() = mesh;
				}
				else
				{
					meshes.push_back(mesh);
				}
			}
		}
	}
	if (!useNormals)
	{
		tempNormals.clear();
	}
	if (!useUVs)
	{
		tempTextureCoords.clear();
	}
	if (!useFaces)
	{
		vertices.swap(tempVertices);
		textureCoords.swap(tempTextureCoords);
		normals.swap(tempNormals);
	}
	auto result = std::make_unique<C3DModel>(dummyModel.GetScale(), dummyModel.GetRotation());
	result->SetModel(vertices, textureCoords, normals, indexes, materialManager, meshes);
	result->SetAnimation(weightsCount, weightsIndexes, weights, joints, animations);
	return result;
}

bool CObjModelFactory::ModelIsSupported(unsigned char* /*data*/, size_t /*size*/, const Path& filePath) const
{
	size_t dotCoord = filePath.native().find_last_of('.') + 1;
	Path::string_type extension = filePath.native().substr(dotCoord, filePath.native().length() - dotCoord);
	std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);
	return extension == L"obj";
}
}
}