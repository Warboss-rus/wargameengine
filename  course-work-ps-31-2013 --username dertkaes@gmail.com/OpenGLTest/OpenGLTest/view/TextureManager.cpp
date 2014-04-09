#include "TextureManager.h"
#include <GL\glut.h>
#include "..\picopng.h"
#include "..\LogWriter.h"
#include "..\ThreadPool.h"

CTextureManager * CTextureManager::m_manager = NULL;

struct sImage
{
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned char * data;
	unsigned int headerSize;
	GLenum format;
	GLuint id;
};

void* LoadBMP(void * data, unsigned int size, void* param)
{
	sImage* img = (sImage*)param;
	unsigned char* imgData = (unsigned char*) data;
	img->headerSize = *(int*)&(imgData[0x0A]);     // Position in the file where the actual data begins
	unsigned int imageSize = *(int*)&(imgData[0x22]);
	img->width = *(int*)&(imgData[0x12]);
	img->height = *(int*)&(imgData[0x16]);
	img->bpp = *(short*)&(imgData[0x1C]);
	img->format = (img->bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;
	if (img->headerSize==0)  // Some BMP files are misformatted, guess missing information
		img->headerSize=54;
	img->data = imgData;
	return img;
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
	return uncompressedData;
}

void* LoadTGA(void * data, unsigned int size, void* param)
{
	sImage* img = (sImage*)param;
	unsigned char* imgData = (unsigned char*) data;
	if(imgData[2] != 2 && imgData[2] != 10) 
		return img; //nonRGB TGA are not supported
	img->width = imgData[13] * 256 + imgData[12];
	img->height = imgData[15] * 256 + imgData[14];
	img->bpp = imgData[16]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	img->format = (img->bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;
	img->headerSize = 18;
	if(imgData[2] == 10) //Compressed
	{
		unsigned char* old = imgData;
		imgData = UncompressTGA(imgData + img->headerSize, img->width, img->height, img->bpp);
		delete [] old;
		img->headerSize = 0;
	}
	img->data = imgData;
	return img;
}

void * LoadPNG(void * data, unsigned int size, void* param)
{
	sImage* img = (sImage*)param;
	unsigned char* imgData = (unsigned char*) data;

	std::vector<unsigned char> uncompressedData;
	unsigned long lheight, lwidth;
	decodePNG(uncompressedData, lwidth, lheight, imgData, size, true);
	img->height = lheight;
	img->width = lwidth;
	delete [] imgData;

	img->bpp = 32;
	long imageSize = img->height * img->width * img->bpp / 8;
	img->format = GL_RGBA;
	img->data = new unsigned char [imageSize];
	img->headerSize = 0;
	for(unsigned long y = 0; y < img->height; ++y)
	{
		memcpy(&img->data[y * img->width * 4], &uncompressedData[(img->height - y - 1) * img->width * 4], sizeof(unsigned char) * 4 * img->width);
	}
	return img;
}

void UseTexture(void* data)
{
	sImage *img = (sImage*)data;
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, img->id);
	// Give the image to OpenGL
	gluBuild2DMipmaps(GL_TEXTURE_2D, img->bpp / 8, img->width, img->height, img->format, GL_UNSIGNED_BYTE, &img->data[img->headerSize]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	delete [] img->data;
	delete img;
}

unsigned int LoadTexture(std::string const& path)
{
	sImage* img = new sImage;
	glGenTextures(1, &img->id);
	unsigned int dotCoord = path.find_last_of('.') + 1;
	std::string extension = path.substr(dotCoord, path.length() - dotCoord);
	if(extension == "bmp")
		CThreadPool::AsyncReadFile(path, LoadBMP, img, UseTexture);
	if(extension == "tga")
		CThreadPool::AsyncReadFile(path, LoadTGA, img, UseTexture);
	if(extension == "png")
		CThreadPool::AsyncReadFile(path, LoadPNG, img, UseTexture);
	return img->id;
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
	CThreadPool::Update();
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