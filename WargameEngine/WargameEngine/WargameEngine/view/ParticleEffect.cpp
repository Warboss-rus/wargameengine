#include "ParticleEffect.h"
#include <sys\timeb.h> 

CParticleEffect::CParticleEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime)
	:m_file(file), m_coords(x, y, z), m_rotation(rotation), m_scale(scale), m_lifetime(lifetime)
{
	struct timeb time;
	ftime(&time);
	m_beginTime = 1000 * time.time + time.millitm;
}

CVector3d const& CParticleEffect::GetCoords() const 
{ 
	return m_coords; 
}

double CParticleEffect::GetRotation() const 
{ 
	return m_rotation; 
}

double CParticleEffect::GetScale() const 
{ 
	return m_scale; 
}

float CParticleEffect::GetTime() const
{
	if (m_beginTime == 0L) return 0.0f;
	struct timeb time;
	ftime(&time);
	long delta = 1000 * time.time + time.millitm - m_beginTime;
	return (double)delta / 1000.0f;
}

float CParticleEffect::GetLifetime() const
{
	return m_lifetime;
}

std::string const& CParticleEffect::GetModel() const
{
	return m_file;
}