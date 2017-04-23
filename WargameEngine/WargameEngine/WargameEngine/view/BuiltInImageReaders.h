#pragma once
#include "IImageReader.h"

class CBmpImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CTgaImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CDdsImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};

class CStbImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, const Path& filePath, sReaderParameters const& params) override;
};