#include "OBJModelFactory.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>

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
	C3DModel * model = new C3DModel();
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
					model->m_indexes.push_back(faces[indexes]);
				}
				else//New vertex/texcoord/normal
				{
					FaceIndex faceIndex  = ParseFaceIndex(indexes);
					model->m_vertices.push_back(vertices[faceIndex.vertex - 1]);
					if(faceIndex.textureCoord != 0)
					{
						model->m_textureCoords.push_back(textureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						model->m_textureCoords.push_back(sPoint2());
					}
					if(faceIndex.normal != 0)
					{
						model->m_normals.push_back(normals[faceIndex.normal - 1]);
					}
					else
					{
						model->m_normals.push_back(sPoint3());
					}
					model->m_indexes.push_back(model->m_vertices.size() - 1);
					faces[indexes] = model->m_vertices.size() - 1;
				}
			}
		}
		if(type == "mtllib")//Load materials file
		{
			std::string path;
			lineStream >> path;
			model->m_materials.LoadMTL(path);
		}
		if(type == "usemtl")//apply material
		{
			sUsingMaterial material;
			lineStream >> material.materialName;
			material.polygonIndex = model->m_indexes.size();
			model->m_usedMaterials.push_back(material);
		}
	}
	if(!useNormals)
	{
		model->m_normals.clear();
	}
	if(!useUVs)
	{
		model->m_textureCoords.clear();
	}
	iFile.close();
	if(!useFaces)
	{
		model->m_vertices.swap(vertices);
		model->m_textureCoords.swap(textureCoords);
		model->m_normals.swap(normals);
	}
	return model;
}