#include "Projectile.h"

CProjectile::CProjectile(CVector3f const& origin, CVector3f & target, float speed, std::wstring const& model, CParticleEffect * particleEffect, std::function<void()> const& onHit, std::function<void()> const& onCollision)
	: CStaticObject(model, origin, 0.0f, model.empty()), m_target(target), m_speed(speed), m_particle(particleEffect), m_onHit(onHit), m_onCollision(onCollision)
{
}

bool CProjectile::Update(long long timeSinceLastUpdate)
{
	CVector3f dir = m_target - GetCoords();
	dir.Normalize();
	dir = dir * static_cast<float>(timeSinceLastUpdate) / 1000.0f * m_speed;
	if (dir.GetLength() > (m_target - GetCoords()).GetLength()) dir = (m_target - GetCoords());
	SetCoords(GetCoords() + dir);
	m_time += timeSinceLastUpdate;
	if ((GetCoords() - m_target).GetLength() < 0.0001f)
	{
		m_speed = 0.0f;
		if (m_onHit) m_onHit();
		return true;//Particle needs to be deleted
	}
	return false;//Particle is not yet finished
}

const CParticleEffect* CProjectile::GetParticle() const
{ 
	return m_particle.get(); 

}
void  CProjectile::CallOnCollision() const
{ 
	m_onCollision(); 
}

float CProjectile::GetTime() const
{ 
	return static_cast<float>((double)m_time / 1000.0); 
}