#include "ParticleTracer.h"
#include <sys/timeb.h> 

CParticleTracer::CParticleTracer(std::string const& file, CVector3d const& start, CVector3d const& end, double rotation, double scale, double speed)
	:m_file(file), m_coords(start), m_end(end), m_rotation(rotation), m_scale(scale), m_speed(speed)
{
	struct timeb time;
	ftime(&time);
	m_beginTime = 1000 * time.time + time.millitm;
	m_prevTime = m_beginTime;
}

CVector3d const& CParticleTracer::GetCoords()
{
	struct timeb time;
	ftime(&time);
	long long current = 1000ll * time.time + time.millitm;
	CVector3d dir = m_end - m_coords;
	dir.Normalize();
	dir = dir * static_cast<double>(current - m_prevTime) / 1000.0 * m_speed;
	if (dir.GetLength() > (m_end - m_coords).GetLength()) dir = (m_end - m_coords);
	m_coords += dir;
	m_prevTime = current;
	return m_coords;
}

double CParticleTracer::GetRotation() const
{
	return m_rotation;
}

double CParticleTracer::GetScale() const
{
	return m_scale;
}

float CParticleTracer::GetTime() const
{
	if (m_beginTime == 0L) return 0.0f;
	struct timeb time;
	ftime(&time);
	long long delta = 1000ll * time.time + time.millitm - m_beginTime;
	return static_cast<float>((double)delta / 1000.0);
}

bool CParticleTracer::IsEnded() const 
{ 
	return (m_coords - m_end).GetLength() < 0.0001;
}

std::string const& CParticleTracer::GetModel() const
{
	return m_file;
}