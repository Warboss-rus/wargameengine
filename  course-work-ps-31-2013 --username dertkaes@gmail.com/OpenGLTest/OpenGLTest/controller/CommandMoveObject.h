#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandMoveObject :
	public ICommand
{
public:
	CCommandMoveObject(double deltaX, double deltaY);
	void Execute();
	void Rollback();
private:
	std::shared_ptr<IObject> m_pObject;
	double m_deltaX;
	double m_deltaY;
};

