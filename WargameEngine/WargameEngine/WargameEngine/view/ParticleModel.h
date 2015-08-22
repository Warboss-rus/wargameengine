#include <string>
#include <map>
#include "Particle.h"
#include "ShaderManager.h"
#include "IRenderer.h"

struct sParticleInstance {
	CVector3d position;
	unsigned int particle;
	double rotation;
	double speed;
	double scale;
	float start;
	std::map<std::string, std::vector<float>> uniforms;
};

class CParticleModel
{
public:
	CParticleModel(std::string const& file, IRenderer & renderer);
	CParticleModel(IRenderer & renderer):m_renderer(renderer) {}
	void Draw(float time) const;
	float GetDuration() const;
private:
	void DrawParticle(CVector3f const& position, float width, float height) const;
	std::vector<sParticleInstance> m_instances;
	std::vector<CParticle> m_particles;
	std::vector<std::string> m_textures;
	std::vector<CShaderManager> m_shaders;
	float m_duration;
	IRenderer & m_renderer;
};