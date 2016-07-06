#pragma once
#include <vector>
#include "ITextureHelper.h"

class CImage
{
public:
	virtual ~CImage() {}
	CImage()
		:m_width(0), m_height(0), m_bpp(0),m_data(nullptr), m_flags(0), m_size(0)
	{}
	CImage(unsigned char* data, unsigned int width, unsigned int height, unsigned short bpp, int flags = 0, size_t size = 0)
		:m_width(width), m_height(height), m_bpp(bpp), m_data(data), m_flags(flags), m_size(size)
	{}
	CImage(std::vector<unsigned char> && uncompressedData, unsigned int width, unsigned int height, unsigned short bpp, int flags = 0, size_t size = 0)
		:m_width(width), m_height(height), m_bpp(bpp), m_flags(flags), m_size(size),m_uncompressedData(uncompressedData)
	{
		m_data = m_uncompressedData.data();
	}
	TextureMipMaps& GetMipmaps() { return m_mipmaps; }
	std::vector<unsigned char>& GetUncompressedData() { return m_uncompressedData; }
	void SetBpp(unsigned short bpp) { m_bpp = bpp; }
	void SetFlags(int flags) { m_flags = flags; }

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	unsigned short GetBPP() const { return m_bpp; }
	unsigned char* GetData() const { return (m_uncompressedData.empty() ? m_data : (unsigned char*)m_uncompressedData.data()); }
	int GetFlags() const { return m_flags; }
	const TextureMipMaps& GetMipmaps() const { return m_mipmaps; }
	bool IsCompressed() const { return m_size != 0; }
	size_t GetImageSize() const { return IsCompressed() ? m_size : m_width * m_height * m_bpp; }
private:
	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_bpp;
	unsigned char * m_data;
	int m_flags;
	size_t m_size;//0 if uncompressed
	std::vector<unsigned char> m_uncompressedData;
	TextureMipMaps m_mipmaps;
};

class IImageReader
{
public:
	virtual ~IImageReader() {}

	virtual bool ImageIsSupported(unsigned char * data, size_t size, std::string const& filePath) const = 0;
	virtual CImage ReadImage(unsigned char * data, size_t size, std::string const& filePath, bool flipBmp = false, bool force32bit = false) = 0;
};