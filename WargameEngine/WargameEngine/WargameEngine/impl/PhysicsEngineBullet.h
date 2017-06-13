#pragma once
#include "../IPhysicsEngine.h"
#include <memory>

class CPhysicsEngineBullet : public wargameEngine::IPhysicsEngine
{
public:
	using IObject = wargameEngine::model::IObject;
	using IBaseObject = wargameEngine::model::IBaseObject;

	CPhysicsEngineBullet();
	~CPhysicsEngineBullet();

	void Update(std::chrono::microseconds deltaTime) override;
	void Reset(wargameEngine::model::IBoundingBoxManager& boundingManager) override;
	void AddDynamicObject(IObject* object, double mass) override;
	void AddStaticObject(IBaseObject* staticObject) override;
	void RemoveObject(IBaseObject* object) override;
	void SetGround(wargameEngine::model::Landscape* landscape) override;
	CastRayResult CastRay(CVector3f const& origin, CVector3f const& dest, std::vector<IBaseObject*> const& excludeObjects = std::vector<IBaseObject*>()) const override;
	bool TestObject(IBaseObject* object) const override;
	void Draw(wargameEngine::view::IRenderer& renderer) const override;

private:
	struct Impl;
	std::unique_ptr<Impl> m_pImpl;
};
