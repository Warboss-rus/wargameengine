#include "CommandDeleteObject.h"
#include "..\model\GameModel.h"

CCommandDeleteObject::CCommandDeleteObject(std::shared_ptr<IObject> object):
	m_pObject(object)
{
}

void CCommandDeleteObject::Execute()
{
	CGameModel::GetIntanse().lock()->DeleteObjectByPtr(m_pObject);
}

void CCommandDeleteObject::Rollback()
{
	CGameModel::GetIntanse().lock()->AddObject(m_pObject);
}