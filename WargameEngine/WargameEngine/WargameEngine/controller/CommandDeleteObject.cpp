#include "CommandDeleteObject.h"

CCommandDeleteObject::CCommandDeleteObject(std::shared_ptr<IObject> object, IGameModel& model)
	:m_pObject(object), m_model(model)
{
}

void CCommandDeleteObject::Execute()
{
	m_model.DeleteObjectByPtr(m_pObject);
}

void CCommandDeleteObject::Rollback()
{
	m_model.AddObject(m_pObject);
}

std::vector<char> CCommandDeleteObject::Serialize() const
{
	std::vector<char> result;
	result.resize(5);
	result[0] = 1;//This is a CCommandDeleteObject action
	void* address = m_pObject.get();
	memcpy(&result[1], &address, 4);
	return result;
}