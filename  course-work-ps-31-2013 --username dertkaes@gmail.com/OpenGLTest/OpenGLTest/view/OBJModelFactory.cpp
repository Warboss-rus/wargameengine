#include "OBJModelFactory.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

struct FaceIndex
{
	unsigned int vertex;
	unsigned int normal;
	unsigned int textureCoord;
};

unsigned int ParseStringUntilSlash(std::stringstream& indexStream, char ch = 0)
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
	return (!index.empty()) ? atoi(index.c_str()): 0;
}

FaceIndex ParseFaceIndex(std::string const& str)
{
	std::stringstream indexStream(str);
	std::string index;
	FaceIndex res;
	res.vertex = ParseStringUntilSlash(indexStream, '/');
	res.textureCoord = ParseStringUntilSlash(indexStream, '/');
	res.normal = ParseStringUntilSlash(indexStream);
	return res;
}

C3DModel * LoadObjModel(std::string const& path)
{
	std::vector<CVector3d> vertices;
	std::vector<CVector2d> textureCoords;
	std::vector<CVector3d> normals;
	std::map<std::string, unsigned int> faces;
	std::ifstream iFile(path);
	std::string line;
	std::string type;
	CVector3d p3;
	CVector2d p2;
	sMesh mesh;
	bool useFaces = false;
	bool useNormals = false;
	bool useUVs = false;
	std::vector<CVector3d> newVertices;
	std::vector<CVector2d> newTextureCoords;
	std::vector<CVector3d> newNormals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	while(iFile.good())
	{
		iFile >> type;
		if(type.empty() || type[0] == '#')//Empty line or commentary
			continue;

		if(type == "v")// Vertex
		{
			iFile >> p3.x;
			iFile >> p3.y;
			iFile >> p3.z;
			vertices.push_back(p3);
		}

		if(type == "vt")// Texture coords
		{
			useUVs = true;
			iFile >> p2.x;
			iFile >> p2.y;
			textureCoords.push_back(p2);
		}
		if(type == "vn")// Normals
		{
			useNormals = true;
			iFile >> p3.x;
			iFile >> p3.y;
			iFile >> p3.z;
			normals.push_back(p3);
		}
		if(type == "f")// faces
		{
			useFaces = true;
			for(unsigned int i = 0; i < 3; ++i)
			{
				std::string index3;
				iFile >> index3;
				if(faces.find(index3) != faces.end()) //This vertex/texture coord/normal already exist
				{
					indexes.push_back(faces[index3]);
				}
				else//New vertex/texcoord/normal
				{
					FaceIndex faceIndex  = ParseFaceIndex(index3);
					newVertices.push_back(vertices[faceIndex.vertex - 1]);
					if(faceIndex.textureCoord != 0)
					{
						newTextureCoords.push_back(textureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						newTextureCoords.push_back(CVector2d());
					}
					if(faceIndex.normal != 0)
					{
						newNormals.push_back(normals[faceIndex.normal - 1]);
					}
					else
					{
						newNormals.push_back(CVector3d());
					}
					indexes.push_back(newVertices.size() - 1);
					faces[index3] = newVertices.size() - 1;
				}
			}
		}
		if(type == "mtllib")//Load materials file
		{
			std::string path;
			iFile >> path;
			materialManager.LoadMTL(path);
		}
		if(type == "usemtl")//apply material
		{
			iFile >> mesh.materialName;
			mesh.polygonIndex = indexes.size();
			if(!meshes.empty() && mesh.polygonIndex == meshes.back().polygonIndex)
			{
				meshes.back() = mesh;
			}
			else
			{
				meshes.push_back(mesh);
			}
		}
		if(type == "g")//apply material
		{
			std::string name;
			iFile.get();
			std::getline(iFile, name);
			if(!name.empty())
			{
				mesh.name = name;
				mesh.polygonIndex = indexes.size();
				if(!meshes.empty() && mesh.polygonIndex == meshes.back().polygonIndex)
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
	if(!useNormals)
	{
		newNormals.clear();
	}
	if(!useUVs)
	{
		newTextureCoords.clear();
	}
	iFile.close();
	if(!useFaces)
	{
		newVertices.swap(vertices);
		newTextureCoords.swap(textureCoords);
		newNormals.swap(normals);
	}
	std::string boundingPath = path.substr(0, path.find_last_of('.')) + ".txt";
	double scale = 1.0;
	std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(boundingPath, scale);
	return new C3DModel(newVertices, newTextureCoords, newNormals, indexes, materialManager, meshes, bounding, scale);
}