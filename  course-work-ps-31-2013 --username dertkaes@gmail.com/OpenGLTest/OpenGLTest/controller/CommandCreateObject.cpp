#include "CommandCreateObject.h"
#include "..\model\GameModel.h"
#include "..\3dObject.h"


CCommandCreateObject::CCommandCreateObject(std::shared_ptr<IObject> object):m_pObject(object) {}

void CCommandCreateObject::Execute()
{
	//m_pObject.reset(new C3DObject(m_model, m_x, m_y, m_rotation)); 
	CGameModel::GetIntanse().lock()->AddObject(m_pObject);
}

void CCommandCreateObject::Rollback()
{
	CGameModel::GetIntanse().lock()->DeleteObjectByPtr(m_pObject);
}