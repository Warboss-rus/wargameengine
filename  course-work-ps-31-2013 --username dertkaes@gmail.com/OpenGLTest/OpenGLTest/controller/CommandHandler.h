#pragma once
#include "ICommand.h"
#include "..\model\ObjectInterface.h"
#include <vector>
#include <memory>
#include "CommandCompound.h"

class CCommandHandler
{
public:
	CCommandHandler():m_current(-1), m_compound(NULL) {}
	~CCommandHandler();
	static std::weak_ptr<CCommandHandler> GetInstance();
	static void FreeInstance();
	void AddNewCreateObject(std::shared_ptr<IObject> object);
	void AddNewDeleteObject(std::shared_ptr<IObject> object);
	void AddNewMoveObject(double deltaX, double deltaY);
	void AddNewRotateObject(double deltaRotation);
	void AddNewChangeProperty(std::string const& key, std::string const& value);
	void AddNewChangeGlobalProperty(std::string const& key, std::string const& value);
	void Undo();
	void Redo();
	void BeginCompound();
	void EndCompound();
private:
	void AddNewCommand(ICommand * command);
	static std::shared_ptr<CCommandHandler> m_instance;
	std::shared_ptr<CCommandCompound> m_compound;
	std::vector<ICommand*> m_commands;
	int m_current;
};

