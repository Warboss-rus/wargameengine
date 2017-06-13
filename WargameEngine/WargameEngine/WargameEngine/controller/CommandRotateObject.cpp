#include "CommandRotateObject.h"
#include "../model/Model.h"
#include "../IMemoryStream.h"

namespace wargameEngine
{
namespace controller
{

CCommandRotateObject::CCommandRotateObject(std::shared_ptr<model::IObject> object, float deltaRotation) :
	m_pObject(object), m_deltaRotation(deltaRotation)
{
}

CCommandRotateObject::CCommandRotateObject(IReadMemoryStream & stream, model::IModel& model)
{
	m_pObject = model.Get3DObject(reinterpret_cast<model::IObject*>(stream.ReadPointer()));
	m_deltaRotation = stream.ReadFloat();
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
	stream.WriteFloat(m_deltaRotation);
}

}
}