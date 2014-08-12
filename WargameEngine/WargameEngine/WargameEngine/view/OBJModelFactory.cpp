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

void * LoadObjModel(void* data, unsigned int size, void* param)
{
	sOBJLoader * loader = (sOBJLoader*)param;
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::map<std::string, unsigned int> faces;
	std::stringstream iFile((char*)data);
	std::string line;
	std::string type;
	CVector3f p3;
	CVector2f p2;
	sMesh mesh;
	bool useFaces = false;
	bool useNormals = false;
	bool useUVs = false;
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
					loader->indexes.push_back(faces[index3]);
				}
				else//New vertex/texcoord/normal
				{
					FaceIndex faceIndex  = ParseFaceIndex(index3);
					loader->vertices.push_back(vertices[faceIndex.vertex - 1]);
					if(faceIndex.textureCoord != 0)
					{
						loader->textureCoords.push_back(textureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						loader->textureCoords.push_back(CVector2f());
					}
					if(faceIndex.normal != 0)
					{
						loader->normals.push_back(normals[faceIndex.normal - 1]);
					}
					else
					{
						loader->normals.push_back(CVector3f());
					}
					loader->indexes.push_back(loader->vertices.size() - 1);
					faces[index3] = loader->vertices.size() - 1;
				}
			}
		}
		if(type == "mtllib")//Load materials file
		{
			std::string path;
			iFile >> path;
			loader->materialManager.LoadMTL(path);
		}
		if(type == "usemtl")//apply material
		{
			iFile >> mesh.materialName;
			mesh.polygonIndex = loader->indexes.size();
			if(!loader->meshes.empty() && mesh.polygonIndex == loader->meshes.back().polygonIndex)
			{
				loader->meshes.back() = mesh;
			}
			else
			{
				loader->meshes.push_back(mesh);
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
				mesh.polygonIndex = loader->indexes.size();
				if(!loader->meshes.empty() && mesh.polygonIndex == loader->meshes.back().polygonIndex)
				{
					loader->meshes.back() = mesh;
				}
				else
				{
					loader->meshes.push_back(mesh);
				}
			}
		}
	}
	if(!useNormals)
	{
		loader->normals.clear();
	}
	if(!useUVs)
	{
		loader->textureCoords.clear();
	}
	delete [] data;
	if(!useFaces)
	{
		loader->vertices.swap(vertices);
		loader->textureCoords.swap(textureCoords);
		loader->normals.swap(normals);
	}
	return loader;
}