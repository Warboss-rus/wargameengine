#pragma once
#include "ICommand.h"
#include "../model/ObjectInterface.h"
#include <vector>
#include <memory>
#include "CommandCompound.h"

class CCommandHandler
{
public:
	CCommandHandler():m_current(-1) {}
	static std::weak_ptr<CCommandHandler> GetInstance();
	static void FreeInstance();
	void AddNewCreateObject(std::shared_ptr<IObject> object, bool local = true);
	void AddNewDeleteObject(std::shared_ptr<IObject> object, bool local = true);
	void AddNewMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY, bool local = true);
	void AddNewRotateObject(std::shared_ptr<IObject> object, double deltaRotation, bool local = true);
	void AddNewChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value, bool local = true);
	void AddNewChangeGlobalProperty(std::string const& key, std::string const& value, bool local = true);
	void Undo();
	void Redo();
	void BeginCompound();
	void EndCompound();
private:
	void AddNewCommand(ICommand * command, bool local);
	static std::shared_ptr<CCommandHandler> m_instance;
	std::unique_ptr<CCommandCompound> m_compound;
	std::vector<std::unique_ptr<ICommand>> m_commands;
	int m_current;
};

