#include "CommandGoTo.h"
#include "../model/ObjectInterface.h"

CCommandGoTo::CCommandGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed)
	:m_object(object), m_x(x), m_y(y), m_speed(speed), m_animation(animation), m_animationSpeed(animationSpeed)
{
	m_oldCoords = object->GetCoords();
}

void CCommandGoTo::Execute()
{
	m_object->GoTo(CVector3d(m_x, m_y, 0.0), m_speed, m_animation, m_animationSpeed);
}

void CCommandGoTo::Rollback()
{
	m_object->SetCoords(m_oldCoords);
}

std::vector<char> CCommandGoTo::Serialize() const
{
	std::vector<char> result;
	result.resize(1 + 4 + 8 + 8 + 8 + 4 + 4 + m_animation.size());
	result[0] = 7;//its a goto command
	void* address = m_object.get();
	memcpy(&result[1], &address, 4);
	memcpy(&result[5], &m_x, sizeof(double));
	memcpy(&result[13], &m_y, sizeof(double));
	memcpy(&result[21], &m_speed, sizeof(double));
	memcpy(&result[29], &m_animationSpeed, sizeof(float));
	unsigned int size = m_animation.size();
	memcpy(&result[33], &size, sizeof(unsigned int));
	memcpy(&result[37], m_animation.c_str(), m_animation.size());
	return result;
}
