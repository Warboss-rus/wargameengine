#pragma once
#include <memory>
#include "../IPhysicsEngine.h"

class CPhysicsEngineBullet : public IPhysicsEngine
{
public:
	CPhysicsEngineBullet();
	~CPhysicsEngineBullet();
	virtual void Update(long long deltaTime) override;
	virtual void Reset() override;
	virtual void AddDynamicObject(IObject * object, double mass) override;
	virtual void AddStaticObject(IBaseObject * staticObject) override;
	virtual void RemoveDynamicObject(IObject * object) override;
	virtual void SetGround(CLandscape * landscape) override;
	virtual bool CastRay(CVector3f const& origin, CVector3f const& dest, IObject ** obj, CVector3f & hitPoint, std::vector<IObject*> const& excludeObjects = std::vector<IObject*>()) const override;
	virtual bool TestObject(IObject * object) const override;
	virtual void Draw() const override;
	virtual void EnableDebugDraw(IRenderer & renderer) override;
	//IBoundingBoxManager
	virtual void AddBounding(std::wstring const& modelName, sBounding const& bounding) override;
	virtual sBounding GetBounding(std::wstring const& path) const override;
private:
	struct Impl;
	std::unique_ptr<Impl> m_pImpl;
};
