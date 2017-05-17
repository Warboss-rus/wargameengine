#pragma once
#include <vector>
#include "../Typedefs.h"
#include "Image.h"

namespace wargameEngine
{
namespace view
{
struct sReaderParameters
{
	bool flipBmp = false;
	bool force32bit = false;
	bool convertBgra = false;
};

class IImageReader
{
public:
	virtual ~IImageReader() {}

	virtual bool ImageIsSupported(unsigned char * data, size_t size, const Path& filePath) const = 0;
	virtual Image ReadImage(unsigned char * data, size_t size, const Path& filePath, sReaderParameters const& params) = 0;
};
}
}