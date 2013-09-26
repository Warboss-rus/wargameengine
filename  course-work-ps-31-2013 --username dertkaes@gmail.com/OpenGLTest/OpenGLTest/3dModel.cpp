#include "3dModel.h"
#include <GL\glut.h>
#include <fstream>
#include <string>
#include <sstream>

void Normalize(std::vector<double> & vect)
{
	for(size_t i = 0; i < vect.size(); i += 3)
	{
		double length = 0;
		for(unsigned int j = i; j < i + 3; ++j)
		{
			length += vect[j] * vect[j];
		}
		length = sqrt(length);
		for(unsigned int j = i; j < i + 3; ++j)
		{
			vect[j] /= length;
		}
	}
}

C3DModel::C3DModel(std::string const& path)
{
	std::ifstream iFile;
	iFile.open(path);
	std::string line;
	std::string type;
	double coords;
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
				coords = 0.0;
				lineStream >> coords;
				m_vertices.push_back(coords);
			}
		}

		if(type == "vt")// Texture coords
		{
			for(unsigned int i = 0; i < 2; ++i)
			{
				coords = 0.0;
				lineStream >> coords;
				m_textureCoords.push_back(coords);
			}
		}
		if(type == "vn")// Normals
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				coords = 0.0;
				lineStream >> coords;
				m_normals.push_back(coords);
			}
		}
		if(type == "f")// faces
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> coords;
				--coords;
				m_faces.push_back(coords);
			}
		}
	}
	iFile.close();
	Normalize(m_normals);
}

void C3DModel::Draw()
{
	if(m_vertices.size() > 2)
	{
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
	if(!m_faces.empty())
	{
		glDrawElements(GL_TRIANGLES, m_faces.size(), GL_UNSIGNED_INT, &m_faces[0]);
	}
	else 
	{
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}