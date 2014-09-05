#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"
#include "../Network.h"

std::shared_ptr<CCommandHandler> CCommandHandler::m_instance = NULL;

void CCommandHandler::AddNewCommand(ICommand * command, bool local)
{
	if(!m_commands.empty() && m_current != m_commands.size() - 1)
	{
		m_commands.erase(m_commands.begin() + m_current + 1, m_commands.end());
	}
	if(m_compound)
	{
		m_compound->AddChildren(command);
	}
	else
	{
		m_commands.push_back(std::shared_ptr<ICommand>(command));
	}
	m_current = m_commands.size() - 1;
	if (CNetwork::GetInstance().lock()->IsConnected() && local) CNetwork::GetInstance().lock()->SendAction(command, true);
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object, bool local)
{
	ICommand* action = new CCommandCreateObject(object);
	action->Execute();
	AddNewCommand(action, local);
}

std::weak_ptr<CCommandHandler> CCommandHandler::GetInstance()
{
	if (!m_instance.get())
	{
		m_instance.reset(new CCommandHandler());
	}
	std::weak_ptr<CCommandHandler> pView(m_instance);

	return pView;
}
void CCommandHandler::FreeInstance()
{
	m_instance.reset();
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
	if(m_current == -1) return;
	m_commands[m_current]->Rollback();
	m_current--;
}

void CCommandHandler::Redo()
{
	if(m_current == m_commands.size() - 1) return;
	m_current++;
	m_commands[m_current]->Execute();
}

void CCommandHandler::BeginCompound()
{
	m_compound.reset(new CCommandCompound());
}

void CCommandHandler::EndCompound()
{
	if(m_compound)
	{
		m_commands.push_back(m_compound);
	}
	m_compound.reset();
}