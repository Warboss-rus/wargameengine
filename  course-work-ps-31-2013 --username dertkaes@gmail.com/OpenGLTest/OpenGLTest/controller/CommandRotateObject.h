#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandRotateObject :
	public ICommand
{
public:
	CCommandRotateObject(double deltaRotation);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
	double m_deltaRotation;
};

