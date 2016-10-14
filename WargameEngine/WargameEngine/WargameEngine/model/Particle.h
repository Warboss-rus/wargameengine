#pragma once
#include <vector>
#include <string>
#include "..\view\Vector3.h"

struct sParticle
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
	std::vector<float> const& GetPositionCache() const;
	std::vector<float> const& GetColorCache() const;
	std::vector<float> const& GetTexCoordCache() const;
private:
	const IParticleUpdater * m_updater;
	size_t m_maxParticles;
	std::wstring m_effectPath;
	std::vector<sParticle> m_particles;
	std::vector<float> m_positionCache;
	std::vector<float> m_texCoordCache;
	std::vector<float> m_colorCache;
	CVector3f m_center;
	float m_scale;
};