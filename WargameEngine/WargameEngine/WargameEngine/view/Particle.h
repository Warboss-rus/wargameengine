#include <vector>
#include "Vector3.h"
#include <string.h>

class CParticle
{
public:
	CParticle(std::vector<float> const& keyframes, std::vector<float> const& positions, unsigned int material, float width, float height);
	~CParticle();
	std::vector<float> const& GetKeyFrames() const;
	std::vector<CVector3f> const& GetPositions() const;
	unsigned int GetMaterial() const;
	float GetWidth() const;
	float GetHeight() const;
private:
	std::vector<float> m_keyframes;
	std::vector<CVector3f> m_positions;
	unsigned int m_material;
	float m_width;
	float m_height;
};