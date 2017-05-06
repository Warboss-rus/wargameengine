#include "CommandGoTo.h"
#include "../model/IObject.h"
#include "../IMemoryStream.h"
#include "../model/IGameModel.h"
#include "GameController.h"

CCommandGoTo::CCommandGoTo(std::shared_ptr<CObjectDecorator> object, CVector3f const& target, float speed, std::string const& animation, float animationSpeed)
	:m_object(object), m_target(target), m_speed(speed), m_animation(animation), m_animationSpeed(animationSpeed)
{
	m_oldCoords = object->GetObject()->GetCoords();
}


CCommandGoTo::CCommandGoTo(IReadMemoryStream & stream, IGameModel & model, CGameController& controller)
{
	auto objectPtr = reinterpret_cast<IObject*>(stream.ReadPointer());
	m_object = controller.GetDecorator(model.Get3DObject(objectPtr));
	float x = stream.ReadFloat();
	float y = stream.ReadFloat();
	m_target = { x, y, 0.0f };
	m_speed = stream.ReadFloat();
	m_animationSpeed = stream.ReadFloat();
	m_animation = stream.ReadString();
}

void CCommandGoTo::Execute()
{
	m_object->GoTo(m_target, m_speed, m_animation, m_animationSpeed);
}

void CCommandGoTo::Rollback()
{
	m_object->GetObject()->SetCoords(m_oldCoords);
}

void CCommandGoTo::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(7);//its a goto command
	stream.WritePointer(m_object.get());
	stream.WriteFloat(m_target.x);
	stream.WriteFloat(m_target.y);
	stream.WriteFloat(m_speed);
	stream.WriteFloat(m_animationSpeed);
	stream.WriteString(m_animation);
}
