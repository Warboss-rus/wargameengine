#pragma once
#include "IModelReader.h"

class CObjModelFactory : public IModelReader
{
public:
	virtual bool ModelIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, C3DModel const& dummyModel, const Path& filePath) override;
};