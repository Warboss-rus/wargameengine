#pragma once
#include "../model/Animation.h"
#include <functional>
#include <memory>
#include <vector>

namespace wargameEngine
{
class IReadMemoryStream;

namespace model
{
class IObject;
class IModel;
}

namespace controller
{
class ICommand;
class CCommandCompound;
class ObjectDecorator;

class CommandHandler
{
public:
	CommandHandler();
	~CommandHandler();
	void AddNewCreateObject(std::shared_ptr<model::IObject> object, model::IModel& model);
	void AddNewDeleteObject(std::shared_ptr<model::IObject> object, model::IModel& model);
	void AddNewMoveObject(std::shared_ptr<model::IObject> object, float deltaX, float deltaY);
	void AddNewRotateObject(std::shared_ptr<model::IObject> object, float deltaRotation);
	void AddNewChangeProperty(std::shared_ptr<model::IObject> object, std::wstring const& key, std::wstring const& value);
	void AddNewChangeGlobalProperty(std::wstring const& key, std::wstring const& value, model::IModel& model);
	void AddNewPlayAnimation(std::shared_ptr<model::IObject> object, std::string const& animation, model::AnimationLoop loopMode, float speed);
	void AddNewGoTo(std::shared_ptr<ObjectDecorator> object, float x, float y, float speed, std::string const& animation, float animationSpeed);
	void Undo();
	void Redo();
	void BeginCompound();
	void EndCompound();
	void DoOnNewCommand(std::function<void(ICommand*)> const& handler);
	void ReadCommandFromStream(IReadMemoryStream& stream, model::IModel& model);

private:
	void AddNewCommand(std::unique_ptr<ICommand>&& command, bool local = true);
	std::unique_ptr<CCommandCompound> m_compound;
	std::vector<std::unique_ptr<ICommand>> m_commands;
	size_t m_current;
	std::function<void(ICommand*)> m_onNewCommand;
};
}
}