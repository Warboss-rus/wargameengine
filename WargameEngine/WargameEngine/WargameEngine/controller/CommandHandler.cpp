#include "CommandHandler.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"
#include "CommandPlayAnimation.h"
#include "CommandGoTo.h"

void CCommandHandler::AddNewCommand(std::unique_ptr<ICommand> && command, bool local)
{
	if(!m_commands.empty() && m_current < m_commands.size())
	{
		m_commands.erase(m_commands.begin() + m_current, m_commands.end());
	}
	if(m_compound)
	{
		m_compound->AddChild(command.get());
	}
	else
	{
		m_commands.push_back(std::move(command));
	}
	m_current = m_commands.size();
	if (local && m_onNewCommand)
	{
		m_onNewCommand(command.get());
	}
}

CCommandHandler::CCommandHandler()
	:m_current(0)
{
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object, IGameModel & model, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandCreateObject>(object, model);
	action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewDeleteObject(std::shared_ptr<IObject> object, IGameModel & model, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandDeleteObject>(object, model);
	action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandMoveObject>(object, deltaX, deltaY);
	if (!local) action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewRotateObject(std::shared_ptr<IObject> object, double deltaRotation, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandRotateObject>(object, deltaRotation);
	if (!local) action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandChangeProperty>(object, key, value);
	action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewChangeGlobalProperty(std::string const& key, std::string const& value, IGameModel & model, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CommandChangeGlobalProperty>(key, value, model);
	action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, int loopMode, float speed, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandPlayAnimation>(object, animation, loopMode, speed);
	action->Execute();
	AddNewCommand(std::move(action), local);
}

void CCommandHandler::AddNewGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed, bool local)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandGoTo>(object, x, y, speed, animation, animationSpeed);
	action->Execute();
	AddNewCommand(std::move(action), local);
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
