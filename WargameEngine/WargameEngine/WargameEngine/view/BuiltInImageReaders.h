#pragma once
#include "IImageReader.h"

namespace wargameEngine
{
namespace view
{
class CBmpImageReader : public IImageReader
{
public:
	bool ImageIsSupported(unsigned char* data, size_t size, const Path& filePath) const override;
	Image ReadImage(unsigned char* data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CTgaImageReader : public IImageReader
{
public:
	bool ImageIsSupported(unsigned char* data, size_t size, const Path& filePath) const override;
	Image ReadImage(unsigned char* data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CDdsImageReader : public IImageReader
{
public:
	bool ImageIsSupported(unsigned char* data, size_t size, const Path& filePath) const override;
	Image ReadImage(unsigned char* data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CStbImageReader : public IImageReader
{
public:
	bool ImageIsSupported(unsigned char* data, size_t size, const Path& filePath) const override;
	Image ReadImage(unsigned char* data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};
}
}