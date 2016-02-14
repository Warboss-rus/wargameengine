#include "CommandChangeProperty.h"
#include "..\model\ObjectInterface.h"
#include "..\IMemoryStream.h"
#include "..\model\IGameModel.h"

CCommandChangeProperty::CCommandChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value) :m_key(key), m_newValue(value), m_pObject(object),
	m_oldValue(m_pObject->GetProperty(key))
{

}

CCommandChangeProperty::CCommandChangeProperty(IReadMemoryStream & stream, IGameModel& model)
{
	m_pObject = model.Get3DObject(reinterpret_cast<IObject*>(stream.ReadPointer()));
	m_key = stream.ReadString();
	m_newValue = stream.ReadString();
	m_oldValue = stream.ReadString();
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
	stream.WriteString(m_key);
	stream.WriteString(m_newValue);
	stream.WriteString(m_oldValue);
}