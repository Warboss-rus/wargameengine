#pragma once
#include "ObjectStatic.h"
#include <functional>

class CProjectile : public CStaticObject
{
public:
	CProjectile(CVector3d const& origin, CVector3d & target, double speed, std::wstring const& model, std::wstring const& particleFile, std::function<void()> const& onHit, std::function<void()> const& onCollision);
	bool Update();
	const std::wstring GetParticle() const;
	void CallOnCollision() const;
	float GetTime() const;
private:
	CVector3d m_target;
	double m_speed;
	std::wstring m_particle;
	long long m_lastUpdateTime;
	std::function<void()> m_onHit;
	std::function<void()> m_onCollision;
};