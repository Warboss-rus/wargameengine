#include "3dModel.h"
#include <GL\glut.h>
#include <fstream>
#include <string>
#include <sstream>
//#define NEW_RENDERING //uncomment it to render normals and texture coords in exchange of speed

C3DModel::C3DModel(std::string const& path)
{
	std::ifstream iFile;
	iFile.open(path);
	std::string line;
	std::string type;
	double dvalue;
	unsigned int uintvalue;
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
				m_vertices.push_back(dvalue);
			}
		}

		if(type == "vt")// Texture coords
		{
			for(unsigned int i = 0; i < 2; ++i)
			{
				dvalue = 0.0;
				lineStream >> dvalue;
				m_textureCoords.push_back(dvalue);
			}
		}
		if(type == "vn")// Normals
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				dvalue = 0.0;
				lineStream >> dvalue;
				m_normals.push_back(dvalue);
			}
		}
		if(type == "f")// faces
		{
			for(unsigned int i = 0; i < 3; ++i)
			{
				std::string indexes;
				lineStream >> indexes;
				std::stringstream indexStream(indexes);
				std::string index;
				//vertex index
				std::getline(indexStream, index, '/');
				uintvalue = atoi(index.c_str());
				--uintvalue;
				m_faces.push_back(uintvalue);
#ifdef NEW_RENDERING
				//texture coord index.
				std::getline(indexStream, index, '/');
				if(!index.empty()) uintvalue = atoi(index.c_str());
				--uintvalue;
				m_faces.push_back(uintvalue);
				//normal index
				std::getline(indexStream, index);
				if(!index.empty()) uintvalue = atoi(index.c_str());
				--uintvalue;
				m_faces.push_back(uintvalue);/**/
#endif
			}
		}
	}
	iFile.close();
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
	if(!m_faces.empty())
	{
#ifdef NEW_RENDERING //new rendering with normals and texcoords. Slow
		glBegin(GL_TRIANGLES); 
		for(unsigned int i = 0; i < m_faces.size(); i += 3)
		{
			glTexCoord2d(m_vertices[m_faces[i + 1] * 2], m_vertices[m_faces[i + 1] * 2 + 1]);
			glNormal3d(m_vertices[m_faces[i + 2] * 3], m_vertices[m_faces[i + 2] * 3 + 1], m_vertices[m_faces[i + 2] * 3 + 2]);
			glVertex3d(m_vertices[m_faces[i] * 3], m_vertices[m_faces[i] * 3 + 1], m_vertices[m_faces[i] * 3 + 2]);
		}
		glEnd();
#else//Old rendering with incorrect normals and texcoords. Very fast.
		glDrawElements(GL_TRIANGLES, m_faces.size(), GL_UNSIGNED_INT, &m_faces[0]);
#endif
	}
	else 
	{
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}