#pragma once
#include <map>
#include <vector>
#include <memory>
#include "ParticleModel.h"
#include "IParticleEffect.h"
#include "IRenderer.h"

class CParticleSystem
{
public:
	CParticleSystem(IRenderer & renderer);
	void DrawParticles();
	//World effects
	void AddEffect(std::wstring const& file, double x, double y, double z, double rotation, double scale, float lifetime);
	//moving effect
	void AddTracer(std::wstring const& file, CVector3d const& start, CVector3d const& end, double rotation, double scale, double speed);
	//For effects inside models and projectiles
	void DrawEffect(std::wstring const& effect, float time);
private:
	std::map<std::wstring, CParticleModel> m_models;
	std::vector<std::unique_ptr<IParticleEffect>> m_effects;
	IRenderer & m_renderer;
};