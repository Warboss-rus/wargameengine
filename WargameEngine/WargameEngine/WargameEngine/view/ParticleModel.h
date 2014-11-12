#include <string>
#include "Particle.h"
#include "ShaderManager.h"

struct sParticleInstance {
	CVector3d position;
	unsigned int particle;
	double rotation;
	double speed;
	double scale;
	float start;
};

class CParticleModel
{
public:
	CParticleModel(std::string const& file);
	CParticleModel() {}
	void Draw(float time) const;
	float GetDuration() const;
private:
	std::vector<sParticleInstance> m_instances;
	std::vector<CParticle> m_particles;
	std::vector<std::string> m_textures;
	std::vector<CShaderManager> m_shaders;
	float m_duration;
};