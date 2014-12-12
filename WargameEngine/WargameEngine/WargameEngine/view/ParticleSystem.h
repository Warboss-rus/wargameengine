#include <map>
#include <vector>
#include "ParticleEffect.h"
#include "ParticleModel.h"

class CParticleSystem
{
public:
	void DrawParticles();
	//World effects
	void AddEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime);
	//For effects inside models and projectiles
	void DrawEffect(std::string const& effect, float time);
private:
	std::map<std::string, CParticleModel> m_models;
	std::vector<CParticleEffect> m_effects;
};