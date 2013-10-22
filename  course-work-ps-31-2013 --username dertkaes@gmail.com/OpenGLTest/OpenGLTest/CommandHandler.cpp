#include "CommandHandler.h"
#include "CommandCreateObject.h"

void CCommandHandler::AddNewCreateObject(std::string const& model, double x, double y, double rotation)
{
	m_commands.push_back(new CCommandCreateObject(model, x, y, rotation));
	m_current = m_commands.size() - 1;
	m_commands[m_current]->Execute();
}

void CCommandHandler::Undo()
{
	m_commands[m_current]->Rollback();
	m_current--;
}

void CCommandHandler::Redo()
{
	m_current++;
	m_commands[m_current]->Execute();
}