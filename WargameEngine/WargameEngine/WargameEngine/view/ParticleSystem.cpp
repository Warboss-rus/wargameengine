#include "ParticleSystem.h"
#include "gl.h"

void CParticleSystem::DrawParticles() 
{
	for (unsigned int i = 0; i < m_effects.size(); ++i)
	{
		while (m_effects[i].GetTime() > m_effects[i].GetLifetime() && m_effects[i].GetLifetime() > 0.0f)
		{
			std::swap(m_effects[i], m_effects.back()); 
			m_effects.pop_back();
			if (i == m_effects.size()) return;
		}
		glPushMatrix();
		CVector3d const & coords = m_effects[i].GetCoords();
		glTranslated(coords.x, coords.y, coords.z);
		glRotated(m_effects[i].GetRotation(), 0.0, 0.0, 1.0);
		m_models[m_effects[i].GetModel()].Draw(m_effects[i].GetTime());
		glPopMatrix();
	}
}

void CParticleSystem::AddEffect(std::string const& file, double x, double y, double z, double rotation, double scale, float lifetime)
{
	if (m_models.find(file) == m_models.end())
	{
		m_models[file] = CParticleModel(file);
	}
	if (lifetime == 0.0f)
	{
		lifetime = m_models[file].GetDuration();
	}
	m_effects.push_back(CParticleEffect(file, x, y, z, rotation, scale, lifetime));
}

void CParticleSystem::DrawEffect(std::string const& effectFile, float time)
{
	m_models[effectFile].Draw(time);
}