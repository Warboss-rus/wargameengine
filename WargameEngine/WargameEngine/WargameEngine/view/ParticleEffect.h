#include <string>
#include "Vector3.h"

class CParticleEffect
{
public:
	CParticleEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime);
	std::string const& GetModel() const;
	CVector3d const& GetCoords() const;
	double GetRotation() const;
	double GetScale() const;
	float GetTime() const;
	float GetLifetime() const;
private:
	std::string m_file;
	CVector3d m_coords;
	double m_rotation;
	double m_scale;
	long m_beginTime;
	float m_lifetime;
};