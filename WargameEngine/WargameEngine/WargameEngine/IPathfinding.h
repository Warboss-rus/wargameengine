#pragma once
#include <vector>
#include "view\Vector3.h"

class CGameModel;
class IBoundingBoxManager;

class IPathfinding
{
public:
	virtual ~IPathfinding() = default;

	virtual void Init(CGameModel& model, const IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution) = 0;
	virtual std::vector<CVector3f> GetPath(const CVector3f& from, const CVector3f& to) const = 0;
};