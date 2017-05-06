#pragma once
#include "BaseObject.h"
#include <functional>
#include <memory>
#include "Particle.h"

class CProjectile : public StaticObject
{
public:
	CProjectile(CVector3f const& origin, CVector3f & target, float speed, const Path& model, CParticleEffect* particleEffect, std::function<void()> const& onHit, std::function<void()> const& onCollision);
	bool Update(std::chrono::microseconds timeSinceLastUpdate);
	const CParticleEffect* GetParticle() const;
	void CallOnCollision() const;
	float GetTime() const;
private:
	CVector3f m_target;
	float m_speed;
	std::shared_ptr<CParticleEffect> m_particle;
	std::chrono::microseconds m_time;
	std::function<void()> m_onHit;
	std::function<void()> m_onCollision;
};