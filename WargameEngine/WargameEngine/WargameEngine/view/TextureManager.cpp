#include "TextureManager.h"
#include "../LogWriter.h"
#include "../ThreadPool.h"
#include "../Module.h"
#pragma warning( push )
#pragma warning( disable : 4457 4456)
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#pragma warning( pop )
#include "../AsyncReadTask.h"

struct sImage
{
	std::string filename;
	unsigned int width;
	unsigned int height;
	unsigned short bpp;
	unsigned char * data;
	int flags = 0;
	size_t size = 0;//0 if uncompressed
	std::vector<sTeamColor> teamcolor;
	std::vector<unsigned char> uncompressedData;
	TextureMipMaps mipmaps;
};

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
}

void LoadSTBI(void * data, unsigned int size, sImage & img)
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
}

inline int clamp_size(int size)
{
	return size <= 0 ? 1 : size;
}

inline size_t size_dxtc(unsigned int width, unsigned int height, int flags)
{
	return ((width + 3) / 4)*((height + 3) / 4)* ((flags & TEXTURE_COMPRESSION_MASK) == TEXTURE_COMPRESSION_DXT1 ? 8 : 16);
}

void LoadDDS(void * data, unsigned int /*size*/, sImage & img)
{
	struct DDS_PIXELFORMAT
	{
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwFourCC;
		unsigned int dwRGBBitCount;
		unsigned int dwRBitMask;
		unsigned int dwGBitMask;
		unsigned int dwBBitMask;
		unsigned int dwABitMask;
	};
	struct DDS_HEADER
	{
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwHeight;
		unsigned int dwWidth;
		unsigned int dwPitchOrLinearSize;
		unsigned int dwDepth;
		unsigned int dwMipMapCount;
		unsigned int dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		unsigned int dwCaps1;
		unsigned int dwCaps2;
		unsigned int dwReserved2[3];
	};
	unsigned char* charData = (unsigned char*)data;
	if (strncmp((char*)charData, "DDS ", 4) != 0)
	{
		return;
	}
	charData += 4;
	DDS_HEADER ddsh;
	memcpy(&ddsh, charData, sizeof(ddsh));
	charData += sizeof(ddsh);
	//swap_endian(&ddsh.dwSize);swap everything with _byteswap_ulong

	const unsigned int DDS_FOURCC = 0x00000004;
	const unsigned int FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
	const unsigned int FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
	const unsigned int FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))
	const unsigned int DDS_RGB = 0x00000040;
	const unsigned int DDS_RGBA = 0x00000041;
	const unsigned int DDS_DEPTH = 0x00800000;
	const unsigned int DDS_CUBEMAP = 0x00000200;
	const unsigned int DDS_VOLUME = 0x00200000;
	if (ddsh.dwCaps2 & DDS_CUBEMAP || ddsh.dwCaps2 & DDS_VOLUME)
	{
		LogWriter::WriteLine("DDS Loader: Only 2D RGB/RGBA textures are supported");
		return;
	}
	bool compressed = false;
	img.bpp = 24;
	img.flags |= TEXTURE_BGRA;
	if (ddsh.ddspf.dwFlags & DDS_FOURCC)
	{
		compressed = true;
		switch (ddsh.ddspf.dwFourCC)
		{
		case FOURCC_DXT1:
			img.flags |= TEXTURE_COMPRESSION_DXT1;
			break;
		case FOURCC_DXT3:
			img.flags |= TEXTURE_COMPRESSION_DXT3;
			img.bpp = 32;
			break;
		case FOURCC_DXT5:
			img.flags |= TEXTURE_COMPRESSION_DXT5;
			img.bpp = 32;
			break;
		default:
			LogWriter::WriteLine("DDS Loader: Unknown compression: " + std::to_string(ddsh.ddspf.dwFourCC));
			return;
		}
	}
	else if ((ddsh.ddspf.dwFlags == DDS_RGBA || ddsh.ddspf.dwFlags == DDS_RGB) && ddsh.ddspf.dwRGBBitCount == 32)
	{
		img.flags |= TEXTURE_HAS_ALPHA;
		img.bpp = 32;
	}
	else if (!(ddsh.ddspf.dwFlags == DDS_RGB && ddsh.ddspf.dwRGBBitCount == 24))
	{
		LogWriter::WriteLine("DDS Loader: Unknown format");
		return;
	}
	img.width = ddsh.dwWidth;
	img.height = ddsh.dwHeight;
	int depth = clamp_size(ddsh.dwDepth);
	size_t imageSize = compressed ? size_dxtc(img.width, img.height, img.flags) : (img.width * img.height * depth * img.bpp / 8);
	img.size = compressed ? imageSize : 0;
	img.data = charData;
	charData += imageSize;

	int numMipmaps = ddsh.dwMipMapCount - 1;
	unsigned int w = clamp_size(img.width >> 1);
	unsigned int h = clamp_size(img.height >> 1);
	int d = clamp_size(depth >> 1);
	for (int i = 0; i < numMipmaps && (w || h); i++)
	{
		imageSize = compressed ? size_dxtc(w, h, img.flags) : (w * h * d * img.bpp / 8);
		//flip image
		img.mipmaps.push_back({ charData, w, h, imageSize });
		w = clamp_size(w >> 1);
		h = clamp_size(h >> 1);
		d = clamp_size(d >> 1);
		charData += imageSize;
	}
	if (numMipmaps > 0)
	{
		img.flags &= ~TEXTURE_BUILD_MIPMAPS;
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
	unsigned char bLength, bLoop;
	while (index < imageSize)
	{
		if (*pCur & 0x80) // Run length chunk (High bit = 1)
		{
			bLength = *pCur - 127; // Get run length
			pCur++;            // Move to pixel data  

							   // Repeat the next pixel bLength times
			for (bLoop = 0; bLoop != bLength; ++bLoop, index += iPixelSize)
				memcpy(&uncompressedData[index], pCur, iPixelSize);

			pCur += iPixelSize; // Move to the next descriptor chunk
		}
		else // Raw chunk
		{
			bLength = *pCur + 1; // Get run length
			pCur++;          // Move to pixel data

							 // Write the next bLength pixels directly
			for (bLoop = 0;bLoop != bLength;++bLoop, index += iPixelSize, pCur += iPixelSize)
				memcpy(&uncompressedData[index], pCur, iPixelSize);
		}
	}
	return uncompressedData;
}

void LoadTGA(void * data, unsigned int size, sImage & img)
{
	unsigned char* imgData = (unsigned char*)data;
	if (imgData[2] != 2 && imgData[2] != 10)//use stb_image for non-RGB textures
	{
		LoadSTBI(data, size, img);
		return;
	}
	img.width = imgData[13] * 256 + imgData[12];
	img.height = imgData[15] * 256 + imgData[14];
	img.bpp = imgData[16]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	if (img.bpp != 24) img.flags |= TEXTURE_HAS_ALPHA;
	img.flags |= TEXTURE_BGRA;
	if (imgData[2] == 10) //Compressed
	{
		img.uncompressedData = UncompressTGA(imgData, img.width, img.height, img.bpp);
		img.data = img.uncompressedData.data();
	}
	else
	{
		img.data = imgData + 18;
	}
}

void ApplyTeamcolor(sImage & image, std::string const& maskFile, unsigned char color[3]);
#pragma region NV_DDS_H compressed image flipping code
struct DXTColBlock
{
	unsigned short col0;
	unsigned short col1;

	unsigned char row[4];
};

void swap(void *byte1, void *byte2, int size)
{
	unsigned char *tmp = new unsigned char[size];

	memcpy(tmp, byte1, size);
	memcpy(byte1, byte2, size);
	memcpy(byte2, tmp, size);

	delete[] tmp;
}

void flip_blocks_dxtc1(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;

	for (int i = 0; i < numBlocks; i++)
	{
		swap(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
		swap(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

		curblock++;
	}
}

void flip_blocks_dxtc3(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	struct DXT3AlphaBlock
	{
		unsigned short row[4];
	};
	DXT3AlphaBlock *alphablock;

	for (int i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT3AlphaBlock*)curblock;

		swap(&alphablock->row[0], &alphablock->row[3], sizeof(unsigned short));
		swap(&alphablock->row[1], &alphablock->row[2], sizeof(unsigned short));

		curblock++;

		swap(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
		swap(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

		curblock++;
	}
}

struct DXT5AlphaBlock
{
	unsigned char alpha0;
	unsigned char alpha1;

	unsigned char row[6];
};

void flip_dxt5_alpha(DXT5AlphaBlock *block)
{
	unsigned char gBits[4][4];

	const unsigned int mask = 0x00000007;          // bits = 00 00 01 11
	unsigned int bits = 0;
	memcpy(&bits, &block->row[0], sizeof(unsigned char) * 3);

	gBits[0][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (unsigned char)(bits & mask);

	bits = 0;
	memcpy(&bits, &block->row[3], sizeof(unsigned char) * 3);

	gBits[2][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (unsigned char)(bits & mask);

	// clear existing alpha bits
	memset(block->row, 0, sizeof(unsigned char) * 6);

	unsigned int *pBits = ((unsigned int*)&(block->row[0]));

	*pBits = *pBits | (gBits[3][0] << 0);
	*pBits = *pBits | (gBits[3][1] << 3);
	*pBits = *pBits | (gBits[3][2] << 6);
	*pBits = *pBits | (gBits[3][3] << 9);

	*pBits = *pBits | (gBits[2][0] << 12);
	*pBits = *pBits | (gBits[2][1] << 15);
	*pBits = *pBits | (gBits[2][2] << 18);
	*pBits = *pBits | (gBits[2][3] << 21);

	pBits = ((unsigned int*)&(block->row[3]));

	*pBits = *pBits | (gBits[1][0] << 0);
	*pBits = *pBits | (gBits[1][1] << 3);
	*pBits = *pBits | (gBits[1][2] << 6);
	*pBits = *pBits | (gBits[1][3] << 9);

	*pBits = *pBits | (gBits[0][0] << 12);
	*pBits = *pBits | (gBits[0][1] << 15);
	*pBits = *pBits | (gBits[0][2] << 18);
	*pBits = *pBits | (gBits[0][3] << 21);
}

// flip a DXT5 color block
void flip_blocks_dxtc5(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	DXT5AlphaBlock *alphablock;

	for (int i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT5AlphaBlock*)curblock;

		flip_dxt5_alpha(alphablock);

		curblock++;

		swap(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
		swap(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

		curblock++;
	}
}
#pragma endregion NV_DDS_H compressed image flipping code

void FlipImage(unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, bool compressed, int flags)
{
	if (!compressed)
	{
		unsigned char * temp = new unsigned char[width * bpp];
		for (unsigned int y = 0; y < height; ++y)
		{
			memcpy(temp, &data[y * width * bpp], sizeof(unsigned char) * bpp * width);
			memcpy(&data[y * width * bpp], &data[(height - y - 1) * width * bpp], sizeof(unsigned char) * bpp * width);
			memcpy(&data[(height - y - 1) * width * bpp], temp, sizeof(unsigned char) * bpp * width);
		}
		delete[] temp;
	}
	else
	{
		void (*flipblocks)(DXTColBlock*, int) = nullptr;
		int xblocks = width / 4;
		int yblocks = height / 4;
		int blocksize = 16;

		switch (flags & TEXTURE_COMPRESSION_MASK)
		{
		case TEXTURE_COMPRESSION_DXT1:
			blocksize = 8;
			flipblocks = &flip_blocks_dxtc1;
			break;
		case TEXTURE_COMPRESSION_DXT3:
			flipblocks = &flip_blocks_dxtc3;
			break;
		case TEXTURE_COMPRESSION_DXT5:
			flipblocks = &flip_blocks_dxtc5;
			break;
		default:
			return;
		}

		size_t linesize = xblocks * blocksize;

		DXTColBlock *top;
		DXTColBlock *bottom;

		unsigned char * temp = new unsigned char[linesize];
		for (int j = 0; j < (yblocks >> 1); j++)
		{
			top = (DXTColBlock*)(data + j * linesize);
			bottom = (DXTColBlock*)(data + (((yblocks - j) - 1) * linesize));

			(*flipblocks)(top, xblocks);
			(*flipblocks)(bottom, xblocks);

			swap(bottom, top, linesize);
		}
		delete[] temp;
	}
}

unsigned char* Convert24To32Bit(unsigned char * data, unsigned int width, unsigned int height, std::vector<unsigned char> & result)
{
	size_t oldSize = result.size();
	result.reserve(oldSize + width * height * 4);
	for (size_t i = 0; i < width * height; ++i)
	{
		result.push_back(data[i * 3]);
		result.push_back(data[i * 3 + 1]);
		result.push_back(data[i * 3 + 2]);
		result.push_back(255);
	}
	return result.data() + oldSize + 1;
}

void ConvertTo32Bit(sImage &img)
{
	std::vector<unsigned char> result;
	Convert24To32Bit(img.data, img.width, img.height, result);
	for (auto& mipmap : img.mipmaps)
	{
		mipmap.data = Convert24To32Bit(mipmap.data, mipmap.width, mipmap.height, result);
	}
	img.uncompressedData = std::move(result);
	img.data = img.uncompressedData.data();
	img.bpp = 32;
	img.flags |= TEXTURE_HAS_ALPHA;
}

void LoadImage(void * data, unsigned int size, sImage & img, bool flipBmp = false, bool force32bit = false)
{
	bool flipped = true;
	std::string extension = img.filename.substr(img.filename.find_last_of('.') + 1);
	if (extension == "tga")
	{
		LoadTGA(data, size, img);
		flipped = false;
	}
	else if (strncmp((char*)data, "BM", 2) == 0)
	{
		LoadBMP(data, size, img);
		flipped = false;
	}
	else if (strncmp((char*)data, "DDS ", 4) == 0)
	{
		LoadDDS(data, size, img);
	}
	else
	{
		LoadSTBI(data, size, img);
	}
	bool compressed = img.size != 0;
	if (!compressed)//don't apply teamcolor to compressed images
	{
		for (size_t i = 0; i < img.teamcolor.size(); ++i)
		{
			ApplyTeamcolor(img, img.teamcolor[i].suffix, img.teamcolor[i].color);
		}
	}
	if (flipBmp != flipped)
	{
		FlipImage(img.data, img.width, img.height, img.bpp / 8, compressed, img.flags);
		for (auto& mipmap : img.mipmaps)
		{
			FlipImage(mipmap.data, mipmap.width, mipmap.height, img.bpp / 8, compressed, img.flags);
		}
	}
	
	if (force32bit && img.bpp == 24 && !compressed)
	{
		ConvertTo32Bit(img);
	}
}

void CTextureManager::UseTexture(sImage const& img, ICachedTexture& texture)
{
	if (img.size != 0)
	{
		m_helper.UploadCompressedTexture(texture, img.data, img.width, img.height, img.size, img.flags, img.mipmaps);
	}
	else
	{
		m_helper.UploadTexture(texture, img.data, img.width, img.height, img.bpp, img.flags, img.mipmaps);
	}
	m_helper.SetTextureAnisotropy(m_anisotropyLevel);
}

std::unique_ptr<ICachedTexture> CTextureManager::LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, bool now, int flags)
{
	std::shared_ptr<sImage> img = std::make_shared<sImage>();
	img->filename = path;
	img->flags = flags | TEXTURE_BUILD_MIPMAPS;
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;	
	img->teamcolor = teamcolor;
	bool force32b = m_helper.Force32Bits();
	bool forceFlip = m_helper.ForceFlipBMP();
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(path, [=](void* data, unsigned int size) {
		LoadImage(data, size, *img, forceFlip, force32b);
	});
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