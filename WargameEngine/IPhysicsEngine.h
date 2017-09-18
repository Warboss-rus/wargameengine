#pragma once
#include "view/Vector3.h"
#include <chrono>
#include <vector>

namespace wargameEngine
{
namespace model
{
class IObject;
class IBaseObject;
class Landscape;
class IBoundingBoxManager;
}
namespace view
{
class IRenderer;
}

class IPhysicsEngine
{
public:
	struct CastRayResult
	{
		bool success = false;
		model::IBaseObject* object = nullptr;
		CVector3f hitPoint;
	};

	virtual ~IPhysicsEngine() {}

	virtual void Update(std::chrono::microseconds timeDelta) = 0;
	virtual void Reset(model::IBoundingBoxManager& boundingManager) = 0;
	virtual void AddDynamicObject(model::IObject* object, double mass) = 0;
	virtual void AddStaticObject(model::IBaseObject* staticObject) = 0;
	virtual void RemoveObject(model::IBaseObject* object) = 0;
	virtual void SetGround(model::Landscape* landscape) = 0;
	virtual CastRayResult CastRay(CVector3f const& origin, CVector3f const& dest, std::vector<model::IBaseObject*> const& excludeObjects = std::vector<model::IBaseObject*>()) const = 0;
	virtual bool TestObject(model::IBaseObject* object) const = 0;
	virtual void Draw(view::IRenderer& renderer) const = 0; //for debug purposes
};
}