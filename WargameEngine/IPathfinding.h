#pragma once
#include "view\Vector3.h"
#include <vector>

namespace wargameEngine
{
namespace model
{
class Model;
class IBoundingBoxManager;
}

class IPathfinding
{
public:
	virtual ~IPathfinding() = default;

	virtual void Init(model::Model& model, model::IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution) = 0;
	virtual std::vector<CVector3f> GetPath(const CVector3f& from, const CVector3f& to) const = 0;
};
}