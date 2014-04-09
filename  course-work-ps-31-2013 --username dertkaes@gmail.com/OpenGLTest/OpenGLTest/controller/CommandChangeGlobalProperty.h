#pragma once
#include "ICommand.h"
#include <string>
#include <memory>

class IObject;

class CommandChangeGlobalProperty : public ICommand
{
public:
	CommandChangeGlobalProperty(std::string const& key, std::string const& value);
	void Execute();
	void Rollback();
private:
	std::string m_key;
	std::string m_oldValue;
	std::string m_newValue;
};