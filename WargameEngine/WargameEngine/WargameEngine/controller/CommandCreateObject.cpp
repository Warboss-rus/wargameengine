#include "CommandCreateObject.h"
#include "../model/GameModel.h"
#include "../model/3dObject.h"
#include "../Network.h"

CCommandCreateObject::CCommandCreateObject(std::shared_ptr<IObject> object):m_pObject(object) {}

void CCommandCreateObject::Execute()
{
	//m_pObject.reset(new C3DObject(m_model, m_x, m_y, m_rotation)); 
	CGameModel::GetInstance().lock()->AddObject(m_pObject);
}

void CCommandCreateObject::Rollback()
{
	CGameModel::GetInstance().lock()->DeleteObjectByPtr(m_pObject);
}

std::vector<char> CCommandCreateObject::Serialize() const
{
	std::string path = m_pObject->GetPathToModel();
	std::vector<char> result;
	result.resize(path.size() + 33);
	result[0] = 0;//This is a CCommandCreateObject action
	unsigned int address = CNetwork::GetInstance().lock()->GetAddress(m_pObject);
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