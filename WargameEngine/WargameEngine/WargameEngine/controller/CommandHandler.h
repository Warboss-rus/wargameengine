#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "..\model\Animation.h"

class ICommand;
class IObject;
class IGameModel;
class IReadMemoryStream;
class CCommandCompound;

class CCommandHandler
{
public:
	CCommandHandler();
	~CCommandHandler();
	void AddNewCreateObject(std::shared_ptr<IObject> object, IGameModel & model);
	void AddNewDeleteObject(std::shared_ptr<IObject> object, IGameModel & model);
	void AddNewMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY);
	void AddNewRotateObject(std::shared_ptr<IObject> object, double deltaRotation);
	void AddNewChangeProperty(std::shared_ptr<IObject> object, std::wstring const& key, std::wstring const& value);
	void AddNewChangeGlobalProperty(std::wstring const& key, std::wstring const& value, IGameModel & model);
	void AddNewPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, eAnimationLoopMode loopMode, float speed);
	void AddNewGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed);
	void Undo();
	void Redo();
	void BeginCompound();
	void EndCompound();
	void DoOnNewCommand(std::function<void(ICommand*)> const& handler);
	void ReadCommandFromStream(IReadMemoryStream & stream, IGameModel & model);
private:
	void AddNewCommand(std::unique_ptr<ICommand> && command, bool local = true);
	std::unique_ptr<CCommandCompound> m_compound;
	std::vector<std::unique_ptr<ICommand>> m_commands;
	size_t m_current;
	std::function<void(ICommand*)> m_onNewCommand;
};

