#include "TextureManager.h"
#include "gl.h"
#include "..\picopng.h"

CTextureManager * CTextureManager::m_manager = NULL;

unsigned char * LoadBMPTexture(std::string const& path, unsigned int & width, unsigned int & height, unsigned int & bpp, GLenum & format)
{
	// Data read from the header of the BMP file
	unsigned char header[54];
	FILE * file = fopen(path.c_str(),"rb");
	if(!file)
	{
		width = 0; 
		height = 0;
		bpp = 0;
		format = GL_BGR_EXT;
		MessageBoxA(NULL, (std::string("Cannot find ") + path).c_str(), "Error loading texture", 0);
		return NULL;
	}
	fread(header, 1, 54, file);
	unsigned int dataPos = *(int*)&(header[0x0A]);     // Position in the file where the actual data begins
	unsigned int imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	bpp = *(short*)&(header[0x1C]);
	format = (bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*bpp / 8; 
	if (dataPos==0)      dataPos=54; 

	unsigned char * data = new unsigned char [imageSize];
	fread(data,1,imageSize,file);
	fclose(file);

	return data;
}

unsigned char * UncompressTGA(unsigned char * data, unsigned int width, unsigned int height, unsigned int bpp)
{
	short iPixelSize = bpp / 8;
	unsigned int imageSize = width * height * iPixelSize;
	unsigned char * uncompressedData = new unsigned char[imageSize];
	unsigned int index = 0;
	unsigned char *pCur = &data[0];
	unsigned char bLength,bLoop;
	while(index < imageSize) 
	{
		if(*pCur & 0x80) // Run length chunk (High bit = 1)
		{
			bLength =* pCur - 127; // Get run length
			pCur++;            // Move to pixel data  

			// Repeat the next pixel bLength times
			for(bLoop = 0; bLoop != bLength; ++bLoop, index += iPixelSize)
				memcpy(&uncompressedData[index],pCur,iPixelSize);

			pCur += iPixelSize; // Move to the next descriptor chunk
		}
		else // Raw chunk
		{
			bLength =* pCur + 1; // Get run length
			pCur++;          // Move to pixel data

			// Write the next bLength pixels directly
			for(bLoop=0;bLoop!=bLength;++bLoop,index+=iPixelSize,pCur+=iPixelSize)
				memcpy(&uncompressedData[index],pCur,iPixelSize);
		}
	}
	delete [] data;
	return uncompressedData;
}

unsigned char * LoadTGATexture(std::string const& path, unsigned int & width, unsigned int & height, unsigned int & bpp, GLenum & format)
{
	FILE * fTGA = fopen(path.c_str(), "rb");
	if(!fTGA)
	{
		width = 0; 
		height = 0;
		bpp = 0;
		format = GL_BGR_EXT;
		MessageBoxA(NULL, (std::string("Cannot find ") + path).c_str(), "Error loading texture", 0);
		return NULL;
	}
	unsigned char header[18];
	fread(&header, 18, 1, fTGA); //read header
	if(header[2] != 2 && header[2] != 10) 
		return NULL; //nonRGB TGA are not supported
	width = header[13] * 256 + header[12];
	height = header[15] * 256 + header[14];
	bpp = header[16]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	format = (bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;

	fseek(fTGA, 0L, SEEK_END);
	unsigned int imageSize = ftell(fTGA) - 18;
	fseek(fTGA, 18L, SEEK_SET);

	unsigned char * data = new unsigned char [imageSize];
	fread(data, 1, imageSize, fTGA);
	fclose(fTGA);

	if(header[2] == 2) //uncompressed
		return data;

	return UncompressTGA(data, width, height, bpp);
}

unsigned char * LoadPNGTexture(std::string const& path, unsigned int & width, unsigned int & height, unsigned int & bpp, GLenum & format)
{
	FILE * file = fopen(path.c_str(),"rb");
	if(!file)
	{
		width = 0; 
		height = 0;
		bpp = 0;
		format = GL_BGR_EXT;
		MessageBoxA(NULL, (std::string("Cannot find ") + path).c_str(), "Error loading texture", 0);
		return NULL;
	}
	fseek(file, 0L, SEEK_END);
	unsigned int imageSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	unsigned char * data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	std::vector<unsigned char> uncompressedData;
	unsigned long lheight, lwidth;
	decodePNG(uncompressedData, lwidth, lheight, data, imageSize, true);
	height = lheight;
	width = lwidth;
	delete [] data;

	bpp = 32;
	imageSize = height * width * bpp / 8;
	format = GL_RGBA;
	data = new unsigned char [imageSize];
	for(unsigned long y = 0; y < height; ++y)
	{
		memcpy(&data[y * width * 4], &uncompressedData[(height - y - 1) * width * 4], sizeof(unsigned char) * 4 * width);
	}
	return data;
}

unsigned int LoadTexture(std::string const& path)
{
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int bpp = 0;
	unsigned char * data;
	GLenum format;
	unsigned int dotCoord = path.find_last_of('.') + 1;
	std::string extension = path.substr(dotCoord, path.length() - dotCoord);
	if(extension == "bmp")
		data = LoadBMPTexture(path, width, height, bpp, format);
	if(extension == "tga")
		data = LoadTGATexture(path, width, height, bpp, format);
	if(extension == "png")
		data =  LoadPNGTexture(path, width, height, bpp, format);
	if(!data) return 0;
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	gluBuild2DMipmaps(GL_TEXTURE_2D, bpp / 8, width, height, format, GL_UNSIGNED_BYTE, &data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	delete [] data;
	return textureID; //unknown
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

void CTextureManager::SetShaderVarLocation(GLuint textureLocation)
{
	m_shaderTextureVarLocation = textureLocation;
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
	glUniform1i ( m_shaderTextureVarLocation, 0/*m_textures[path]*/ );
	glBindTexture(GL_TEXTURE_2D, m_textures[path]);
}