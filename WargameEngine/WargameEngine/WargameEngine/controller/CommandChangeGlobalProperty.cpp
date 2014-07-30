#include "CommandChangeGlobalProperty.h"
#include "../model/GameModel.h"

CommandChangeGlobalProperty::CommandChangeGlobalProperty(std::string const& key, std::string const& value):m_key(key), m_newValue(value), m_oldValue(CGameModel::GetIntanse().lock()->GetProperty(key))
{

}

void CommandChangeGlobalProperty::Execute()
{
	CGameModel::GetIntanse().lock()->SetProperty(m_key, m_newValue);
}

void CommandChangeGlobalProperty::Rollback()
{
	CGameModel::GetIntanse().lock()->SetProperty(m_key, m_oldValue);
}
