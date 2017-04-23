#include "Particle.h"

CParticleEffect::CParticleEffect(const IParticleUpdater* updater, const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles)
	: m_updater(updater), m_maxParticles(maxParticles), m_effectPath(effectPath), m_center(position), m_scale(scale)
{
#ifdef _DEBUG
	m_maxParticles = m_maxParticles > 1000 ? 1000 : m_maxParticles;
#endif
	m_particles.resize(m_maxParticles);
	m_positionCache.resize(m_maxParticles * 4);
	m_colorCache.resize(m_maxParticles * 4);
	m_texCoordCache.resize(m_maxParticles * 2);
	for (size_t i = 0; i < m_maxParticles; ++i)
	{
		m_particles[i].m_position = m_positionCache.data() + i * 4;
		m_particles[i].m_color = m_colorCache.data() + i * 4;
		m_particles[i].m_scale = m_positionCache.data() + i * 4 + 3;
		m_particles[i].m_texCoord = m_texCoordCache.data() + i * 2;
		m_particles[i].m_age = 1.0f;
		m_particles[i].m_lifeTime = 0.0f;
	}
}

std::vector<sParticle> const& CParticleEffect::GetParticles() const
{
	return m_particles;
}

CVector3f CParticleEffect::GetPosition() const
{
	return m_center;
}

float CParticleEffect::GetScale() const
{
	return m_scale;
}

void CParticleEffect::Update(std::chrono::microseconds deltaTime)
{
	float ftime = std::chrono::duration<float>(deltaTime).count();
	size_t particlesEmmitted = 0;
	for (auto& particle : m_particles)
	{
		particle.m_age += ftime;
		if (m_updater)
		{
			if (particle.m_age > particle.m_lifeTime)
			{
				particle.m_age = 0.0f;
				m_updater->InitParticle(particle);
				particlesEmmitted++;
			}
		}
		for (int i = 0; i < 3; ++i)
		{
			particle.m_position[i] += particle.m_velocity[i] * ftime;
		}
	}
	if (m_updater)
	{
		m_updater->UpdateParticles(m_particles);
	}
	/*if (m_updater)
	{
		size_t newParticles = max<size_t>(static_cast<size_t>(m_particles.size() * ftime / m_updater->GetAverageLifeTime()), 0u);
		size_t oldSize = m_particles.size();
		m_particles.resize(oldSize + newParticles);
		for (size_t i = oldSize; i < m_particles.size(); ++i)
		{
			m_updater->InitParticle(m_particles[i]);
		}
	}*/
}

Path CParticleEffect::GetEffectPath() const
{
	return m_effectPath;
}

std::vector<float> const& CParticleEffect::GetPositionCache() const
{
	return m_positionCache;
}

std::vector<float> const& CParticleEffect::GetColorCache() const
{
	return m_colorCache;
}

std::vector<float> const& CParticleEffect::GetTexCoordCache() const
{
	return m_texCoordCache;
}
