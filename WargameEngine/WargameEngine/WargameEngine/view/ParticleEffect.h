#include "IParticleEffecth.h"

class CParticleEffect: public IParticleEffect
{
public:
	CParticleEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime);
	std::string const& GetModel() const;
	CVector3d const& GetCoords();
	double GetRotation() const;
	double GetScale() const;
	float GetTime() const;
	bool IsEnded() const;
private:
	std::string m_file;
	CVector3d m_coords;
	double m_rotation;
	double m_scale;
	long long m_beginTime;
	float m_lifetime;
};