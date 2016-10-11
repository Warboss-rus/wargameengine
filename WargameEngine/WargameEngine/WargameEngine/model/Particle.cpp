#include "Particle.h"

template <class T>
T max(T const& first, T const& second)
{
	return first > second ? first : second;
}

CParticleEffect::CParticleEffect(const IParticleUpdater * updater, std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles)
	: m_updater(updater), m_effectPath(effectPath), m_center(position), m_scale(scale)
{
#ifdef _DEBUG
	maxParticles = maxParticles > 1000 ? 1000 : maxParticles;
#endif
	sParticle deadParticle;
	deadParticle.m_age = 1.0f;
	deadParticle.m_lifeTime = 0.0f;
	m_particles.resize(maxParticles, deadParticle);
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

void CParticleEffect::Update(long long deltaTime)
{
	float ftime = static_cast<float>(static_cast<double>(deltaTime) / 1000.0);
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
			else
			{
				m_updater->UpdateParticle(particle);
			}
		}
		particle.m_position += particle.m_velocity * ftime;
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

std::wstring CParticleEffect::GetEffectPath() const
{
	return m_effectPath;
}
