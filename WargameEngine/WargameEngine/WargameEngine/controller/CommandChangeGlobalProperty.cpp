#include "CommandChangeGlobalProperty.h"
#include "../model/GameModel.h"
#include "../IMemoryStream.h"

CommandChangeGlobalProperty::CommandChangeGlobalProperty(std::wstring const& key, std::wstring const& value, IGameModel& model)
	:m_key(key), m_oldValue(model.GetProperty(key)), m_newValue(value), m_model(model)
{

}

CommandChangeGlobalProperty::CommandChangeGlobalProperty(IReadMemoryStream & stream, IGameModel& model)
	: m_model(model)
{
	m_key = stream.ReadWString();
	m_newValue = stream.ReadWString();
	m_oldValue = stream.ReadWString();
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
	stream.WriteWString(m_key);
	stream.WriteWString(m_newValue);
	stream.WriteWString(m_oldValue);
}