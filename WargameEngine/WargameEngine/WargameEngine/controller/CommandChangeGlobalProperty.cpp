#include "CommandChangeGlobalProperty.h"
#include "../model/GameModel.h"

CommandChangeGlobalProperty::CommandChangeGlobalProperty(std::string const& key, std::string const& value):m_key(key), m_newValue(value), m_oldValue(CGameModel::GetInstance().lock()->GetProperty(key))
{

}

void CommandChangeGlobalProperty::Execute()
{
	CGameModel::GetInstance().lock()->SetProperty(m_key, m_newValue);
}

void CommandChangeGlobalProperty::Rollback()
{
	CGameModel::GetInstance().lock()->SetProperty(m_key, m_oldValue);
}

std::vector<char> CommandChangeGlobalProperty::Serialize() const
{
	std::vector<char> result;
	result.resize(m_key.size() + m_newValue.size() + m_oldValue.size() + 13);
	result[0] = 5;//This is a CommandChangeGlobalProperty action
	unsigned int size = m_key.size();
	memcpy(&result[1], &size, 4);
	memcpy(&result[5], m_key.c_str(), size);
	unsigned int begin = size + 5;
	size = m_newValue.size();
	memcpy(&result[begin], &size, 4);
	memcpy(&result[begin + 4], m_newValue.c_str(), size);
	begin += size;
	size = m_oldValue.size();
	memcpy(&result[begin], &size, 4);
	memcpy(&result[begin + 4], m_oldValue.c_str(), size);
	return result;
}