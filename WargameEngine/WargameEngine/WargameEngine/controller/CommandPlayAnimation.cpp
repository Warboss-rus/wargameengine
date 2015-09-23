#include "CommandPlayAnimation.h"
#include "../model/ObjectInterface.h"
#include "../view/3dModel.h"

CCommandPlayAnimation::CCommandPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, int loopMode, float speed)
	:m_object(object), m_animation(animation), m_loopMode(loopMode), m_speed(speed)
{
}

void CCommandPlayAnimation::Execute()
{
	m_object->PlayAnimation(m_animation, sAnimation::eLoopMode(m_loopMode), m_speed);
}

void CCommandPlayAnimation::Rollback()
{
}

std::vector<char> CCommandPlayAnimation::Serialize() const
{
	std::vector<char> result;
	result.resize(1 + 4 + 1 + 4 + m_animation.size());
	result[0] = 6;//its a play animation
	void* address = m_object.get();
	memcpy(&result[1], &address, 4);
	result[5] = static_cast<char>(m_loopMode);
	memcpy(&result[6], &m_speed, sizeof(float));
	unsigned int size = m_animation.size();
	memcpy(&result[10], &size, sizeof(unsigned int));
	memcpy(&result[14], m_animation.c_str(), m_animation.size());
	return result;
}
