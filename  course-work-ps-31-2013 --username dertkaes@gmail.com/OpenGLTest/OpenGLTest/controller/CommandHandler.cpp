#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"


std::shared_ptr<CCommandHandler> CCommandHandler::m_instance = NULL;

void CCommandHandler::AddNewCommand(ICommand * command)
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
		m_commands.push_back(command);
	}
	m_current = m_commands.size() - 1;
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object)
{
	ICommand* action = new CCommandCreateObject(object);
	action->Execute();
	AddNewCommand(action);
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

CCommandHandler::~CCommandHandler()
{
	m_compound.reset();
	for(auto i = m_commands.begin(); i != m_commands.end(); ++i)
	{
//		delete *i;
	}
}


void CCommandHandler::AddNewDeleteObject(std::shared_ptr<IObject> object)
{
	ICommand* action = new CCommandDeleteObject(object);
	action->Execute();
	AddNewCommand(action);
}

void CCommandHandler::AddNewMoveObject(double deltaX, double deltaY)
{
	ICommand* action = new CCommandMoveObject(deltaX, deltaY);
	AddNewCommand(action);
}

void CCommandHandler::AddNewRotateObject(double deltaRotation)
{
	ICommand* action = new CCommandRotateObject(deltaRotation);
	AddNewCommand(action);
}

void CCommandHandler::AddNewChangeProperty(std::string const& key, std::string const& value)
{
	ICommand* action = new CCommandChangeProperty(key, value);
	action->Execute();
	AddNewCommand(action);
}

void CCommandHandler::AddNewChangeGlobalProperty(std::string const& key, std::string const& value)
{
	ICommand* action = new CommandChangeGlobalProperty(key, value);
	action->Execute();
	AddNewCommand(action);
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
		m_commands.push_back(m_compound.get());
	}
	m_compound.reset();
}