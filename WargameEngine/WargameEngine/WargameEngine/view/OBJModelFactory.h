#pragma once
#include "IModelReader.h"

namespace wargameEngine
{
namespace view
{
class CObjModelFactory : public IModelReader
{
public:
	bool ModelIsSupported(unsigned char* data, size_t size, const Path& filePath) const override;
	std::unique_ptr<C3DModel> LoadModel(unsigned char* data, size_t size, C3DModel const& dummyModel, const Path& filePath) override;
};
}
}