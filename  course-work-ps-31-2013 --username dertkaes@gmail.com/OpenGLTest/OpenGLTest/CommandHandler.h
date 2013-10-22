#pragma once
#include "ICommand.h"
#include <vector>
class CCommandHandler
{
public:
	void AddNewCreateObject(std::string const& model, double x, double y, double rotation);
	void Undo();
	void Redo();
private:
	std::vector<ICommand *> m_commands;
	size_t m_current;
};

