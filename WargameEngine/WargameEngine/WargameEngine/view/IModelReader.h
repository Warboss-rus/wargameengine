#pragma once
#include <memory>
#include <string>

class C3DModel;

class IModelReader
{
public:
	virtual ~IModelReader() {}

	virtual bool ModelIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const = 0;
	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, C3DModel const& dummyModel, std::wstring const& filePath) = 0;
};
