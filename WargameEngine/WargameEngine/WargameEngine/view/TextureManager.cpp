#include "TextureManager.h"
#include <GL/glew.h>
#include "gl.h"
#include "../LogWriter.h"
#include "../ThreadPool.h"
#include "../Module.h"
#pragma warning( push )
#pragma warning( disable : 4457 4456)
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#pragma warning( pop )
#include "../nv_dds.h"
#include "../AsyncReadTask.h"

struct sImage
{
	std::string filename;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned char * data;
	int flags;
	GLenum format;
	std::vector<sTeamColor> teamcolor;
	std::vector<unsigned char> uncompressedData;
};

void ApplyTeamcolor(sImage & image, std::string const& maskFile, unsigned char color[3]);

void LoadBMP(void * data, unsigned int /*size*/, sImage & img)
{
	unsigned char* imgData = (unsigned char*) data;
	unsigned int headerSize = *(int*)&(imgData[0x0A]);     // Position in the file where the actual data begins
	img.width = *(int*)&(imgData[0x12]);
	img.height = *(int*)&(imgData[0x16]);
	img.bpp = *(short*)&(imgData[0x1C]);
	img.format = (img.bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;
	if (headerSize==0)  // Some BMP files are misformatted, guess missing information
		headerSize=54;
	img.data = imgData + headerSize;
	for (size_t i = 0; i < img.teamcolor.size(); ++i)
	{
		ApplyTeamcolor(img, img.teamcolor[i].suffix, img.teamcolor[i].color);
	}
}

void UnpackTexture(void * data, unsigned int size, sImage & img)
{
	int width, height, bpp;
	unsigned char * newData = stbi_load_from_memory((const unsigned char*)data, size, &width, &height, &bpp, 4);
	img.uncompressedData.resize(width * height * 4);
	for (int y = 0; y < height; ++y)
	{
		memcpy(&img.uncompressedData[y * width * 4], &newData[(height - y - 1) * width * 4], sizeof(unsigned char) * 4 * width);
	}
	stbi_image_free(newData);
	img.width = width;
	img.height = height;
	img.bpp = 32;
	img.format = GL_RGBA;
	img.data = img.uncompressedData.data();
	for (size_t i = 0; i < img.teamcolor.size(); ++i)
	{
		ApplyTeamcolor(img, img.teamcolor[i].suffix, img.teamcolor[i].color);
	}
}

std::vector<unsigned char> UncompressTGA(unsigned char * data, unsigned int width, unsigned int height, unsigned int bpp)
{
	unsigned int iPixelSize = bpp / 8;
	unsigned int imageSize = width * height * iPixelSize;
	std::vector<unsigned char> uncompressedData;
	uncompressedData.resize(imageSize);
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

void LoadTGA(void * data, unsigned int size, sImage & img)
{
	unsigned char* imgData = (unsigned char*) data;
	if (imgData[2] != 2 && imgData[2] != 10)//use stb_image for non-RGB textures
	{
		UnpackTexture(data, size, img);
		return;
	}
	img.width = imgData[13] * 256 + imgData[12];
	img.height = imgData[15] * 256 + imgData[14];
	img.bpp = imgData[16]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	img.format = (img.bpp == 24)?GL_BGR_EXT:GL_BGRA_EXT;
	if(imgData[2] == 10) //Compressed
	{
		img.uncompressedData = UncompressTGA(imgData, img.width, img.height, img.bpp);
		img.data = img.uncompressedData.data();
	}
	else
	{
		img.data = imgData + 18;
	}
	for (size_t i = 0; i < img.teamcolor.size(); ++i)
	{
		ApplyTeamcolor(img, img.teamcolor[i].suffix, img.teamcolor[i].color);
	}
}

void UseDDS(nv_dds::CDDSImage & image, unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (image.get_num_mipmaps() == 0)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	image.upload_texture2D(0, GL_TEXTURE_2D);
}

void UseTexture(sImage const& img, unsigned int id, float anisotropy)
{
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (img.flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (img.flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (GLEW_EXT_texture_filter_anisotropic)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	// Give the image to OpenGL
	gluBuild2DMipmaps(GL_TEXTURE_2D, img.bpp / 8, img.width, img.height, img.format, GL_UNSIGNED_BYTE, img.data);
}

unsigned int LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, float anisotropy, bool now = false, int flags = 0)
{
	std::shared_ptr<sImage> img = std::make_shared<sImage>();
	img->filename = path;
	img->flags = flags;
	unsigned int id;
	glGenTextures(1, &id);
	unsigned int dotCoord = path.find_last_of('.') + 1;
	std::string extension = path.substr(dotCoord, path.length() - dotCoord);
	img->teamcolor = teamcolor;
	std::function<void(void* data, unsigned int size)> loadingFunc;
	if(extension == "bmp")
		loadingFunc = [img](void* data, unsigned int size) {
			LoadBMP(data, size, *img);
		};
	else if(extension == "tga")
		loadingFunc = [img](void* data, unsigned int size) {
			LoadTGA(data, size, *img);
		};
	else if (extension == "dds")
	{
		std::shared_ptr<nv_dds::CDDSImage> image = std::make_shared<nv_dds::CDDSImage>();
		ThreadPool::RunFunc([image, path] {
			image->load(path);
			if (!image->is_valid())
			{
				throw std::exception(("Cannot open file " + path).c_str());
			}
		}, [image, id]() {
			UseDDS(*image, id);
		});
	}
	else 
		loadingFunc = [img](void* data, unsigned int size) {
			UnpackTexture(data, size, *img);
		};
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(path, loadingFunc);
	readTask->AddOnCompleteHandler([=]() {
		UseTexture(*img, id, anisotropy);
	});
	readTask->AddOnFailHandler([](std::exception const& e) {
		LogWriter::WriteLine(e.what());
	});
	ThreadPool::AddTask(readTask);
	if (now)
	{
		WaitForTask(*readTask);
	}
	return id;
}

void CTextureManager::SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor, int flags)
{
	if(path.empty()) 
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		return;	
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if(m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, m_anisotropyLevel, false, flags);
	}
	glBindTexture(GL_TEXTURE_2D, m_textures[pair]);
}

void CTextureManager::SetAnisotropyLevel(float level)
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, i->second);
		if (GLEW_EXT_texture_filter_anisotropic)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	m_anisotropyLevel = level;
}

void CTextureManager::LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags)
{
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, m_anisotropyLevel, true, flags);
	}
}

void CTextureManager::Reset()
{
	m_textures.clear();
}

CTextureManager::~CTextureManager()
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		glDeleteTextures(1, &i->second);
	}
}

void CTextureManager::SetTexture(std::string const& path, TextureSlot slot, int flags)
{
	glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	if (path.empty())
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		return;
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, m_anisotropyLevel, false, flags);
	}
	glBindTexture(GL_TEXTURE_2D, m_textures[pair]);
	glActiveTexture(GL_TEXTURE0);
}

void ApplyTeamcolor(sImage & image, std::string const& maskFile, unsigned char * color)
{
	std::string path = image.filename.substr(0, image.filename.find_last_of('.')) + maskFile + ".bmp";
	FILE * fmask = fopen(path.c_str(), "rb");
	if (!fmask)
	{
		LogWriter::WriteLine("Texture manager: Cannot open mask file " + path);
		return;
	}
	fseek(fmask, 0L, SEEK_END);
	unsigned int maskSize = ftell(fmask);
	fseek(fmask, 0L, SEEK_SET);
	std::vector<unsigned char> maskData;
	maskData.resize(maskSize);
	fread(maskData.data(), 1, maskSize, fmask);
	fclose(fmask);
	int maskHeight = *(int*)&(maskData[0x12]);
	int maskWidth = *(int*)&(maskData[0x16]);
	short maskbpp = *(short*)&(maskData[0x1C]);
	if (image.format == GL_BGRA || image.format == GL_BGR)
	{
		std::swap(color[0], color[2]);
	}
	if (maskbpp != 8)
	{
		LogWriter::WriteLine("Texture manager: Mask file is not greyscale " + maskFile);
		return;
	}
	for (unsigned int x = 0; x < image.width; ++x)
	{
		for (unsigned int y = 0; y < image.height; ++y)
		{
			unsigned int pos = (x * image.height + y) * image.bpp / 8;
			unsigned int maskPos = 54 + x * maskWidth / image.width * maskHeight + y * maskHeight / image.height;
			for (unsigned int i = 0; i < 3; ++i)
			{
				image.data[pos + i] = static_cast<unsigned char>(image.data[pos + i] * (1.0 - maskData[maskPos] / 255.0) + color[i] * (maskData[maskPos] / 255.0));
			}
		}
	}
}