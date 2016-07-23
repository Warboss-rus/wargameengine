#pragma once
#include <string>
#include <map>
#include "Particle.h"
#include "IShaderManager.h"
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
	CParticleModel(std::wstring const& file, IRenderer & renderer);
	CParticleModel(IRenderer & renderer):m_renderer(renderer) {}
	void Draw(float time) const;
	float GetDuration() const;
private:
	void DrawParticle(CVector3f const& position, float width, float height) const;
	std::vector<sParticleInstance> m_instances;
	std::vector<CParticle> m_particles;
	std::vector<std::wstring> m_textures;
	std::vector<std::unique_ptr<IShaderManager>> m_shaders;
	float m_duration = 0.0f;
	IRenderer & m_renderer;
};