#include "CommandRotateObject.h"
#include "../model/GameModel.h"
#include "../Network.h"

CCommandRotateObject::CCommandRotateObject(std::shared_ptr<IObject> object, double deltaRotation) :
	m_deltaRotation(deltaRotation), m_pObject(object)
{
}

void CCommandRotateObject::Execute()
{
	m_pObject->Rotate(m_deltaRotation);
}

void CCommandRotateObject::Rollback()
{
	m_pObject->Rotate(-m_deltaRotation);
}

std::vector<char> CCommandRotateObject::Serialize() const
{
	std::vector<char> result;
	result.resize(13);
	result[0] = 3;//This is a CCommandRotateObject action
	unsigned int address = CNetwork::GetInstance().lock()->GetAddress(m_pObject);
	memcpy(&result[1], &address, 4);
	memcpy(&result[5], &m_deltaRotation, 8);
	return result;
}