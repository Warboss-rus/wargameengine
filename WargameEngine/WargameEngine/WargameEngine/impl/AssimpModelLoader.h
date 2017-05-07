#pragma once
#include "..\view\IModelReader.h"

//OBJ not supported since it needs an external file
class CAssimpModelLoader : public IModelReader
{
public:
	CAssimpModelLoader();
	~CAssimpModelLoader();

	bool ModelIsSupported(unsigned char * data, size_t size, const Path& filePath) const override;
	std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, const C3DModel & dummyModel, const Path& filePath) override;

private:
	struct Impl;
	std::shared_ptr<Impl> m_pImpl;
};