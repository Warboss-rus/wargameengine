#include "CommandChangeProperty.h"
#include "../model/IObject.h"
#include "../IMemoryStream.h"
#include "../model/IModel.h"

namespace wargameEngine
{
namespace controller
{

CCommandChangeProperty::CCommandChangeProperty(std::shared_ptr<model::IObject> object, std::wstring const& key, std::wstring const& value) :m_pObject(object), m_key(key),
m_oldValue(m_pObject->GetProperty(key)), m_newValue(value)
{

}

CCommandChangeProperty::CCommandChangeProperty(IReadMemoryStream & stream, model::IModel& model)
{
	m_pObject = model.Get3DObject(reinterpret_cast<model::IObject*>(stream.ReadPointer()));
	m_key = stream.ReadWString();
	m_newValue = stream.ReadWString();
	m_oldValue = stream.ReadWString();
}

void CCommandChangeProperty::Execute()
{
	m_pObject->SetProperty(m_key, m_newValue);
}

void CCommandChangeProperty::Rollback()
{
	m_pObject->SetProperty(m_key, m_oldValue);
}

void CCommandChangeProperty::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(4);//This is a CCommandChangeProperty action
	stream.WritePointer(m_pObject.get());
	stream.WriteWString(m_key);
	stream.WriteWString(m_newValue);
	stream.WriteWString(m_oldValue);
}
}
}