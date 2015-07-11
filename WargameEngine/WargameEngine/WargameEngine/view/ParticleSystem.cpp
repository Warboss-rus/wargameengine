#include "ParticleSystem.h"
#include "gl.h"
#include "ParticleEffect.h"
#include "ParticleTracer.h"

using namespace std;

void CParticleSystem::DrawParticles() 
{
	for (size_t i = 0; i < m_effects.size(); ++i)
	{
		while (m_effects[i]->IsEnded())
		{
			swap(m_effects[i], m_effects.back()); 
			m_effects.pop_back();
			if (i == m_effects.size()) return;
		}
		glPushMatrix();
		CVector3d const & coords = m_effects[i]->GetCoords();
		glTranslated(coords.x, coords.y, coords.z);
		glRotated(m_effects[i]->GetRotation(), 0.0, 1.0, 0.0);
		m_models[m_effects[i]->GetModel()].Draw(m_effects[i]->GetTime());
		glPopMatrix();
	}
}

void CParticleSystem::AddEffect(string const& file, double x, double y, double z, double rotation, double scale, float lifetime)
{
	if (m_models.find(file) == m_models.end())
	{
		m_models[file] = CParticleModel(file);
	}
	if (lifetime == 0.0f)
	{
		lifetime = m_models[file].GetDuration();
	}
	m_effects.push_back(make_unique<CParticleEffect>(file, x, y, z, rotation, scale, lifetime));
}

void CParticleSystem::AddTracer(string const& file, CVector3d const& start, CVector3d const& end, double rotation, double scale, double speed)
{
	if (m_models.find(file) == m_models.end())
	{
		m_models[file] = CParticleModel(file);
	}
	m_effects.push_back(make_unique<CParticleTracer>(file, start, end, rotation, scale, speed));
}

void CParticleSystem::DrawEffect(string const& effectFile, float time)
{
	m_models[effectFile].Draw(time);
}