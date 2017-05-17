#pragma once
#include "ITextureHelper.h"
#include <vector>

namespace wargameEngine
{
namespace view
{
class Image
{
public:
	Image();
	Image(unsigned char* data, size_t width, size_t height, unsigned short bpp, int flags = 0, size_t size = 0);
	Image(std::vector<unsigned char>&& uncompressedData, size_t width, size_t height, unsigned short bpp, int flags = 0, size_t size = 0);
	TextureMipMaps& GetMipmaps();
	std::vector<unsigned char>& GetUncompressedData();
	void SetBpp(unsigned short bpp);
	void SetFlags(int flags);

	size_t GetWidth() const;
	size_t GetHeight() const;
	unsigned short GetBPP() const;
	unsigned char* GetData() const;
	int GetFlags() const;
	const TextureMipMaps& GetMipmaps() const;
	bool IsCompressed() const;
	size_t GetImageSize() const;
	void StoreData();

private:
	size_t m_width;
	size_t m_height;
	unsigned short m_bpp;
	unsigned char* m_data;
	int m_flags;
	size_t m_size; //0 if uncompressed
	std::vector<unsigned char> m_uncompressedData;
	TextureMipMaps m_mipmaps;
};
}
}