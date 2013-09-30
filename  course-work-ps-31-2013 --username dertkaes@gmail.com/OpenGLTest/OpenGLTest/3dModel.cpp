#include "3dModel.h"
#include <GL\glut.h>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "C3DModelUtils.h"

C3DModel::C3DModel(std::string const& path)
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
					m_indexes.push_back(faces[indexes]);
				}
				else//New vertex/texcoord/normal
				{
					C3DModelUtils::FaceIndex faceIndex  = C3DModelUtils::ParseFaceIndex(indexes);
					//vertex index
					m_vertices.push_back(vertices[faceIndex.vertex - 1]);
				
					if(faceIndex.textureCoord != 0)
					{
						m_textureCoords.push_back(textureCoords[faceIndex.textureCoord - 1]);
					}
					else
					{
						m_textureCoords.push_back(sPoint2());
					}
					if(faceIndex.normal != 0)
					{
						m_normals.push_back(normals[faceIndex.normal - 1]);
					}
					else
					{
						m_normals.push_back(sPoint3());
					}
					m_indexes.push_back(m_vertices.size() - 1);
					faces[indexes] = m_vertices.size() - 1;
				}
			}
		}
		if(type == "mtllib")//Load materials file
		{
			std::string path;
			lineStream >> path;
			m_materials.LoadMTL(path);
		}
		if(type == "usemtl")//apply material
		{
			sUsingMaterial material;
			lineStream >> material.materialName;
			material.polygonIndex = m_indexes.size();
			m_usedMaterials.push_back(material);
		}
	}
	if(!useNormals)
	{
		m_normals.clear();
	}
	if(!useUVs)
	{
		m_textureCoords.clear();
	}
	iFile.close();
	if(!useFaces)
	{
		m_vertices.swap(vertices);
		m_textureCoords.swap(textureCoords);
		m_normals.swap(normals);
	}
}

void SetMaterial(sMaterial * material)
{
	if(!material)
	{
		return;
	}
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material->specular);
	glMaterialf(GL_FRONT,GL_SHININESS,material->shininess);
	glBindTexture(GL_TEXTURE_2D, material->textureID);
}

void C3DModel::Draw()
{
	if(m_vertices.size() > 2)
	{;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &m_vertices[0]);
	}
	if(m_normals.size() > 2)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, &m_normals[0]);
	}
	if(m_textureCoords.size() > 2)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_DOUBLE, 0, &m_textureCoords[0]);
	}
	if(!m_indexes.empty()) //Draw by indexes;
	{
		unsigned int begin = 0;
		unsigned int end;
		for(unsigned int i = 0; i < m_usedMaterials.size(); ++i)
		{
			end = m_usedMaterials[i].polygonIndex;
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
			SetMaterial(m_materials.GetMaterial(m_usedMaterials[i].materialName));
			begin = end;
		}
		end = m_indexes.size();
		glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
	}
	else //Draw in a row
	{
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	sMaterial empty;
	SetMaterial(&empty);
}