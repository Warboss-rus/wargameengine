#pragma once
#include "../Typedefs.h"
#include <memory>

namespace wargameEngine
{
namespace view
{
class C3DModel;

class IModelReader
{
public:
	virtual ~IModelReader() {}

	virtual bool ModelIsSupported(unsigned char* data, size_t size, const Path& filePath) const = 0;
	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char* data, size_t size, const C3DModel& dummyModel, const Path& filePath) = 0;
};
}
}