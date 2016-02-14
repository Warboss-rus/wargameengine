#include "CommandChangeGlobalProperty.h"
#include "../model/GameModel.h"
#include "../IMemoryStream.h"

CommandChangeGlobalProperty::CommandChangeGlobalProperty(std::string const& key, std::string const& value, IGameModel& model)
	:m_key(key), m_newValue(value), m_model(model), m_oldValue(model.GetProperty(key))
{

}

CommandChangeGlobalProperty::CommandChangeGlobalProperty(IReadMemoryStream & stream, IGameModel& model)
	: m_model(model)
{
	m_key = stream.ReadString();
	m_newValue = stream.ReadString();
	m_oldValue = stream.ReadString();
}

void CommandChangeGlobalProperty::Execute()
{
	m_model.SetProperty(m_key, m_newValue);
}

void CommandChangeGlobalProperty::Rollback()
{
	m_model.SetProperty(m_key, m_oldValue);
}

void CommandChangeGlobalProperty::Serialize(IWriteMemoryStream & stream) const
{
	stream.WriteByte(5);//This is a CommandChangeGlobalProperty action
	stream.WriteString(m_key);
	stream.WriteString(m_newValue);
	stream.WriteString(m_oldValue);
}