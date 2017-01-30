#include "CommandDeleteObject.h"
#include "../model/IGameModel.h"
#include "../IMemoryStream.h"

CCommandDeleteObject::CCommandDeleteObject(std::shared_ptr<IObject> object, IGameModel& model)
	:m_pObject(object), m_model(model)
{
}

CCommandDeleteObject::CCommandDeleteObject(IReadMemoryStream & stream, IGameModel& model)
	:m_model(model)
{
	m_pObject = m_model.Get3DObject(reinterpret_cast<IBaseObject*>(stream.ReadPointer()));
}

void CCommandDeleteObject::Execute()
{
	m_model.DeleteObjectByPtr(m_pObject);
}

void CCommandDeleteObject::Rollback()
{
	m_model.AddObject(m_pObject);
}

void CCommandDeleteObject::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(1);//This is a CCommandDeleteObject action
	stream.WritePointer(m_pObject.get());
}