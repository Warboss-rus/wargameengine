#include "CommandDeleteObject.h"
#include "../model/GameModel.h"
#include "../Network.h"

CCommandDeleteObject::CCommandDeleteObject(std::shared_ptr<IObject> object):
	m_pObject(object)
{
}

void CCommandDeleteObject::Execute()
{
	CGameModel::GetInstance().lock()->DeleteObjectByPtr(m_pObject);
}

void CCommandDeleteObject::Rollback()
{
	CGameModel::GetInstance().lock()->AddObject(m_pObject);
}

std::vector<char> CCommandDeleteObject::Serialize() const
{
	std::vector<char> result;
	result.resize(5);
	result[0] = 1;//This is a CCommandDeleteObject action
	unsigned int address = CNetwork::GetInstance().lock()->GetAddress(m_pObject);
	memcpy(&result[1], &address, 4);
	return result;
}