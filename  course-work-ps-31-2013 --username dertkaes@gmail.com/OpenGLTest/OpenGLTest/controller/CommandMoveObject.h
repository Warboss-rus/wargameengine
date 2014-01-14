#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandMoveObject :
	public ICommand
{
public:
	CCommandMoveObject(float deltaX, float deltaY);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
	float m_deltaX;
	float m_deltaY;
};

