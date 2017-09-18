#include "Image.h"

namespace wargameEngine
{
namespace view
{

void Image::SetBpp(unsigned short bpp)
{
	m_bpp = bpp;
}

void Image::SetFlags(int flags)
{
	m_flags = flags;
}

size_t Image::GetWidth() const
{
	return m_width;
}

size_t Image::GetHeight() const
{
	return m_height;
}

unsigned short Image::GetBPP() const
{
	return m_bpp;
}

unsigned char* Image::GetData() const
{
	return (m_uncompressedData.empty() ? m_data : (unsigned char*)m_uncompressedData.data());
}

int Image::GetFlags() const
{
	return m_flags;
}

Image::Image(std::vector<unsigned char>&& uncompressedData, size_t width, size_t height, unsigned short bpp, int flags /*= 0*/, size_t size /*= 0*/)
	: m_width(width)
	, m_height(height)
	, m_bpp(bpp)
	, m_flags(flags)
	, m_size(size)
	, m_uncompressedData(uncompressedData)
{
	m_data = m_uncompressedData.data();
}

Image::Image()
	: m_width(0)
	, m_height(0)
	, m_bpp(0)
	, m_data(nullptr)
	, m_flags(0)
	, m_size(0)
{
}

Image::Image(unsigned char* data, size_t width, size_t height, unsigned short bpp, int flags /*= 0*/, size_t size /*= 0*/)
	: m_width(width)
	, m_height(height)
	, m_bpp(bpp)
	, m_data(data)
	, m_flags(flags)
	, m_size(size)
{
}

TextureMipMaps& Image::GetMipmaps()
{
	return m_mipmaps;
}

const TextureMipMaps& Image::GetMipmaps() const
{
	return m_mipmaps;
}

std::vector<unsigned char>& Image::GetUncompressedData()
{
	return m_uncompressedData;
}

bool Image::IsCompressed() const
{
	return m_size != 0;
}

size_t Image::GetImageSize() const
{
	return IsCompressed() ? m_size : m_width * m_height * m_bpp;
}

void Image::StoreData()
{
	unsigned char* data = GetData();
	if (data != m_uncompressedData.data())
	{
		m_uncompressedData.resize(m_width * m_height * m_bpp / 8);
		memcpy(m_uncompressedData.data(), data, m_width * m_height * m_bpp / 8);
		m_data = m_uncompressedData.data();
	}
}
}
}