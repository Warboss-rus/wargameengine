#include "CommandCreateObject.h"
#include "../model/ObjectInterface.h"
#include "../model/IGameModel.h"
#include "../IMemoryStream.h"
#include "../model/Object.h"

CCommandCreateObject::CCommandCreateObject(std::shared_ptr<IObject> object, IGameModel& model)
	:m_pObject(object), m_model(model) 
{
}

CCommandCreateObject::CCommandCreateObject(IReadMemoryStream & stream, IGameModel& model)
	: m_model(model)
{
	stream.ReadPointer();//skip pointer
	double x = stream.ReadDouble();
	double y = stream.ReadDouble();
	double z = stream.ReadDouble();
	double rotation = stream.ReadDouble();
	std::string path = stream.ReadString();
	bool shadow = stream.ReadBool();
	m_pObject = std::make_shared<CObject>(path, x, y, z, rotation, shadow);
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
	stream.WriteDouble(m_pObject->GetX());
	stream.WriteDouble(m_pObject->GetY());
	stream.WriteDouble(m_pObject->GetZ());
	stream.WriteDouble(m_pObject->GetRotation());
	stream.WriteString(m_pObject->GetPathToModel());
	stream.WriteBool(m_pObject->CastsShadow());
}