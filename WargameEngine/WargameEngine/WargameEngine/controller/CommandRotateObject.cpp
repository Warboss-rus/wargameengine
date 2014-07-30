#include "CommandRotateObject.h"
#include "../model/GameModel.h"

CCommandRotateObject::CCommandRotateObject(double deltaRotation):
	m_deltaRotation(deltaRotation), m_pObject(CGameModel::GetIntanse().lock()->GetSelectedObject())
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
