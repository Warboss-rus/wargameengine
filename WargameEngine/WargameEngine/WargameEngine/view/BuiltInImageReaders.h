#pragma once
#include "IImageReader.h"

class CBmpImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, std::wstring const& filePath, bool flipBmp = false, bool force32bit = false) override;
};

class CTgaImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, std::wstring const& filePath, bool flipBmp = false, bool force32bit = false) override;
};

class CDdsImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, std::wstring const& filePath, bool flipBmp = false, bool force32bit = false) override;
};

class CStbImageReader : public IImageReader
{
public:
	virtual bool ImageIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const override;
	virtual CImage ReadImage(unsigned char * data, size_t size, std::wstring const& filePath, bool flipBmp = false, bool force32bit = false) override;
};