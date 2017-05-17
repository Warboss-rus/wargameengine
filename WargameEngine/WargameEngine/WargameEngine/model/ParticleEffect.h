#pragma once
#include <vector>
#include <string>
#include "../view/Vector3.h"
#include "../Typedefs.h"
#include <chrono>

namespace wargameEngine
{
namespace model
{
struct Particle
{
	float* m_position;
	float* m_color;
	float* m_scale;
	float* m_texCoord;
	float m_lifeTime;
	float m_age;
	CVector3f m_velocity;
};

class IParticleUpdater
{
public:
	virtual ~IParticleUpdater() {}
	virtual float GetAverageLifeTime() const = 0;
	virtual void InitParticle(Particle & particle) const = 0;
	virtual void UpdateParticles(std::vector<Particle> & particles) const = 0;
};

class ParticleEffect
{
public:
	ParticleEffect(const IParticleUpdater * updater, const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	std::vector<Particle> const& GetParticles() const;
	CVector3f GetPosition() const;
	float GetScale() const;
	void Update(std::chrono::microseconds deltaTime);
	Path GetEffectPath() const;
	std::vector<float> const& GetPositionCache() const;
	std::vector<float> const& GetColorCache() const;
	std::vector<float> const& GetTexCoordCache() const;
private:
	const IParticleUpdater * m_updater;
	size_t m_maxParticles;
	Path m_effectPath;
	std::vector<Particle> m_particles;
	std::vector<float> m_positionCache;
	std::vector<float> m_texCoordCache;
	std::vector<float> m_colorCache;
	CVector3f m_center;
	float m_scale;
};
}
}