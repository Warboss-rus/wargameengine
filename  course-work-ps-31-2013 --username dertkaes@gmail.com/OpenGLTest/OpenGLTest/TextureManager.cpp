#include "TextureManager.h"
#include <GL\glut.h>

CTextureManager * CTextureManager::m_manager = NULL;

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
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete [] data;
	return textureID;
}

CTextureManager * CTextureManager::GetInstance()
{
	if(!m_manager)
	{
		m_manager = new CTextureManager;
	}
	return m_manager;
}

void CTextureManager::FreeInstance()
{
	delete m_manager;
	m_manager = NULL;
}

unsigned int CTextureManager::GetTexture(std::string const& path)
{
	if(m_textures.find(path) == m_textures.end())
	{
		m_textures[path] = LoadBMPTexture(path);
	}
	return m_textures[path];
}