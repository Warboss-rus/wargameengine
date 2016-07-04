#include "CommandRotateObject.h"
#include "../model/GameModel.h"
#include "../IMemoryStream.h"

CCommandRotateObject::CCommandRotateObject(std::shared_ptr<IObject> object, double deltaRotation) :
	m_pObject(object), m_deltaRotation(deltaRotation)
{
}

CCommandRotateObject::CCommandRotateObject(IReadMemoryStream & stream, IGameModel& model)
{
	m_pObject = model.Get3DObject(reinterpret_cast<IObject*>(stream.ReadPointer()));
	m_deltaRotation = stream.ReadDouble();
}

void CCommandRotateObject::Execute()
{
	m_pObject->Rotate(m_deltaRotation);
}

void CCommandRotateObject::Rollback()
{
	m_pObject->Rotate(-m_deltaRotation);
}

void CCommandRotateObject::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(3);//This is a CCommandRotateObject action
	stream.WritePointer(m_pObject.get());
	stream.WriteDouble(m_deltaRotation);
}