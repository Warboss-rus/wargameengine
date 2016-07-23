#include "Projectile.h"
#include "..\Utils.h"

CProjectile::CProjectile(CVector3d const& origin, CVector3d & target, double speed, std::wstring const& model, std::wstring const& particleFile, std::function<void()> const& onHit, std::function<void()> const& onCollision)
	:CStaticObject(model, origin.x, origin.y, 0.0, model.empty()), m_target(target), m_speed(speed), m_particle(particleFile), m_onHit(onHit), m_onCollision(onCollision)
{
	m_lastUpdateTime = GetCurrentTimeLL();
}

bool CProjectile::Update()
{
	long long current = GetCurrentTimeLL();
	CVector3d dir = m_target - m_coords;
	dir.Normalize();
	dir = dir * static_cast<double>(current - m_lastUpdateTime) / 1000.0f * m_speed;
	if (dir.GetLength() > (m_target - m_coords).GetLength()) dir = (m_target - m_coords);
	m_coords += dir;
	m_lastUpdateTime = current;
	if ((m_coords - m_target).GetLength() < 0.0001)
	{
		m_speed = 0.0f;
		if (m_onHit) m_onHit();
		return true;//Particle needs to be deleted
	}
	return false;//Particle is not yet finished
}

const std::wstring CProjectile::GetParticle() const
{ 
	return m_particle; 

}
void  CProjectile::CallOnCollision() const
{ 
	m_onCollision(); 
}

float CProjectile::GetTime() const
{ 
	return static_cast<float>((double)m_lastUpdateTime / 1000.0); 
}