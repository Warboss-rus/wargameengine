#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandRotateObject :
	public ICommand
{
public:
	CCommandRotateObject(float deltaRotation);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
	float m_deltaRotation;
};

