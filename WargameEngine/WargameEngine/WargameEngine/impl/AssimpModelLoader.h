#pragma once
#include "..\view\IModelReader.h"

//OBJ not supported since it needs an external file
class CAssimpModelLoader : public wargameEngine::view::IModelReader
{
public:
	CAssimpModelLoader();
	~CAssimpModelLoader();

	bool ModelIsSupported(unsigned char * data, size_t size, const wargameEngine::Path& filePath) const override;
	std::unique_ptr<wargameEngine::view::C3DModel> LoadModel(unsigned char * data, size_t size, const wargameEngine::view::C3DModel & dummyModel, const wargameEngine::Path& filePath) override;

private:
	struct Impl;
	std::shared_ptr<Impl> m_pImpl;
};