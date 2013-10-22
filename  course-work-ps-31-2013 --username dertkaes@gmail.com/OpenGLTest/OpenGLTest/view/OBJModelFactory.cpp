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

C3DModel * CObjModelCreator::Create(std::string const& path)
{
	std::vector<sPoint3> vertices;
	std::vector<sPoint2> textureCoords;
	std::vector<sPoint3> normals;
	std::map<std::string, unsigned int> faces;
	std::ifstream iFile(path);
	std::string line;
	std::string type;
	sPoint3 p3;
	sPoint2 p2;
	bool useFaces = false;
	bool useNormals = false;
	bool useUVs = false;
	double bounding[6];
	std::vector<sPoint3> newVertices;
	std::vector<sPoint2> newTextureCoords;
	std::vector<sPoint3> newNormals;
	std::vector<unsigned int> newIndexes;
	CMaterialManager newMaterialManager;
	std::vector<sUsingMaterial> newUsedMaterials;
	bounding[0] = DBL_MAX;
	bounding[1] = DBL_MAX;
	bounding[2] = DBL_MAX;
	bounding[3] = -DBL_MAX;
	bounding[4] = -DBL_MAX;
	bounding[5] = -DBL_MAX;
	while(std::getline(iFile, line))
	{
		if(line.empty() || line[0] == '#')//Empty line or commentary
			continue;

		std::istringstream lineStream(line);
		lineStream >> type;

		if(type == "v")// Vertex
		{
			lineStream >> p3.x;
			lineStream >> p3.y;
			lineStream >> p3.z;
			if(p3.x < bounding[0]) bounding[0] = p3.x;
			if(p3.y < bounding[1]) bounding[1] = p3.y;
			if(p3.z < bounding[2]) bounding[2] = p3.z;
			if(p3.x > bounding[3]) bounding[3] = p3.x;
			if(p3.y > bounding[4]) bounding[4] = p3.y;
			if(p3.z > bounding[5]) bounding[5] = p3.z;
			vertices.push_back(p3);
		}

		if(type == "vt")// Texture coords
		{
			useUVs = true;
			lineStream >> p2.x;
			lineStream >> p2.y;
			textureCoords.push_back(p2);
		}
		if(type == "vn")// Normals
		{
			useNormals = true;
			lineStream >> p3.x;
			lineStream >> p3.y;
			lineStream >> p3.z;
			normals.push_back(p3);
		}
		if(type == "f")// faces
		{
			useFaces = true;
			for(unsigned int i = 0; i < 3; ++i)
			{
				std::string indexes;
				lineStream >> indexes;
				if(faces.find(indexes) != faces.end()) //This vertex/texture coord/normal already exist
				{
					newIndexes.push_back(faces[indexes]);
				}
				else//New vertex/texcoord/normal
				{
					FaceIndex faceIndex  = ParseFaceIndex(indexes);
					newVertices.push_back(vertices[faceIndex.vertex - 1]);
					if(faceIndex.textureCoord != 0)
					{
						newTextureCoords.push_back(textureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						newTextureCoords.push_back(sPoint2());
					}
					if(faceIndex.normal != 0)
					{
						newNormals.push_back(normals[faceIndex.normal - 1]);
					}
					else
					{
						newNormals.push_back(sPoint3());
					}
					newIndexes.push_back(newVertices.size() - 1);
					faces[indexes] = newVertices.size() - 1;
				}
			}
		}
		if(type == "mtllib")//Load materials file
		{
			std::string path;
			lineStream >> path;
			newMaterialManager.LoadMTL(path);
		}
		if(type == "usemtl")//apply material
		{
			sUsingMaterial material;
			lineStream >> material.materialName;
			material.polygonIndex = newIndexes.size();
			newUsedMaterials.push_back(material);
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
	return new C3DModel(newVertices, newTextureCoords, newNormals, newIndexes, newMaterialManager, newUsedMaterials, LoadBoundingFromFile(boundingPath));
}