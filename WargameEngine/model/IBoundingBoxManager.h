#pragma once
#include "../Typedefs.h"
#include "Bounding.h"

namespace wargameEngine
{
namespace model
{
class IBoundingBoxManager
{
public:
	virtual Bounding GetBounding(const Path& path) = 0;
	virtual float GetModelScale(const Path& path) = 0;
	virtual CVector3f GetModelRotation(const Path& path) = 0;

	virtual ~IBoundingBoxManager() {}
};
}
}