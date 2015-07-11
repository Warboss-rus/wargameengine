#include "Particle.h"

using namespace std;

CParticle::CParticle(vector<float> const& keyframes, vector<float> const& positions, unsigned int material, float width, float height) :m_material(material), m_width(width), m_height(height), m_keyframes(keyframes)
{
	m_positions.resize(positions.size() / 3);
	memcpy(m_positions.data(), positions.data(), sizeof(float) * positions.size());
}

CParticle::~CParticle()
{

}

vector<float> const& CParticle::GetKeyFrames() const
{
	return m_keyframes;
}

vector<CVector3f> const& CParticle::GetPositions() const
{
	return m_positions;
}

unsigned int CParticle::GetMaterial() const
{
	return m_material;
}

float CParticle::GetWidth() const
{
	return m_width;
}

float CParticle::GetHeight() const
{
	return m_height;
}
