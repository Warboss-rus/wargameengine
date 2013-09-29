#include "MaterialManager.h"
#include <fstream>
#include <string>
#include <sstream>
#include <GL\glut.h>

unsigned int LoadBMPTexture(std::string const& path);

sMaterial * CMaterialManager::GetMaterial(std::string const& name)
{
	if(m_materials.find(name) == m_materials.end())
	{
		return NULL;
	}
	sMaterial * material = m_materials[name];
	if(material->textureID == 0)
	{
		material->textureID = LoadBMPTexture(material->texture);
	}
	return material;
}

CMaterialManager::~CMaterialManager()
{
	for(auto i = m_materials.begin(); i != m_materials.end(); ++i)
	{
		delete i->second;
	}
}

unsigned int LoadBMPTexture(std::string const& path)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;
	// Open the file
	FILE * file = fopen(path.c_str(),"rb");
	fread(header, 1, 54, file);
	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	data = new unsigned char [imageSize];
	fread(data,1,imageSize,file);
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete [] data;
	return textureID;
}

void CMaterialManager::LoadMTL(std::string const& path)
{
	std::ifstream iFile(path);
	std::string line;
	std::string type;
	double dvalue;
	sMaterial * lastMaterial = NULL;
	while(std::getline(iFile, line))
	{
		if(line.empty() || line[0] == '#')//Empty line or commentary
			continue;

		std::istringstream lineStream(line);
		lineStream >> type;

		if(type == "newmtl") //name
		{
			lineStream >> type;
			lastMaterial = new sMaterial;
			m_materials[type] = lastMaterial;
		}

		if(type == "Ka") //ambient color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->ambient[i] = dvalue;
			}
		}
		if(type == "Kd") //diffuse color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->diffuse[i] = dvalue;
			}
		}
		if(type == "Ks") //specular color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->specular[i] = dvalue;
			}
		}
		if(type == "Ns") //specular coefficient
		{
			lineStream >> dvalue;
			lastMaterial->shininess = dvalue;
		}
		if(type == "map_Kd") //texture
		{
			std::string path;
			lineStream >> path;
			lastMaterial->texture = path;
		}
	}
	iFile.close();
}