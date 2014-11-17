#pragma once
#include "ObjectStatic.h"
#include <functional>
#define callback(x) std::function<void()>(x)

class CProjectile : public CStaticObject
{
public:
	CProjectile(CVector3d const& origin, CVector3d & target, double speed, std::string const& model, std::string const& particleFile, callback(onHit), callback(onCollision));
	bool Update();
	const std::string GetParticle() const { return m_particle; }
	void CallOnCollision() const { m_onCollision(); }
private:
	CVector3d m_target;
	double m_speed;
	std::string m_particle;
	long m_lastUpdateTime;
	callback(m_onHit);
	callback(m_onCollision);
};