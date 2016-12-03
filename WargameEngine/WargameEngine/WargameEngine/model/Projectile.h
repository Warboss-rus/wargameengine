#pragma once
#include "ObjectStatic.h"
#include <functional>
#include <memory>
#include "Particle.h"

class CProjectile : public CStaticObject
{
public:
	CProjectile(CVector3f const& origin, CVector3f & target, float speed, std::wstring const& model, CParticleEffect* particleEffect, std::function<void()> const& onHit, std::function<void()> const& onCollision);
	bool Update(long long timeSinceLastUpdate);
	const CParticleEffect* GetParticle() const;
	void CallOnCollision() const;
	float GetTime() const;
private:
	CVector3f m_target;
	float m_speed;
	std::shared_ptr<CParticleEffect> m_particle;
	long long m_time = 0LL;
	std::function<void()> m_onHit;
	std::function<void()> m_onCollision;
};