#pragma once
#include <string>
#include "IParticleEffect.h"

class CParticleTracer: public IParticleEffect
{
public:
	CParticleTracer(std::wstring const& file, CVector3d const& start, CVector3d const& end, double rotation, double scale, double speed);
	std::wstring const& GetModel() const;
	CVector3d const& GetCoords();
	double GetRotation() const;
	double GetScale() const;
	float GetTime() const;
	bool IsEnded() const;
private:
	std::wstring m_file;
	CVector3d m_coords;
	CVector3d m_end;
	double m_speed;
	double m_rotation;
	double m_scale;
	long long m_prevTime;
	long long m_beginTime;
};
