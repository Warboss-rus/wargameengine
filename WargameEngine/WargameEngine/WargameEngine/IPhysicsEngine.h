#pragma once
#include "view\Vector3.h"
#include "model\IBoundingBoxManager.h"

class IObject;
class CStaticObject;
class CLandscape;
class IRenderer;

class IPhysicsEngine : public IBoundingBoxManager
{
public:
	virtual ~IPhysicsEngine() {}
	virtual void Update(long long timeDelta) = 0;
	virtual void Reset() = 0;
	virtual void AddDynamicObject(IObject * object, double mass) = 0;
	virtual void AddStaticObject(CStaticObject * staticObject) = 0;
	virtual void RemoveDynamicObject(IObject * object) = 0;
	virtual void SetGround(CLandscape * landscape) = 0;
	virtual bool CastRay(CVector3d const& origin, CVector3d const& dest, IObject ** obj, CVector3d & hitPoint, std::vector<IObject*> const& excludeObjects = std::vector<IObject*>()) const = 0;
	virtual bool TestObject(IObject * object) const = 0;
	virtual void AddBounding(std::wstring const& modelName, sBounding const& bounding) = 0;
	virtual void Draw() const = 0;//for debug purposes
	virtual void EnableDebugDraw(IRenderer & renderer) = 0;
};