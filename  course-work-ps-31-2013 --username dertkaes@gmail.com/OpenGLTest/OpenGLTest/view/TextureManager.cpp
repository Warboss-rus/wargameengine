#include "TextureManager.h"
#include <GL\glut.h>

CTextureManager * CTextureManager::m_manager = NULL;

unsigned int LoadBMPTexture(std::string const& path)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	FILE * file = fopen(path.c_str(),"rb");
	fread(header, 1, 54, file);
	// Read ints from the byte array
	unsigned int dataPos    = *(int*)&(header[0x0A]);     // Position in the file where the actual data begins
	unsigned int imageSize  = *(int*)&(header[0x22]);
	unsigned int width      = *(int*)&(header[0x12]);
	unsigned int height     = *(int*)&(header[0x16]);
	unsigned int bpp		= *(short*)&(header[0x1C]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Actual RGB data
	unsigned char * data = new unsigned char [imageSize];
	fread(data,1,imageSize,file);
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, (bpp==24)?GL_BGR_EXT:GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete [] data;
	return textureID;
}

unsigned int LoadTGATexture(std::string const& path)
{
	FILE * fTGA = fopen(path.c_str(), "rb");
	unsigned char header[12];
	fread(&header, 12, 1, fTGA); //read header
	if(header[2] != 2) return 0; //compressed TGA are not supported
	fread(header, 6, 1, fTGA);
	unsigned int width = header[1] * 256 + header[0];
	unsigned int height = header[3] * 256 + header[2];
	unsigned int bpp = header[4]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	unsigned int imageSize = width * height * bpp / 8;
	unsigned char * data = new unsigned char [imageSize];
	fread(data, 1, imageSize, fTGA);
	fclose(fTGA);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, (bpp==24)?GL_BGR_EXT:GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete [] data;
	return textureID;
}

unsigned int LoadTexture(std::string const& path)
{
	unsigned int pointCoord = path.find_last_of('.') + 1;
	std::string extension = path.substr(pointCoord, path.length() - pointCoord);
	if(extension == "bmp")
		return LoadBMPTexture(path);
	if(extension == "tga")
		return LoadTGATexture(path);
	return 0; //unknown
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

void CTextureManager::SetTexture(std::string const& path)
{
	if(path.empty()) 
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		return;	
	}
	if(m_textures.find(path) == m_textures.end())
	{
		m_textures[path] = LoadTexture("texture\\" + path);
	}
	glBindTexture(GL_TEXTURE_2D, m_textures[path]);
}