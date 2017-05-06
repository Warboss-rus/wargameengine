#include "CommandCreateObject.h"
#include "../model/IObject.h"
#include "../model/IGameModel.h"
#include "../IMemoryStream.h"
#include "../model/Object.h"
#include "../Utils.h"

CCommandCreateObject::CCommandCreateObject(std::shared_ptr<IObject> const& object, IGameModel& model)
	:m_pObject(object), m_model(model) 
{
}

CCommandCreateObject::CCommandCreateObject(IReadMemoryStream & stream, IGameModel& model)
	: m_model(model)
{
	stream.ReadPointer();//skip pointer
	float x = stream.ReadFloat();
	float y = stream.ReadFloat();
	float z = stream.ReadFloat();
	float rotation = stream.ReadFloat();
	std::string path = stream.ReadString();
	bool shadow = stream.ReadBool();
	m_pObject = std::make_shared<CObject>(make_path(path), CVector3f{ x, y, z }, rotation, shadow);
}

void CCommandCreateObject::Execute()
{
	m_model.AddObject(m_pObject);
}

void CCommandCreateObject::Rollback()
{
	m_model.DeleteObjectByPtr(m_pObject);
}

void CCommandCreateObject::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(0);//This is a CCommandCreateObject action
	stream.WritePointer(m_pObject.get());
	stream.WriteFloat(m_pObject->GetX());
	stream.WriteFloat(m_pObject->GetY());
	stream.WriteFloat(m_pObject->GetZ());
	stream.WriteFloat(m_pObject->GetRotation());
	stream.WriteString(to_string(m_pObject->GetPathToModel()));
	stream.WriteBool(m_pObject->CastsShadow());
}