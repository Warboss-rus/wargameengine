#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"
#include "GameController.h"

void CCommandHandler::AddNewCommand(ICommand * command, bool local)
{
	if(!m_commands.empty() && m_current != m_commands.size() - 1)
	{
		m_commands.erase(m_commands.begin() + m_current + 1, m_commands.end());
	}
	if(m_compound)
	{
		m_compound->AddChild(command);
	}
	else
	{
		m_commands.push_back(std::unique_ptr<ICommand>(command));
	}
	m_current = m_commands.size() - 1;
	CNetwork & network = CGameController::GetInstance().lock()->GetNetwork();
	if (local && network.IsConnected())
	{
		network.SendAction(command->Serialize(), true);
	}
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object, bool local)
{
	ICommand* action = new CCommandCreateObject(object);
	action->Execute();
	if (local)
	{
		CGameController::GetInstance().lock()->GetNetwork().AddAddressLocal(object);
	}
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
		m_commands.push_back(std::unique_ptr<ICommand>(m_compound.get()));
	}
	m_compound.reset();
}