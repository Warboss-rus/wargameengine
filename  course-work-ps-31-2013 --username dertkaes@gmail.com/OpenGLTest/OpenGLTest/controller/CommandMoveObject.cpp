#include "CommandMoveObject.h"
#include "..\model\GameModel.h"


CCommandMoveObject::CCommandMoveObject(double deltaX, double deltaY):
	m_deltaX(deltaX), m_deltaY(deltaY), m_pObject(CGameModel::GetIntanse().lock()->GetSelectedObject())
{
}

void CCommandMoveObject::Execute()
{
	m_pObject->MoveTo(m_pObject->GetX() + m_deltaX, m_pObject->GetY() + m_deltaY, 0);
}

void CCommandMoveObject::Rollback()
{
	m_pObject->MoveTo(m_pObject->GetX() - m_deltaX, m_pObject->GetY() - m_deltaY, 0);
}
