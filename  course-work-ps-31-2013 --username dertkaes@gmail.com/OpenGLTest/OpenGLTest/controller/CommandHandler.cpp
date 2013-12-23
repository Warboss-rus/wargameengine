#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"

std::shared_ptr<CCommandHandler> CCommandHandler::m_instance = NULL;

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object)
{
	if(!m_commands.empty())
		m_commands.erase(m_commands.begin() + m_current + 1, m_commands.end());
	m_commands.push_back(new CCommandCreateObject(object));
	std::vector<ICommand*>::iterator i = m_commands.begin();
	m_current = m_commands.size() - 1;
	m_commands[m_current]->Execute();
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

void CCommandHandler::AddNewDeleteObject(std::shared_ptr<IObject> object)
{
	m_commands.push_back(new CCommandDeleteObject(object));
	m_current = m_commands.size() - 1;
	m_commands[m_current]->Execute();
}

void CCommandHandler::AddNewMoveObject(double deltaX, double deltaY)
{
	m_commands.push_back(new CCommandMoveObject(deltaX, deltaY));
	m_current = m_commands.size() - 1;
}

void CCommandHandler::AddNewRotateObject(double deltaRotation)
{
	m_commands.push_back(new CCommandRotateObject(deltaRotation));
	m_current = m_commands.size() - 1;
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