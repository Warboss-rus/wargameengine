#include "CommandPlayAnimation.h"
#include "../model/IObject.h"
#include "../IMemoryStream.h"
#include "../model/IModel.h"

namespace wargameEngine
{
namespace controller
{

CCommandPlayAnimation::CCommandPlayAnimation(std::shared_ptr<model::IObject> object, std::string const& animation, model::AnimationLoop loopMode, float speed)
	:m_object(object), m_animation(animation), m_loopMode(loopMode), m_speed(speed)
{
}


CCommandPlayAnimation::CCommandPlayAnimation(IReadMemoryStream & stream, model::IModel &model)
{
	m_object = model.Get3DObject(reinterpret_cast<model::IObject*>(stream.ReadPointer()));
	m_loopMode = static_cast<model::AnimationLoop>(stream.ReadByte());
	m_speed = stream.ReadFloat();
	m_animation = stream.ReadString();
}

void CCommandPlayAnimation::Execute()
{
	m_object->PlayAnimation(m_animation, model::AnimationLoop(m_loopMode), m_speed);
}

void CCommandPlayAnimation::Rollback()
{
}

void CCommandPlayAnimation::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(6);//its a play animation
	stream.WritePointer(m_object.get());
	stream.WriteByte(static_cast<unsigned char>(m_loopMode));
	stream.WriteFloat(m_speed);
	stream.WriteString(m_animation);
}

}
}