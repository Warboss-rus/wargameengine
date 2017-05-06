#pragma once
#include "view/Vector3.h"
#include "model/IBoundingBoxManager.h"
#include <chrono>

class IObject;
class IBaseObject;
class Landscape;
class IRenderer;

class IPhysicsEngine : public IBoundingBoxManager
{
public:
	virtual ~IPhysicsEngine() {}
	virtual void Update(std::chrono::microseconds timeDelta) = 0;
	virtual void Reset() = 0;
	virtual void AddDynamicObject(IObject * object, double mass) = 0;
	virtual void AddStaticObject(IBaseObject * staticObject) = 0;
	virtual void RemoveDynamicObject(IObject * object) = 0;
	virtual void SetGround(Landscape * landscape) = 0;
	virtual bool CastRay(CVector3f const& origin, CVector3f const& dest, IObject ** obj, CVector3f & hitPoint, std::vector<IObject*> const& excludeObjects = std::vector<IObject*>()) const = 0;
	virtual bool TestObject(IObject * object) const = 0;
	virtual void AddBounding(const Path& modelName, Bounding const& bounding) = 0;
	virtual void Draw() const = 0;//for debug purposes
	virtual void EnableDebugDraw(IRenderer & renderer) = 0;
};