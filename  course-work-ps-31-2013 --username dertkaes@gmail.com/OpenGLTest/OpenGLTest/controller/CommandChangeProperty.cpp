#include "CommandChangeProperty.h"
#include "..\model\GameModel.h"

CCommandChangeProperty::CCommandChangeProperty(std::string const& key, std::string const& value):m_key(key), m_newValue(value), m_pObject(CGameModel::GetIntanse().lock()->GetSelectedObject()),
	m_oldValue(m_pObject->GetProperty(key))
{

}

void CCommandChangeProperty::Execute()
{
	m_pObject->SetProperty(m_key, m_newValue);
}

void CCommandChangeProperty::Rollback()
{
	m_pObject->SetProperty(m_key, m_oldValue);
}
