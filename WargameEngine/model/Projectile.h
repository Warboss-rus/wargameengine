#pragma once
#include "BaseObject.h"
#include <functional>
#include <memory>
#include "ParticleEffect.h"

namespace wargameEngine
{
namespace model
{
class Projectile : public StaticObject
{
public:
	Projectile(CVector3f const& origin, CVector3f & target, float speed, const Path& model, ParticleEffect* particleEffect, std::function<void()> const& onHit, std::function<void()> const& onCollision);
	bool Update(std::chrono::microseconds timeSinceLastUpdate);
	const ParticleEffect* GetParticle() const;
	void CallOnCollision() const;
	float GetTime() const;
private:
	CVector3f m_target;
	float m_speed;
	std::shared_ptr<ParticleEffect> m_particle;
	std::chrono::microseconds m_time;
	std::function<void()> m_onHit;
	std::function<void()> m_onCollision;
};
}
}