#include "CommandMoveObject.h"
#include "../model/GameModel.h"
#include "../IMemoryStream.h"

CCommandMoveObject::CCommandMoveObject(std::shared_ptr<IObject> object, float deltaX, float deltaY) :
	m_pObject(object), m_deltaX(deltaX), m_deltaY(deltaY)
{
}

CCommandMoveObject::CCommandMoveObject(IReadMemoryStream & stream, IGameModel& model)
{
	m_pObject = model.Get3DObject(reinterpret_cast<IObject*>(stream.ReadPointer()));
	m_deltaX = stream.ReadFloat();
	m_deltaY = stream.ReadFloat();
}

void CCommandMoveObject::Execute()
{
	m_pObject->Move(m_deltaX, m_deltaY, 0);
}

void CCommandMoveObject::Rollback()
{
	m_pObject->Move(-m_deltaX,-m_deltaY, 0);
}

void CCommandMoveObject::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(2);//This is a CCommandMoveObject action
	stream.WritePointer(m_pObject.get());
	stream.WriteFloat(m_deltaX);
	stream.WriteFloat(m_deltaY);
}
