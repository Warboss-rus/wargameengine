#pragma once
#include "ICommand.h"
#include <vector>
#include <memory>
#include "CommandCompound.h"

class IObject;
class CNetwork;

class CCommandHandler
{
public:
	CCommandHandler(CNetwork& network);
	void AddNewCreateObject(std::shared_ptr<IObject> object, bool local = true);
	void AddNewDeleteObject(std::shared_ptr<IObject> object, bool local = true);
	void AddNewMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY, bool local = true);
	void AddNewRotateObject(std::shared_ptr<IObject> object, double deltaRotation, bool local = true);
	void AddNewChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value, bool local = true);
	void AddNewChangeGlobalProperty(std::string const& key, std::string const& value, bool local = true);
	void AddNewPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, int loopMode, float speed, bool local = true);
	void AddNewGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed, bool local = true);
	void Undo();
	void Redo();
	void BeginCompound();
	void EndCompound();
private:
	void AddNewCommand(std::unique_ptr<ICommand> && command, bool local);
	std::unique_ptr<CCommandCompound> m_compound;
	std::vector<std::unique_ptr<ICommand>> m_commands;
	size_t m_current;
	CNetwork& m_network;
};

