#include "CommandMoveObject.h"
#include "..\model\GameModel.h"


CCommandMoveObject::CCommandMoveObject(float deltaX, float deltaY):
	m_deltaX(deltaX), m_deltaY(deltaY), m_pObject(CGameModel::GetIntanse().lock()->GetSelectedObject())
{
}

void CCommandMoveObject::Execute()
{
	m_pObject->Move(m_deltaX, m_deltaY, 0);
}

void CCommandMoveObject::Rollback()
{
	m_pObject->Move(-m_deltaX,-m_deltaY, 0);
}
