#include "CommandGoTo.h"
#include "../model/ObjectInterface.h"
#include "../IMemoryStream.h"
#include "../model/IGameModel.h"

CCommandGoTo::CCommandGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed)
	:m_object(object), m_x(x), m_y(y), m_speed(speed), m_animation(animation), m_animationSpeed(animationSpeed)
{
	m_oldCoords = object->GetCoords();
}


CCommandGoTo::CCommandGoTo(IReadMemoryStream & stream, IGameModel& model)
{
	m_object = model.Get3DObject(reinterpret_cast<IObject*>(stream.ReadPointer()));
	m_x = stream.ReadDouble();
	m_y = stream.ReadDouble();
	m_speed = stream.ReadDouble();
	m_animationSpeed = stream.ReadFloat();
	m_animation = stream.ReadString();
}

void CCommandGoTo::Execute()
{
	m_object->GoTo(CVector3d(m_x, m_y, 0.0), m_speed, m_animation, m_animationSpeed);
}

void CCommandGoTo::Rollback()
{
	m_object->SetCoords(m_oldCoords);
}

void CCommandGoTo::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(7);//its a goto command
	stream.WritePointer(m_object.get());
	stream.WriteDouble(m_x);
	stream.WriteDouble(m_y);
	stream.WriteDouble(m_speed);
	stream.WriteFloat(m_animationSpeed);
	stream.WriteString(m_animation);
}
