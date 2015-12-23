#include "CommandCreateObject.h"
#include "../model/ObjectInterface.h"
#include "../model/IGameModel.h"

CCommandCreateObject::CCommandCreateObject(std::shared_ptr<IObject> object, IGameModel& model):m_pObject(object), m_model(model) {}

void CCommandCreateObject::Execute()
{
	m_model.AddObject(m_pObject);
}

void CCommandCreateObject::Rollback()
{
	m_model.DeleteObjectByPtr(m_pObject);
}

std::vector<char> CCommandCreateObject::Serialize() const
{
	std::string path = m_pObject->GetPathToModel();
	std::vector<char> result;
	result.resize(path.size() + 33);
	result[0] = 0;//This is a CCommandCreateObject action
	IObject* address = m_pObject.get();
	memcpy(&result[1], &address, 4);
	double pos = m_pObject->GetX();
	memcpy(&result[5], &pos, 8);
	pos = m_pObject->GetY();
	memcpy(&result[13], &pos, 8);
	pos = m_pObject->GetRotation();
	memcpy(&result[21], &pos, 8);
	unsigned int size = path.size();
	memcpy(&result[29], &size, 4);
	memcpy(&result[33], &path[0], size);
	return result;
}