#pragma once
#include "ICommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandChangeProperty : public ICommand
{
public:
	CCommandChangeProperty(std::string const& key, std::string const& value);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
	std::string m_key;
	std::string m_oldValue;
	std::string m_newValue;
};