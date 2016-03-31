#pragma once
#include "IModelReader.h"

class CWBMModelFactory : public IModelReader
{
public:
	virtual bool ModelIsSupported(unsigned char * data, size_t size, std::string const& filePath) const override;
	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, C3DModel const& dummyModel, std::string const& filePath) override;
};