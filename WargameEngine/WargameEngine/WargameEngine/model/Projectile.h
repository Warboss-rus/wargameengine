#pragma once
#include "ObjectStatic.h"
#include <functional>

class CProjectile : public CStaticObject
{
public:
	CProjectile(CVector3d const& origin, CVector3d & target, double speed, std::string const& model, std::string const& particleFile, std::function<void()> const& onHit, std::function<void()> const& onCollision);
	bool Update();
	const std::string GetParticle() const;
	void CallOnCollision() const;
	float GetTime() const;
private:
	CVector3d m_target;
	double m_speed;
	std::string m_particle;
	long long m_lastUpdateTime;
	std::function<void()> m_onHit;
	std::function<void()> m_onCollision;
};