#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"

void CCommandHandler::AddNewCommand(ICommand * command, bool local)
{
	if(!m_commands.empty() && m_current < m_commands.size())
	{
		m_commands.erase(m_commands.begin() + m_current, m_commands.end());
	}
	if(m_compound)
	{
		m_compound->AddChild(command);
	}
	else
	{
		m_commands.push_back(std::unique_ptr<ICommand>(command));
	}
	m_current = m_commands.size();
	if (local && m_onNewCommand)
	{
		m_onNewCommand(command);
	}
}

CCommandHandler::CCommandHandler()
	:m_current(0)
{
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object, bool local)
{
	ICommand* action = new CCommandCreateObject(object);
	action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::AddNewDeleteObject(std::shared_ptr<IObject> object, bool local)
{
	ICommand* action = new CCommandDeleteObject(object);
	action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::AddNewMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY, bool local)
{
	ICommand* action = new CCommandMoveObject(object, deltaX, deltaY);
	if (!local) action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::AddNewRotateObject(std::shared_ptr<IObject> object, double deltaRotation, bool local)
{
	ICommand* action = new CCommandRotateObject(object, deltaRotation);
	if (!local) action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::AddNewChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value, bool local)
{
	ICommand* action = new CCommandChangeProperty(object, key, value);
	action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::AddNewChangeGlobalProperty(std::string const& key, std::string const& value, bool local)
{
	ICommand* action = new CommandChangeGlobalProperty(key, value);
	action->Execute();
	AddNewCommand(action, local);
}

void CCommandHandler::Undo()
{
	if(m_current == 0) return;
	m_current--;
	m_commands[m_current]->Rollback();
}

void CCommandHandler::Redo()
{
	if(m_current == m_commands.size()) return;
	m_commands[m_current]->Execute();
	m_current++;
}

void CCommandHandler::BeginCompound()
{
	m_compound.reset(new CCommandCompound());
}

void CCommandHandler::EndCompound()
{
	if(m_compound)
	{
		m_commands.push_back(std::unique_ptr<ICommand>(m_compound.get()));
	}
	m_compound.reset();
}

void CCommandHandler::DoOnNewCommand(std::function<void(ICommand*)> const& handler)
{
	m_onNewCommand = handler;
}
