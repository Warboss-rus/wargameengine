#include "3dModel.h"
#include <GL\glut.h>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

C3DModel::C3DModel(std::string const& path)
{
	std::vector<double> vertices;
	std::vector<double> textureCoords;
	std::vector<double> normals;
	std::map<std::string, unsigned int> faces;
	std::ifstream iFile(path);
	std::string line;
	std::string type;
	double dvalue;
	bool useFaces = false;
	while(std::getline(iFile, line))
	{
		if(line.empty() || line[0] == '#')//Empty line or commentary
			continue;

		std::istringstream lineStream(line);
		lineStream >> type;

		if(type == "v")// Vertex
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				dvalue = 0.0;
				lineStream >> dvalue;
				vertices.push_back(dvalue);
			}
		}

		if(type == "vt")// Texture coords
		{
			for(unsigned int i = 0; i < 2; ++i)
			{
				dvalue = 0.0;
				lineStream >> dvalue;
				textureCoords.push_back(dvalue);
			}
		}
		if(type == "vn")// Normals
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				dvalue = 0.0;
				lineStream >> dvalue;
				normals.push_back(dvalue);
			}
		}
		if(type == "f")// faces
		{
			useFaces = true;
			for(unsigned int i = 0; i < 3; ++i)
			{
				std::string indexes;
				lineStream >> indexes;
				if(faces.find(indexes) != faces.end()) //This vertex\normals\texture coords already exist
				{
					m_polygon.push_back(faces[indexes]);
				}
				else//New vertex\normal\texcoord
				{
					std::stringstream indexStream(indexes);
					std::string index;
					unsigned int vertexIndex;
					unsigned int textureIndex;
					unsigned int normalIndex;
					//vertex index
					std::getline(indexStream, index, '/');
					vertexIndex = (atoi(index.c_str()) - 1) * 3;
					m_vertices.push_back(vertices[vertexIndex]);
					m_vertices.push_back(vertices[vertexIndex + 1]);
					m_vertices.push_back(vertices[vertexIndex + 2]);
					index.clear();
					//texture coord index.
					std::getline(indexStream, index, '/');
					if(!index.empty())
					{
						textureIndex = (atoi(index.c_str()) - 1) * 2;
						m_textureCoords.push_back(textureCoords[textureIndex]);
						m_textureCoords.push_back(textureCoords[textureIndex + 1]);
					}
					index.clear();
					//normal index
					std::getline(indexStream, index);
					if(!index.empty())
					{
						normalIndex = (atoi(index.c_str()) - 1) * 3;
						m_normals.push_back(normals[normalIndex]);
						m_normals.push_back(normals[normalIndex + 1]);
						m_normals.push_back(normals[normalIndex + 2]);
					}
					m_polygon.push_back((m_vertices.size() - 1) / 3);
					faces[indexes] = (m_vertices.size() - 1) / 3;
				}
			}
		}
	}
	iFile.close();
	if(!useFaces)
	{
		m_vertices = vertices;
		m_textureCoords = textureCoords;
		m_normals = normals;
	}
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
	if(!m_polygon.empty()) //Draw by indexes;
	{
		glDrawElements(GL_TRIANGLES, m_polygon.size(), GL_UNSIGNED_INT, &m_polygon[0]);
	}
	else //Draw in a row
	{
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}