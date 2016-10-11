#pragma once
#include <vector>
#include <string>
#include "..\view\Vector3.h"

struct sParticle
{
	float m_lifeTime;
	CVector3f m_position;
	CVector3f m_velocity;
	float m_color[4];
	float m_scale;
	float m_age;
};

class IParticleUpdater
{
public:
	virtual ~IParticleUpdater() {}
	virtual float GetAverageLifeTime() const = 0;
	virtual void InitParticle(sParticle & particle) const = 0;
	virtual void UpdateParticle(sParticle & particle) const = 0;
};

class CParticleEffect
{
public:
	CParticleEffect(const IParticleUpdater * updater, std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	std::vector<sParticle> const& GetParticles() const;
	CVector3f GetPosition() const;
	float GetScale() const;
	void Update(long long deltaTime);
	std::wstring GetEffectPath() const;
private:
	const IParticleUpdater * m_updater;
	std::wstring m_effectPath;
	std::vector<sParticle> m_particles;
	CVector3f m_center;
	float m_scale;
};