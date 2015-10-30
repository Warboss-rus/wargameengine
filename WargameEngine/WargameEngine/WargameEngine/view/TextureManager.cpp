#include "TextureManager.h"
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
	unsigned short bpp;
	unsigned char * data;
	int flags = 0;
	std::vector<sTeamColor> teamcolor;
	std::vector<unsigned char> uncompressedData;
};

void ApplyTeamcolor(sImage & image, std::string const& maskFile, unsigned char color[3]);

void LoadBMP(void * data, unsigned int /*size*/, sImage & img)
{
	unsigned char* imgData = (unsigned char*)data;
	unsigned int headerSize = *(int*)&(imgData[0x0A]);     // Position in the file where the actual data begins
	img.width = *(int*)&(imgData[0x12]);
	img.height = *(int*)&(imgData[0x16]);
	img.bpp = *(short*)&(imgData[0x1C]);
	if(img.bpp != 24)img.flags |= TEXTURE_HAS_ALPHA;
	img.flags |= TEXTURE_BGRA;
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
	img.flags |= TEXTURE_HAS_ALPHA;
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
	if (img.bpp != 24) img.flags |= TEXTURE_HAS_ALPHA;
	img.flags |= TEXTURE_BGRA;
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

void UseDDS(nv_dds::CDDSImage & image, ICachedTexture& texture, ITextureHelper & helper)
{
	int flags = TEXTURE_BGRA;
	if (image.get_components() == 4) flags |= TEXTURE_HAS_ALPHA;
	if (!image.get_num_mipmaps()) flags |= TEXTURE_BUILD_MIPMAPS;
	TextureMipMaps mipmaps;
	for (auto i = 0; i < image.get_num_mipmaps(); i++)
	{
		auto& mipmap = image.get_mipmap(i);
		char * data = mipmap;
		sTextureMipMap texMipMap = { reinterpret_cast<unsigned char*>(data), static_cast<unsigned int>(mipmap.get_width()), static_cast<unsigned int>(mipmap.get_height()), static_cast<unsigned int>(mipmap.get_size()) };
		mipmaps.push_back(texMipMap);
	}

	char * data = image;
	if (image.is_compressed())
	{
		static const std::map<int, TextureFlags> compressionMap = {
			{ GL_COMPRESSED_RGB_S3TC_DXT1_EXT, TEXTURE_COMPRESSION_DXT1_NO_ALPHA },
			{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, TEXTURE_COMPRESSION_DXT1 },
			{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, TEXTURE_COMPRESSION_DXT3 },
			{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, TEXTURE_COMPRESSION_DXT5 }
		};
		helper.UploadCompressedTexture(texture, reinterpret_cast<unsigned char*>(data), image.get_width(), image.get_height(), image.get_size(), compressionMap.at(image.get_format()), mipmaps);
	}
	else
	{
		helper.UploadTexture(texture, reinterpret_cast<unsigned char*>(data), image.get_width(), image.get_height(), static_cast<unsigned short>(image.get_components() * 8), flags, mipmaps);
	}
}

void CTextureManager::UseTexture(sImage const& img, ICachedTexture& texture)
{
	m_helper.UploadTexture(texture, img.data, img.width, img.height, img.bpp, img.flags | TEXTURE_BUILD_MIPMAPS);
	m_helper.SetTextureAnisotropy(m_anisotropyLevel);
}

std::unique_ptr<ICachedTexture> CTextureManager::LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, bool now, int flags)
{
	std::shared_ptr<sImage> img = std::make_shared<sImage>();
	img->filename = path;
	img->flags = flags;
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;
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
		}, [image, &texRef, this]() {
			UseDDS(*image, texRef, m_helper);
		});
	}
	else 
		loadingFunc = [img](void* data, unsigned int size) {
			UnpackTexture(data, size, *img);
		};
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(path, loadingFunc);
	readTask->AddOnCompleteHandler([=, &texRef]() {
		UseTexture(*img, texRef);
	});
	readTask->AddOnFailHandler([](std::exception const& e) {
		LogWriter::WriteLine(e.what());
	});
	ThreadPool::AddTask(readTask);
	if (now)
	{
		WaitForTask(*readTask);
	}
	return tex;
}

CTextureManager::CTextureManager(ITextureHelper & helper)
	:m_helper(helper)
{
}

void CTextureManager::SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor, int flags)
{
	if(path.empty()) 
	{
		m_helper.UnbindTexture();
		return;	
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if(m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, false, flags);
	}
	m_textures[pair]->Bind();
}

void CTextureManager::SetAnisotropyLevel(float level)
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		i->second->Bind();
		m_helper.SetTextureAnisotropy(level);
	}
	m_helper.UnbindTexture();
	m_anisotropyLevel = level;
}

void CTextureManager::LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags)
{
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, true, flags);
	}
}

void CTextureManager::Reset()
{
	m_textures.clear();
}

void CTextureManager::SetTexture(std::string const& path, TextureSlot slot, int flags)
{
	m_helper.ActivateTextureSlot(slot);
	if (path.empty())
	{
		m_helper.UnbindTexture();
		m_helper.ActivateTextureSlot(TextureSlot::eDiffuse);
		return;
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(sModule::textures + path, pair.second, false, flags);
	}
	m_textures[pair]->Bind();
	m_helper.ActivateTextureSlot(TextureSlot::eDiffuse);
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
	if (image.flags & TEXTURE_HAS_ALPHA)
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