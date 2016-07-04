#include "ParticleEffect.h"
#include <chrono>
namespace
{
	long long GetCurrentTime()
	{
		using namespace std::chrono;
		return time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch().count();
	}
}

CParticleEffect::CParticleEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime)
	:m_file(file), m_coords(x, y, z), m_rotation(rotation), m_scale(scale), m_lifetime(lifetime)
{
	m_beginTime = GetCurrentTime();
}

CVector3d const& CParticleEffect::GetCoords()
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
	long long delta = GetCurrentTime() - m_beginTime;
	return static_cast<float>((double)delta / 1000.0);
}

bool CParticleEffect::IsEnded() const 
{ 
	return m_beginTime > m_lifetime && m_lifetime > 0.0f; 
}

std::string const& CParticleEffect::GetModel() const
{
	return m_file;
}