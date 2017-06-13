#include "CommandDeleteObject.h"
#include "../model/IModel.h"
#include "../IMemoryStream.h"

namespace wargameEngine
{
namespace controller
{

CCommandDeleteObject::CCommandDeleteObject(std::shared_ptr<model::IObject> object, model::IModel& model)
	:m_pObject(object), m_model(model)
{
}

CCommandDeleteObject::CCommandDeleteObject(IReadMemoryStream & stream, model::IModel& model)
	: m_model(model)
{
	m_pObject = m_model.Get3DObject(reinterpret_cast<model::IBaseObject*>(stream.ReadPointer()));
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

}
}