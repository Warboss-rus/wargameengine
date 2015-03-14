#include <vector>
#include "Vector3.h"
#include <string.h>

class CParticle
{
public:
	CParticle(std::vector<float> keyframes, std::vector<float> positions, unsigned int material, float width, float height) 
		:m_material(material), m_width(width), m_height(height)
	{
		m_keyframes.swap(keyframes);
		m_positions.resize(positions.size() / 3);
		memcpy(&m_positions[0].x, &positions[0], sizeof(float) * positions.size());
	}
	std::vector<float> const& GetKeyFrames() const { return m_keyframes; }
	std::vector<CVector3f> const& GetPositions() const { return m_positions; }
	unsigned int GetMaterial() const { return m_material; }
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
private:
	std::vector<float> m_keyframes;
	std::vector<CVector3f> m_positions;
	unsigned int m_material;
	float m_width;
	float m_height;
};