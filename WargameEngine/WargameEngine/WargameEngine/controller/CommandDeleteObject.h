#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandDeleteObject :
	public ICommand
{
public:
	CCommandDeleteObject(std::shared_ptr<IObject> object);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
};

