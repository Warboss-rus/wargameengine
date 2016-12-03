#include "CommandHandler.h"
#include "ICommand.h"
#include "CommandCreateObject.h"
#include "CommandDeleteObject.h"
#include "CommandMoveObject.h"
#include "CommandRotateObject.h"
#include "CommandChangeProperty.h"
#include "CommandChangeGlobalProperty.h"
#include "CommandPlayAnimation.h"
#include "CommandGoTo.h"
#include "CommandCompound.h"
#include "..\IMemoryStream.h"

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
		m_onNewCommand(m_commands.back().get());
	}
}

CCommandHandler::CCommandHandler()
	:m_current(0)
{
}

CCommandHandler::~CCommandHandler()
{
}

void CCommandHandler::AddNewCreateObject(std::shared_ptr<IObject> object, IGameModel & model)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandCreateObject>(object, model);
	action->Execute();
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewDeleteObject(std::shared_ptr<IObject> object, IGameModel & model)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandDeleteObject>(object, model);
	action->Execute();
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewMoveObject(std::shared_ptr<IObject> object, float deltaX, float deltaY)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandMoveObject>(object, deltaX, deltaY);
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewRotateObject(std::shared_ptr<IObject> object, float deltaRotation)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandRotateObject>(object, deltaRotation);
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewChangeProperty(std::shared_ptr<IObject> object, std::wstring const& key, std::wstring const& value)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandChangeProperty>(object, key, value);
	action->Execute();
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewChangeGlobalProperty(std::wstring const& key, std::wstring const& value, IGameModel & model)
{
	std::unique_ptr<ICommand> action = std::make_unique<CommandChangeGlobalProperty>(key, value, model);
	action->Execute();
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, eAnimationLoopMode loopMode, float speed)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandPlayAnimation>(object, animation, loopMode, speed);
	action->Execute();
	AddNewCommand(std::move(action));
}

void CCommandHandler::AddNewGoTo(std::shared_ptr<CObjectDecorator> object, float x, float y, float speed, std::string const& animation, float animationSpeed)
{
	std::unique_ptr<ICommand> action = std::make_unique<CCommandGoTo>(object, CVector3f{ x, y, 0.0f }, speed, animation, animationSpeed);
	action->Execute();
	AddNewCommand(std::move(action));
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

void CCommandHandler::ReadCommandFromStream(IReadMemoryStream & stream, IGameModel & model)
{
	unsigned char command = stream.ReadByte();
	std::unique_ptr<ICommand> action;
	switch (command)
	{
	case 0://CreateObject
	{
		action = std::make_unique<CCommandCreateObject>(stream, model);
	}break;
	case 1://DeleteObject
	{
		action = std::make_unique<CCommandDeleteObject>(stream, model);
	}break;
	case 2://MoveObject
	{
		action = std::make_unique<CCommandMoveObject>(stream, model);
	}break;
	case 3://RotateObject
	{
		action = std::make_unique<CCommandRotateObject>(stream, model);
	}break;
	case 4://ChangeProperty
	{
		action = std::make_unique<CCommandChangeProperty>(stream, model);
	}break;
	case 5://ChangeGlobalProperty
	{
		action = std::make_unique<CommandChangeGlobalProperty>(stream, model);
	}break;
	case 6://PlayAnimation
	{
		action = std::make_unique<CCommandPlayAnimation>(stream, model);
	}break;
	case 7://GoTo
	{
		//action = std::make_unique<CCommandGoTo>(stream, model);
	}break;
	default:return;
	}
	action->Execute();
	AddNewCommand(std::move(action), false);
}
