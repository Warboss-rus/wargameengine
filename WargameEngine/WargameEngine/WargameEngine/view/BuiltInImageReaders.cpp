#include "BuiltInImageReaders.h"
#pragma warning (push)
#pragma warning (disable: 4244)
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#pragma warning (pop)
#include <algorithm>
#include <string>
#include "../Utils.h"
#include "../MemoryStream.h"

unsigned char* Convert24To32Bit(unsigned char * data, size_t width, size_t height, std::vector<unsigned char> & result)
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

void ConvertTo32Bit(unsigned char* data, size_t width, size_t height, std::vector<unsigned char> & uncompressedData, TextureMipMaps * mipmaps = nullptr)
{
	std::vector<unsigned char> result;
	Convert24To32Bit(data, width, height, result);
	if (mipmaps)
	{
		for (auto& mipmap : *mipmaps)
		{
			mipmap.data = Convert24To32Bit(mipmap.data, mipmap.width, mipmap.height, result);
		}
	}
	uncompressedData.clear();
	uncompressedData = result;
}

inline int clamp_size(int size)
{
	return size <= 0 ? 1 : size;
}

inline size_t size_dxtc(unsigned int width, unsigned int height, int flags)
{
	return ((width + 3) / 4)*((height + 3) / 4)* ((flags & TEXTURE_COMPRESSION_MASK) == TEXTURE_COMPRESSION_DXT1 ? 8 : 16);
}

struct DXTColBlock
{
	unsigned short col0;
	unsigned short col1;

	unsigned char row[4];
};

void swap(void *byte1, void *byte2, size_t size)
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

	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			gBits[i][j] = (unsigned char)(bits & mask);
			bits >>= 3;
		}
	}

	bits = 0;
	memcpy(&bits, &block->row[3], sizeof(unsigned char) * 3);

	for (size_t i = 2; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			gBits[i][j] = (unsigned char)(bits & mask);
			bits >>= 3;
		}
	}

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

void FlipImage(unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, bool compressed, int flags)
{
	if (!compressed)
	{
		std::vector<unsigned char> temp(width * bpp);
		for (unsigned int y = 0; y < height; ++y)
		{
			memcpy(temp.data(), &data[y * width * bpp], sizeof(unsigned char) * bpp * width);
			memcpy(&data[y * width * bpp], &data[(height - y - 1) * width * bpp], sizeof(unsigned char) * bpp * width);
			memcpy(&data[(height - y - 1) * width * bpp], temp.data(), sizeof(unsigned char) * bpp * width);
		}
	}
	else
	{
		void(*flipblocks)(DXTColBlock*, int) = nullptr;
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

void ConvertBgra(unsigned char * data, size_t count, unsigned short bpp)
{
	for (size_t i = 0; i < count; ++i)
	{
		std::swap(data[i * bpp], data[i * bpp + 2]);
	}
}

bool CBmpImageReader::ImageIsSupported(unsigned char * data, size_t /*size*/, std::wstring const& /*filePath*/) const
{
	return strncmp((char*)data, "BM", 2) == 0;
}

CImage CBmpImageReader::ReadImage(unsigned char * data, size_t /*size*/, std::wstring const& /*filePath*/, sReaderParameters const& params)
{
	CReadMemoryStream stream(reinterpret_cast<char*>(data));
	stream.Seek(0x0A);
	int headerSize = stream.ReadInt();
	stream.ReadInt();//skip 4 bytes
	int height = stream.ReadInt();
	int width = stream.ReadInt();
	stream.ReadShort();//skip 2 bytes
	short bpp = stream.ReadShort();
	int flags = TEXTURE_BGRA;
	if (bpp != 24)flags |= TEXTURE_HAS_ALPHA;
	if (headerSize == 0)  // Some BMP files are misformatted, guess missing information
		headerSize = 54;
	data += headerSize;
	if (params.flipBmp)
	{
		FlipImage(data, width, height, bpp / 8, false, flags);
	}
	if (params.convertBgra)
	{
		ConvertBgra(data, width * height, bpp / 8);
		flags &= ~TEXTURE_BGRA;
	}
	if (params.force32bit && bpp == 24)
	{
		flags |= TEXTURE_HAS_ALPHA;
		std::vector<unsigned char> uncompressed;
		ConvertTo32Bit(data, width, height, uncompressed);
		return CImage(std::move(uncompressed), width, height, 32, flags);
	}
	return CImage(data, width, height, bpp, flags);
}

bool CTgaImageReader::ImageIsSupported(unsigned char * data, size_t /*size*/, std::wstring const& filePath) const
{
	std::wstring extension = filePath.substr(filePath.find_last_of('.') + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	return extension == L"tga" && (data[2] == 2 || data[2] == 10);//non-rgb texture
}

CImage CTgaImageReader::ReadImage(unsigned char * data, size_t /*size*/, std::wstring const& /*filePath*/, sReaderParameters const& params)
{
	unsigned int width = data[13] * 256 + data[12];
	unsigned int height = data[15] * 256 + data[14];
	unsigned short bpp = data[16]; //bytes per pixel. Can be 24 (without alpha) or 32 (with alpha)
	unsigned int flags = TEXTURE_BGRA;
	if (bpp != 24) flags |= TEXTURE_HAS_ALPHA;
	CImage result;
	if (data[2] == 10) //Compressed
	{
		result =  CImage(UncompressTGA(data, width, height, bpp), width, height, bpp, flags);
	}
	else
	{
		result = CImage(data + 18, width, height, bpp, flags);
	}
	if (params.convertBgra)
	{
		ConvertBgra(result.GetData(), result.GetWidth() * result.GetHeight(), result.GetBPP() / 8);
		result.SetFlags(flags & ~TEXTURE_BGRA);
	}
	if (params.flipBmp)
	{
		FlipImage(result.GetData(), result.GetWidth(), result.GetHeight(), result.GetBPP() / 8, false, result.GetFlags());
	}
	if (params.force32bit && bpp == 24)
	{
		ConvertTo32Bit(result.GetData(), result.GetWidth(), result.GetHeight(), result.GetUncompressedData(), &result.GetMipmaps());
		result.SetBpp(32);
		result.SetFlags(result.GetFlags() | TEXTURE_HAS_ALPHA);
	}
	return result;
}

bool CDdsImageReader::ImageIsSupported(unsigned char * data, size_t /*size*/, std::wstring const& /*filePath*/) const
{
	return strncmp((char*)data, "DDS ", 4) == 0;
}

CImage CDdsImageReader::ReadImage(unsigned char * data, size_t /*size*/, std::wstring const& /*filePath*/, sReaderParameters const& params)
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
	if (strncmp((char*)data, "DDS ", 4) != 0)
	{
		throw std::runtime_error("DDS Loader: Not a DDS file");
	}
	data += 4;
	DDS_HEADER ddsh;
	memcpy(&ddsh, data, sizeof(ddsh));
	data += sizeof(ddsh);
	//swap_endian(&ddsh.dwSize);swap everything with _byteswap_ulong

	const unsigned int DDS_FOURCC = 0x00000004;
	const unsigned int FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
	const unsigned int FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
	const unsigned int FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))
	const unsigned int DDS_RGB = 0x00000040;
	const unsigned int DDS_RGBA = 0x00000041;
	const unsigned int DDS_CUBEMAP = 0x00000200;
	const unsigned int DDS_VOLUME = 0x00200000;
	if (ddsh.dwCaps2 & DDS_CUBEMAP || ddsh.dwCaps2 & DDS_VOLUME)
	{
		throw std::runtime_error("DDS Loader: Only 2D RGB/RGBA textures are supported");
	}
	bool compressed = false;
	unsigned short bpp = 24;
	unsigned int flags = TEXTURE_BGRA;
	if (ddsh.ddspf.dwFlags & DDS_FOURCC)
	{
		compressed = true;
		flags |= TEXTURE_HAS_ALPHA;
		switch (ddsh.ddspf.dwFourCC)
		{
		case FOURCC_DXT1:
			flags |= TEXTURE_COMPRESSION_DXT1;
			break;
		case FOURCC_DXT3:
			flags |= TEXTURE_COMPRESSION_DXT3;
			bpp = 32;
			break;
		case FOURCC_DXT5:
			flags |= TEXTURE_COMPRESSION_DXT5;
			bpp = 32;
			break;
		default:
			throw std::runtime_error("DDS Loader: Unknown compression: " + std::to_string(ddsh.ddspf.dwFourCC));
		}
	}
	else if ((ddsh.ddspf.dwFlags == DDS_RGBA || ddsh.ddspf.dwFlags == DDS_RGB) && ddsh.ddspf.dwRGBBitCount == 32)
	{
		flags |= TEXTURE_HAS_ALPHA;
		bpp = 32;
	}
	else if (!(ddsh.ddspf.dwFlags == DDS_RGB && ddsh.ddspf.dwRGBBitCount == 24))
	{
		throw std::runtime_error("DDS Loader: Unknown format");
	}
	unsigned int width = ddsh.dwWidth;
	unsigned int height = ddsh.dwHeight;
	int depth = clamp_size(ddsh.dwDepth);
	size_t imageSize = compressed ? size_dxtc(width, height, flags) : (width * height * depth * bpp / 8);
	size_t resultSize = compressed ? imageSize : 0;
	auto result = CImage(data, width, height, bpp, flags, resultSize);
	data += imageSize;

	int numMipmaps = ddsh.dwMipMapCount - 1;
	unsigned int w = clamp_size(width >> 1);
	unsigned int h = clamp_size(height >> 1);
	int d = clamp_size(depth >> 1);
	for (int i = 0; i < numMipmaps && (w || h); i++)
	{
		imageSize = compressed ? size_dxtc(w, h, flags) : (w * h * d * bpp / 8);
		//flip image
		result.GetMipmaps().push_back({ data, w, h, imageSize });
		w = clamp_size(w >> 1);
		h = clamp_size(h >> 1);
		d = clamp_size(d >> 1);
		data += imageSize;
	}
	if (!params.flipBmp)
	{
		FlipImage(result.GetData(), width, height, bpp / 8, compressed, flags);
		for (auto& mipmap : result.GetMipmaps())
		{
			FlipImage(mipmap.data, mipmap.width, mipmap.height, bpp / 8, compressed, flags);
		}
	}
	if (params.force32bit && bpp == 24)
	{
		ConvertTo32Bit(result.GetData(), width, height, result.GetUncompressedData(), &result.GetMipmaps());
		result.SetBpp(32);
		result.SetFlags(result.GetFlags() | TEXTURE_HAS_ALPHA);
	}
	return result;
}

bool CStbImageReader::ImageIsSupported(unsigned char * /*data*/, size_t /*size*/, std::wstring const& /*filePath*/) const
{
	return true;
}

CImage CStbImageReader::ReadImage(unsigned char * data, size_t size, std::wstring const& /*filePath*/, sReaderParameters const& params)
{
	int width, height, bpp;
	unsigned char * newData = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &bpp, 4);
	if (!newData)
	{
		throw std::runtime_error(stbi_failure_reason());
	}
	std::vector<unsigned char> uncompressedData(height * width * bpp * sizeof(unsigned char));
	for (int y = 0; y < height; ++y)
	{
		memcpy(&uncompressedData[y * width * 4], &newData[(height - y - 1) * width * 4], sizeof(unsigned char) * 4 * width);
	}
	stbi_image_free(newData);
	if (params.flipBmp)
	{
		FlipImage(uncompressedData.data(), width, height, static_cast<unsigned short>(bpp), false, TEXTURE_HAS_ALPHA);
	}
	return CImage(std::move(uncompressedData), width, height, static_cast<unsigned short>(bpp * 8), TEXTURE_HAS_ALPHA);
}
