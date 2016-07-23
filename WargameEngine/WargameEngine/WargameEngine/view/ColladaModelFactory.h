#pragma once
#include "IModelReader.h"

class CColladaModelFactory : public IModelReader
{
public:
	virtual bool ModelIsSupported(unsigned char * data, size_t size, std::wstring const& filePath) const override;

	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, C3DModel const& dummyModel, std::wstring const& filePath) override;

};
